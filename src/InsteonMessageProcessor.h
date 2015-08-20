/*
 * InsteonMessageProcessor.h
 *
 *  Created on: 2014-10-23
 *      Author: Étienne Dupéré Larivière
 */

#ifndef INSTEONMESSAGEPROCESSOR_H_
#define INSTEONMESSAGEPROCESSOR_H_

#include "Defs.h"
#include "InsteonMessage.h"
#include <vector>
#include <map>

using namespace std;

class InsteonMessageProcessor
{
public:
	static bool processMessage(vector<uint8> data, uint32 offset, int32 &count, InsteonMessage* &message);

private:
	static bool deviceLinkCleanupMessage(vector<uint8> data, uint32 offset, int32 &count, InsteonMessage* &message);
	static bool deviceLinkMessage(vector<uint8> data, uint32 offset, int32 &count, InsteonMessage* &message);
	static bool deviceLinkRecordMessage(vector<uint8> data, uint32 offset, int32 &count, InsteonMessage* &message);
	static bool extendedMessage(vector<uint8> data, uint32 offset, int32 &count, InsteonMessage* &message);
	static bool getAddressProperty(PropertyKey key, vector<uint8> data, uint32 offset, int32 &count, map<PropertyKey, int32> &properties);
	static bool getIMInfo(vector<uint8> data, uint32 offset, int32 &count, InsteonMessage* &message);
	static bool getMessageFlagProperty(vector<uint8> data, uint32 offset, int32 &count, map<PropertyKey, int32> &properties);
	static InsteonMessageType getMessageType(vector<uint8> data, uint32 offset, map<PropertyKey, int32> &properties);
	static bool standardMessage(vector<uint8> data, uint32 offset, int32 &count, InsteonMessage* &message);
};

#endif /* INSTEONMESSAGEPROCESSOR_H_ */
