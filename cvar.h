#ifndef __CVAR_H__
#define __CVAR_H__


class Cvar 
{
public:
	Cvar(void);
	
	void Register(const char * CvarName);
	void set(float Value);
	void set(const char * Value);
	float get(void) const;
	const char * string(void) const;
	int boolean(void) const;
	
private:
	const char * m_Name;
	cvar_t * m_Ptr;
};


#endif /*__CVAR_H__*/
