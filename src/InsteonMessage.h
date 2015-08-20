/*
 * InsteonMessage.h
 *
 *  Created on: 2014-10-23
 *      Author: Étienne Dupéré Larivière
 */

#ifndef INSMESAGE_H_INCLUDED
#define INSMESAGE_H_INCLUDED

#include "Defs.h"
#include <string>
#include <map>

using namespace std;

class InsteonMessage
{
public:
	InsteonMessage(int32 messageId, InsteonMessageType messageType, map<PropertyKey, int32> properties);


	/**
	 * Return the string messageType of the INSTEON message.
	 */
	string toString();

	/**
	 * Converts this instance to its equivalent string representation.
	 * <param format> The format representation of the INSTEON message
	 */
	string toString(string format);

    string getKey() { return key; }
    int32 getMessageId() { return messageId; }
    InsteonMessageType getMessageType() { return messageType; }
    map<PropertyKey, int32> getProperties() { return properties; }

private:
	string key;
    int32 messageId;
    InsteonMessageType messageType;
    map<PropertyKey, int32> properties;
};

#endif
