/*
 * InsteonMessage.cpp
 *
 *  Created on: 2014-10-23
 *      Author: Étienne Dupéré Larivière
 */

#include "InsteonMessage.h"
#include "Util/Utilities.h"
#include <sstream>

using namespace std;

InsteonMessage::InsteonMessage(int32 messageId, InsteonMessageType messageType, map<PropertyKey, int32> properties)
{
	this->messageId = messageId;
	this->messageType = messageType;
	this->properties = properties;
	this->key = "Key";
}

string InsteonMessage::toString()
{
	return EnumString<InsteonMessageType>::From(messageType);
}

string InsteonMessage::toString(string format)
{
	if (format == "Log")
	{
		stringstream ss;
		ss << "  MessageId={" << uppercase << hex << messageId << "}\n";
		ss << "  MessageType={" << EnumString<InsteonMessageType>::From(messageType) << "}";
		ss << Utilities::formatProperties(properties, true, false);
		return ss.str();
	}
	else if (format == "Key")
	{
		stringstream ss;
		ss << "MessageId={" << uppercase << hex << messageId << "} ";
		ss << Utilities::formatProperties(properties, false, true);
		return ss.str();
	}
	else
	{
		return toString();
	}
}

