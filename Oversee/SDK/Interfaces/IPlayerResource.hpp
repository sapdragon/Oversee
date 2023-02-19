#pragma once
#include "Panorama/IPanorama.hpp"

#define BASIC_OFFSET( name, offset, type) \
__forceinline type& name( int nEntIndex ) \
{\
    return *( type* )( ( DWORD )( this ) + offset + ( nEntIndex * 4 ) ); \
}\
class Vector;
class IPlayerResource
{
public:
    BASIC_OFFSET(m_nPersonaDataPublicLevel, 0x4DD4, int);
    BASIC_OFFSET(m_iCompetitiveWins, 0x1B88, int);
    BASIC_OFFSET(m_iHostageEntityIDs, 0x1694, int);
    BASIC_OFFSET(m_iScore, 0x1980, int);
    BASIC_OFFSET(m_iPlayerC4, 0x165C, int);
    BASIC_OFFSET(m_bHasCommunicationAbuseMute, 0x51E4, bool);
    BASIC_OFFSET(m_iPlayerVIP, 0x1660, int);
    BASIC_OFFSET(m_iMatchStats_LiveTime_Total, 0x642C, int);
    BASIC_OFFSET(m_iTotalCashSpent, 0x48C0, int);
    BASIC_OFFSET(m_vecBombsiteCenterB, 0x1670, Vector);
    BASIC_OFFSET(m_bHostageAlive, 0x167C, bool);
    BASIC_OFFSET(m_isHostageFollowingSomeone, 0x1688, bool);
    BASIC_OFFSET(m_vecBombsiteCenterA, 0x1664, Vector);
    BASIC_OFFSET(m_bHasHelmet, 0x1839, bool);
    BASIC_OFFSET(m_iMatchStats_Deaths_Total, 0x5D10, int);
    BASIC_OFFSET(m_bControllingBot, 0x1FDC, bool);
    BASIC_OFFSET(m_iMVPs, 0x16F4, int);
    BASIC_OFFSET(m_iArmor, 0x187C, int);
    BASIC_OFFSET(m_bHasDefuser, 0x17F8, bool);
    BASIC_OFFSET(m_iMatchStats_KillReward_Total, 0x6328, int);
    BASIC_OFFSET(m_iCompetitiveRanking, 0x1A84, int);
    BASIC_OFFSET(m_iMatchStats_Objective_Total, 0x6634, int);
    BASIC_OFFSET(m_nCharacterDefIndex, 0x47BC, int);
    BASIC_OFFSET(m_iCompetitiveRankType, 0x1C8C, int);
    BASIC_OFFSET(m_szCrosshairCodes, 0x5225, const char*);
    BASIC_OFFSET(m_iCompTeammateColor, 0x1CD0, int);
    BASIC_OFFSET(m_iLifetimeStart, 0x1DD4, int);
    BASIC_OFFSET(m_iLifetimeEnd, 0x1ED8, int);
    BASIC_OFFSET(m_iControlledPlayer, 0x2020, int);
    BASIC_OFFSET(m_iControlledByPlayer, 0x2124, int);
    BASIC_OFFSET(m_szClan, 0x43AC, const char*);
    BASIC_OFFSET(m_iBotDifficulty, 0x42A8, int);
    BASIC_OFFSET(m_iGunGameLevel, 0x49C4, int);
    BASIC_OFFSET(m_iMatchStats_EnemiesFlashed_Total, 0x6940, int);
    BASIC_OFFSET(m_iCashSpentThisRound, 0x4AC8, int);
    BASIC_OFFSET(m_nEndMatchNextMapVotes, 0x6A88, int);
    BASIC_OFFSET(m_bEndMatchNextMapAllVoted, 0x6B8C, bool);
    BASIC_OFFSET(m_nActiveCoinRank, 0x4BCC, int);
    BASIC_OFFSET(m_nPersonaDataPublicCommendsLeader, 0x4ED8, int);
    BASIC_OFFSET(m_nMusicID, 0x4CD0, int);
    BASIC_OFFSET(m_nPersonaDataPublicCommendsTeacher, 0x4FDC, int);
    BASIC_OFFSET(m_nPersonaDataPublicCommendsFriendly, 0x50E0, int);
    BASIC_OFFSET(m_iMatchStats_Kills_Total, 0x5B08, int);
    BASIC_OFFSET(m_iMatchStats_5k_Total, 0x601C, int);
    BASIC_OFFSET(m_iMatchStats_4k_Total, 0x5F18, int);
    BASIC_OFFSET(m_iMatchStats_3k_Total, 0x5E14, int);
    BASIC_OFFSET(m_iMatchStats_Damage_Total, 0x6120, int);
    BASIC_OFFSET(m_iMatchStats_EquipmentValue_Total, 0x6224, int);
    BASIC_OFFSET(m_iMatchStats_Assists_Total, 0x5C0C, int);
    BASIC_OFFSET(m_iMatchStats_HeadShotKills_Total, 0x6530, int);
    BASIC_OFFSET(m_iMatchStats_CashEarned_Total, 0x6738, int);
    BASIC_OFFSET(m_iMatchStats_UtilityDamage_Total, 0x683C, int);
};