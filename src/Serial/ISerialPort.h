/*
 * ISerialPort.h
 *
 *  Created on: 2014-10-25
 *      Author: etienne
 */

#include "../Defs.h"
#include <boost/function.hpp>
#include <vector>

using namespace std;

#ifndef ISERIALPORT_H_
#define ISERIALPORT_H_

// Provides an abstract serial communication interface to an INSTEON controller device.
class ISerialPort
{
public:
	virtual void close()=0;
	virtual void open()=0;
	virtual vector<uint8> readAll()=0;
	virtual void setNotify(boost::function<void ()> notify)=0;
	virtual void disconnectNotify()=0;
	virtual void write(vector<uint8> data)=0;
	virtual void wait(uint32 timeout)=0;
	virtual ~ISerialPort() {};
};

#endif /* ISERIALPORT_H_ */
