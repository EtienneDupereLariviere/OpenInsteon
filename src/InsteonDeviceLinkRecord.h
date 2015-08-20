/*
 * InsteonDeviceLinkRecord.h
 *
 *  Created on: 2014-11-14
 *      Author: etienne
 */

#ifndef INSTEONDEVICELINKRECORD_H_
#define INSTEONDEVICELINKRECORD_H_

#include "InsteonAddress.h"
#include "Defs.h"

// Represents a link to another device within an INSTEON device's link table.
class InsteonDeviceLinkRecord
{
public:
	InsteonDeviceLinkRecord(map<PropertyKey, int32> properties);
	~InsteonDeviceLinkRecord();

	/**
	 * Determines whether the device link is a controller link or a responder link.
	 */
	InsteonDeviceLinkRecordType getRecordType();

	InsteonAddress* getAddress() { return address; }
	uint8 getData1() { return data1; }
	uint8 getData2() { return data2; }
	uint8 getData3() { return data3; }
	uint8 getGroup() { return group; }
	uint8 getLinkRecordFlags() { return linkRecordFlags; }

private:
	/**
	 * The INSTEON address of the device link.
	 */
	InsteonAddress* address;

	/**
	 * Device specific link information for the device link.
	 * For a responder link, this value typically represents the on-level.
	 * For a controller link, this value specifies the number of times to retry the command.
	 */
	uint8 data1;

	/**
	 * Device specific link information for the device link.
	 * For a responder link, this value typically represents the ramp rate.
	 */
	uint8 data2;

	/**
	 * Device specific link information for the device link.
	 * For a responder link in a KeypadLinc device, this value typically represents the keypad button number.
	 */
	uint8 data3;

	/**
	 * The group number of the device link.
	 */
	uint8 group;

	/**
	 *Indicates if the device link is in use, and whether it is a controller or a responder link.
	 */
	uint8 linkRecordFlags;
};

#endif /* INSTEONDEVICELINKRECORD_H_ */
