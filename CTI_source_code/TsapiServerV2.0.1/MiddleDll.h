#ifndef _MIDDLE_DLL_H_
#define _MIDDLE_DLL_H_

class CMiddleDll
{
public:
	CMiddleDll();
	virtual ~CMiddleDll();
public:
	void MiddleDllInit();
public:
	LPRESTOREEVENT lpfnRestoreEvent;// = NULL;
	LPSETCALLBACK lpSetCallbalck;// = NULL;
	SENDTODEST	SendToDestFunc;// = NULL;
	BEGINCHECKALIVE	BeginCheckAliveFunc;
	MIDDLEDLLINIT MiddleDllInitFunc;
	SETKEEPALIVEFLAG	SetKeepAliveFlagFunc;
private:
#ifdef _VC6_
	HINSTANCE m_hMidlleDll;
#endif
};


#endif
