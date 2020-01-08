#include "sc-prot.h"


//players array .. index 0 is used for invalid entities
CPlayer players[33];


void CPlayer::PutInServer(edict_t *pThis)
{
	// If index is 0 pThis is invalid :(
	// Don't think proxies and spectators as players
	if(m_index==0 || pThis->v.flags & FL_PROXY || pThis->v.flags & FL_SPECTATOR) 
	{
		m_connected = false;
		m_pEdict    = NULL;
		
		SetMetaResult(MRES_IGNORED);
		return;
	}
	
	//Set defaults
	m_connected = true;
	m_pEdict    = pThis;
	
	Reset(gpGlobals->time);
		
	SetMetaResult(MRES_HANDLED);
	return;
}


void CPlayer::Disconnect()
{
	//reset
	int index = m_index;
	memset(this, 0, sizeof(CPlayer));
	m_index = index;
	
	//Mark disconnected
	m_connected = false;
	m_pEdict = NULL;
		
	SetMetaResult(MRES_HANDLED);
	return;
}


void CPlayer::PostThink()
{
	//Set default
	SetMetaResult(MRES_IGNORED);
		
	//OP4CTF Spectator check .. if protection is not active but takedamage is 0 we are in spectator mode!
	if(!m_ProtectionActive && m_pEdict->v.takedamage == 0)
		return;
	
	//Set new default
	SetMetaResult(MRES_HANDLED);
	
	//Op4CTF & TFC Flag Check
	if(globalModType == modtype_OP4 || globalModType == modtype_TFC)
	{
		if(gpGlobals->time >= m_NextTimeToCheckFlag)
		{
			m_IsCarringFlag = IsPlayerCarringFlag();
			
			m_NextTimeToCheckFlag = gpGlobals->time + 0.5f;
		}
	}
	
	//If has flag.. set prot off and exit
	if(m_IsCarringFlag)
	{
		if(m_ProtectionActive)
		{
			//Time to deactive protection..
			DeactivateGodmode();
			m_ProtectionActive = false;
		}
		
		return;
	}
	
	//Check for buttons pressed
	if (m_ProtectionActive)
	{
		//Updates backuped data in ProtectionFX code
		ActivateGodmode();
		
		//Check if player has pressed any movement buttons
		if(IsPressingAnyButton())
		{
			//This is needed for chat protection!
			m_LastTimeWhenPressedButton = gpGlobals->time;
		}
		
		//Check if attacked.. and set prot off if off_on_fire set on (added 0.5sec extra off time for spawn (because usually players press fire when they want to respawn))
		if(m_pEdict->v.button & (IN_ATTACK | IN_ATTACK2) && scp_off_onfire.boolean())
		{
			if(m_ProtectionType == ChatProtection || (m_ProtectionType == SpawnProtection && m_StartOfProtection + 0.5f <= gpGlobals->time))
			{
				//Time to deactive protection..
				DeactivateGodmode();
				m_ProtectionActive = false;
				
				//Make sure that ATTACK & ATTACK2 doesn't work (fix)
				m_pEdict->v.button &= ~(IN_ATTACK | IN_ATTACK2);
			}
		}
	}
	else
	{
		//Check if player has pressed any movement buttons 
		//..check if is not looking at wall
		//..check if on ladders
		//..check if in water
		if(IsPressingAnyButton() || !LookingAtWall() || 
			(m_pEdict->v.movetype == MOVETYPE_FLY && !scp_chat_on_ladders.boolean()) ||
			(m_pEdict->v.waterlevel >= 3 && !scp_chat_in_water.boolean()))
		{
			//This is needed for chatp!
			m_LastTimeWhenPressedButton = gpGlobals->time;
		}
	}
	
	//Spawn protection code (returns 1 if wants to exit whole PlayerThink)
	if(DoSpawnProtection())
		return;
	
	//Chat protection code
	DoChatProtection();
	
	return;
}


