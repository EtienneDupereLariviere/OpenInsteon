/*
 * InsteonNetworkBridge.cpp
 *
 *  Created on: 2014-10-25
 *      Author: etienne
 */

#include "InsteonNetworkBridge.h"
#include "Serial/SerialPortCreator.h"
#include "Serial/SerialImp/serial.h"
#include "Util/Utilities.h"
#include "Util/Log.h"
#include <boost/bind.hpp>

using namespace boost;

InsteonNetworkBridge::InsteonNetworkBridge(IMessageProcessor *messageProcessor) {
	if (messageProcessor == NULL)
		throw invalid_argument( "received null messageProcessor" );
	this->messageProcessor = messageProcessor;
}

void InsteonNetworkBridge::close()
{
	if (port != NULL)
	{
		port->close();
		delete port;
		port = NULL;
	}
}

bool InsteonNetworkBridge::connect(InsteonConnection *connection, map<PropertyKey, int32> &properties)
{
	if (port != NULL)
		port->close();

	port = SerialPortCreator::create(connection);

	vector<uint8> input{ 0x02, 0x60 };
	vector<uint8> response;

	for (int32 i = 1; i <= negotiateRetries; ++i)
	{
		Log::writeLine(Severity_level::info, "TX: " + Utilities::byteArrayToString(input));
		port->write(input);

		port->wait(openTimeout);
		vector<uint8> output = port->readAll();
		if (output.size() <= 0)
		{
			sleep(0.1);
			continue; // try again
		}

		response.clear();
		response.insert(response.end(), output.begin(), output.end());

		while (output.size() > 0 && response.size() < 9)
		{
			port->wait(openTimeout);
			output = port->readAll();
			response.insert(response.end(), output.begin(), output.end());
		}

		Log::writeLine(Severity_level::info, "RX: " + Utilities::byteArrayToString(response));

		uint32 offset = 0;
		for (uint32 j = 0; j < response.size(); ++j) {
			if (response[j] == 0x02)
			{
				offset = j;
				break;
			}
		}

		if (response.size() >= offset + 9 && response[offset] == 0x02 && response[offset + 1] == 0x60 && response[offset + 8] == 0x06)
		{
			properties[PropertyKey::Address] = response[offset + 2] << 16 | response[offset + 3] << 8 | response[offset + 4];
			properties[PropertyKey::DevCat] = response[offset + 5];
			properties[PropertyKey::SubCat] = response[offset + 6];
			properties[PropertyKey::FirmwareVersion] = response[offset + 7];
			break; // found
		}
	}


	if (response.size() == 0)
		return false;

	if (properties.size() == 0)
	{
		port->close();
		delete port;
		port = NULL;
		return false;
	}

	Log::writeLine(Severity_level::info, "Successfully negotiated with INSTEON controller on connection '" + connection->toString() + "'");
	port->setNotify(bind(&InsteonNetworkBridge::dataAvailable, this));
	return true;
}

void InsteonNetworkBridge::dataAvailable()
{
	processData();
}

void InsteonNetworkBridge::processData()
{
	 if (port == NULL)
		 throw invalid_argument( "port is not initialize" );

	vector<uint8> data = readData(0, false);
	if (data.size() > 0)
		Log::writeLine(Severity_level::info, "RX: " + Utilities::byteArrayToString(data));

	mtx.lock();
		if (data.size() > 0)
			buffer.insert(buffer.end(), data.begin(), data.end());
		data = buffer;
		buffer = vector<uint8>();
	mtx.unlock();

	if (data.size() > 0)
	{
		int32 count = 0;
		uint32 offset = 0;
		uint32 last = 0;
		while (offset < data.size())
		{
			if (data[offset++] == 0x02)
			{
				if (last != offset - 1)
					Log::writeLine(Severity_level::warning, "Skipping " + to_string(offset - last) + " bytes to '" + Utilities::byteArrayToString(data, (offset - 1)) + "', discarded '" + Utilities::byteArrayToString(data, 0, (offset - 1)) + "'");

				// loop until message successfully processed or until there is no more data available on the serial port...
				while (true)
				{
					if (messageProcessor->processMessage(data, offset, count))
					{
						offset += count;
						last = offset;
						break; // break out of the loop when message successfully processed
					}
					else
					{
						vector<uint8> appendData = readData(1, false); // try to read at least one more byte, waits up to Constants.readTime milliseconds
						if (appendData.size() == 0)
						{
							Log::writeLine(Severity_level::warning, "Could not process data '" + Utilities::byteArrayToString(data) + "'");
							break; // break out of the loop when there is no more data available on the serial port
						}

						vector<uint8> list(data.begin(), data.end());
						list.insert(list.end(), appendData.begin(), appendData.end());
						Log::writeLine(Severity_level::info, "RX: " + Utilities::byteArrayToString(list) + " (appended)");
					}
				}
			}
		}

		if (last != offset)
			Log::writeLine(Severity_level::warning, "Discarding " + to_string(offset - last) + " bytes '" + Utilities::byteArrayToString(data, last) + "'");
	}
}

