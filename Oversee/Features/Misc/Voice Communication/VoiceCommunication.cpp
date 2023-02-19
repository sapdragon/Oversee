#include "VoiceCommunication.hpp"
#include "../../Game/Dormant/DormantSystem.hpp"
#include "../../Visuals/Chams/Chams.hpp"

void C_VoiceCommunication::SendDataMsg( C_VoiceCommunicationData* pData )
{
	// Creating message
	C_CLCMsg_VoiceData msg;
	memset( &msg, 0, sizeof( msg ) );

	const auto ConstructVoiceDataMessageFn = ( uint32_t( __fastcall* )( void*, void* ) ) SDK::EngineData::m_AddressList[ CheatAddressList::ConstructVoiceMessage ];
	ConstructVoiceDataMessageFn( ( void* ) &msg, nullptr );

	// Setup custom data
	msg.SetData( pData );

	// :D
	CommunicationString_t CommunicationString { };

	// Setup voice message
	msg.pData = &CommunicationString; // Its mad code
	msg.m_nFormat = 0; // VoiceFormat_Steam
	msg.m_nFlags = 63; // All flags

	SDK::Interfaces::EngineClient->GetNetChannel( )->SendNetMsg( ( INetMessage* ) &msg, false, true );
}
void C_VoiceCommunication::SendSharedESPData( VoiceTransferPacketFlags nType, C_SharedESPData pData )
{
	if ( SDK::Interfaces::EngineClient->GetNetChannel( ) )
	{
		// Filling packet
		C_VoiceCommunicationData data;
		memcpy( data.GetRawData( ), &pData, sizeof( pData ) );
		SendDataMsg( &data );
	}
}
void C_VoiceCommunication::RunSharedChamsData( )
{
	if ( !g_SettingsManager->B[ _S( "misc.shared_chams" ) ] )
		return;

	/* skip local servers */
	INetChannel* m_NetChannel = SDK::Interfaces::EngineClient->GetNetChannel( );
	if ( !m_NetChannel || m_NetChannel->IsLoopback( ) )
		return;

	if ( SDK::Interfaces::EngineClient->GetNetChannel( ) )
	{
		C_SharedChamsData Data { };

		strcpy( Data.m_szID, _S( "OFC" ) );

		ChamsSettings_t Real = g_SettingsManager->GetChamsSettings( _S( "real" ) );

		Data.m_iMainMaterial = Real.m_iFirstChamsLayerStyle;
		Color cMainColor = Real.m_Color;
		Data.m_iMainMaterialColor = ( ( ( cMainColor.r( ) * 255 ) & 0x0ff ) << 16 ) | ( ( ( cMainColor.g( ) * 255 ) & 0x0ff ) << 8 ) | ( ( cMainColor.b( ) * 255 ) & 0x0ff );
		Data.m_iLayerColor[ 0 ] = ( ( Real.m_aMaterialsColor[ 1 ].r( ) & 0x0ff ) << 16 ) | ( ( Real.m_aMaterialsColor[ 1 ].g( ) & 0x0ff ) << 8 ) | ( Real.m_aMaterialsColor[ 1 ].b( ) & 0x0ff );
		Data.m_iLayerColor[ 1 ] = ( ( Real.m_aMaterialsColor[ 2 ].r( ) & 0x0ff ) << 16 ) | ( ( Real.m_aMaterialsColor[ 2 ].g( ) & 0x0ff ) << 8 ) | ( Real.m_aMaterialsColor[ 2 ].b( ) & 0x0ff );
		Data.m_iLayerColor[ 2 ] = ( ( Real.m_aMaterialsColor[ 2 ].r( ) & 0x0ff ) << 16 ) | ( ( Real.m_aMaterialsColor[ 3 ].g( ) & 0x0ff ) << 8 ) | ( Real.m_aMaterialsColor[ 3 ].b( ) & 0x0ff );

		Data.m_bLayerEnabled[ 0 ] = Real.m_aMaterialParams[ 1 ];
		Data.m_bLayerEnabled[ 1 ] = Real.m_aMaterialParams[ 2 ];
		Data.m_bLayerEnabled[ 2 ] = Real.m_aMaterialParams[ 3 ];

		// Filling packet
		C_VoiceCommunicationData data;
		memcpy( data.GetRawData( ), &Data, sizeof( C_SharedChamsData ) );
		SendDataMsg( &data );
	}

	return;
}
int C_VoiceCommunication::SendVoiceTransferData( VoiceTransferPacketFlags nType, void* pData, size_t nDataBytes )
{
	// Check if not many bytes
	if ( nDataBytes > MAX_VOICETRANSFER_PACKET_DATA )
		return -1;

	// Filling packet data
	VoiceTransferDataTransmit_t Packet;
	memset( &Packet, 0, sizeof( VoiceTransferDataTransmit_t ) );
	strcpy( Packet.m_szID, _S( "OVS" ) ); // 
	memcpy( Packet.m_szDataPacket, pData, nDataBytes );
	Packet.m_nDataType |= nType;

	// filling raw
	C_VoiceCommunicationData data;
	memcpy( data.GetRawData( ), &Packet, sizeof( Packet ) );
	// send voice message
	SendDataMsg( &data );
	return 0;
}
int C_VoiceCommunication::SendSplitPacketMessage( VoiceTransferPacketFlags nType, void* pData, size_t nDataBytes )
{
	if ( nDataBytes > MAX_VOICETRANSFER_NETWORKABLE )
		return 0;

	char* buffer[ MAX_VOICETRANSFER_PACKET_DATA ];
	memset( buffer, 0, MAX_VOICETRANSFER_PACKET_DATA );
	VoiceTransferSplitPacketHeader_t* pSplitPacket = reinterpret_cast< VoiceTransferSplitPacketHeader_t* >( buffer );

	// Handshake with the first package @ SapDragon
	pSplitPacket->nCount = 0;
	VoiceTransferFirstSplitPacket_t InitializeTransfer;
	InitializeTransfer.nSizeOfSplit = ( uint8_t ) nDataBytes; // Let them know how much we are sending out 
	memcpy( pSplitPacket->pDataPacket, &InitializeTransfer, sizeof( VoiceTransferFirstSplitPacket_t ) ); // Copy over the Transfer Initializer
	SendVoiceTransferData( VoiceTransferPacketFlags::SplitPacket, pSplitPacket, MAX_VOICETRANSFER_PACKET_DATA ); // Send that out

	int nDataBytesSent = 0;
	int nDataBytesLeft = nDataBytes;
	for ( int i = 0; i < MAX_VOICETRANSFER_SPLITPACKET; i++ ) { // Now we Send out the split packets
		pSplitPacket->nCount++;
		memset( pSplitPacket->pDataPacket, 0, MAX_VOICETRANSFER_PACKET_DATA );	// Ensure Data is Zero'd
		int nBytes = min( sizeof( MAX_VOICETRANSFER_SPLITPACKET_DATA ), nDataBytesLeft );
		memcpy( pSplitPacket->pDataPacket, &pData + nBytes, MAX_VOICETRANSFER_SPLITPACKET_DATA ); // Copy Data Chunck
		SendVoiceTransferData( VoiceTransferPacketFlags::SplitPacket, pSplitPacket, nBytes ); // Send Data Chunk
		nDataBytesLeft -= nBytes;
		nDataBytesSent += nBytes;
	}
	return nDataBytesSent;
}

