#include "sc-prot.h"


int globalAlreadyDisabled = false;


static void ClientPutInServer( edict_t *pEdict )
{
	if (!gpGlobals->deathmatch) RETURN_META(MRES_IGNORED);
	
	//Get player pointer
	CPlayer *pPlayer = CPlayer::GetPlayerFromArray(pEdict);
	
	//call player class function
	pPlayer->PutInServer(pEdict);
	
	RETURN_META(pPlayer->GetMetaResult());
}

static void ClientDisconnect( edict_t *pEdict )
{
	if (!gpGlobals->deathmatch) RETURN_META(MRES_IGNORED);
	
	//Get player pointer
	CPlayer *pPlayer = CPlayer::GetPlayerFromArray(pEdict);
	
	if(!pPlayer->IsValid())
	{
		//Not valid!
		RETURN_META(MRES_IGNORED);
	}

	//call player class function
	pPlayer->Disconnect();
	
	RETURN_META(pPlayer->GetMetaResult());
}

static void PlayerPostThink( edict_t *pEdict )
{	
	if (!gpGlobals->deathmatch) RETURN_META(MRES_IGNORED);
	if (!scp_on.boolean())
	{
		if(!globalAlreadyDisabled)
			CPlayer::DeactivateProtectionForAll();
		
		globalAlreadyDisabled = true;
		
		RETURN_META(MRES_IGNORED);
	}
	else
		globalAlreadyDisabled = false;
	
	//Get player pointer
	CPlayer *pPlayer = CPlayer::GetPlayerFromArray(pEdict);
	
	if(!pPlayer->IsValid())
	{
		//Metamod bot fix
		if(pEdict && pEdict->v.flags & FL_FAKECLIENT)
		{
			//call player class function
			pPlayer->PutInServer(pEdict);
		}
		
		//Not valid!
		RETURN_META(MRES_IGNORED);
	}
	
	//call player class function
	pPlayer->PostThink();
	
	RETURN_META(pPlayer->GetMetaResult());
}

static void ClientCommand( edict_t *pThis )
{
	if (!gpGlobals->deathmatch || !scp_on.boolean()) RETURN_META(MRES_IGNORED);
	
	//Get player pointer
	CPlayer *pPlayer = CPlayer::GetPlayerFromArray(pThis);
	
	if(!pPlayer->IsValid())
	{
		//Not valid!
		RETURN_META(MRES_IGNORED);
	}
	
	//call player class function
	pPlayer->Command();
	
	RETURN_META(pPlayer->GetMetaResult());
}

static void ClientCommand_Post( edict_t *pThis )
{
	if (!gpGlobals->deathmatch || !scp_on.boolean()) RETURN_META(MRES_IGNORED);
	
	//Get player pointer
	CPlayer *pPlayer = CPlayer::GetPlayerFromArray(pThis);
	
	if(!pPlayer->IsValid())
	{
		//Not valid!
		RETURN_META(MRES_IGNORED);
	}
	
	//call player class function
	pPlayer->Command_Post();
	
	RETURN_META(pPlayer->GetMetaResult());
}

static void UpdateClientData_Post(const struct edict_s *ent, int sendweapons, struct clientdata_s *cd)
{
	if (!gpGlobals->deathmatch || !scp_on.boolean()) RETURN_META(MRES_IGNORED);
	
	//Get player pointer
	CPlayer *pPlayer = CPlayer::GetPlayerFromArray(ent);
	
	if(!pPlayer->IsValid())
	{
		//Not valid!
		RETURN_META(MRES_IGNORED);
	}
	
	//call player class function
	pPlayer->UpdateClientData_Post(sendweapons, cd);
	
	RETURN_META(pPlayer->GetMetaResult());
}

static void ClientUserInfoChanged(edict_t *pThis, char *infobuffer)
{
	if (!gpGlobals->deathmatch || !scp_on.boolean()) RETURN_META(MRES_IGNORED);
	
	//Get player pointer
	CPlayer *pPlayer = CPlayer::GetPlayerFromArray(pThis);
	
	if(!pPlayer->IsValid())
	{
		//Not valid!
		RETURN_META(MRES_IGNORED);
	}
	
	//call player class function
	pPlayer->ClientUserInfoChanged(infobuffer);
	
	RETURN_META(pPlayer->GetMetaResult());
}