//
void CPlayer::Command(void)
{
	//Set default
	SetMetaResult(MRES_IGNORED);
	
	//Only check on OP4 mod
	if(globalModType != modtype_OP4)
		return;
	
	//command string
	const char *pcmd = CMD_ARGV(0);
	
	//OP4CTF switch to spectator-mode
	if (!stricmp(pcmd,"spectate"))
	{
		//Set Godmode off!
		DeactivateGodmode();
		m_ProtectionActive = false;
		
		//Set negative health so that spawn works after spectating
		m_OldHealth = -100.0f;
		
		SetMetaResult(MRES_HANDLED);
		return;
	}
	else if (!stricmp(pcmd,"selectchar"))		//OP4CTF-Team change
	{
		//Let's set protection off
		if (m_ProtectionActive)
			DisableProtectionFX();
		
		m_ProcessingOverGameDLLClientCommand = true;
		
		SetMetaResult(MRES_HANDLED);
		return;
	}
	
	return;
}


//
void CPlayer::Command_Post(void)
{
	if(m_ProcessingOverGameDLLClientCommand)
	{
		m_ProcessingOverGameDLLClientCommand = false;
		
		//Reactivate protection
		if (m_ProtectionActive)
			EnableProtectionFX();
		
		SetMetaResult(MRES_HANDLED);
	}
	else
		SetMetaResult(MRES_IGNORED);
	
	//
	//Help system
	//
	if(!scp_help_on.boolean() || CMD_ARGC() < 2)
		return;
	
	const char *pcmd  = CMD_ARGV(0);
	const char *saycmd= CMD_ARGV(1);
	
	if (!stricmp(pcmd,"say") || !stricmp(pcmd,"say_team"))
	{
		if(!strnicmp(saycmd, "chat", 4) || !strnicmp(saycmd, "prot", 4))
		{
			char output[512];
			char format[256];
			
			snprintf(format, 255, "["VLOGTAG"] %s\n", scp_chat_help.string());
			format[255] = 0;
			
			snprintf(output, 511, format, scp_chat_delay.get());
			output[511] = 0;
			
			for(int i = 1; i <= 32; i++)
			{
				CPlayer *pPlayer = CPlayer::GetPlayerFromArray(i);
				
				if(!pPlayer->IsValid())
					continue;
				
				pPlayer->SendTextMsg(HUD_PRINTTALK, output);
			}
			
			SetMetaResult(MRES_HANDLED);
			return;
		}
	}
}


//
void CPlayer::UpdateClientData_Post(int sendweapons, struct clientdata_s *cd)
{
	//Fix clientside weapon animations
	if(sendweapons && m_ProtectionActive)
	{
		if (cd->m_flNextAttack < gpGlobals->time)
			cd->m_flNextAttack = (m_StartOfProtection + scp_off_delay.get()) - (gpGlobals->time + 0.001f);
		else
			cd->m_flNextAttack = (m_StartOfProtection + scp_off_delay.get()) - 0.001f;
		
		SetMetaResult(MRES_HANDLED);
		return;
	}
	
	SetMetaResult(MRES_IGNORED);
	return;
}


//
void CPlayer::ClientUserInfoChanged(char *infobuffer)
{
	//Let's set protection off
	if (m_ProtectionActive)
		DisableProtectionFX();
	
	m_ProcessingOverGameDLLClientUserInfoChanged = true;
	
	SetMetaResult(MRES_HANDLED);
	return;
}


//
void CPlayer::ClientUserInfoChanged_Post(char *infobuffer)
{
	if(m_ProcessingOverGameDLLClientUserInfoChanged)
	{
		m_ProcessingOverGameDLLClientUserInfoChanged = false;
		
		//Reactivate protection
		if (m_ProtectionActive)
			EnableProtectionFX();
		
		SetMetaResult(MRES_HANDLED);
		return;
	}
	else
	{
		SetMetaResult(MRES_IGNORED);
		return;
	}
}


