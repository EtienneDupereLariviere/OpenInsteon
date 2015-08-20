/*
 * InsteonDeviceLinkRecord.cpp
 *
 *  Created on: 2014-11-14
 *      Author: etienne
 */

#include "InsteonDeviceLinkRecord.h"

InsteonDeviceLinkRecord::InsteonDeviceLinkRecord(map<PropertyKey, int32> properties)
{
	this->address = new InsteonAddress(properties[PropertyKey::LinkAddress]);
	this->group = (uint8) properties[PropertyKey::LinkGroup];
	this->data1 = (uint8) properties[PropertyKey::LinkData1];
	this->data2 = (uint8) properties[PropertyKey::LinkData2];
	this->data3 = (uint8) properties[PropertyKey::LinkData3];
	this->linkRecordFlags = (uint8) properties[PropertyKey::LinkRecordFlags];
}

InsteonDeviceLinkRecord::~InsteonDeviceLinkRecord() {
	delete address;
}

InsteonDeviceLinkRecordType InsteonDeviceLinkRecord::getRecordType()
{
	if ((LinkRecordFlags & 0x80) == 0)
		return InsteonDeviceLinkRecordType::Empty;
	else if ((LinkRecordFlags & 0x40) != 0)
		return InsteonDeviceLinkRecordType::DeviceController;
	else
		return InsteonDeviceLinkRecordType::DeviceResponder;
}

