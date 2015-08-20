/*
 * InsteonAddress.h
 *
 *  Created on: 2014-10-21
 *      Author: Etienne Dupéré Larivière
 */

#ifndef INSADD_H_INCLUDED
#define INSADD_H_INCLUDED

#include <string>
#include "Defs.h"

using namespace std;

class InsteonAddress
{
public:

	/**
	 * Constructor.
	 * <param address> represent a INSTEON address. Example: 0x199E4E.
	 */
	InsteonAddress(int32 address);

	/**
	 * Constructor.
	 * <param a2> The high order byte part of the INSTEON address.
     * <param a1> The middle order byte part of the INSTEON address.
     * <param a0> The low order byte part of the INSTEON address.
	 */
	InsteonAddress(uint8 a2, uint8 a1, uint8 a0);

    /**
 	 * Returns the specified part of the INSTEON address.
	 * <param index> specifies the byte part to be returned. Valid indexes are 0, 1, and 2.
	 * <param byte> the result byte part.
	 * Returns true if the index was valid.
	 */
	bool getByteIndex(int32 index, uint8 &byte);

	/**
	 * Formats the INSTEON address as a string.
	 * <param address> is the INSTEON address, example: 0x199E4E.</param>
	 */
	static string format(int32 address);

	/**
	 * Gets a value indicating whether this InsteonAddress is empty.
	 */
	bool isEmpty();

	/**
	 * Converts the string representation of an INSTEON address to its numeric equivalent.
	 * <param value> is a string specifying the INSTEON address, example: "19.9E.4E".
     * <param address> is an object representing the resulting INSTEON address.
     * Returns true if the string was successfully parsed.</returns>
	 */
	static bool parse(string value, InsteonAddress*  &address);

	/**
	 * Converts the numeric value to its equivalent string representation.
	*/
	string toString();

	/**
	 * Returns the integer representation of the INSTEON address.
	*/
	int32 getValue();

private:
	int32 value;
};

#endif
