/*
 * InsteonMessageProcessor.cpp
 *
 *  Created on: 2014-10-23
 *      Author: Étienne Dupéré Larivière
 */

#include "InsteonMessageProcessor.h"
#include "InsteonAddress.h"

bool InsteonMessageProcessor::deviceLinkCleanupMessage(vector<uint8> data, uint32 offset, int32 &count, InsteonMessage* &message)
{
	message = NULL;
	count = 0;
	if (data.size() <= offset + 8)
		return false;

	uint8 messageId = data[offset];
	InsteonMessageType messageType = InsteonMessageType::DeviceLinkCleanup;
	map<PropertyKey, int32> properties;

	properties[PropertyKey::LinkStatus] = data[offset + 1];
	count = 2;

	message = new InsteonMessage(messageId, messageType, properties);
	return true;
}

bool InsteonMessageProcessor::deviceLinkMessage(vector<uint8> data, uint32 offset, int32 &count, InsteonMessage* &message)
{
	message = NULL;
	count = 0;
	if (data.size() < offset + 9)
		return false;

	uint8 messageId = data[offset];
	InsteonMessageType messageType = InsteonMessageType::DeviceLink;
	map<PropertyKey, int32> properties;

	properties[PropertyKey::LinkType] = data[offset + 1];
	properties[PropertyKey::LinkGroup] = data[offset + 2];
	getAddressProperty(PropertyKey::Address, data, offset + 3, count, properties);
	properties[PropertyKey::DevCat] = data[offset + 6];
	properties[PropertyKey::SubCat] = data[offset + 7];
	properties[PropertyKey::FirmwareVersion] = data[offset + 8];
	count = 9;

	message = new InsteonMessage(messageId, messageType, properties);
	return true;
}

bool InsteonMessageProcessor::deviceLinkRecordMessage(vector<uint8> data, uint32 offset, int32 &count, InsteonMessage* &message)
{
	message = NULL;
	count = 0;
	if (data.size() < offset + 9)
		return false;

	uint8 messageId = data[offset];
	InsteonMessageType messageType = InsteonMessageType::DeviceLinkRecord;
	map<PropertyKey, int32> properties;
	InsteonAddress tmpAddress(data[offset + 3], data[offset + 4], data[offset + 5]);

	properties[PropertyKey::LinkRecordFlags] = data[offset + 1];
	properties[PropertyKey::LinkGroup] = data[offset + 2];
	properties[PropertyKey::LinkAddress] = tmpAddress.getValue();
	properties[PropertyKey::LinkData1] = data[offset + 6];
	properties[PropertyKey::LinkData2] = data[offset + 7];
	properties[PropertyKey::LinkData3] = data[offset + 8];
	count = 9;

	message = new InsteonMessage(messageId, messageType, properties);
	return true;
}

bool InsteonMessageProcessor::extendedMessage(vector<uint8> data, uint32 offset, int32 &count, InsteonMessage* &message)
{
	message = NULL;
	count = 0;
	if (data.size() < offset + 23)
		return false;

	standardMessage(data, offset, count, message);
	message->getProperties()[PropertyKey::Data1] = data[offset + 10];
	message->getProperties()[PropertyKey::Data2] = data[offset + 11];
	message->getProperties()[PropertyKey::Data3] = data[offset + 12];
	message->getProperties()[PropertyKey::Data4] = data[offset + 13];
	message->getProperties()[PropertyKey::Data5] = data[offset + 14];
	message->getProperties()[PropertyKey::Data6] = data[offset + 15];
	message->getProperties()[PropertyKey::Data7] = data[offset + 16];
	message->getProperties()[PropertyKey::Data8] = data[offset + 17];
	message->getProperties()[PropertyKey::Data9] = data[offset + 18];
	message->getProperties()[PropertyKey::Data10] = data[offset + 19];
	message->getProperties()[PropertyKey::Data11] = data[offset + 20];
	message->getProperties()[PropertyKey::Data12] = data[offset + 21];
	message->getProperties()[PropertyKey::Data13] = data[offset + 22];
	message->getProperties()[PropertyKey::Data14] = data[offset + 23];
	count = 23;
	return true;
}

bool InsteonMessageProcessor::getAddressProperty(PropertyKey key, vector<uint8> data, uint32 offset, int32 &count, map<PropertyKey, int32> &properties)
{
	count = 0;
	if (data.size() <= offset + 2)
		return false;

	properties[key] = data[offset + 0] << 16 | data[offset + 1] << 8 | data[offset + 2];
	count = 2;
	return true;
}

bool InsteonMessageProcessor::getIMInfo(vector<uint8> data, uint32 offset, int32 &count, InsteonMessage* &message)
{
	message = NULL;
	count = 0;
	if (data.size() < offset + 7)
		return false;

	uint8 messageId = data[offset];
	InsteonMessageType messageType = InsteonMessageType::GetIMInfo;
	map<PropertyKey, int32> properties;

	InsteonAddress tmpAddress(data[offset + 1], data[offset + 2], data[offset + 3]);

	properties[PropertyKey::Address] = tmpAddress.getValue();
	properties[PropertyKey::DevCat] = data[offset + 4];
	properties[PropertyKey::SubCat] = data[offset + 5];
	properties[PropertyKey::FirmwareVersion] = data[offset + 6];
	count = 7;

	message = new InsteonMessage(messageId, messageType, properties);
	return true;
}

