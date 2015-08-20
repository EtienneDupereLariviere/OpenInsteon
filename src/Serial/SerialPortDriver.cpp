/*
 * SerialPortDriver.cpp
 *
 *  Created on: 2014-10-20
 *      Author: Etienne Dupéré Larivère
 */

#include "SerialPortDriver.h"

using namespace std;
using namespace serial;

AutoResetEvent SerialPortDriver::waitEvent(false);
bool SerialPortDriver::notifyConnect = false;
boost::signals2::signal<void ()> SerialPortDriver::dataAvailable;

SerialPortDriver::SerialPortDriver(string name)
{
	port = new serial::Serial(name, 19200, serial::Timeout::simpleTimeout(20000), dataReceived);
}

SerialPortDriver::~SerialPortDriver()
{
	delete port;
}

void SerialPortDriver::close()
{
	if (port->isOpen())
	{
		port->close();
		disconnectNotify();
	}
}

void SerialPortDriver::open()
{
	if (port->isOpen())
		port->close();
	port->open();
}

void SerialPortDriver::write(vector<uint8> data)
{
	if (!port->isOpen())
		port->open();
	port->write(data);
}

void SerialPortDriver::setNotify(boost::function<void ()> notify)
{
	dataAvailable.connect(notify);
	SerialPortDriver::notifyConnect = true;
}

void SerialPortDriver::disconnectNotify()
{
	dataAvailable.disconnect_all_slots();
	SerialPortDriver::notifyConnect = false;
}

vector<uint8> SerialPortDriver::readAll()
{
	if (!port->isOpen())
		port->open();
	int count = port->available();
	vector<uint8> data;
	port->read(data, count);
	return data;
}

void SerialPortDriver::wait(uint32 timeout)
{
	SerialPortDriver::waitEvent.reset();
	SerialPortDriver::waitEvent.waitOne(timeout);
}

void SerialPortDriver::dataReceived(int sig, siginfo_t *sinfo, void *value)
{
	if (sinfo->si_code == POLL_IN)
	{
		if (notifyConnect)
			dataAvailable();
		waitEvent.set();
	}
}
