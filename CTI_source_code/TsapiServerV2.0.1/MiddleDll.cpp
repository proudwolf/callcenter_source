#ifdef _VC6_
#include "StdAfx.h"
#include "TsapiServer.h"
#include "TsapiServerDlg.h"
#endif
#include "GlobalTsapiServer.h"
#include "MiddleDll.h"
#include "mainentry.h"

extern void TraceEx( char *buf, ...);
extern CMainEntry m_MainEntry;

#ifdef _linux_
char  middle_dll_file[128] = "libMiddlePipe.so";
#else
char  middle_dll_file[128] = "MiddlePipe.dll";
#endif

QLibrary middleDll(middle_dll_file);

void Server_Restore_Event(void *wParam, void *lParam)
{
	try
	{
#ifdef _VC6_
	CTsapiServerDlg *pDlg = (CTsapiServerDlg *)AfxGetMainWnd();
#endif

	if( wParam == NULL || lParam == NULL )
		return;

    char szLog[256] = {0};
	char *Cmd = (char *)wParam;
	char *buf = (char *)lParam;
	int icmd_value = atoi( Cmd );
	switch( icmd_value )
	{
	case MAIN_CTI_RESTORE:
		{
            m_MainEntry.m_iMainCTIRunning = 1;
            sprintf( szLog, "cmd = %d, buf = %s, m_iMainCTIRunning = %d, and set_timer now",
                     MAIN_CTI_RESTORE, buf, m_MainEntry.m_iMainCTIRunning );
            TraceEx( szLog );
#ifdef _VC6_
			pDlg->SetTimer( 3, 15 * 1000, NULL );
#else
            m_MainEntry.ServerShutDown(15);
#endif
		}
		break;
	case MAIN_CTI_SHUTDOWN:
		{
            m_MainEntry.m_iMainCTIRunning = 0;
            sprintf( szLog, "check main_cti_shutdown now, m_iMainCTIRunning = %d", m_MainEntry.m_iMainCTIRunning );
            TraceEx( szLog );
		}
		break;
	default:
		break;
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, server_restore_event() " );
	}
}


CMiddleDll::CMiddleDll()
{
}

CMiddleDll::~CMiddleDll()
{
    middleDll.unload();
}

void CMiddleDll::MiddleDllInit()
{
	try
	{
#ifdef _VC6_
	m_hMidlleDll = NULL;
	m_hMidlleDll = LoadLibrary( "MiddlePipe.dll" );
	if( m_hMidlleDll == NULL )
	{
		AfxMessageBox( "MiddlePipe.dll 没找到！" );
		return;
	}

	lpfnRestoreEvent = Server_Restore_Event;

	lpSetCallbalck = ( LPSETCALLBACK )GetProcAddress( m_hMidlleDll, "SetCallback" );
	if( lpSetCallbalck == NULL )
	{
		AfxMessageBox( "GetProcAddress - lpSetCallbalck failed!" );
		return;
	}

	lpSetCallbalck( lpfnRestoreEvent );

	SendToDestFunc = ( SENDTODEST )GetProcAddress( m_hMidlleDll, "Send2Dest" );
	if( SendToDestFunc == NULL )
	{
		AfxMessageBox( "GetProcAddress - Send2Dest failed!" );
		return;
	}

	MiddleDllInitFunc = ( MIDDLEDLLINIT )GetProcAddress( m_hMidlleDll, "Init" );
	if( MiddleDllInitFunc == NULL )
	{
		AfxMessageBox( "GetProcAddress - middle_dll Init failed!" );
		return;
	}
	MiddleDllInitFunc( "cat" );

	BeginCheckAliveFunc = ( BEGINCHECKALIVE )GetProcAddress( m_hMidlleDll, "BeginCheckAlive" );
	if( BeginCheckAliveFunc == NULL )
	{
		AfxMessageBox( "GetProcAddress - BeginCheckAlive failed!" );
		return;
	}
	
	BeginCheckAliveFunc( "cat" );

//	SETKEEPALIVEFLAG	SetKeepAliveFlagFunc;
	SetKeepAliveFlagFunc = ( SETKEEPALIVEFLAG )GetProcAddress( m_hMidlleDll, "SetKeepAliveFlag" );
	if( SetKeepAliveFlagFunc == NULL )
	{
		AfxMessageBox( "GetProcAddress - SetKeepAliveFlag failed!" );
		return;
	}
#else
        if (!middleDll.load())
        {
            TraceEx( "error, middleDll.dll load failed!");
            return;
        }

        lpfnRestoreEvent = Server_Restore_Event;
        lpSetCallbalck = ( LPSETCALLBACK )middleDll.resolve("SetCallback" );
        if( lpSetCallbalck == NULL )
        {
            TraceEx( "GetProcAddress - lpSetCallbalck failed!" );
            return;
        }
        lpSetCallbalck( lpfnRestoreEvent );

        SendToDestFunc = ( SENDTODEST )middleDll.resolve( "Send2Dest" );
        if( SendToDestFunc == NULL )
        {
            TraceEx( "GetProcAddress - Send2Dest failed!" );
            return;
        }

        MiddleDllInitFunc = ( MIDDLEDLLINIT )middleDll.resolve( "Init" );
        if( MiddleDllInitFunc == NULL )
        {
            TraceEx( "GetProcAddress - middle_dll Init failed!" );
            return;
        }
        MiddleDllInitFunc( "cat" );

        BeginCheckAliveFunc = ( BEGINCHECKALIVE )middleDll.resolve( "BeginCheckAlive" );
        if( BeginCheckAliveFunc == NULL )
        {
            TraceEx( "GetProcAddress - BeginCheckAlive failed!" );
            return;
        }

        BeginCheckAliveFunc( "cat" );

    //	SETKEEPALIVEFLAG	SetKeepAliveFlagFunc;
        SetKeepAliveFlagFunc = ( SETKEEPALIVEFLAG )middleDll.resolve( "SetKeepAliveFlag" );
        if( SetKeepAliveFlagFunc == NULL )
        {
            TraceEx( "GetProcAddress - SetKeepAliveFlag failed!" );
            return;
        }
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CMiddleDll::middle_dll_init() " );
	}
}