EchoStatus InsteonNetworkBridge::processEcho(int echoLength)
{
	if (port == NULL)
		throw invalid_argument( "port is not initialize" );

	vector<uint8> data = readData(echoLength, true);
	if (data.size() == 0)
	{
		Log::writeLine(Severity_level::error, "No data read from port");
		return EchoStatus::None;
	}

	// if the first byte is a NAK (15) then return a NAK and add whatever additional data was read to the buffer
	if (data[0] == 0x15)
	{
		Log::writeLine(Severity_level::info, "RX: " + Utilities::byteArrayToString(data));
		if (data.size() > 1)
		{
			int32 remainingCount = data.size() - 1;

			mtx.lock();
				buffer.insert(buffer.end(), data.begin() + 1, data.begin() + (1 + remainingCount));
			mtx.unlock();
			processData(); //process the rest of the data stream
		}
		return EchoStatus::NAK;
	}

	// scan until a MESSAGE START byte (02) is detected, which should be the first byte
	uint32 offset = 0;
	while (offset < data.size())
		if (data[offset++] == 0x02)
			break;

	// exit if no MESSAGE START byte detected
	if (offset >= data.size())
	{
		Log::writeLine(Severity_level::error, "RX: " + Utilities::byteArrayToString(data) + " Failed to find MESSAGE START byte (02)");
		return EchoStatus::Unknown;
	}

	Log::writeLine(Severity_level::info, "RX: " + Utilities::byteArrayToString(data));

	// warn about any skipped bytes
	if (offset > 1)
		Log::writeLine(Severity_level::warning, "Skipping " + to_string(offset - 1) + " bytes to '" + Utilities::byteArrayToString(data, offset - 1)  + "', discarded '" + Utilities::byteArrayToString(data, 0, offset - 1)  + "'");

	// process the echo and decode the trailing status byte
	int32 count;
	if (messageProcessor->processEcho(data, offset, count))
	{
		uint32 j = offset + count;
		uint8 result = j < data.size() ? data[j] : (uint8)0x00;
		j += 1;
		if (data.size() > j) // if there's data beyond the echo then add it to the buffer
		{
			int32 remainingCount = data.size() - j;

			mtx.lock();
				buffer.insert(buffer.end(), data.begin() + j, data.begin() + (j + remainingCount));
			mtx.unlock();
			processData(); //process the rest of the data stream
		}
		if (result == 0x06)
		{
			messageProcessor->setEchoStatus(EchoStatus::ACK);
			Log::writeLine(Severity_level::info, "PLM: " + Utilities::byteArrayToString(data, offset - 1, count + 2) + " ACK"); // +1 for MESSAGE START byte (02), +1 for ACK byte (06)
			return EchoStatus::ACK;
		}
		else if (result == 0x15)
		{
			Log::writeLine(Severity_level::info, "PLM: " + Utilities::byteArrayToString(data, offset - 1, count + 2) + " NAK"); // +1 for MESSAGE START byte (02), +1 for NAK byte (15)
			messageProcessor->setEchoStatus(EchoStatus::NAK);
			return EchoStatus::NAK;
		}
		else
		{
			Log::writeLine(Severity_level::info, "PLM: " + Utilities::byteArrayToString(data, offset - 1, count + 2) + " Unknown trailing byte"); // +1 for MESSAGE START byte (02), +1 for unknown byte
			messageProcessor->setEchoStatus(EchoStatus::Unknown);
			return EchoStatus::Unknown;
		}
	}
	else
	{
		Log::writeLine(Severity_level::info, "PLM: " + Utilities::byteArrayToString(data, offset - 1) + " Echo mismatch");
		return EchoStatus::Unknown;
	}
}

