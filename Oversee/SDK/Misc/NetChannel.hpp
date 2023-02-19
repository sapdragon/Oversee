#pragma once

#define FLOW_OUTGOING	0
#define FLOW_INCOMING	1
#define MAX_FLOWS		2		
#include "NetMessage.hpp"

class INetChannel
{
public:
	enum
	{
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		TOTAL,			// must be last and is not a real group
	};

	char pad_0000[ 16 ];           //0x0000
	bool m_bProcessingMessages;  //0x0014
	bool m_bShouldDelete;        //0x0015
	char pad_0016[ 2 ];            //0x0016
	int m_iOutSequenceNr;    //0x0018 last send outgoing sequence number
	int m_iInSequenceNr;     //0x001C last received incoming sequnec number
	int m_iOutSequenceNrAck; //0x0020 last received acknowledge outgoing sequnce number
	int m_iOutReliableState; //0x0024 state of outgoing reliable data (0/1) flip flop used for loss detection
	int m_iInReliableState;  //0x0028 state of incoming reliable data
	int m_iChokedCommands;   //0x002C number of choked packets
	char pad_0030[ 1044 ];         //0x0030

	virtual const char* GetName( void ) const = 0;	// get channel name
	virtual const char* GetAddress( void ) const = 0; // get channel IP address as string
	virtual float		GetTime( void ) const = 0;	// current net time
	virtual float		GetTimeConnected( void ) const = 0;	// get connection time in seconds
	virtual int			GetBufferSize( void ) const = 0;	// netchannel packet history size
	virtual int			GetDataRate( void ) const = 0; // send data rate in byte/sec
	virtual bool		IsLoopback( void ) const = 0;	// true if loopback channel
	virtual bool		IsTimingOut( void ) const = 0;	// true if timing out
	virtual bool		IsPlayback( void ) const = 0;	// true if demo playback
	virtual float		GetLatency( int flow ) const = 0;	 // current latency (RTT), more accurate but jittering
	virtual float		GetAvgLatency( int flow ) const = 0; // average packet latency in seconds
	virtual float		GetAvgLoss( int flow ) const = 0;	 // avg packet loss[0..1]
	virtual float		GetAvgChoke( int flow ) const = 0;	 // avg packet choke[0..1]
	virtual float		GetAvgData( int flow ) const = 0;	 // data flow in bytes/sec
	virtual float		GetAvgPackets( int flow ) const = 0; // avg packets/sec
	virtual int			GetTotalData( int flow ) const = 0;	 // total flow in/out in bytes
	virtual int			GetSequenceNr( int flow ) const = 0;	// last send seq number
	virtual bool		IsValidPacket( int flow, int frame_number ) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime( int flow, int frame_number ) const = 0; // time when packet was send
	virtual int			GetPacketBytes( int flow, int frame_number, int group ) const = 0; // group size of this packet
	virtual bool		GetStreamProgress( int flow, int* received, int* total ) const = 0;  // TCP progress if transmitting
	virtual float		GetTimeSinceLastReceived( void ) const = 0;	// get time since last recieved packet in seconds
	virtual	float		GetCommandInterpolationAmount( int flow, int frame_number ) const = 0;
	virtual void		GetPacketResponseLatency( int flow, int frame_number, int* pnLatencyMsecs, int* pnChoke ) const = 0;
	virtual void		GetRemoteFramerate( float* pflFrameTime, float* pflFrameTimeStdDeviation ) const = 0;
	virtual float		GetTimeoutSeconds( ) const = 0;

	VFunc(SendNetMsg(INetMessage* pMsg, bool bForceReliable, bool bVoice = false), 40, bool(__thiscall*)(void*, INetMessage*, bool, bool), pMsg, bForceReliable, bVoice);
	VFunc( SendDatagram( LPVOID Data ), 46, int( __thiscall* )( LPVOID, LPVOID ), Data );
};
