/*
 * InsteonNetwork.h
 *
 *  Created on: 2014-11-11
 *      Author: etienne
 */

#ifndef INSTEONNETWORK_H_
#define INSTEONNETWORK_H_

#include "InsteonConnection.h"
#include "InsteonMessenger.h"
#include "InsteonController.h"
#include "InsteonDeviceList.h"
#include <boost/signals2.hpp>
#include <list>

using namespace std;

class InsteonMessenger;
class InsteonController;
class InsteonDeviceList;

// Represents the top level INSTEON network, including the set of known INSTEON devices and the controller device.
class InsteonNetwork
{
public:
	boost::signals2::signal<void ()> connected; // Invoked when a connection to an INSTEON network is established.
	boost::signals2::signal<void (int progressPercentage, string status)> connectProgress; // Communicates progress status during the sometimes lengthy process of connecting to a network.
	boost::signals2::signal<void ()> closing; // Invoked when the INSTEON network is shutting down.
	boost::signals2::signal<void ()> disconnected; // Invoked when the connection to an INSTEON network is interrupted.

	bool autoAdd = false; // Determines whether devices are automatically added to the device collection when a message is received from a device not already in the device collection.

	InsteonNetwork();
	~InsteonNetwork();

	/**
	 * Connects to an INSTEON network using the specified connection.
	 * <param connection> Specifies the connection to the INSTEON powerline controller device, which can accessed serially or over the network. Examples: "serial: COM1" or "net: 192.168.2.5".
	 */
	void connect(InsteonConnection *connection);

	/**
	 * Disconnects from the active INSTEON network and closes all open connections.
	 */
	void close();


	void disconnect();

	void onDisconnected();


	/**
	 * Returns the available network and serial connections.
	 */
	vector<InsteonConnection*> getAvailableConnections();

	/**
	 * Returns the available serial connections.
	 * \Returns an array of objects representing each available serial connection.
	 */
	vector<InsteonConnection*> getAvailableSerialConnections();

	/**
	 * Sets the folder path where INSTEON log files will be written.
	 */
	static void initLog();

	/**
	 * Attempts to locate an INSTEON controller
	 * The first successful connection will be returned.
	 * \ Returns true if a connection was successfully made, or false if unable to find a connection.
	 */
	bool tryConnect();

	/**
	 * Connects to an INSTEON network using the specified connection.
	 * <param connection>Specifies the connection to the INSTEON controller device, which can accessed serially or over the network. Examples: "serial: COM1" or "net: 192.168.2.5".
	 * \ Returns true if a connection was successfully made, or false if unable to find a connection.
	 */
	bool tryConnect(InsteonConnection *connection);

	/**
	 * Attempts to connect to an INSTEON network by trying each specified connection. The first successful connection will be returned.
	 * <param connections>Specifies the list of connections.
	 * \Returns true if a connection was successfully made, or false if unable to find a connection.
	 */
	bool tryConnect(vector<InsteonConnection*> connections);

	/**
	 * Verifies that the connection to the INSTEON network is active.
	 * \Returns true if the connection is verified.
	 */
	bool verifyConnection();

	bool isConnected() { return connection != NULL; }
	InsteonMessenger* getMessenger() { return messenger; }
	InsteonConnection* getConnection() { return connection; }
	InsteonDeviceList* getDevices() { return devices; }
	InsteonController* getController() { return controller; }

private:
	InsteonMessenger *messenger;
	InsteonDeviceList *devices; // A collection of known INSTEON devices linked to the network.
	InsteonController *controller; // The INSTEON controller device which interfaces to the various other INSTEON devices on the network.
	InsteonConnection *connection = NULL;


	void onConnected();
	void onConnectProgress(int progressPercentage, string status);
	void onClosing();
};

#endif /* INSTEONNETWORK_H_ */