vector<uint8> InsteonNetworkBridge::readData(uint32 expectedBytes, bool isEcho)
{
	vector<uint8> list;
	vector<uint8> data = port->readAll();
	list.insert(list.end(), data.begin(), data.end());

	// if we are expecting an echo response and the first byte received was a NAK (15) then don't bother waiting for more data
	if (isEcho && data.size() > 0 && data[0] == 0x15)
		return list;  // caller will log the NAK

	// if we didn't get the expected number of bytes then try to read more data within a timeout period
	if (expectedBytes > 0)
	{
		int32 retryCount = 0;
		while (list.size() == 0 && ++retryCount <= readDataRetries)
		{
			port->wait(readDataRetryTime);
			data = port->readAll();
			list.insert(list.end(), data.begin(), data.end());
		}

		if (list.size() < expectedBytes)
		{
			do
			{
				port->wait(readDataTimeout);
				data = port->readAll();
				list.insert(list.end(), data.begin(), data.end());
			} while (data.size() > 0);
		}

		// if we are expecting an echo response and the first byte received was a NAK (15) then don't bother waiting for more data
		if (isEcho && list.size() > 0 && list[0] == 0x15)
			return list; // caller will log the NAK

		if (list.size() < expectedBytes)
		{
			if (list.size() > 0)
				Log::writeLine(Severity_level::warning, "Could not read the expected number of bytes from the serial port; BytesRead='" + Utilities::byteArrayToString(list) + "', Expected=" + to_string(expectedBytes) + ", Received=" + to_string(list.size()) + ", Timeout=" + to_string(readDataTimeout) + "ms");
			else
				Log::writeLine(Severity_level::warning, "Could not read the expected number of bytes from the serial port; Expected=" + to_string(expectedBytes) + ", Received=0, Timeout=" + to_string(readDataTimeout) + "ms");
		}
	}
	return list;
}

EchoStatus InsteonNetworkBridge::send(vector<uint8> message, bool retryOnNak, int32 echoLength)
{
	if (port == NULL)
		throw invalid_argument( "port is not initialize" );

	port->disconnectNotify();
	EchoStatus status = EchoStatus::None;

	processData(); // process any pending data before sending a new command

	vector<uint8> input = { 0x02 };
	input.insert(input.end(), message.begin(), message.end());

	int32 retry = -1;
	while (retry++ < sendMessageRetries)
	{
		if (retry <= 0)
		{
			Log::writeLine(Severity_level::info, "TX: " + Utilities::byteArrayToString(input));
		}
		else
		{
			sleep(retry * (sendMessageWaitTime/1000));
			Log::writeLine(Severity_level::info, "TX: " + Utilities::byteArrayToString(input) + " - RETRY " + to_string(retry) + " of " + to_string(sendMessageRetries));
		}
		port->write(input);
		status = processEcho(echoLength + 2); // +1 for leading 02 byte, +1 for trailing ACK/NAK byte
		if (status == EchoStatus::ACK)
			break;
		if (status == EchoStatus::NAK && !retryOnNak)
			break;
	}

	if((status != EchoStatus::ACK) && (status != EchoStatus::NAK && retryOnNak))
		Log::writeLine(Severity_level::error, "Send failed after " + to_string(sendMessageRetries) + " retries");

	port->setNotify(bind(&InsteonNetworkBridge::dataAvailable, this));
	return status;
}
