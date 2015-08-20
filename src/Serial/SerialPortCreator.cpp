/*
 * SerialPortCreator.cpp
 *
 *  Created on: 2014-10-29
 *      Author: etienne
 */

#include "SerialPortCreator.h"
#include "SerialPortDriver.h"
#include "../Defs.h"

ISerialPort* SerialPortCreator::create(InsteonConnection *connection)
{
	if(connection == NULL)
		throw invalid_argument( "connection is null" );
	switch (connection->getType())
	{
		case InsteonConnectionType::Serial: return new SerialPortDriver(connection->getValue());
	}
	throw invalid_argument( "connection type is not valid" );
}

