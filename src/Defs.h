/*
 * Defs.h
 *
 *  Created on: 2014-10-21
 *      Author: Etienne Dupéré Larivière
 */

#ifndef DEFS_H_INCLUDED
#define DEFS_H_INCLUDED

#include "Util/EnumString.h"
										// note: all times are in milliseconds
static const int deviceAckTimeout       = 4000;  // timeout to receive expected ACK reply from a device
static const int deviceCommandRetries   = 3;     // number of times to retry sending a device command (if no ACK was received from the device within deviceAckTimeout time)
static const int deviceCommandWaitTime  = 200;   // minimum time between to wait between sending two different device commands
static const int duplicateInterval      = 1000;  // interval within which to treat multiple matching messages received from a device as duplicates, where matching is defined as all message bytes are the same except for the hop-count bits
static const int echoTimeout            = 1000;  // time to wait for echo response after sending data
static const int negotiateRetries       = 5;     // number of times to retry negotiating with a serial device
static const unsigned int openTimeout   = 1000;  // time to wait for initial response after opening port
static const int readDataRetries        = 5;     // number of times to retry reading data from the serial port
static const int readDataRetryTime      = 5;     // time to wait between retries when reading data from the serial port
static const int readDataTimeout        = 1000;  // maximum amount of time to wait for additional data when reading data from the serial port
static const int sendMessageRetries     = 5;     // number of times to retry sending a message
static const int sendMessageWaitTime    = 100;   // amount of time to wait after failing to send a message before retrying, note time is multiplied on each retry 1x, 2x, 3x, ... up to sendMessageRetries times
static const int sendReceiveTimeout     = 2000;  // timeout to receive expected reply from a blocking send/receive message such as a Controller.GetLinks or a Device.GetOnLevel
static const int webRequestTimeout      = 5000;  // timeout for web requests to smartlinc.smarthome.com and for accessing SmartLinc devices over the local network

// Basic types
typedef signed char			int8;
typedef unsigned char		uint8;

typedef signed short		int16;
typedef unsigned short		uint16;

typedef signed int			int32;
typedef unsigned int		uint32;

typedef signed long long	int64;
typedef unsigned long long  uint64;

typedef float				float32;
typedef double				float64;

enum InsteonDeviceCommands
{
   DeviceCommandNull = -1,
   EnterLinkingMode = 0x09,
   EnterUnlinkingMode = 0x0A,
   IDRequest = 0x10,
   DeviceOn = 0x11,
   DeviceFastOn = 0x12,
   DeviceOff = 0x13,
   DeviceFastOff = 0x14,
   DeviceBrighten = 0x15,
   DeviceDim = 0x16,
   DeviceStartDimming = 0x17,
   DeviceStopDimming = 0x18,
   StatusRequest = 0x19
};

// String support for InsteonDeviceCommands
Begin_Enum_String(InsteonDeviceCommands)
{
	Enum_String( DeviceCommandNull );
	Enum_String( EnterLinkingMode );
	Enum_String( EnterUnlinkingMode );
	Enum_String( IDRequest );
	Enum_String( DeviceOn );
	Enum_String( DeviceFastOn );
	Enum_String( DeviceOff );
	Enum_String( DeviceFastOff );
	Enum_String( DeviceBrighten );
	Enum_String( DeviceDim );
	Enum_String( DeviceStartDimming );
	Enum_String( DeviceStopDimming );
	Enum_String( StatusRequest );
}
End_Enum_String;

InsteonDeviceCommands const deviceCommandsValues[] = { DeviceCommandNull, EnterLinkingMode, EnterUnlinkingMode, IDRequest, DeviceOn,
		DeviceFastOn, DeviceOff, DeviceFastOff, DeviceBrighten, DeviceDim, DeviceStartDimming, DeviceStopDimming, StatusRequest };

// Represents the various device states for an INSTEON device.
enum InsteonDeviceStatus
{
	DeviceStatusUnknown = 0,
	DeviceStatusOn = 1,
	DeviceStatusOff = 2,
	DeviceStatusFastOn = 3,
	DeviceStatusFastOff = 4,
	DeviceStatusBrighten = 5,
	DeviceStatusDim = 6
};

// String support for InsteonDeviceStatus
Begin_Enum_String(InsteonDeviceStatus)
{
	Enum_String( DeviceStatusUnknown );
	Enum_String( DeviceStatusOn );
	Enum_String( DeviceStatusOff );
	Enum_String( DeviceStatusFastOn );
	Enum_String( DeviceStatusFastOff );
	Enum_String( DeviceStatusBrighten );
	Enum_String( DeviceStatusDim );
}
End_Enum_String;

// Represents the type of link record.
enum InsteonDeviceLinkRecordType
{
	 Empty = 0,
	 DeviceResponder,
	 DeviceController
};

// String support for InsteonDeviceLinkRecordType
Begin_Enum_String(InsteonDeviceLinkRecordType)
{
	Enum_String( Empty );
	Enum_String( DeviceResponder );
	Enum_String( DeviceController );
}
End_Enum_String;

// Represents the set of commands that can be sent to all INSTEON devices linked to a controller in the specified group.
enum InsteonControllerGroupCommands
{
	On = 0x11,
	FastOn = 0x12,
	Off = 0x13,
	FastOff = 0x14,
	Brighten = 0x15,
	Dim = 0x16,
	StartDimming = 0x17,
	StopDimming = 0x18
};

// String support for InsteonControllerGroupCommands
Begin_Enum_String(InsteonControllerGroupCommands)
{
	Enum_String( On );
	Enum_String( FastOn );
	Enum_String( Off );
	Enum_String( FastOff );
	Enum_String( Brighten );
	Enum_String( Dim );
	Enum_String( StartDimming );
	Enum_String( StopDimming );
}
End_Enum_String;

