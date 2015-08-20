/*
 * Utilities.cpp
 *
 *  Created on: 2014-10-23
 *      Author: etienne
 */

#include "Utilities.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

vector<uint8> Utilities::arraySubset(vector<uint8> data, uint32 offset)
{
	return arraySubset(data, offset, data.size() - offset);
}

vector<uint8> Utilities::arraySubset(vector<uint8> data, uint32 offset, uint32 count)
{
	if (count > data.size() - offset)
		count = data.size() - offset;
	vector<uint8> result;
	result.insert(result.end(), data.begin() + offset, data.begin() + offset + count);
	return result;
}

bool Utilities::arraySequenceEquals(vector<uint8> a, vector<uint8> b)
{
	if (a.size() == b.size())
	{
		for (uint32 i = 0; i < a.size(); ++i)
		{
			if (a[i] != b[i])
				return false;
		}
	}
	return true;
}

string Utilities::byteToString(uint8 byte)
{
	stringstream ss;
	ss << hex << uppercase << (unsigned int) byte;
	return ss.str();
}

string Utilities::byteArrayToString(vector<uint8> data)
{
	return byteArrayToString(data, 0, data.size());
}

string Utilities::byteArrayToString(vector<uint8> data, uint32 offset)
{
	return byteArrayToString(data, offset, data.size() - offset);
}

string Utilities::byteArrayToString(vector<uint8> data, uint32 offset, int count)
{
	ostringstream os;

	os << hex << uppercase << setfill('0');
	for (uint32 i = offset; i < (offset + count); ++i)
	{
		if (i < data.size())
		{
			string startcharacter;

			if(i == offset)
				startcharacter = "";
			else
				startcharacter = " ";

			os << startcharacter << setw(2) << (unsigned int) data[i];
		}
	}
	return os.str();
}

string Utilities::formatProperties(map<PropertyKey, int32> properties, bool multiline, bool filterMessageFlags)
{
	bool first = true;
	stringstream ss;

	for (map<PropertyKey, int32>::iterator it=properties.begin(); it!=properties.end(); ++it)
	{
		if (!filterMessageFlags || (it->first != PropertyKey::MessageFlagsRemainingHops && it->first != PropertyKey::MessageFlagsMaxHops))
		{
			if (multiline)
			{
				ss << "\n  ";
			}
			else
			{
				if (!first)
					ss << " ";
			}
			ss << EnumString<PropertyKey>::From(it->first) << "=" << uppercase << hex << it->second;
			first = false;
		}
	}
	return ss.str();
}