//
void CPlayer::Touch(edict_t *pentTouched)
{
	//Check if touched trigger_hurt
	if(FNullEnt(pentTouched) || !pentTouched->v.classname || stricmp((STRING(pentTouched->v.classname)),"trigger_hurt"))
	{
		SetMetaResult(MRES_IGNORED);
		return;
	}
	
	//Let's set protection off
	if (m_ProtectionActive)
		DisableProtectionFX();
	
	m_ProcessingOverGameDLLTouch = true;
	
	SetMetaResult(MRES_HANDLED);
	return;
}


//
void CPlayer::Touch_Post(edict_t *pentTouched)
{
	if(m_ProcessingOverGameDLLTouch)
	{
		m_ProcessingOverGameDLLTouch = false;
		
		//Reactivate protection
		if (m_ProtectionActive)
			EnableProtectionFX();
		
		SetMetaResult(MRES_HANDLED);
		return;
	}
	else
	{
		SetMetaResult(MRES_IGNORED);
		return;
	}
}


//
void CPlayer::Reset(float time)
{
	DeactivateGodmode(1);
	
	m_NextTimeToCheckFlag		=
	m_LastTimeWhenPressedButton	=
	m_StartOfProtection		= time;
	
	m_IsInGodmode		=
	m_ProtectionActive	=	
	m_IsCarringFlag		= 
	m_ProtectionFXActive    = false;
	
	m_ProtectionType	= NoProtection;
	
	m_OldHealth		= -1.0f;	
}


//Spawn protection code
int CPlayer::DoSpawnProtection(void)
{
	if(!WasAlive()) //Dude is dead .. check for respawn
	{
		if(IsAlive()) //Player has respawned! .. start spawn-protection!
		{
			m_LastTimeWhenPressedButton = gpGlobals->time;
			
			if(!scp_spawn_on.boolean()) 
			{
				//Set new health here
				m_OldHealth = m_pEdict->v.health;
				
				//Return 0 so we get to the Chat Protection code
				return(0);
			}
			
			//Mark this protmode as spawn
			m_ProtectionType = SpawnProtection;
			
			//Set start of spawnprotection .. and set godmode on
			m_StartOfProtection = gpGlobals->time;
			m_ProtectionActive  = true;
			ActivateGodmode();
			
			if(scp_spawn_stayon.boolean())
			{
				//Make player keep protection if not moving
				m_LastTimeWhenPressedButton = gpGlobals->time - scp_chat_delay.get() + 0.5f;
			}
			
			//Set new health here
			m_OldHealth = m_pEdict->v.health;
			
			//Make sure that ATTACK&ATTACK2 doesn't work
			m_pEdict->v.button &= ~(IN_ATTACK | IN_ATTACK2);
			
			//continue to chatprotection
			return(0);
		}
		else
		{
			//Still dead .. check godmode .. then exit
			DeactivateGodmode();
			m_ProtectionActive = false;
			
			SetMetaResult(MRES_IGNORED);
			return(1);
		}
	}
	else
	{
		//Was alive before .. check if is still alive!
		if(IsAlive())
		{
			//yep .. player is alive.. check if we are in prot mode
			if (m_ProtectionActive)
			{
				//Check if we should set prot off
				if (m_StartOfProtection + scp_off_delay.get() <= gpGlobals->time)
				{
					//Time to set sprot off..
					DeactivateGodmode();
					m_ProtectionActive = false;
					
					//Make sure that ATTACK&ATTACK2 doesn't work (fix)
					m_pEdict->v.button &= ~(IN_ATTACK | IN_ATTACK2);
				}
				else
				{
					//Make sure that ATTACK&ATTACK2 doesn't work
					m_pEdict->v.button &= ~(IN_ATTACK | IN_ATTACK2);
				}
			}
			
			//Continue chat protection
			return(0);
		}
		else
		{
			//was alive .. now dead
			DeactivateGodmode();
			m_ProtectionActive = false;
			
			//Set new health here
			m_OldHealth = m_pEdict->v.health;
			
			SetMetaResult(MRES_IGNORED);
			return(1);
		}
	}
	
	return(0);
}


