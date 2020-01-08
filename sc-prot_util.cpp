#include "sc-prot.h"


//
edict_t *UTIL_FindEntityByClassname( edict_t *pentStart, const char *szValue )
{
	edict_t *pentEntity = FIND_ENTITY_BY_STRING( pentStart, "classname", szValue );
	
	if(!FNullEnt(pentEntity))
		return(pentEntity);
	
	return(NULL);
}


//Get mod name
void UTIL_GetModName(char * mod_name, int max_size)
{
	int pos;
	char game_dir[1024];
	
	// find the directory name of the currently running MOD...
	(*g_engfuncs.pfnGetGameDir)(game_dir);
	
	pos = strlen(game_dir) - 1;
	
	// scan backwards till first directory separator...
	while ((pos) && (game_dir[pos] != '/'))
		pos--;
	
	if (pos) pos++;
	
	strncpy(mod_name, &game_dir[pos], max_size);
}
