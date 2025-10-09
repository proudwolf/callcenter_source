#include "GlobalTsapiServer.h"
#include "cticlientmodleevent.h"
#include "mainentry.h"

extern void TraceEx( char *buf, ...);

extern CMainEntry m_MainEntry;

CCtiClientModleEvent::CCtiClientModleEvent()
{
}

CCtiClientModleEvent::~CCtiClientModleEvent()
{
}

void CCtiClientModleEvent::OnConnectServerSucEvent()
{
    m_MainEntry.m_ocxInterface.RegisterExtentionFunc(m_MainEntry.m_lpszVirtualCTIExt);
    char szLog[512] = {0};
    sprintf( szLog, "cti_client_modle_event, connect server success and register ext, extension = %s",
            m_MainEntry.m_lpszVirtualCTIExt );
    TraceEx( szLog );
}

void CCtiClientModleEvent::OnConnectServerFailedEvent()
{
    TraceEx( "warnning, cti_client_modle_event conncet server failed");
}

void CCtiClientModleEvent::OnRegisterExtensionEvent(string *strInfo, PACKET_HEAD *pHead)
{
    if( strInfo == NULL || pHead == NULL )
        return;

    char szLog[512] = {0};
    if( pHead->cmd_type == RC_RESPONSE_OK )
    {
        if( m_MainEntry.m_iCTIType == loadbalance_cti_type )
        {
            m_MainEntry.m_ocxInterface.SendMsg2ExtFunc( m_MainEntry.m_lpszVirtualCTIExt, "VSQL",  "AgentLogin",
                m_MainEntry.m_lpszVirtualCTIExt, "AgentReady" );
        }

        sprintf( szLog, "register extension success, ext = %s", (char *)strInfo[0].c_str() );
    }
    else
    {
        sprintf( szLog, "warinning, register extension failed, ext = %s", (char *)strInfo[0].c_str() );
    }

    TraceEx( szLog );
}

void CCtiClientModleEvent::OnCallMsgEvent(char * szDestExtNo, char * SourceNumber, char * Param1, char * Param2, char * Param3)
{

}
