/*
 * InsteonController.h
 *
 *  Created on: 2014-11-13
 *      Author: etienne
 */

#ifndef INSTEONCONTROLLER_H_
#define INSTEONCONTROLLER_H_

#include "Util/Timer.h"
#include "InsteonNetwork.h"
#include "InsteonAddress.h"
#include "InsteonDevice.h"
#include "InsteonIdentity.h"
#include "InsteonDeviceLinkRecord.h"
#include "Defs.h"
#include <list>
#include <boost/signals2.hpp>
#include <boost/asio.hpp>

class InsteonNetwork;
class InsteonDevice;
class InsteonController
{
public:
	InsteonController(InsteonNetwork *network);
	~InsteonController();

	boost::signals2::signal<void (InsteonDevice *insteonDevice)> deviceLinked;
	boost::signals2::signal<void ()> deviceLinkTimeout;
	boost::signals2::signal<void (InsteonDevice *insteonDevice)> deviceUnlinked;

	/**
	 * Cancels linking mode in the controller.
	 */
	void cancelLinkMode();

	/**
	 * Places the INSTEON controller into linking mode in order to link or unlink a device.
	 * <param mode> Determines the linking mode for the controller.
	 * <param group> Specifies the INSTEON group number to which the device will be linked.
	 */
	void enterLinkMode(InsteonLinkMode mode, uint8 group);

	/**
	 * Returns an array of device links in the INSTEON controller.
	 */
	vector<InsteonDeviceLinkRecord> getLinks();

	/**
	 * Sends an INSTEON group broadcast command to the controller.
	 * <param command> Specifies the INSTEON controller group command to be invoked.
	 * <param group> Specifies the group number for the command.
	 */
	void groupCommand(InsteonControllerGroupCommands command, uint8 group);

	/**
	 * Sends an INSTEON group broadcast command to the controller.
	 * <param command> Specifies the INSTEON controller group command to be invoked.
	 * <param group> Specifies the group number for the command.
	 * <param value> A parameter value required by some group commands.
	 */
	void groupCommand(InsteonControllerGroupCommands command, uint8 group, uint8 value);

	/**
	 * Cancels linking mode in the controller.
	 */
	bool tryCancelLinkMode();

	/**
	 * Places the INSTEON controller into linking mode in order to link or unlink a device.
	 * <param mode> Determines the linking mode as controller, responder, either, or delete.
	 * <param group> Specifies the INSTEON group number to which the device will be linked.
	 */
	bool tryEnterLinkMode(InsteonLinkMode mode, uint8 group);

	/**
	 * Returns an array of device links in the INSTEON controller.
	 * <param links> An array of objects representing each device link.
	 * \return true if get links
	 */
	bool tryGetLinks(vector<InsteonDeviceLinkRecord> &links);

	/**
	 * Sends an INSTEON group broadcast command to the controller.
	 * <param command> Specifies the INSTEON controller group command to be invoked.
	 * <param group> Specifies the group number for the command.
	 * \return true if command group work
	 */
	bool tryGroupCommand(InsteonControllerGroupCommands command, uint8 group);

	/**
	 * Sends an INSTEON group broadcast command to the controller.
	 * <param command> Specifies the INSTEON controller group command to be invoked.
	 * <param group> Specifies the group number for the command.
	 * <param value> A parameter value required by some group commands.
	 * \return true if command group work
	 */
	bool tryGroupCommand(InsteonControllerGroupCommands command, uint8 group, uint8 value);

	InsteonAddress* getAddress() { return address; }
	InsteonIdentity* getIdentity() { return identity; }
	bool getIsInLinkingMode() { return isInLinkingMode; }
	void onMessage(InsteonMessage *message);

private:
	InsteonNetwork *network;
	InsteonAddress *address; // The INSTEON address of the controller device.
	InsteonIdentity *identity; // Indicates the type of the INSTEON device.
	Timer *timer;

	InsteonLinkMode linkingMode;
	bool isInLinkingMode = false;

	bool tryGetLinksInternal(vector<InsteonDeviceLinkRecord> &links);
	void timer_Elapsed(boost::system::error_code err, Timer& t);

	void onDeviceLinked(InsteonDevice *device);
	void onDeviceLinkTimeout();
	void onDeviceUnlinked(InsteonDevice *device);
};

#endif /* INSTEONCONTROLLER_H_ */
