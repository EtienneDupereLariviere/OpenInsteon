/*
 * InsteonDeviceList.h
 *
 *  Created on: 2014-11-16
 *      Author: etienne
 */

#ifndef INSTEONDEVICELIST_H_
#define INSTEONDEVICELIST_H_

#include "Defs.h"
#include "InsteonNetwork.h"
#include "InsteonAddress.h"
#include "InsteonIdentity.h"
#include "InsteonDevice.h"
#include <boost/signals2.hpp>
#include <map>

// Represents the collection of known INSTEON devices within the INSTEON network.
class InsteonDeviceList {
public:
	boost::signals2::signal<void (InsteonDevice* device)> deviceAdded; // Invoked whenever a device is added to the network.
	boost::signals2::signal<void (InsteonDevice* device)> deviceCommandTimeout; // Invoked when a device fails to respond to a command within the timeout period of 2 seconds.
	boost::signals2::signal<void (InsteonDevice* device)> deviceIdentified; // Invoked when a device is identified.
	boost::signals2::signal<void (InsteonDevice* device, InsteonDeviceStatus status)> deviceStatusChanged; // Invoked when a status message is received from any known INSTEON device in the network, for example when a device turns on or off.

	InsteonDeviceList(InsteonNetwork *network);
	~InsteonDeviceList();

	/**
	 * Adds an INSTEON device to the list of known devices.
	 * <param address>The INSTEON address of the device to add
     * <param identity>The INSTEON identity of the device to add
     * \ Returns an object representing the specified device.
	 */
	InsteonDevice* add(InsteonAddress *address, InsteonIdentity *identity);

	/**
	 * Determine whether the specified INSTEON device address is contained within the list of known devices.
	 * <param address>The specified INSTEON address.
	 * \ Returns true if the list contains the specified INSTEON device.
	 */
	bool containsKey(int32 address);

	/**
	 * Determine whether the specified INSTEON device address is contained within the list of known devices.
	 * <param address>The specified INSTEON address.
	 * \ Returns true if the list contains the specified INSTEON device.
	 */
	bool containsKey(InsteonAddress *address);

	/**
	 * Returns the number of devices in the known INSTEON device collection.
	 */
	uint32 count();

	/**
	 * Finds the object representation of the specified device within the list of known devices.
	 * <param address>The specified INSTEON address.
	 * \ Returns an object representing the specified INSTEON device.
	 */
	InsteonDevice* find(int32 address);

	/**
	 * Finds the object representation of the specified device within the list of known devices.
	 * <param address>The specified INSTEON address.
	 * \ Returns an object representing the specified INSTEON device.
	 */
	InsteonDevice* find(InsteonAddress *address);

	/**
	 * Returns a vector of InsteonDevice object that can be used to iterate.
	 */
	vector<InsteonDevice*> getEnumerator();

	void onDeviceAdded(InsteonDevice *device);
	void onDeviceCommandTimeout(InsteonDevice *device);
	void onDeviceIdentified(InsteonDevice *device);
	void onDeviceStatusChanged(InsteonDevice *device, InsteonDeviceStatus status);

private:
	InsteonNetwork *network;
	map<int32, InsteonDevice*> devices;
};

#endif /* INSTEONDEVICELIST_H_ */
