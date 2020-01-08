#ifndef SCPROT_H
#define SCPROT_H
 
 
#include <extdll.h>
#include <dllapi.h>
#include <meta_api.h>
#include <entity_state.h>

#ifdef __linux__
	#define strncmpi strncasecmp
#endif

#include "cvar.h"
#include "sc-prot_util.h"
#include "vers_plugin.h"

#include "cplayer.h"


enum EnumModType {
	modtype_Other = 0,
	modtype_TFC,
	modtype_OP4,
	modtype_CS,
};


extern Cvar scp_on;
extern Cvar scp_chat_on;
extern Cvar scp_spawn_on;
extern Cvar scp_help_on;

extern Cvar scp_transamt;
extern Cvar scp_canpickup;

extern Cvar scp_off_onfire;
extern Cvar scp_off_delay;
extern Cvar scp_off_msg;

extern Cvar scp_spawn_stayon;

extern Cvar scp_chat_delay;
extern Cvar scp_chat_on_ladders;
extern Cvar scp_chat_in_water;
extern Cvar scp_chat_msg;
extern Cvar scp_chat_help;

extern Cvar scp_version;

extern EnumModType globalModType;

extern int globalAlreadyDisabled;


void plugin_init(void);


#endif /*SCPROT_H*/


