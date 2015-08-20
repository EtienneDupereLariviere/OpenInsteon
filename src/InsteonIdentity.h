/*
 * InsteonIdentity.h
 *
 *  Created on: 2014-11-14
 *      Author: etienne
 */

#ifndef INSTEONIDENTITY_H_
#define INSTEONIDENTITY_H_

#include "Defs.h"

// Represents the INSTEON device type.
class InsteonIdentity
{
public:
	InsteonIdentity(uint8 devCat, uint8 subCat, uint8 firmwareVersion);

	uint8 getDevCat() { return devCat; }
	uint8 getSubCat() { return subCat; }
	uint8 getFirmwareVersion() { return firmwareVersion; }

	bool isEmpty() {  return devCat == 0 && subCat == 0 && firmwareVersion == 0; }

private:
	uint8 devCat; // The device category representing a product family.
	uint8 subCat; // The device sub-category representing various types of products within a product family.
	uint8 firmwareVersion; // The firmware version running within the device.
};

#endif /* INSTEONIDENTITY_H_ */