// Determines the linking mode for the EnterLinkMode method of the INSTEON controller device.
enum InsteonLinkMode
{
	Null = -1,
	Responder = 0x00,
	Controller = 0x01,
	Either = 0x03,
	Delete = 0xFF
};

// String support for InsteonLinkMode
Begin_Enum_String(InsteonLinkMode)
{
	Enum_String( Null );
	Enum_String( Responder );
	Enum_String( Controller );
	Enum_String( Either );
	Enum_String( Delete );
}
End_Enum_String;

// Represents the status of the serial message echoed from the controller.
enum EchoStatus
{
	None = 0, // No response
	Unknown = 1, // Unknown acknowledgment response (i.e. not a 0x06 or a 0x15)
	ACK = 0x06, // Acknowledge (OK)
	NAK = 0x15 // Negative Acknowledge (ERROR)
};

// String support for EchoStatus
Begin_Enum_String(EchoStatus)
{
	Enum_String( None );
	Enum_String( Unknown );
	Enum_String( ACK );
	Enum_String( NAK );
}
End_Enum_String;

// Represents the type of an INSTEON connection
enum InsteonConnectionType
{
	Net,
	Serial
};

// String support for ConnectionType
Begin_Enum_String(InsteonConnectionType)
{
	Enum_String( Net );
	Enum_String( Serial );
}
End_Enum_String;

// A list of defined keys for the property list.
enum PropertyKey
{
   Address,
   Cmd1,
   Cmd2,
   Data1,
   Data2,
   Data3,
   Data4,
   Data5,
   Data6,
   Data7,
   Data8,
   Data9,
   Data10,
   Data11,
   Data12,
   Data13,
   Data14,
   DevCat,
   FirmwareVersion,
   FromAddress,
   Group,
   IncrementDirection,
   LinkAddress,
   LinkData1,
   LinkData2,
   LinkData3,
   LinkGroup,
   LinkRecordFlags,
   LinkStatus,
   LinkType,
   MessageFlagsMaxHops,
   MessageFlagsRemainingHops,
   MessageFlagsExtendedFlag,
   MessageFlagsAck,
   MessageFlagsAllLink,
   MessageFlagsBroadcast,
   ResponderCmd1,
   ResponderCount,
   ResponderGroup,
   ResponderErrorCount,
   SubCat,
   ToAddress
};

// String support for PropertyKey
Begin_Enum_String(PropertyKey)
{
	Enum_String( Address );
	Enum_String( Cmd1 );
	Enum_String( Cmd2 );
	Enum_String( Data1 );
	Enum_String( Data2 );
	Enum_String( Data3 );
	Enum_String( Data4 );
	Enum_String( Data5 );
	Enum_String( Data6 );
	Enum_String( Data7 );
	Enum_String( Data8 );
	Enum_String( Data9 );
	Enum_String( Data10 );
	Enum_String( Data11 );
	Enum_String( Data12 );
	Enum_String( Data13 );
	Enum_String( Data14 );
	Enum_String( DevCat );
	Enum_String( FirmwareVersion );
	Enum_String( FromAddress );
	Enum_String( Group );
	Enum_String( IncrementDirection );
	Enum_String( LinkAddress );
	Enum_String( LinkData1 );
	Enum_String( LinkData2 );
	Enum_String( LinkData3 );
	Enum_String( LinkGroup );
	Enum_String( LinkRecordFlags );
	Enum_String( LinkStatus );
	Enum_String( LinkType );
	Enum_String( MessageFlagsMaxHops );
	Enum_String( MessageFlagsRemainingHops );
	Enum_String( MessageFlagsExtendedFlag );
	Enum_String( MessageFlagsAck );
	Enum_String( MessageFlagsAllLink );
	Enum_String( MessageFlagsBroadcast );
	Enum_String( ResponderCmd1 );
	Enum_String( ResponderCount );
	Enum_String( ResponderGroup );
	Enum_String( ResponderErrorCount );
	Enum_String( SubCat );
	Enum_String( ToAddress );
}
End_Enum_String;

// Identfies the type of INSTEON message.
enum InsteonMessageType {
   Other = 0,
   Ack,
   DeviceLink,
   DeviceLinkCleanup,
   DeviceLinkRecord,
   FastOffBroadcast,
   FastOffCleanup,
   FastOnBroadcast,
   FastOnCleanup,
   GetIMInfo,
   IncrementBeginBroadcast,
   IncrementEndBroadcast,
   OffBroadcast,
   OffCleanup,
   OnBroadcast,
   OnCleanup,
   SetButtonPressed,
   SuccessBroadcast };

// String support for MessageType
Begin_Enum_String(InsteonMessageType)
{
	Enum_String( Other );
	Enum_String( Ack );
	Enum_String( DeviceLink );
	Enum_String( DeviceLinkCleanup );
	Enum_String( DeviceLinkRecord );
	Enum_String( FastOffBroadcast );
	Enum_String( FastOffCleanup );
	Enum_String( FastOnBroadcast );
	Enum_String( FastOnCleanup );
	Enum_String( GetIMInfo );
	Enum_String( IncrementBeginBroadcast );
	Enum_String( IncrementEndBroadcast );
	Enum_String( OffBroadcast );
	Enum_String( OffCleanup );
	Enum_String( OnBroadcast );
	Enum_String( OnCleanup );
	Enum_String( SetButtonPressed );
	Enum_String( SuccessBroadcast );
}
End_Enum_String;

#endif
