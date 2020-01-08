#ifndef CPLAYER_H
#define CPLAYER_H

// define a new bit flag for bot identification
#define FL_THIRDPARTYBOT (1 << 27)

enum EnumProtectionType {
	NoProtection = 0,
	SpawnProtection,
	ChatProtection,	
};

class CPlayer
{
public:
	int		m_connected;	//Client connected to this slot
	
	edict_t	*	m_pEdict;
	int		m_index;
   	
   	int		m_IsInGodmode;
   	int             m_ProtectionActive;
   	int		m_IsCarringFlag; //For Op4CTF and TFC
   	
   	float		m_OldHealth;
   	
   	float		m_LastTimeWhenPressedButton;
   	float		m_NextTimeToCheckFlag;
   	float		m_StartOfProtection;
	
	EnumProtectionType m_ProtectionType;
	
	int		m_ProcessingOverGameDLLClientCommand;
	int		m_ProcessingOverGameDLLClientUserInfoChanged;
	int		m_ProcessingOverGameDLLTouch;
	
	
	//DLLAPI:
	void PutInServer(edict_t *pThis);
	void Disconnect(void);
	void PostThink(void);
	void Command(void);
	void Command_Post(void);
	void UpdateClientData_Post(int sendweapons, struct clientdata_s *cd);
	void ClientUserInfoChanged(char *infobuffer);
	void ClientUserInfoChanged_Post(char *infobuffer);
	void Touch(edict_t *pentTouched);
	void Touch_Post(edict_t *pentTouched);
	
	
	//
	int IsValid()
	{
		return(m_connected && !FNullEnt(m_pEdict) && 
		       !((m_pEdict->v.flags & FL_FAKECLIENT) == FL_FAKECLIENT ||
		         (m_pEdict->v.flags & FL_THIRDPARTYBOT) == FL_THIRDPARTYBOT ||
		         (m_pEdict->v.flags & FL_THIRDPARTYBOT) == FL_PROXY)
		);
	}
		
	//Get default return type
	META_RES GetMetaResult(void)
	{
		return m_message_mres;
	}
	
	//Gets index to players array
	static CPlayer *GetPlayerFromArray(int index);
	static CPlayer *GetPlayerFromArray(const edict_t *pEnt)
	{
		return GetPlayerFromArray(FNullEnt((edict_t *)pEnt)?0:ENTINDEX((edict_t *)pEnt));
	}
	
	//Deactivate protection for all players
	static void DeactivateProtectionForAll(void);

	//Reset settings
	void Reset(float time);
	
private:
	META_RES m_message_mres;
	
	int      m_ProtectionFXActive;
	int      m_OldRendermode;
	float    m_OldRenderamt;
	float    m_OldTakedamage;
	int      m_OldSolid;
	int      m_TrackSolidValue;
	int      m_RendermodeShouldBe;
	float    m_RenderamtShouldBe;
	float    m_TakedamageShouldBe;
	int      m_SolidShouldBe;
	
	
	//Set return type
	void SetMetaResult(META_RES meta_resu)
	{
		m_message_mres=meta_resu;
	}
	
	//
	int IsPressingAnyButton(void)
	{
		//Allow checking scores and ducking and looking
		return((m_pEdict->v.button & ~(IN_SCORE | IN_DUCK | IN_LEFT | IN_RIGHT)) != 0);
	}
	
	//
	int IsAlive()
	{
		return(m_pEdict->v.health > 0);
	}
	
	//
	int WasAlive()
	{
		return(m_OldHealth > 0);
	}	
	
	//
	int DoSpawnProtection(void);
	void DoChatProtection(void);
	
	//Sets values to player entity
	void EnableProtectionFX(void);
	void DisableProtectionFX(void);	

	//
	void ActivateGodmode(void);
	void DeactivateGodmode(int silent = 0);
	
	//
	int IsPlayerCarringFlag(void);
	
	//Check if player is looking at wall
	int LookingAtWall(void);
	
	//Sends message to player
	void SendTextMsg(int msg_dest, const char* msg_name);
};


extern CPlayer players[33];


//Gets pointer to players array with index
inline CPlayer * CPlayer::GetPlayerFromArray(int index)
{
	if(index<1 || index>32) return &players[0];
	return &players[index];
}


#endif /*CPLAYER_H*/
