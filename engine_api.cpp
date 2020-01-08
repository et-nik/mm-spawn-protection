#include "sc-prot.h"


//Bot PutInServer fix up
static edict_t * CreateFakeClient_Post(const char *netname)
{
	if (!gpGlobals->deathmatch) RETURN_META_VALUE(MRES_IGNORED,NULL);
	
	edict_t *fake_client=META_RESULT_ORIG_RET(edict_t *);
	
	if(FNullEnt(fake_client))
		RETURN_META_VALUE(MRES_IGNORED,NULL);
	
	//Get player pointer
	CPlayer *pPlayer = CPlayer::GetPlayerFromArray(fake_client);
	
	pPlayer->PutInServer(fake_client);
	
	RETURN_META_VALUE(pPlayer->GetMetaResult(),NULL);
	return NULL;
}


//CS 1.5 round start detection
static edict_t* FindEntityByString(edict_t *pEdictStartSearchAfter, const char *pszField, const char *pszValue)
{
	// if a new round has started, tell the bots this...
	if (strcmp(pszValue, "info_map_parameters") != 0) 
		RETURN_META_VALUE(MRES_IGNORED, NULL);
	
	for(int i = 1; i <= 32; i++)
	{
		CPlayer *pPlayer = CPlayer::GetPlayerFromArray(i);
		
		pPlayer->Reset(gpGlobals->time);
	}
	
	RETURN_META_VALUE(MRES_IGNORED, NULL);
	return NULL;
}


//
//
//


int __GetEngineFunctions(enginefuncs_t *pengfuncsFromEngine, int *interfaceVersion ) 
{
	if(!pengfuncsFromEngine) {
		return(FALSE);
	}
	else if(*interfaceVersion != ENGINE_INTERFACE_VERSION) {
		// Tell metamod what version we had, so it can figure out who is out of date.
		*interfaceVersion = ENGINE_INTERFACE_VERSION;
		return(FALSE);
	}
	
	memset(pengfuncsFromEngine, 0, sizeof(enginefuncs_t));
	pengfuncsFromEngine->pfnFindEntityByString = FindEntityByString;
	
	return TRUE;
}


int __GetEngineFunctions_Post(enginefuncs_t *pengfuncsFromEngine_Post, int *interfaceVersion ) 
{
	if(!pengfuncsFromEngine_Post) {
		return(FALSE);
	}
	else if(*interfaceVersion != ENGINE_INTERFACE_VERSION) {
		// Tell metamod what version we had, so it can figure out who is out of date.
		*interfaceVersion = ENGINE_INTERFACE_VERSION;
		return(FALSE);
	}
	
	memset(pengfuncsFromEngine_Post, 0, sizeof(enginefuncs_t));
	pengfuncsFromEngine_Post->pfnCreateFakeClient = CreateFakeClient_Post;
	
	return TRUE;
}