static void ClientUserInfoChanged_Post(edict_t *pThis, char *infobuffer)
{
	if (!gpGlobals->deathmatch || !scp_on.boolean()) RETURN_META(MRES_IGNORED);
	
	//Get player pointer
	CPlayer *pPlayer = CPlayer::GetPlayerFromArray(pThis);
	
	if(!pPlayer->IsValid())
	{
		//Not valid!
		RETURN_META(MRES_IGNORED);
	}
	
	//call player class function
	pPlayer->ClientUserInfoChanged_Post(infobuffer);
	
	RETURN_META(pPlayer->GetMetaResult());
}

static void DispatchTouch(edict_t *pentTouched, edict_t *pThis)
{
	if (!gpGlobals->deathmatch || !scp_on.boolean()) RETURN_META(MRES_IGNORED);
	
	//Get player pointer
	CPlayer *pPlayer = CPlayer::GetPlayerFromArray(pThis);
	
	if(!pPlayer->IsValid())
	{
		//Not valid!
		RETURN_META(MRES_IGNORED);
	}
	
	//call player class function
	pPlayer->Touch(pentTouched);
	
	RETURN_META(pPlayer->GetMetaResult());
}

static void DispatchTouch_Post(edict_t *pentTouched, edict_t *pThis)
{
	if (!gpGlobals->deathmatch || !scp_on.boolean()) RETURN_META(MRES_IGNORED);
	
	//Get player pointer
	CPlayer *pPlayer = CPlayer::GetPlayerFromArray(pThis);
	
	if(!pPlayer->IsValid())
	{
		//Not valid!
		RETURN_META(MRES_IGNORED);
	}
	
	//call player class function
	pPlayer->Touch_Post(pentTouched);
	
	RETURN_META(pPlayer->GetMetaResult());
}

static int DispatchSpawn( edict_t *pent )
{	
	if (!gpGlobals->deathmatch) RETURN_META_VALUE(MRES_IGNORED, 0);
	
	if (!stricmp(STRING(pent->v.classname), "worldspawn"))
	{
		//Reset PlayerList
		memset(&players, 0, sizeof(players));
		
		//Clear players
		for (int i = 1; i <= 32; i++)
		{
			CPlayer *pPlayer = CPlayer::GetPlayerFromArray(i);
			
			pPlayer->m_index = i;
			pPlayer->Reset(gpGlobals->time);
		}
	}
	
	RETURN_META_VALUE(MRES_HANDLED,	0);
	return 0;
}


//
//
//


int __GetEntityAPI2( DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion )
{
	if(!pFunctionTable) {
		return(FALSE);
	}
	else if(*interfaceVersion != INTERFACE_VERSION)	{
		//! Tell engine	what version we	had, so	it can figure out who is out of	date.
		*interfaceVersion = INTERFACE_VERSION;
		return(FALSE);
	}
	
	memset(pFunctionTable, 0, sizeof(DLL_FUNCTIONS));

	pFunctionTable->pfnSpawn		= DispatchSpawn;
	pFunctionTable->pfnTouch		= DispatchTouch;
	pFunctionTable->pfnClientDisconnect	= ClientDisconnect;
	pFunctionTable->pfnClientPutInServer	= ClientPutInServer;
	pFunctionTable->pfnClientCommand	= ClientCommand;
	pFunctionTable->pfnClientUserInfoChanged= ClientUserInfoChanged;
	pFunctionTable->pfnPlayerPostThink	= PlayerPostThink;

	return(TRUE);
}

int __GetEntityAPI2_Post( DLL_FUNCTIONS *pFunctionTable_Post, int *interfaceVersion )
{
	if(!pFunctionTable_Post) {
		return(FALSE);
	}
	else if(*interfaceVersion != INTERFACE_VERSION)	{
		//! Tell engine	what version we	had, so	it can figure out who is out of	date.
		*interfaceVersion = INTERFACE_VERSION;
		return(FALSE);
	}
	
	memset(pFunctionTable_Post, 0, sizeof(DLL_FUNCTIONS));
	
	pFunctionTable_Post->pfnTouch			= DispatchTouch_Post;
	pFunctionTable_Post->pfnClientCommand		= ClientCommand_Post;
	pFunctionTable_Post->pfnClientUserInfoChanged	= ClientUserInfoChanged_Post;
	pFunctionTable_Post->pfnUpdateClientData	= UpdateClientData_Post;

	return(TRUE);
}

