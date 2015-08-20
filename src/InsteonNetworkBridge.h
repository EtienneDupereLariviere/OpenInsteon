/*
 * InsteonNetworkBridge.h
 *
 *  Created on: 2014-10-25
 *      Author: Etienne Dupéré Larivière
 */

#ifndef INSTEONNETWORKBRIDGE_H_
#define INSTEONNETWORKBRIDGE_H_

#include "Defs.h"
#include "Serial/ISerialPort.h"
#include "InsteonConnection.h"
#include <map>
#include <mutex>
#include <vector>

using namespace std;

class IMessageProcessor
{
   public:
	  virtual bool processMessage(vector<uint8> data, uint32 offset, int32 &count) = 0;
	  virtual bool processEcho(vector<uint8> data, uint32 offset, int32 &count) = 0;
	  virtual void setEchoStatus(EchoStatus status) = 0;
	  virtual ~IMessageProcessor() {};
};

// This class is responsible for bridging the logical network to the physical INSTEON network via the serial interface.
// The responsibilities of the bridge include:
//  - Owning the serial connection to the INSTEON controller device.
//  - Directing all serial communications through the serial interface to the INSTEON controller device.
//  - Detecting the beginning of each raw message by identifying the MESSAGE START byte (02).
//  - Delegating to the messenger the interpretation of the bytes following the header byte.
//    Note: The messenger is responsible for reporting back to the bridge whether or not each message is valid, and if valid the size in bytes of the message.
//  - Verifying the message integrity by checking the trailer byte for ACK (06) or NAK (15), and informing the messenger of the result.
class InsteonNetworkBridge
{
public:
	InsteonNetworkBridge(IMessageProcessor *messageProcessor);

	void close();
	bool connect(InsteonConnection *connection, map<PropertyKey, int32> &properties);
	EchoStatus send(vector<uint8> message, bool retryOnNak, int32 echoLength);
	bool isConnected() { return port != NULL; }

private:
	mutex mtx;
	vector<uint8> buffer;
	IMessageProcessor *messageProcessor;
	ISerialPort *port = NULL;

	void dataAvailable();
	void processData();
	EchoStatus processEcho(int echoLength);
	vector<uint8> readData(uint32 expectedBytes, bool isEcho);
};

#endif /* INSTEONNETWORKBRIDGE_H_ */
