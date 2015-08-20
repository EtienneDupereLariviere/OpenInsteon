/*
 * InsteonIdentity.cpp
 *
 *  Created on: 2014-11-14
 *      Author: etienne
 */

#include "InsteonIdentity.h"

InsteonIdentity::InsteonIdentity(uint8 devCat, uint8 subCat, uint8 firmwareVersion) {
	this->devCat = devCat;
	this->subCat = subCat;
	this->firmwareVersion = firmwareVersion;

}


