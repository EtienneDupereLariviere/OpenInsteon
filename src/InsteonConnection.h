/*
 * InsteonConnection.h
 *
 *  Created on: 2014-10-22
 *      Author: Etienne Dupéré Larivière
 */

#ifndef INSCONN_H_INCLUDED
#define INSCONN_H_INCLUDED

#include "InsteonAddress.h"
#include "Defs.h"
#include <string>

using namespace std;

class InsteonConnection
{
public:
	/**
	 * Initializes a new connection instance.
	 * <param type> Type of connection.
	 * <param value> The connection value.
	 */
	InsteonConnection(InsteonConnectionType type, string value);

	/**
	 * Initializes a new connection instance.
	 * <param type>Type of connection.
	 * <param value>The connection value.
	 * <param name>The display name for the connection.
	 * <param address>The INSTEON address of the controller device.
	 */
	InsteonConnection(InsteonConnectionType type, string value, string name, InsteonAddress *address);

	/**
	 * Remove the address pointer when destroy connection.
	 */
	~InsteonConnection();

	/**
	 * Determines whether this connection is the same as the specified connection.
	 * Equality is determined by comparing the Type and Value properties.
	 * The Name and Address properties are not used in the comparison as they are intended for informational display purposes only.
	 * <param other> The specified other connection object
	 * Returns true if this connection is the same as the other connection.
	 */
	bool equals(InsteonConnection other);


	/**
	 * Parses a string into a connection object.
	 * <param text> The specified connection string.
	 * <param connection> The returned connection object
	 * Returns true if the string could be parsed.
	 */
    static bool parse(string text, InsteonConnection* &connection);

    /**
     * Converts this instance to its equivalent string representation.
     */
	string toString();

	/**
	 * The display name for the connection.
	 */
	string getName() {return name;}

	/**
	 * The type of connection (e.g., network or serial).
	 */
	InsteonConnectionType getType() {return type;}

	/**
	 * Value that specifies the network address or serial port (e.g., "192.168.1.1" or "COM3").
	 */
	string getValue() {return value;}

	/**
	 * The INSTEON address of the controller device.
	 */
	InsteonAddress* getAddress() { return address; }

private:
	InsteonAddress *address;
	string name;
	InsteonConnectionType type;
	string value;
};

#endif
