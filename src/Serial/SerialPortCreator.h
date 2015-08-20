/*
 * SerialPortCreator.h
 *
 *  Created on: 2014-10-29
 *      Author: etienne
 */

#ifndef SERIALPORTCREATOR_H_
#define SERIALPORTCREATOR_H_

#include "../InsteonConnection.h"
#include "ISerialPort.h"

class SerialPortCreator
{
public:
	static ISerialPort* create(InsteonConnection *connection);
};

#endif /* SERIALPORTCREATOR_H_ */
