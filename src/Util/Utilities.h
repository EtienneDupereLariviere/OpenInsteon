/*
 * Utilities.h
 *
 *  Created on: 2014-10-23
 *      Author: etienne
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "../Defs.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

class Utilities
{
public:
	static vector<uint8> arraySubset(vector<uint8>  data, uint32 offset);
	static vector<uint8> arraySubset(vector<uint8>  data, uint32 offset, uint32 count);
	static bool arraySequenceEquals(vector<uint8> a, vector<uint8> b);
	static string byteToString(uint8 byte);

	static string byteArrayToString(vector<uint8> data);
	static string byteArrayToString(vector<uint8> data, uint32 offset);
	static string byteArrayToString(vector<uint8> data, uint32 offset, int count);
	static string formatProperties(map<PropertyKey, int32> properties, bool multiline, bool filterMessageFlags);
};

#endif /* UTILITIES_H_ */
