#include "sc-prot.h"


//Constructor
Cvar::Cvar(void)
{
	m_Name = "";
	m_Ptr = 0;
}


//Bind to registered cvar
void Cvar::Register(const char * name)
{
	m_Name = name;
	m_Ptr = CVAR_GET_POINTER(m_Name);
}


//Set float value
void Cvar::set(float value)
{
	CVAR_SET_FLOAT(m_Name, value);
}


//Set string value
void Cvar::set(const char * value)
{
	if(m_Ptr)
		(*g_engfuncs.pfnCvar_DirectSet)(m_Ptr, (char*)value);
	else
		CVAR_SET_STRING(m_Name, value);
}


//Get float value	
float Cvar::get(void) const
{
	if(m_Ptr)
		return(m_Ptr->value);
	else
		return(CVAR_GET_FLOAT(m_Name));
}


//Get string value	
const char * Cvar::string(void) const
{
	if(m_Ptr && m_Ptr->string)
		return(m_Ptr->string);
	else
		return(CVAR_GET_STRING(m_Name));
}


//Get boolean value	
int Cvar::boolean(void) const
{
	return(get() >= 0.01f);
}