bool InsteonMessageProcessor::getMessageFlagProperty(vector<uint8> data, uint32 offset, int32 &count, map<PropertyKey, int32> &properties)
{
	count = 0;
	if (data.size() <= offset + 7)
		return false;

	uint8 messageFlags = data[offset + 7];
	properties[PropertyKey::MessageFlagsMaxHops] = messageFlags & 0x03;
	properties[PropertyKey::MessageFlagsRemainingHops] = (messageFlags & 0x0C) >> 2;
	properties[PropertyKey::MessageFlagsExtendedFlag] = (messageFlags & 0x10) >> 4;
	properties[PropertyKey::MessageFlagsAck] = (messageFlags & 0x20) >> 5;
	properties[PropertyKey::MessageFlagsAllLink] = (messageFlags & 0x40) >> 6;
	properties[PropertyKey::MessageFlagsBroadcast] = (messageFlags & 0x80) >> 7;
	count = 7;
	return true;
}

InsteonMessageType InsteonMessageProcessor::getMessageType(vector<uint8> data, uint32 offset, map<PropertyKey, int32> &properties)
{
	uint8 cmd1 = (uint8) properties[PropertyKey::Cmd1];
	bool ack = properties[PropertyKey::MessageFlagsAck] != 0;
	bool broadcast = properties[PropertyKey::MessageFlagsBroadcast] != 0;
	bool allLink = properties[PropertyKey::MessageFlagsAllLink] != 0;

	InsteonMessageType messageType = InsteonMessageType::Other;
	if (ack)
	{
		messageType = InsteonMessageType::Ack;
	}
	else if (cmd1 == 0x06 && broadcast && allLink)
	{
		messageType = InsteonMessageType::SuccessBroadcast;
		properties[PropertyKey::ResponderCmd1] = data[offset + 4];
		properties[PropertyKey::ResponderCount] = data[offset + 5];
		properties[PropertyKey::ResponderGroup] = data[offset + 6];
		properties[PropertyKey::ResponderErrorCount] = data[offset + 9];
	}
	else if (cmd1 == 0x11 && allLink && broadcast)
	{
		messageType = InsteonMessageType::OnBroadcast;
		properties[PropertyKey::Group] = data[offset + 5];
	}
	else if (cmd1 == 0x11 && allLink && !broadcast)
	{
		messageType = InsteonMessageType::OnCleanup;
		properties[PropertyKey::Group] = data[offset + 9];
	}
	else if (cmd1 == 0x13 && allLink && broadcast)
	{
		messageType = InsteonMessageType::OffBroadcast;
		properties[PropertyKey::Group] = data[offset + 5];
	}
	else if (cmd1 == 0x13 && allLink && !broadcast)
	{
		messageType = InsteonMessageType::OffCleanup;
		properties[PropertyKey::Group] = data[offset + 9];
	}
	else if (cmd1 == 0x12 && allLink && broadcast)
	{
		messageType = InsteonMessageType::FastOnBroadcast;
		properties[PropertyKey::Group] = data[offset + 5];
	}
	else if (cmd1 == 0x12 && allLink && !broadcast)
	{
		messageType = InsteonMessageType::FastOnCleanup;
		properties[PropertyKey::Group] = data[offset + 9];
	}
	else if (cmd1 == 0x14 && allLink && broadcast)
	{
		messageType = InsteonMessageType::FastOffBroadcast;
		properties[PropertyKey::Group] = data[offset + 5];
	}
	else if (cmd1 == 0x14 && allLink && !broadcast)
	{
		messageType = InsteonMessageType::FastOffCleanup;
		properties[PropertyKey::Group] = data[offset + 9];
	}
	else if (cmd1 == 0x17 && allLink && broadcast)
	{
		messageType = InsteonMessageType::IncrementBeginBroadcast;
		properties[PropertyKey::Group] = data[offset + 5];
		properties[PropertyKey::IncrementDirection] = data[offset + 9];
	}
	else if (cmd1 == 0x18 && allLink && broadcast)
	{
		messageType = InsteonMessageType::IncrementEndBroadcast;
		properties[PropertyKey::Group] = data[offset + 5];
	}
	else if (cmd1 == 0x01 || cmd1 == 0x02)
	{
		messageType = InsteonMessageType::SetButtonPressed;
		properties[PropertyKey::DevCat] = data[offset + 3];
		properties[PropertyKey::SubCat] = data[offset + 4];
		properties[PropertyKey::FirmwareVersion] = data[offset + 5];
	}

	return messageType;
}

bool InsteonMessageProcessor::processMessage(vector<uint8> data, uint32 offset, int32 &count, InsteonMessage* &message)
{
	message = NULL;
	count = 0;
	if (data.size() <= offset)
		return false;

	switch (data[offset])
	{
		case 0x50: return standardMessage(data, offset, count, message);
		case 0x51: return extendedMessage(data, offset, count, message);
		case 0x53: return deviceLinkMessage(data, offset, count, message);
		case 0x57: return deviceLinkRecordMessage(data, offset, count, message);
		case 0x58: return deviceLinkCleanupMessage(data, offset, count, message);
		case 0x60: return getIMInfo(data, offset, count, message);
	}

	return false;
}

bool InsteonMessageProcessor::standardMessage(vector<uint8> data, uint32 offset, int32 &count, InsteonMessage* &message)
{
	message = NULL;
	count = 0;
	if (data.size() < offset + 10)
		return false;

	uint8 messageId = data[offset];
	map<PropertyKey, int32> properties;

	getAddressProperty(PropertyKey::FromAddress, data, offset + 1, count, properties);
	getMessageFlagProperty(data, offset, count, properties);
	if (properties[PropertyKey::MessageFlagsBroadcast] == 0)
		getAddressProperty(PropertyKey::ToAddress, data, offset + 4, count, properties);
	properties[PropertyKey::Cmd1] = data[offset + 8];
	properties[PropertyKey::Cmd2] = data[offset + 9];
	count = 10;

	InsteonMessageType messageType = getMessageType(data, offset, properties);
	message = new InsteonMessage(messageId, messageType, properties);
	return true;
}
