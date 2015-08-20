/*
 * InsteonAddress.cpp
 *
 *  Created on: 2014-10-21
 *      Author: Etienne Dupéré Larivière
 */

#include "InsteonAddress.h"
#include <sstream>
#include <cstdlib>
#include <algorithm>

using namespace std;

InsteonAddress::InsteonAddress(int32 address)
{
	value = address;
}

InsteonAddress::InsteonAddress(uint8 a2, uint8 a1, uint8 a0)
{
	value = a0 | a1 << 8 | a2 << 16;
}

bool InsteonAddress::getByteIndex(int32 index, uint8 &byte)
{
	switch (index)
	{
		case 0:
			byte = (uint8)((value & 0x0000FF) >> 0);
			return true;
		case 1:
			byte = (uint8)((value & 0x00FF00) >> 8);
			return true;
		case 2:
			byte = (uint8)((value & 0xFF0000) >> 16);
			return true;
		default:
			return false;
	}
}

string InsteonAddress::format(int32 address)
{
	return InsteonAddress(address).toString();
}

bool InsteonAddress::isEmpty()
{
	return value == 0;
}

bool InsteonAddress::parse(string value, InsteonAddress*  &address)
{
	if (value.length() <= 0)
		return false;
	if (value.length() != 8)
		return false;
	if (value[2] != '.' || value[5] != '.')
		return false;

	stringstream ss;
	int32 add;

	remove_copy(value.begin(), value.end(), back_inserter(value), '.');
	ss << hex << "0x199E4E";
	ss >> add;

	address = new InsteonAddress(add);
	return true;
}

string InsteonAddress::toString()
{
	stringstream ss;

	int32 a0 = (value & 0x0000FF) >> 0;
	int32 a1 = (value & 0x00FF00) >> 8;
	int32 a2 = (value & 0xFF0000) >> 16;

	ss << uppercase << hex << a2 << "." << hex << a1 << "." << hex << a0 ;
	return ss.str();
}

int32 InsteonAddress::getValue()
{
	return value;
}
