#include "GlobalTsapiServer.h"
#include "ocxdll.h"
#include "mainentry.h"


extern void TraceEx(char *szMessage,...);
extern CMainEntry m_MainEntry;

#ifdef _windows_
char lpszOCXDll[64] = {"ocxdll.dll"};
#else
char lpszOCXDll[64] = {"libocxdll.so"};
#endif

QLibrary ocxDll(lpszOCXDll);

void ocx_Interface_Event(void *wParam, void *lParam)
{
    //server - > client,  client_recv
    char *buf = (char *)lParam;
    if( buf == NULL )
        return;

    PACKET_HEAD *pHead = (PACKET_HEAD *)buf;

    char szLog[512] = {0};
    sprintf( szLog, "pHead->cmd=%d pHead->cmd_type=%d",pHead->cmd,pHead->cmd_type);
    TraceEx( szLog );

    string strInfo[32]= {""}, strData = "";
    if( pHead->dwSize > 0 )
    {
        strData = buf + sizeof(PACKET_HEAD);
        m_MainEntry.m_DataProc.DecodeFrame( strInfo, strData, "|", 1 );
    }

    switch( pHead->cmd)
    {
    case CMD_CONNECT_SERVER_SUCCESS:
        m_MainEntry.m_CtiClientModleEvent.OnConnectServerSucEvent();
        break;
     case CMD_CONNECT_SERVER_FAILED:
        m_MainEntry.m_CtiClientModleEvent.OnConnectServerFailedEvent();
        break;
    case CMD_REGISTERDN:
        m_MainEntry.m_CtiClientModleEvent.OnRegisterExtensionEvent(strInfo, pHead );
        break;
    case CMD_UNREGISTERDN:
        break;
    case CMD_EXTENSIONMSG_EVENT: //从trans_station_cti中转CTI服务端返回没有clientid;
    case CMD_SENDMSG2ALL:
        m_MainEntry.m_DataProc.Cmd_Sendmsg_Proc(-1, buf, strInfo, pHead->cmd==CMD_SENDMSG2ALL?true:false);
        break;
    default:
        break;
    }
}

COCXInterface::COCXInterface()
{

}

COCXInterface::~COCXInterface()
{
}

bool COCXInterface::Init()
{
    bool bResult = false;

    SetAgentTypeFunc = NULL;
    SetDualIPFunc = NULL;
    ConnectToServerFunc = NULL;
    RegisterExtentionFunc = NULL;
    SendCmdFunc = NULL;
    SendMsg2ExtFunc = NULL;
    SendMsg2AllFunc = NULL;
    lpSetOCXEventCallbalck = NULL;
    SetAgentParamFunc = NULL;

    char szLog[512] = {0};
    if (!ocxDll.load())
    {
        sprintf( szLog, "error, ocxDll.dll load failed!");
        TraceEx( szLog );
        return bResult;
    }

    SetAgentTypeFunc = ( SETAGENTTYPE )ocxDll.resolve( "SetAgentType" );
    if( SetAgentTypeFunc == NULL )
    {
        sprintf( szLog, "GetProcAddress - SetAgentType failed!" );
        TraceEx( szLog );
        return bResult;
    }

    //SetDualIPFunc
    SetDualIPFunc = ( SETDUALIP )ocxDll.resolve( "SetDualIP" );
    if( SetDualIPFunc == NULL )
    {
        sprintf( szLog, "GetProcAddress - SetDualIP failed!" );
        TraceEx( szLog );
        return bResult;
    }

    //ConnectToServerFunc
    ConnectToServerFunc = ( CONNECTTOSERVER )ocxDll.resolve( "ConnectToServer" );
    if( ConnectToServerFunc == NULL )
    {
        sprintf( szLog, "GetProcAddress - ConnectToServer failed!" );
        TraceEx( szLog );
        return bResult;
    }

    //RegisterExtentionFunc;
    RegisterExtentionFunc = ( REGISTEREXTENSION )ocxDll.resolve( "RegisterExtention" );
    if( RegisterExtentionFunc == NULL )
    {
        sprintf( szLog, "GetProcAddress - RegisterExtention failed!" );
        TraceEx( szLog );
        return bResult;
    }

    //SendCmdFunc
    SendCmdFunc = ( SENDCMD )ocxDll.resolve( "SendCmd" );
    if( SendCmdFunc == NULL )
    {
        sprintf( szLog, "GetProcAddress - SendCmdFunc failed!" );
        TraceEx( szLog );
        return bResult;
    }

    //SendMsg2ExtFunc
    SendMsg2ExtFunc = ( SENDMSG2EXTEINTION )ocxDll.resolve( "SendMsg2Ext" );
    if( SendMsg2ExtFunc == NULL )
    {
        sprintf( szLog, "GetProcAddress - SendMsg2Ext failed!" );
        TraceEx( szLog );
        return bResult;
    }

    //SENDMSG2ALL                     SendMsg2AllFunc;
    SendMsg2AllFunc = ( SENDMSG2ALL )ocxDll.resolve( "SendMsg2All" );
    if( SendMsg2AllFunc == NULL )
    {
        sprintf( szLog, "GetProcAddress - SendMsg2All failed!" );
        TraceEx( szLog );
        return bResult;
    }

    //lpSetOCXEventCallbalck;
    lpfnOCXInterfaceEvent = ocx_Interface_Event;
    lpSetOCXEventCallbalck = ( LPSETOCXINTERFACEEVENTCALLBACK )ocxDll.resolve( "SetSockMsgCallback" );
    if( lpSetOCXEventCallbalck == NULL )
    {
        sprintf( szLog, "ocx_dll, GetProcAddress - SetSockMsgCallback failed!" );
        TraceEx( szLog );
        return bResult;
    }
    lpSetOCXEventCallbalck( lpfnOCXInterfaceEvent );

    //SETAGENTPARAM                   SetAgentParamFunc;
    SetAgentParamFunc = ( SETAGENTPARAM )ocxDll.resolve( "SetAgentParam" );
    if( SetAgentParamFunc == NULL )
    {
        sprintf( szLog, "GetProcAddress - SetAgentParam failed!" );
        TraceEx( szLog );
        return bResult;
    }

    bResult = true;
    return bResult;
}

void COCXInterface::SendMsg2Extension(char *srcDN, char *destDN, char *param1, char *param2, char *param3)
{
    if( srcDN == NULL || destDN == NULL || param1 == NULL || param2 == NULL || param3 == NULL )
        return;

    SendMsg2ExtFunc(srcDN, destDN, param1, param2, param3);
}

bool COCXInterface::ConnectToServer()
{
    SetAgentTypeFunc( loadbalance_cti_server_type );
    SetDualIPFunc( m_MainEntry.m_lpszPeerIPAddr, m_MainEntry.m_lpszPeerIPAddr );

    char szLog[512] = {0};
    sprintf( szLog, "ocx_interface, connect to server, m_lpszPeerIPAddr = %s, peerPort = %d",
             m_MainEntry.m_lpszPeerIPAddr, m_MainEntry.m_iPeerPort );
    TraceEx( szLog );

    return ConnectToServerFunc( m_MainEntry.m_iPeerPort );
}

void COCXInterface::SetTransferedData(char *szExtNo, char *TansferedData)
{
    char szInfo[128] = {0};
    sprintf( szInfo, "%s|%s", szExtNo, TansferedData );

    if(SendCmdFunc != NULL)
        SendCmdFunc(CMD_SETTRANSDATA, szInfo, strlen(szInfo) );
}