//Chat protection code
void CPlayer::DoChatProtection(void)
{
	if(scp_chat_on.boolean() && m_LastTimeWhenPressedButton + scp_chat_delay.get() <= gpGlobals->time)
	{
		//Time to start protection!
		
		//Show Message here (chatp enabled)
		if (!m_ProtectionActive)
		{
			//
			char output[512];
			
			snprintf(output, 511, "%s\n", scp_chat_msg.string());
			SendTextMsg(HUD_PRINTCENTER, output);
		}
		
		//Mark this protmode as chat
		m_ProtectionType = ChatProtection;
		
		//Set start of chatprotection .. and set godmode on
		m_StartOfProtection = gpGlobals->time;
		m_ProtectionActive = true;
		ActivateGodmode();
		
		//Make sure that ATTACK&ATTACK2 doesn't work
		m_pEdict->v.button &= ~(IN_ATTACK | IN_ATTACK2);
	}
}


//Check if player is looking at wall
int CPlayer::LookingAtWall(void)
{
	TraceResult tr;
	
	Vector EyePosition = m_pEdict->v.origin + m_pEdict->v.view_ofs;

	MAKE_VECTORS(m_pEdict->v.v_angle);
	TRACE_LINE (
		EyePosition,					//Start
		EyePosition + gpGlobals->v_forward * 128,	//End
		0x101,						//IgnoreMonsters and IgnoreGlass
		m_pEdict,					//Ignore this ent
		&tr 						//Output
	);
	
	if (tr.flFraction == 1.0) 
		return(false);
	
	if (DotProduct(gpGlobals->v_forward, tr.vecPlaneNormal) > -0.5f) //60deg
		return(false);
	
	return(true);
}


//Check if player is carring flag.. (TFC and Op4CTF)
int CPlayer::IsPlayerCarringFlag(void)
{
	edict_t *pent = NULL;
	
	switch(globalModType)
	{
	default:
		return(false);
		
	case modtype_TFC:
		//Check for TFC
		while ((pent = UTIL_FindEntityByClassname( pent, "item_tfgoal" )) != NULL)
		{
			if (pent->v.owner == m_pEdict)  // is this player carrying the item?
			{
				//player has it .. return true
				return(true);
			}
		}
		
		return(false);
		
	case modtype_OP4:
		
		//Check for Op4CTF
		while ((pent = UTIL_FindEntityByClassname( pent, "item_ctfflag" )) != NULL)
		{
			if (pent->v.owner == m_pEdict)  // is this player carrying the item?
			{
				//Now check if flag's and owner's origins are same
				//If not flag has returned and not owned by anyone!
				return(m_pEdict->v.origin == pent->v.origin);
			}
		}
		
		return(false);
	}
}


//
void CPlayer::ActivateGodmode(void)
{
	EnableProtectionFX();
	m_IsInGodmode = true;
}


//
void CPlayer::DeactivateGodmode(int silent)
{
	DisableProtectionFX();
	m_IsInGodmode = false;
	
	if(!silent)
	{
		char output[512];
		
		snprintf(output, 511, "%s\n", scp_off_msg.string());
		SendTextMsg(HUD_PRINTCENTER, output);
	}
}


