/*
 * InsteonConnection.cpp
 *
 *  Created on: 2014-10-22
 *      Author: Etienne Dupéré Larivière
 */

#include "InsteonConnection.h"
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

using namespace std;
using namespace boost;

InsteonConnection::InsteonConnection(InsteonConnectionType type, string value)
{
	this->type = type;
	this->value = value;
	this->name = value;
	this->address = new InsteonAddress(0);
}

InsteonConnection::InsteonConnection(InsteonConnectionType type, string value, string name, InsteonAddress *address)
{
	this->type = type;
	this->value = value;
	this->name = name;
	this->address = address;
}

InsteonConnection::~InsteonConnection()
{
	delete address;
}

bool InsteonConnection::equals(InsteonConnection other)
{
	if((type == other.type) && (iequals(value, other.value)))
		return true;
	else
		return false;
}

const string connectionTypeTostring(InsteonConnectionType type)
{
	return EnumString<InsteonConnectionType>::From(type);
}

bool InsteonConnection::parse(string text, InsteonConnection* &connection)
{
	regex connectionPattern("\\s*([^: ]+)\\s*:\\s*([^, ]+)\\s*(,\\s*([^,]+)\\s*)?(,\\s*([^, ]+))?");
	cmatch matches;

	if (!regex_match(text.c_str(), matches, connectionPattern))
		return false;

	string type = matches[1];
	string value = matches[2];
	string name = matches[4];
	string address = matches[6];

	if (type.length() <= 0 || value.length() <= 0)
		return false;

	InsteonConnectionType rtype;
	bool result = EnumString<InsteonConnectionType>::To(rtype, type);

	if(!result)
		return false;

	InsteonAddress *raddress;
	result = InsteonAddress::parse(address, raddress);

	if(!result)
		return false;

	connection = new InsteonConnection(rtype, value, name, raddress);
	return true;
}

string InsteonConnection::toString()
{
	string name = (this->name != value) ? this->name : "";

	if (name.length() <= 0 && address->isEmpty())
		return connectionTypeTostring(type) + ": " + value;
	else if (this->address->isEmpty())
		return connectionTypeTostring(type) + ": " + value + ", " + name;
	else
		return connectionTypeTostring(type) + ": " + value + ", " + name + ", " + address->toString();
}

