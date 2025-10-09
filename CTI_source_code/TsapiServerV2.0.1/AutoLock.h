#ifndef _AUTO_LOCK_H_
#define _AUTO_LOCK_H_

#ifdef _VC6_
class CAutoLock
{
public:
	CAutoLock(CRITICAL_SECTION *pcs)
	{
		m_cs = pcs;
		EnterCriticalSection( m_cs );
	}
	~CAutoLock()
	{
		LeaveCriticalSection( m_cs );
	}
private:
	CRITICAL_SECTION *m_cs;
};

#endif

#endif
