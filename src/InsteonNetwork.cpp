/*
 * InsteonNetwork.cpp
 *
 *  Created on: 2014-11-11
 *      Author: etienne
 */

#include "InsteonNetwork.h"
#include "Util/Log.h"
#include "Serial/SerialImp/serial.h"
#include <sstream>

InsteonNetwork::InsteonNetwork()
{
	messenger = new InsteonMessenger(this);
	devices = new InsteonDeviceList(this);
}

InsteonNetwork::~InsteonNetwork()
{
	delete messenger;
	delete devices;
}

void InsteonNetwork::connect(InsteonConnection *connect)
{
	messenger->connect(connect);
	connection = connect;
	controller = new InsteonController(this);
	onConnected();
}

void InsteonNetwork::close()
{
	Log::writeLine(Severity_level::info, "Closing INSTEON network...");
	onClosing();
	messenger->close();
	delete messenger;
	delete connection;
	connection = NULL;
	Log::writeLine(Severity_level::info, "INSTEON network closed");
}

void InsteonNetwork::disconnect()
{
	connection = NULL;
	onDisconnected();
}

vector<InsteonConnection*> InsteonNetwork::getAvailableConnections()
{
	vector<InsteonConnection*> list;
	vector<InsteonConnection*> serialConnections = getAvailableSerialConnections();
	list.insert(list.end(), serialConnections.begin(), serialConnections.end());
	return list;
}

vector<InsteonConnection*> InsteonNetwork::getAvailableSerialConnections()
{
	vector<InsteonConnection*> list;
	vector<serial::PortInfo> devices_found = serial::list_ports();

	for (auto it = devices_found.begin(); it != devices_found.end(); ++it)
	{
		serial::PortInfo device = *it;
		list.push_back(new InsteonConnection(InsteonConnectionType::Serial, device.port));
	}

	return list;
}

void InsteonNetwork::onConnected()
{
	connected();
}

void InsteonNetwork::onConnectProgress(int progressPercentage, string status)
{
	connectProgress(progressPercentage, status);
}

void InsteonNetwork::onDisconnected()
{
	disconnected();
}

void InsteonNetwork::onClosing()
{
	closing();
}

void InsteonNetwork::initLog()
{
	Log::open();
}

bool InsteonNetwork::tryConnect()
{
	vector<InsteonConnection*> connections = getAvailableConnections();
	if (connections.size() <= 0)
		return false;

	for (auto it=connections.begin(); it!=connections.end(); ++it)
		Log::writeLine(Severity_level::info, "Available connection '" + (*it)->toString() + "'");

	bool result = tryConnect(connections);

	if(result)
	{
		for (auto it=connections.begin(); it!=connections.end(); ++it)
		{
			if(connection != (*it))
				delete (*it);
		}
	}
	return result;
}

bool InsteonNetwork::tryConnect(InsteonConnection *connect)
{
	if (!messenger->tryConnect(connect))
		return false;

	connection = connect;
	controller = new InsteonController(this);
	onConnected();

	return true;
}

bool InsteonNetwork::tryConnect(vector<InsteonConnection*> connections)
{
	if (connections.size() > 0)
	{
		for (auto it=connections.begin(); it!=connections.end(); ++it)
		{
			stringstream ss;
			ss << "Trying connection " + to_string((it - connections.begin()) + 1)  + " of " + to_string(connections.size()) + " on " + (*it)->getValue();
			if ((*it)->getName() != (*it)->getValue())
				ss << " '" + (*it)->getName() + "'";
			if (!(*it)->getAddress()->isEmpty())
				ss << "  (" + (*it)->getAddress()->toString() + ")";

			onConnectProgress(50 * (it - connections.begin()) / connections.size() + 50, ss.str()); // 50% to 100% progress

			if (tryConnect(*it))
				return true;
		}
	}
	return false;
}

bool InsteonNetwork::verifyConnection()
{
	return messenger->verifyConnection();
}