#ifdef OVERSEE_DEV
std::string hexDump2( const char* desc, void* addr, int len )
{
	if ( len <= 0 )
		return std::string( "NOT VALID SIZE" );

	char* buffer = ( char* ) malloc( ( ( len * 20 ) * sizeof( char ) ) );
	// DUMB ASS WAY TO DO THIS!!!

	if ( !buffer )
		return std::string( "BUFFER FAILED TO ALLOC" );

	int i;
	unsigned char buff[ 17 ];
	unsigned char* pc = ( unsigned char* ) addr;

	char tempBuf[ 4096 ];

	// Output description if given.
	if ( desc != NULL ) {
		snprintf( tempBuf, 4096, "%s:\n", desc );
		strcat( buffer, tempBuf );
	}

	// Process every byte in the data.
	for ( i = 0; i < len; i++ ) {
		// Multiple of 16 means new line (with line offset).

		if ( ( i % 16 ) == 0 ) {
			// Just don't print ASCII for the zeroth line.
			if ( i != 0 ) {
				snprintf( tempBuf, 4096, "  %s\n", buff );
				strcat( buffer, tempBuf );
			}

			// Output the offset.
			snprintf( tempBuf, 4096, "  %04x ", i );
			strcat( buffer, tempBuf );
		}

		// Now the hex code for the specific character.
		snprintf( tempBuf, 4096, " %02x", pc[ i ] );
		strcat( buffer, tempBuf );

		// And store a printable ASCII character for later.
		if ( ( pc[ i ] < 0x20 ) || ( pc[ i ] > 0x7e ) ) {
			buff[ i % 16 ] = '.';
		}
		else {
			buff[ i % 16 ] = pc[ i ];
		}

		buff[ ( i % 16 ) + 1 ] = '\0';
	}

	// Pad out last line if not exactly 16 characters.
	while ( ( i % 16 ) != 0 ) {
		snprintf( tempBuf, 4096, "   " );
		strcat( buffer, tempBuf );
		i++;
	}

	// And print the final ASCII bit.
	snprintf( tempBuf, 4096, "  %s\n", buff );
	strcat( buffer, tempBuf );

	std::string retStr { buffer };
	free( buffer );
	return retStr;
}
#endif

