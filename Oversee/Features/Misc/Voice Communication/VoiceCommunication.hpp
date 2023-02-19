#pragma once
#include "../../../SDK/SDK.hpp"

// Max Possible Splitpacket Count (-1 as it sets up the transfer)
#define MAX_VOICETRANSFER_SPLITPACKET 255
// Max Possible Data Per 1 voicepacket
#define MAX_VOICETRANSFER_PACKET_SIZE 24
// Max Possible Data we can send with 1 non-splitpacket packet
#define MAX_VOICETRANSFER_PACKET_DATA (MAX_VOICETRANSFER_PACKET_SIZE - 5)
// Max Possible Data we can send per split packet
#define MAX_VOICETRANSFER_SPLITPACKET_DATA (MAX_VOICETRANSFER_PACKET_DATA - 1)
// Max Possible Amount we can network
#define MAX_VOICETRANSFER_NETWORKABLE (MAX_VOICETRANSFER_SPLITPACKET_DATA * MAX_VOICETRANSFER_SPLITPACKET)


// Some tranfer struct
struct VoiceTransferDataTransmit_t
{
	char m_szID[ 4 ]; // Default: Over
	int8_t m_nDataType;
	char m_szDataPacket[ 19 ];
};

enum VoiceTransferPacketFlags {
	ReliableSend = 0b00000001,
	SplitPacket = 0b00000010,
	ReliableAck = 0b00000100,
};
struct CommunicationString_t

{
	char szData[ 16 ] { };
	uint32_t m_nCurrentLen = 0;
	uint32_t m_nMaxLen = 15;
};


struct VoiceTransferFirstSplitPacket_t {
	uint16_t nSizeOfSplit = 0;
};

struct VoiceTransferSplitPacketHeader_t
{
	uint8_t nCount;
	char pDataPacket[ 19 ];
};

struct C_SharedESPData
{
	char m_szID[ 4 ] { };
	uint8_t m_nUserID;
	int16_t m_nOriginX;
	int16_t m_nOriginY;
	int16_t m_nOriginZ;
	int16_t m_nHealth;
};

// 18 bytes < 19 | Good for voice data transfer
class C_SharedChamsData
{
public:
	char m_szID[4]; // unique ID 
	int8_t m_iMainMaterial; // First material 
	int16_t m_iMainMaterialColor; // BitMapped color 
	bool m_bLayerEnabled[3];
	int16_t m_iLayerColor[3];
};


class C_VoiceCommunication
{
public:
	virtual void SendDataMsg( C_VoiceCommunicationData* pData );
	virtual void SendSharedESPData( VoiceTransferPacketFlags nType, C_SharedESPData Data );
	virtual void RunSharedChamsData( );
	virtual int SendVoiceTransferData( VoiceTransferPacketFlags nType, void* pData, size_t nDataBytes );
	virtual int SendSplitPacketMessage( VoiceTransferPacketFlags nType, void* pData, size_t nDataBytes ); // For very big data  ( Shared Resolver / Shared Skins ) @ SapDragon
	virtual void OnVoiceDataReceived( C_SVCMsg_VoiceData* pMsg );
	virtual void RunSharedESP( );
};

inline C_VoiceCommunication* g_VoiceCommunication = new C_VoiceCommunication( );