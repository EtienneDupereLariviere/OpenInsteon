/*
 * SerialPortDriver.h
 *
 *  Created on: 2014-10-08
 *      Author: Etienne Dupéré Larivère
 */

#include <string>
#include "SerialImp/serial.h"
#include "AutoResetEvent.h"
#include "ISerialPort.h"
#include <boost/signals2.hpp>

using namespace std;

class SerialPortDriver : public ISerialPort
{
private:
    serial::Serial *port;
    static AutoResetEvent waitEvent;
    static bool notifyConnect;
    static boost::signals2::signal<void ()> dataAvailable;

    static void dataReceived(int sig, siginfo_t *sinfo, void *value);
public:
    /**
     * Constructor.
     * <param port> device name, example "/dev/ttyUSB0" or "COM4"
     */
	SerialPortDriver(string name);

	/**
	 * Clean the port pointer.
	 */
	~SerialPortDriver();

	/**
	 * Close the serial port
	 */
	void close();

	/**
	 * Close the serial port and empty buffer
	 */
	void dispose();

	/**
	 * Open the serial port
	 */
	void open();

    /**
     * Write data to the serial device.
     * <param data> buffer vector to write
     */
    void write(vector<uint8> data);

    /**
     * Received data from the serial device.
     * \return a vector containing the received value
     */
    vector<uint8> readAll();

    /**
	 * Wait for data do enter the buffer
	 * <param timeout> wait time in milliseconds
	 */
    void wait(uint32 timeout);

    /**
	 * Function to notify if data available
	 * <param notify> function to notify
	 */
    void setNotify(boost::function<void ()> notify);


    /**
	 * Stop to notify if data available
	 */
    void disconnectNotify();
};
