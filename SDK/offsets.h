
struct COffsets
{
	struct
	{
		int m_Local;
		int m_aimPunchAngle;
		int m_viewPunchAngle;
		int m_vecViewOffset;
		int m_nTickBase;
		int m_vecVelocity;
		int m_iHealth;
		int m_lifeState;
		int m_fFlags;
		int m_iObserverMode;
		int m_hObserverTarget;
		int m_hViewModel;
		int m_szLastPlaceName;
		int m_iAccount;
		int m_angRotation1;
        int m_angRotation2;
		int m_flLowerBodyYawTarget;
        int deadflag;
        int m_nHitboxSet;
        int m_dwCrosshairID;
	} DT_BasePlayer;

	struct
	{
        int m_flAnimTime;
        int m_flSimulationTime;
		int m_vecOrigin;
		int m_nRenderMode;
		int m_iTeamNum;
		int m_MoveType;
		int m_Collision;
		int m_bSpotted;
	} DT_BaseEntity;

	struct
	{
		int m_hActiveWeapon;
		int m_hMyWeapons;
        int m_hMyWearables;
	} DT_BaseCombatCharacter;

	struct
	{
		int m_iPing;
		int m_iKills;
		int m_iAssists;
		int m_iDeaths;
		int m_bConnected;
		int m_iTeam;
		int m_iPendingTeam;
		int m_bAlive;
		int m_iHealth;
	} DT_PlayerResource;

	struct
	{
		int m_iPlayerC4;
		int m_iPlayerVIP;
		int m_bHostageAlive;
		int m_isHostageFollowingSomeone;
		int m_iHostageEntityIDs;
		int m_bombsiteCenterB;
		int m_hostageRescueX;
		int m_hostageRescueY;
		int m_hostageRescueZ;
		int m_iMVPs;
		int m_iArmor;
		int m_bHasHelmet;
		int m_bHasDefuser;
		int m_iScore;
		int m_iCompetitiveRanking;
		int m_iCompetitiveWins;
		int m_iCompTeammateColor;
		int m_bControllingBot;
		int m_iControlledPlayer;
		int m_iControlledByPlayer;
		int m_iBotDifficulty;
		int m_szClan;
		int m_iTotalCashSpent;
		int m_iCashSpentThisRound;
		int m_nEndMatchNextMapVotes;
		int m_bEndMatchNextMapAllVoted;
		int m_nActiveCoinRank;
		int m_nMusicID;
		int m_nPersonaDataPublicLevel;
		int m_nPersonaDataPublicCommendsLeader;
		int m_nPersonaDataPublicCommendsTeacher;
		int m_nPersonaDataPublicCommendsFriendly;
	} DT_CSPlayerResource;

	struct
	{
		int m_bBombTicking;
		int m_flC4Blow;
        //float_t m_flTimerLength;
		int m_bBombDefused;
		int m_hBombDefuser;
        int m_flDefuseCountDown;
	} DT_PlantedC4;

	struct
	{
        int m_bIsPlayerGhost;
		int m_iShotsFired;
        int m_angEyeAngles[2];
		int m_ArmorValue;
		int m_bHasDefuser;
		int m_bIsDefusing;
		int m_bIsGrabbingHostage;
		int m_bIsScoped;
		int m_bGunGameImmunity;
		int m_bIsRescuing;
		int m_bHasHelmet;
		int m_flFlashDuration;
		int m_flFlashMaxAlpha;
        int m_nTickBase;
	} DT_CSPlayer;

	struct
	{
		short m_iItemDefinitionIndex;
		int m_iItemIDHigh;
		int m_iAccountID;
		int m_iEntityQuality;
		int m_szCustomName;
		int m_nFallbackPaintKit;
		int m_nFallbackSeed;
		int m_flFallbackWear;
		int m_nFallbackStatTrak;
        int m_OriginalOwnerXuidLow;
        int m_OriginalOwnerXuidHigh;
	} DT_BaseAttributableItem;

	struct
	{
		int m_nModelIndex;
		int m_hWeapon;
		int m_hOwner;
	} DT_BaseViewModel;

	struct
	{
		int m_fAccuracyPenalty;
	} DT_WeaponCSBase;

	struct
	{
		int m_bStartedArming;
	} DT_WeaponC4;

	struct
	{
		int m_flNextPrimaryAttack;
        int m_flPostponeFireReadyTime;
		int m_hOwner;
		int m_iClip1;
		int m_bInReload;
	} DT_BaseCombatWeapon;
    struct
    {
        std::ptrdiff_t m_bRedraw;
        std::ptrdiff_t m_bIsHeldByPlayer;
        std::ptrdiff_t m_bPinPulled;
        std::ptrdiff_t m_fThrowTime;
        std::ptrdiff_t m_bLoopingSoundPlaying;
        std::ptrdiff_t m_flThrowStrength;
    } DT_BaseCSGrenade;

	struct
	{
		int m_bShouldGlow;
	} DT_DynamicProp;
};

namespace Offsets
{
	void getOffsets();
    extern uintptr_t playerAnimStateOffset;
   extern unsigned int playerAnimOverlayOffset;
}

extern COffsets offsets;