//Activate player entity effects
void CPlayer::EnableProtectionFX(void)
{
	if(!m_ProtectionFXActive)
	{
		//Get corrected transparency ammount
		int TransparencyAmmount = (int)scp_transamt.get();
		
		if(TransparencyAmmount < 0)
			TransparencyAmmount = 0;
		else if(TransparencyAmmount > 255)
			TransparencyAmmount = 255;
		
		//Backup old
		m_OldRendermode = m_pEdict->v.rendermode;
		m_OldRenderamt  = m_pEdict->v.renderamt;
		m_OldTakedamage = m_pEdict->v.takedamage;
		m_OldSolid      = m_pEdict->v.solid;
		
		//Set ProtectionFX
		if(!scp_canpickup.boolean())
			m_pEdict->v.solid = SOLID_NOT;
		m_pEdict->v.rendermode    = kRenderTransTexture;
		m_pEdict->v.renderamt     = TransparencyAmmount;	
		m_pEdict->v.takedamage    = DAMAGE_NO;
		
		//Values to compare with
		m_RendermodeShouldBe = m_pEdict->v.rendermode;
		m_RenderamtShouldBe  = m_pEdict->v.renderamt;
		m_TakedamageShouldBe = m_pEdict->v.takedamage;
		m_TrackSolidValue    = !scp_canpickup.boolean();
		if(m_TrackSolidValue)
			m_SolidShouldBe  = m_pEdict->v.solid;
		
		m_ProtectionFXActive = true;
	}
	else
	{
		//Already active .. check for changes and update backuped values if any changes
		if(m_TrackSolidValue)
		{
			if(m_SolidShouldBe != m_pEdict->v.solid)
			{
				m_OldSolid = m_pEdict->v.solid;
				m_pEdict->v.solid = m_SolidShouldBe;
			}
		}
		
		if(m_RendermodeShouldBe != m_pEdict->v.rendermode)
		{
			m_OldRendermode = m_pEdict->v.rendermode;
			m_pEdict->v.rendermode = m_RendermodeShouldBe;
		}
		
		if(m_RenderamtShouldBe != m_pEdict->v.renderamt)
		{
			m_OldRenderamt = m_pEdict->v.renderamt;
			m_pEdict->v.renderamt = m_RenderamtShouldBe;
		}
		
		if(m_TakedamageShouldBe != m_pEdict->v.takedamage)
		{
			m_OldTakedamage = m_pEdict->v.takedamage;
			m_pEdict->v.takedamage = m_TakedamageShouldBe;
		}
	}
}


//Disable player entity effects
void CPlayer::DisableProtectionFX(void)
{
	if(m_ProtectionFXActive)
	{
		if(m_TrackSolidValue)
		{
			if(m_SolidShouldBe == m_pEdict->v.solid)
				m_pEdict->v.solid   = m_OldSolid;
		}
		
		if(m_RendermodeShouldBe == m_pEdict->v.rendermode)
			m_pEdict->v.rendermode	= m_OldRendermode;
		
		if(m_RenderamtShouldBe == m_pEdict->v.renderamt)
			m_pEdict->v.renderamt	= m_OldRenderamt;	
		
		if(m_TakedamageShouldBe == m_pEdict->v.takedamage)
			m_pEdict->v.takedamage	= m_OldTakedamage;
		
		m_ProtectionFXActive = false;
	}
}


//Sends message to player
void CPlayer::SendTextMsg(int msg_dest, const char* msg_name)
{
	static int message_TextMsg = 0;
	
	if(message_TextMsg == 0)
		message_TextMsg = GET_USER_MSG_ID(PLID, "TextMsg", NULL);
	
	if(message_TextMsg == 0 || m_pEdict->v.flags & FL_FAKECLIENT)
		return;
	
	MESSAGE_BEGIN( MSG_ONE, message_TextMsg, NULL, m_pEdict );
		WRITE_BYTE( msg_dest );
		WRITE_STRING( msg_name );
	MESSAGE_END();
}


//
void CPlayer::DeactivateProtectionForAll(void)
{
	for(int i = 1; i <= 32; i++)
	{
		CPlayer *pPlayer = CPlayer::GetPlayerFromArray(i);
		
		if(!pPlayer->IsValid())
			continue;
		
		pPlayer->DeactivateGodmode();
		pPlayer->m_ProtectionActive = false;
	}
}
