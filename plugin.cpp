#include "sc-prot.h"


// Plugin startup.  Register commands and cvars.
static cvar_t scp_cvars[] = {
	{"scp_on",		"1", FCVAR_EXTDLL, 0, NULL},
	{"scp_chat_on",		"1", FCVAR_EXTDLL, 0, NULL},
	{"scp_spawn_on",	"1", FCVAR_EXTDLL, 0, NULL},
	{"scp_help_on",		"0", FCVAR_EXTDLL, 0, NULL},

	{"scp_transamt",	"75", FCVAR_EXTDLL, 0, NULL},
	{"scp_canpickup",	"1", FCVAR_EXTDLL, 0, NULL},

	{"scp_off_onfire",	"1", FCVAR_EXTDLL, 0, NULL},
	{"scp_off_delay",	"0.5", FCVAR_EXTDLL, 0, NULL},
	{"scp_off_msg",		"Your protection has worn off!", FCVAR_EXTDLL, 0, NULL},

	{"scp_spawn_stayon",	"0", FCVAR_EXTDLL, 0, NULL},

	{"scp_chat_delay",	"5", FCVAR_EXTDLL, 0, NULL},
	{"scp_chat_on_ladders",	"0", FCVAR_EXTDLL, 0, NULL},
	{"scp_chat_in_water",	"1", FCVAR_EXTDLL, 0, NULL},
	{"scp_chat_msg",	"Chat-protection has been enabled for you!", FCVAR_EXTDLL, 0, NULL},
	{"scp_chat_help",	"If you want to get chat protection: face wall and don't move or shoot for %.1fsec.", FCVAR_EXTDLL, 0, NULL},

	{"scp_version",		VVERSION, FCVAR_EXTDLL|FCVAR_SERVER, 0, NULL},
	{NULL,NULL,0,0,NULL},
};


//Cvars
Cvar scp_on;
Cvar scp_chat_on;
Cvar scp_spawn_on;
Cvar scp_help_on;
Cvar scp_transamt;
Cvar scp_canpickup;
Cvar scp_off_onfire;
Cvar scp_off_delay;
Cvar scp_off_msg;
Cvar scp_spawn_stayon;
Cvar scp_chat_delay;
Cvar scp_chat_on_ladders;
Cvar scp_chat_in_water;
Cvar scp_chat_msg;
Cvar scp_chat_help;
Cvar scp_version;


//mod type
EnumModType globalModType;


void plugin_init(void) 
{
	char ModName[32];
	
	//Get mod name and set mod type
	UTIL_GetModName(ModName, 32);
	if(!stricmp(ModName, "tfc"))
		globalModType = modtype_TFC;
	else if(!stricmp(ModName, "gearbox"))
		globalModType = modtype_OP4;
	else if(!stricmp(ModName, "cstrike"))
		globalModType = modtype_CS;
	else
		globalModType = modtype_Other;
	
	
	LOG_MESSAGE(PLID, "%s v%s, %s", Plugin_info.name, Plugin_info.version, Plugin_info.date);
	LOG_MESSAGE(PLID, "by %s", Plugin_info.author);
	LOG_MESSAGE(PLID, "   %s", Plugin_info.url);
	LOG_MESSAGE(PLID, "compiled: "__DATE__ "," __TIME__" EET");
	
	
	//Register list of cvars
	for(int i = 0; scp_cvars[i].name; i++ )
		CVAR_REGISTER( &scp_cvars[i] );
	
	
	#define ActivateCvar(name) name.Register( #name )
	
	ActivateCvar(scp_on);
	ActivateCvar(scp_chat_on);
	ActivateCvar(scp_spawn_on);
	ActivateCvar(scp_help_on);
	ActivateCvar(scp_transamt);
	ActivateCvar(scp_canpickup);
	ActivateCvar(scp_off_onfire);
	ActivateCvar(scp_off_delay);
	ActivateCvar(scp_off_msg);
	ActivateCvar(scp_spawn_stayon);
	ActivateCvar(scp_chat_delay);
	ActivateCvar(scp_chat_on_ladders);
	ActivateCvar(scp_chat_in_water);
	ActivateCvar(scp_chat_msg);
	ActivateCvar(scp_chat_help);
	ActivateCvar(scp_version);
	
	
	//Reset PlayerList
	memset(&players, 0, sizeof(players));
	
	//Clear players
	for(int u = 1; u <= 32; u++)
	{
		CPlayer *pPlayer = CPlayer::GetPlayerFromArray(u);
			
		pPlayer->m_index = u;
		pPlayer->Reset(gpGlobals->time);
	}
}