void C_VoiceCommunication::RunSharedESP( )
{
	if ( !g_SettingsManager->B[ _S( "misc.shared_esp" ) ] )
		return;

	/* skip local servers */
	INetChannel* m_NetChannel = SDK::Interfaces::EngineClient->GetNetChannel( );
	if ( !m_NetChannel || m_NetChannel->IsLoopback( ) )
		return;

	/* build message */
	C_SharedESPData Data;
	strcpy( Data.m_szID, _S( "SEE" ) );

	/* iterate all players */
	for ( int nPlayer = 1; nPlayer <= SDK::Interfaces::ClientState->m_nMaxClients; nPlayer++ )
	{
		/* get player */
		C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( nPlayer );
		if ( !pPlayer || !pPlayer->IsPlayer( ) || !pPlayer->IsAlive( ) || pPlayer->IsDormant( ) || pPlayer == g_Globals->m_LocalPlayer )
			continue;

		/* build data */
		Data.m_nUserID = nPlayer;
		Data.m_nOriginX = pPlayer->m_vecOrigin( ).x;
		Data.m_nOriginY = pPlayer->m_vecOrigin( ).y;
		Data.m_nOriginZ = pPlayer->m_vecOrigin( ).z;
		Data.m_nHealth = pPlayer->m_iHealth( );

		/* send data */
		g_VoiceCommunication->SendSharedESPData( ReliableSend, Data );
	}
}
void C_VoiceCommunication::OnVoiceDataReceived( C_SVCMsg_VoiceData* pMsg )
{
	/* check shared esp */
	if ( !g_SettingsManager->B[ _S( "misc.shared_esp" ) ] && !g_SettingsManager->B[ _S( "misc.shared_chams" ) ] )
		return;

	/* check format */
	C_VoiceCommunicationData VoiceData = pMsg->GetData( );
	if ( pMsg->m_nFormat != 0 )
		return;

	/* check localplayer, do not process OUR own packet */
	if ( !g_Globals->m_LocalPlayer || g_Globals->m_LocalPlayer->EntIndex( ) == pMsg->m_iClient + 1 )
		return;

	/* packet must not be empty */
	if ( !VoiceData.m_nSectionNumber && !VoiceData.m_nSequenceBytes && !VoiceData.m_nUnCompressedSampleOffset )
		return;

	/* get ESP Data */
	C_SharedESPData Data = *( C_SharedESPData* ) ( VoiceData.GetRawData( ) );
	C_SharedChamsData pChamsData = *( C_SharedChamsData* ) ( VoiceData.GetRawData( ) );

	/* is it ESP from our cheat? */
	if ( strstr( Data.m_szID, _S( "SEE" ) ) )
		g_DormantSystem->OnSharedESPReceived( Data );
	else if ( strstr( pChamsData.m_szID, _S( "OFC" ) ) )
		g_ChamsSystem->OnSharedChamsDataReceived( pChamsData, pMsg->m_iClient + 1 );
}