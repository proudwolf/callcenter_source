#include "commonenginedll.h"
#include "dataproc.h"

extern void TraceEx( char *szMessage, ...);
extern CCommonEngineDll m_CommonEngineDll;
extern CDataProc   m_DataProc;

#ifdef _windows_
char lpszCommEngine[64] = {"CommEngine.dll"};
#else
char lpszCommEngine[64] = {"libCommEngine.so"};
#endif

QLibrary commEngineDll(lpszCommEngine);

void CommEngineSocketMsgEvent(void *wParam, void *lParam)
{
    //TraceEx( "----------commengine-msg_event-------------");
    try
    {
    if( wParam == NULL )
    {
        TraceEx( "error: OnNetworkMsg----wParam == NULL" );
        return;
    }

    //TraceEx( "CTsapiServerDlg::OnNetworkMsg begin" );

    PCONNECTINFO pInfo = (PCONNECTINFO)wParam;
    switch( pInfo->iNetworkEvent )
    {
    case ON_NETWORK_RECV://OnRecv
        {
        m_DataProc.data_proc_from_cti( pInfo->iClientId, (char *)lParam );
        m_CommonEngineDll.RecvCompleteFunc( pInfo->iThreadId );
        }
        break;
    case ON_NETWORK_CLOSE://OnClose
        {
        char szLog[256] = {0};
        sprintf( szLog, "OnNetworkMsg----OnClose, iClientId = %d", pInfo->iClientId );
        TraceEx( szLog );
        m_DataProc.OnClientClose( pInfo->iClientId );
        }
        break;
    default:
        TraceEx( "warnning: OnNetworkMsg-----OtherMsg");
        break;
    }

//	TraceEx( "CTsapiServerDlg::OnNetworkMsg end" );
    }
    catch(...)
    {
        TraceEx( "error, exception, CTsapiServerDlg::on_network_msg() " );
    }
}


CCommonEngineDll::CCommonEngineDll()
{

}

CCommonEngineDll::~CCommonEngineDll()
{

}

void CCommonEngineDll::ParamInit()
{
    QSettings *configIni = new QSettings("AESServer.ini", QSettings::IniFormat);

    QString strServerPort = configIni->value("/AESServer/ServerPort").toString();

    strServerPort = (strServerPort.length() > 0)?strServerPort:"21893";
    m_iPort = atoi( (char *)strServerPort.toStdString().c_str() );
    configIni->setValue("/AESServer/ServerPort", strServerPort);

    char szLog[256] = {0};
    sprintf( szLog, "param_init, set servr port, port = %d", m_iPort );
    TraceEx( szLog );

    delete configIni;
}

bool CCommonEngineDll::DllInit()
{
#ifdef _VC6_
    //CommEngine.dll
        m_hSocketDll = NULL;
        m_hSocketDll = LoadLibrary( "CommEngine.dll" );
        if( m_hSocketDll == NULL )
        {
            AfxMessageBox( "CommEngine.dll cant find" );
            return FALSE;
        }

        RegNetworkHwndFunc = ( REGNETWORKHWND )GetProcAddress( m_hSocketDll, "RegWnd" );
        if( RegNetworkHwndFunc == NULL )
        {
            AfxMessageBox( "GetProcAddress - RegWnd failed!" );
            return FALSE;
        }
        RegNetworkHwndFunc( this->m_hWnd, ON_NETWORK_MSG );

        SetPortFunc = ( SETPORT )GetProcAddress( m_hSocketDll, "SetPort" );
        if( SetPortFunc == NULL )
        {
            AfxMessageBox( "GetProcAddress - SetPort failed!" );
            return FALSE;
        }
        int iPort = atoi( (LPSTR &)m_strPort );
        SetPortFunc( iPort );

        RecvCompleteFunc = ( RECVCOMPLETE )GetProcAddress( m_hSocketDll, "EndRecv" );
        if( RecvCompleteFunc == NULL )
        {
            AfxMessageBox( "GetProcAddress - EndRecv failed!" );
            return FALSE;
        }

        SendToClientFunc = ( SENDTOCLIENT )GetProcAddress( m_hSocketDll, "Send2Client" );
        if( SendToClientFunc == NULL )
        {
            AfxMessageBox( "GetProcAddress - Send2Client failed!" );
            return FALSE;
        }

        CloseSockFunc = ( CLOSESOCK )GetProcAddress( m_hSocketDll, "CloseASock" );
        if( CloseSockFunc == NULL )
        {
            AfxMessageBox( "GetProcAddress - CloseASock failed!" );
            return FALSE;
        }

        NetworkInitFunc = ( NETWORKINIT )GetProcAddress( m_hSocketDll, "StartSocketEngine" );
        if( NetworkInitFunc == NULL )
        {
            AfxMessageBox( "GetProcAddress - StartSocketEngine failed!" );
            return FALSE;
        }
        if( !NetworkInitFunc() )
        {
            AfxMessageBox( "StartSocketEngine failed" );
            return FALSE;
        }

        TerminateDllThreadFunc = ( TERMINATEDLLTHREAD )GetProcAddress( m_hSocketDll, "EndSession" );
        {
            if( TerminateDllThreadFunc == NULL )
            {
                AfxMessageBox( "GetProcAddress EndSession failed" );
                return FALSE;
            }
        }
#else

    if (!commEngineDll.load())
    {
        TraceEx( "error, commEngine.dll load failed!");
        return false;
    }

    SetSocketMsgEventCallbackFunc = (LPSETSOCKETMSGEVENTCALLBACK)commEngineDll.resolve("SetSockMsgCallback");
    if( SetSocketMsgEventCallbackFunc == NULL )
    {
        TraceEx( "error, GetProcAddress - SetSockMsgCallback failed!" );
        return false;
    }
    SetSocketMsgEventCallbackFunc( CommEngineSocketMsgEvent );

    SetPortFunc = (SETPORT)commEngineDll.resolve("SetPort");
    if( SetPortFunc == NULL )
    {
        SetPortFunc = (SETPORT)commEngineDll.resolve("SetServerPort");
        if( SetPortFunc == NULL )
        {
            TraceEx( "error, GetProcAddress - SetPort failed!" );
            return false;
        }
    }

    SetPortFunc( m_iPort );

    RecvCompleteFunc = ( RECVCOMPLETE )commEngineDll.resolve( "EndRecv" );
    if( RecvCompleteFunc == NULL )
    {
        TraceEx( "GetProcAddress - EndRecv failed!" );
        return false;
    }

    SendToClientFunc = ( SENDTOCLIENT )commEngineDll.resolve( "Send2Client" );
    if( SendToClientFunc == NULL )
    {
        TraceEx( "GetProcAddress - Send2Client failed!" );
        return false;
    }

    CloseSockFunc = ( CLOSESOCK )commEngineDll.resolve( "CloseASock" );
    if( CloseSockFunc == NULL )
    {
        TraceEx( "GetProcAddress - CloseASock failed!" );
        return false;
    }

    NetworkInitFunc = ( NETWORKINIT )commEngineDll.resolve( "StartSocketEngine" );
    if( NetworkInitFunc == NULL )
    {
        TraceEx( "GetProcAddress - StartSocketEngine failed!" );
        return false;
    }
    if( !NetworkInitFunc() )
    {
        TraceEx( "StartSocketEngine failed" );
        return false;
    }

    TerminateDllThreadFunc = ( TERMINATEDLLTHREAD )commEngineDll.resolve( "EndSession" );
    {
        if( TerminateDllThreadFunc == NULL )
        {
            TraceEx( "GetProcAddress EndSession failed" );
            return false;
        }
    }

#endif
    return true;
}
