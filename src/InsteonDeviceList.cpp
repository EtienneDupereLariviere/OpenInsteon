/*
 * InsteonDeviceList.cpp
 *
 *  Created on: 2014-11-16
 *      Author: etienne
 */

#include "InsteonDeviceList.h"

using namespace std;

InsteonDeviceList::InsteonDeviceList(InsteonNetwork *network)
{
	this->network = network;
}

InsteonDeviceList::~InsteonDeviceList()
{
	for (auto it=devices.begin(); it!=devices.end(); ++it)
		delete it->second;
}

InsteonDevice* InsteonDeviceList::add(InsteonAddress *address, InsteonIdentity *identity)
{
	if (devices.find(address->getValue()) != devices.end())
		return devices[address->getValue()];

	InsteonDevice *device = new InsteonDevice(network, address, identity);
	devices.insert(pair<int32,InsteonDevice*>(address->getValue(), device));
	onDeviceAdded(device);
	return device;
}

bool InsteonDeviceList::containsKey(int32 address)
{
	if(devices.find(address) != devices.end())
		return true;
	else
		return false;
}

bool InsteonDeviceList::containsKey(InsteonAddress *address)
{
	if(devices.find(address->getValue()) != devices.end())
		return true;
	else
		return false;
}

uint32 InsteonDeviceList::count()
{
	return devices.size();
}

InsteonDevice* InsteonDeviceList::find(int address)
{
	return devices[address];
}

InsteonDevice* InsteonDeviceList::find(InsteonAddress *address)
{
	return devices[address->getValue()];
}

vector<InsteonDevice*> InsteonDeviceList::getEnumerator()
{
	vector<InsteonDevice*> deviceList;
	for (auto it=devices.begin(); it!=devices.end(); ++it)
		deviceList.push_back(it->second);

	return deviceList;
}

void InsteonDeviceList::onDeviceAdded(InsteonDevice *device)
{
	deviceAdded(device);
}

void InsteonDeviceList::onDeviceCommandTimeout(InsteonDevice *device)
{
	deviceCommandTimeout(device);
}

void InsteonDeviceList::onDeviceIdentified(InsteonDevice *device)
{
	deviceIdentified(device);
}

void InsteonDeviceList::onDeviceStatusChanged(InsteonDevice *device, InsteonDeviceStatus status)
{
	deviceStatusChanged(device, status);
}

