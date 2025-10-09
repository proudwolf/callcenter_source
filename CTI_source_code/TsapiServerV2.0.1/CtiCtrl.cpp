#define _linux_
//define _VC6_

#ifdef _VC6_
#include "stdafx.h"
#include "TsapiServer.h"
#include "TsapiServerDlg.h"
extern CTsapiServerDlg *g_lpthis;
#endif

//#ifdef _linux_
#include "GlobalTsapiServer.h"
//#endif

#include <pthread.h>
#include "CtiCtrl.h"
#include "mainentry.h"

extern CMainEntry m_MainEntry;

extern void TraceEx( char *buf, ...);
char escbuf[1024*10] = {0};

extern string   m_sServerName;
extern string	m_sLoginName;
extern string	m_sPassword;

void  AutoReconnect(void *param, DWORD  dwTime);
void  SysStatTimer();

pthread_t state_request_iThreadId = 0;

void *sysStateThread(void *)
{
    while( 1 )
    {
        sleep( 5 );
        SysStatTimer();
    }
}

void *autoReconnectThread(void *)
{
    while( 1 )
    {
        if( !m_MainEntry.m_bAutoReconnect )
            break;

        AutoReconnect( NULL, 0 );
        sleep( 5 );
    }
}


void On_Acs_Notify_Event(void *wParam, void *lParam)
{
    m_MainEntry.m_CtiEvent.CtiEvent( wParam, lParam );
}

CCtiCtrl::CCtiCtrl()
{
	m_nRetCode = 0;
	m_sProgName = "CstaTst32";
	my_setTimer = 0;
	pPrivateData = NULL;
	m_nACSLevel=ACS_LEVEL1;
	m_iState = 1;
}

CCtiCtrl::~CCtiCtrl()
{
}

bool CCtiCtrl::AcsOpenStream(StreamType_t apiLevel,bool bReconnect)
{	
	try
	{
	pPrivateData = (PrivateData_t*) escbuf;
	strcpy (pPrivateData->vendor,"VERSION");
	memcpy (pPrivateData->data,"\0ECS#2-6\0",9);
	pPrivateData->length=9;

    m_nRetCode = acsOpenStream (&m_MainEntry.m_acsHandle,
                LIB_GEN_ID,
                RC_INVOKEID,
				// Changed by G.L. 11 february 98
				apiLevel,
                //ST_CSTA2,
                (ServerID_t *)m_sServerName.c_str(),
                (LoginID_t *)m_sLoginName.c_str(),
                (Passwd_t *)m_sPassword.c_str(),
                (AppName_t *)m_sProgName.c_str(),
                m_nACSLevel,
                (Version_t *)CSTA_API_VERSION,
                0,
                0,
                0,
                0,
                pPrivateData);

	TraceEx("ACS REQUEST Open Stream");
	if (m_nRetCode < 0) 
	{
        char szLog[256] = {0};
        sprintf( szLog, "ACS REQUEST Open Stream ERROR, m_nRetCode = %d, m_sServerName = %s",
                 m_nRetCode, (char *)m_sServerName.c_str() );

        TraceEx ( szLog );
		AcsCloseStream();

        TryToReconnect();	/*YB 240800*/
        return false;
	}

    m_MainEntry.AcsGetEventBlock(0,0);	//once to receive the ACS_OPEN_STREAM_CONF

#ifdef _VC6_
    m_nRetCode = acsEventNotify( m_MainEntry.m_acsHandle, m_hWnd, WM_ACSEVENT, true);
	if (m_nRetCode != ACSPOSITIVE_ACK)
		TraceEx ("ACS Event Notify Error");
#endif
    /*
    AEAddressDesc *targetAddr;
    m_nRetCode = acsEventNotify (m_MainEntry.m_acsHandle, targetAddr, true);
    */
    m_lpfnAcsNotifyEvent = On_Acs_Notify_Event;
    acsEventNotifyCallback( m_lpfnAcsNotifyEvent );

	MonitorAllVDN(); // add by liu, 2013-4-25 19:13

	if( bReconnect )
        m_MainEntry.m_DataProc.RemonitorAllExt();

    return true;
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::acs_open_stream() " );
	}
    return false;
}


#ifdef _VC6_
void CALLBACK AutoReconnect(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
//	m_MainEntry.m_DataProc.CloseAllClient();
	try
	{
	g_lpthis->m_CtiCtrl.AcsOpenStream(ST_CSTA, TRUE);
	}
	catch(...)
	{
		TraceEx( "error, exception, auto_reconnect() " );
	}
	return;
}
#endif

#ifndef _VC6_
void  AutoReconnect(void *param, DWORD  dwTime)
{
//	m_MainEntry.m_DataProc.CloseAllClient();
    try
    {
        m_MainEntry.m_CtiCtrl.AcsOpenStream(ST_CSTA, TRUE);
    }
    catch(...)
    {
        TraceEx( "error, exception, auto_reconnect() " );
    }
    return;
}
#endif

void CCtiCtrl::TryToReconnect()
{
	try
	{
    if (!m_MainEntry.m_bAutoReconnect)
	{
        m_MainEntry.m_bAutoReconnect = true;

        pthread_t iThreadId;
        if( pthread_create(&iThreadId, NULL, autoReconnectThread, NULL) != 0 )
        {
            printf("Create pthread error!\n");
            return;
        }

        //m_MainEntry.my_reconnectTimer = SetTimer(NULL, 0, 5000, AutoReconnect);

		TraceEx("TryToReconnect Started");
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::try_to_reconnect() " );
	}
}

void CCtiCtrl::AcsCloseStream()
{
	try
	{
    if(m_MainEntry.m_bOpenedStream)
	{
        m_nRetCode = acsCloseStream (m_MainEntry.m_acsHandle,
				  (InvokeID_t) 0,
				  pPrivateData);
		if (m_nRetCode < 0) 
		{
			/*Essai+*/
			//      MessageBoxError ("acsCloseStream", m_nRetCode);
			TraceEx("ACS REQUEST Close Stream ERROR, m_nRetCode = %d", m_nRetCode);
			/*Essai-*/

			exit( 0 );	//add by liu
			return;
		}
		TraceEx ("ACS REQUEST Close Stream");
		//theApp.m_bPhoneOn = FALSE;
        m_MainEntry.m_bOpenedStream = FALSE;
		TraceEx("CSTA Application. Not Connected");
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, ctictrl::acs_close_stream() " );
	}
}

#ifdef _VC6_
void CCtiCtrl::SetOwner(void *lpVoid)
{
	if( lpVoid != NULL )
		g_lpthis = (CTsapiServerDlg *)lpVoid;
}
#endif

#ifdef _VC6_
void CALLBACK SysStatTimer(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	try
	{
	TraceEx("SysStatTimer");
	((CTsapiServerDlg*) AfxGetMainWnd())->m_CtiCtrl.CstaSysStatReqConfEventTimer ();
	::KillTimer(hwnd, idEvent);
	return;
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::sys_stat_timer() " );
	}
}
#endif

void SysStatTimer()
{
    m_MainEntry.m_CtiCtrl.CstaSysStatReq();
}

void CCtiCtrl::CstaSysStatReq()
{
	try
	{
    m_nRetCode = cstaSysStatReq (m_MainEntry.m_acsHandle,
                 (InvokeID_t)RC_INVOKEID,
                 pPrivateData);
	if (m_nRetCode < 0) 
	{
	/*Essai+*/
	//    MessageBoxError ("cstaSysStatReq", m_nRetCode);
		TraceEx("CSTA REQUEST System Status Request ERROR, m_nRetCode = %d", m_nRetCode);
		
		if( m_nRetCode == -10 )
		{
			exit( 0 );	//add by liu, 2010-12-06
			return;
		}
	/*Essai-*/
	/*YB 240800+*/
	  // kill the periodic KeepAlive timer
        m_MainEntry.m_bAutoKeepAlive = TRUE;
        m_MainEntry.AutoKeepalive();
		AcsCloseStream();	// add by liu, 2010-11-30
		TryToReconnect();
	/*YB 240800-*/
		return;
		
	}
	TraceEx ("CSTA REQUEST System Status Request");

	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::csta_sys_stat_req() " );
	}
}

void CCtiCtrl::CstaSysStatReqConfEvent()
{
	try
	{
	CSTASysStatReqConfEvent_t *pEvent;
    string eventString;

    pEvent = &m_MainEntry.m_CtiEvent.m_cstaEvent.event.cstaConfirmation.u.sysStatReq;

    char lpszTempString[256] = {0};
    sprintf(lpszTempString, "CSTA CONFIRMATION System Status: %d",pEvent->systemStatus);
    eventString = lpszTempString;
	eventString += AddSystemStatus (pEvent->systemStatus);
    TraceEx ((char *)eventString.c_str());
	/*YB 240800+*/
	if (pEvent->systemStatus < 0 || pEvent->systemStatus == 4 )
	{
	  // kill the periodic KeepAlive timer
        m_MainEntry.m_bAutoKeepAlive = TRUE;
        m_MainEntry.AutoKeepalive();
		AcsCloseStream();
		TryToReconnect();
	}
	/*YB 240800-*/
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::csta_sys_stat_req_conf_event() " );
	}
}

void CCtiCtrl::CstaSysStatReqConfEventTimer()
{
	try
	{
	CSTASysStatReqConfEvent_t *pEvent;
    string eventString;

    pEvent = &m_MainEntry.m_CtiEvent.m_cstaEvent.event.cstaConfirmation.u.sysStatReq;
	pEvent->systemStatus = SS_DISABLED;
	eventString = "CSTA CONFIRMATION System Status: ";
	eventString += AddSystemStatus (pEvent->systemStatus);
    m_MainEntry.m_CtiEvent.DisplayString(eventString);
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::csta_sys_stat_req_conf_event_timer() " );
	}
}

string CCtiCtrl::AddSystemStatus(SystemStatus_t systemStatus)
{
	try
	{
    string eventString;
	char str[15];

	switch (systemStatus) {
	case SS_INITIALIZING :
	  eventString = "Initializing";
	  break;
	case SS_ENABLED :
	  eventString = "Enabled";
	  break;
	case SS_NORMAL :
	  eventString = "Normal";
	  break;
	case SS_MESSAGES_LOST :
	  eventString = "Messages lost";
	  break;
	case SS_DISABLED :
	  eventString = "Disabled";
	  break;
	case SS_OVERLOAD_IMMINENT :
	  eventString = "Overload imminent";
	  break;
	case SS_OVERLOAD_REACHED :
	  eventString = "Overload reached";
	  break;
	case SS_OVERLOAD_RELIEVED :
	  eventString = "Overload relieved";
	  break;
	default :
	  sprintf (str, "%ld", systemStatus);
	  eventString += str;
	  break;
	}
	return (eventString);
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::add_system_status() " );
	}
	return "";
}

int CCtiCtrl::CstaMonitorDevice(const char *szExtNo)
{
	try
	{

	CSTAMonitorFilter_t monFilter;
	ATTPrivateFilter_t attFilter;
    DeviceID_t deviceID;

	CSTAMonitorFilter_t* pFilter= &monFilter;

	pFilter->call = 0;
	pFilter->feature = 0;
	pFilter->agent = 0;
	pFilter->maintenance = 0;
	pFilter->privateFilter = 0;

	attFilter = ATT_ENTERED_DIGITS_FILTER|ATT_CHARGE_ADVICE_FILTER;

	strcpy (deviceID, szExtNo);

	ATTPrivateData_t my_correlatorData;
    memset(my_correlatorData.data, 0, sizeof(my_correlatorData.data));

	my_correlatorData.length = 0;
	strcpy(my_correlatorData.vendor,ECS_VENDOR_STRING);

    char szLog[256] = {0};

    m_nRetCode = cstaMonitorDevice (m_MainEntry.m_acsHandle,
                    RC_INVOKEID,
                    &deviceID,
                    &monFilter,
                    pPrivateData);

    if (m_nRetCode < 0) 
	{
        sprintf( szLog, "cstaMonitorDevice Failed errcode=%d, deviceID = %s",
                m_nRetCode, deviceID);
        TraceEx( szLog );
	}
	else
	{
        sprintf( szLog, "CSTA REQUEST Monitor Device:%s ",szExtNo);
        TraceEx( szLog );
	}

	return m_nRetCode;
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::csta_monitor_device() " );
	}
	return 0;
}

void CCtiCtrl::CstaSetAgentState(int nAgentCmd, char *szExtNo, const char *sAgentID, const char *sAgentGroup, const char *sAgentPassword, int iClientId)
{
	try
	{
	if( iClientId <= 0 )
		return;

    DeviceID_t deviceID;          
    AgentID_t agentID;
    AgentGroup_t agentGroup;
    AgentPassword_t agentPassword;
	int nAgentMode;
    
	switch(nAgentCmd) 
	{
	case CMD_AGENTLOGON:
		{
			nAgentMode=0;
            m_MainEntry.m_DataProc.AddExtAgent( szExtNo, (char *)sAgentID );
		}
		break;
	case CMD_AGENTREADY:
		nAgentMode=3;
		break;
	case CMD_AGENTLOGOFF:
		{
			nAgentMode=1;
            m_MainEntry.m_DataProc.DelExtAgentMap( szExtNo, (char *)sAgentID );
		}
		break;
	case CMD_AGENTNOTREADY:
		nAgentMode=2;
		break;
	default:
		break;
	}
	
    strcpy (deviceID, szExtNo);
    strcpy (agentID, sAgentID);
    strcpy (agentGroup, sAgentGroup);
    strcpy (agentPassword, sAgentPassword);
	
    m_nRetCode = cstaSetAgentState (m_MainEntry.m_acsHandle,
            (InvokeID_t)RC_INVOKEID,
            (DeviceID_t FAR *)&deviceID,
            (AgentMode_t)nAgentMode,
            (AgentID_t FAR *)&agentID,
            (AgentGroup_t FAR *)&agentGroup,
            (AgentPassword_t FAR *)&agentPassword,
            NULL);
	
    if (m_nRetCode < 0) 
	{
		char szCode[50]={0};
		sprintf(szCode,"%s|%d",szExtNo,m_nRetCode);
        m_MainEntry.m_DataProc.Send_command(iClientId, nAgentCmd,RC_RESPONSE_FAILED,szCode,strlen(szCode));
		return;
    }
    m_MainEntry.m_DataProc.Send_command(iClientId, nAgentCmd,RC_RESPONSE_OK,szExtNo,strlen(szExtNo));
    string eventString;
    
    TraceEx("CSTA REQUEST Set Agent State for Device ");
    eventString += deviceID;
    eventString += " Agent Mode: ";
    switch (nAgentMode) 
	{
      case 0 :
        eventString += "LOG IN";
        break;
      case 1 :
        eventString += "LOG OUT";
        break;
      case 2 :
        eventString += "NOT READY";
        break;
      case 3 :
        eventString += "READY";
        break;
      case 4 :
        eventString += "WORK NOT READY";
        break;
      case 5 :
        eventString += "WORK READY";
        break;
      case 6 :
        eventString += "NO ANSWER EXTERNAL";
        break;
      default :
        eventString += "UNKNOW TYPE";
        break;
    }
    eventString += ", Agent ID: ";
    eventString += agentID;
    eventString += ", Agent Group: ";
    eventString += agentGroup;
    eventString += ", Agent Password: ";
    eventString += agentPassword;
    m_MainEntry.m_CtiEvent.DisplayString (eventString);
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::csta_set_agent_state() " );
	}
}

void CCtiCtrl::CstaHoldCall(int iClientId, PEXTINFO pEInfo)
{
	try
	{
	if( pEInfo == NULL )
	{
		TraceEx( "csta hold call failed, pEInfo == NULL" );
		return;
	}
	ConnectionID_t connectionID;  

	connectionID.callID = pEInfo->nCallID;
	strcpy (connectionID.deviceID, pEInfo->szExtNo);
	connectionID.devIDType = (ConnectionID_Device_t)(int)STATIC_ID;
    m_nRetCode = cstaHoldCall (m_MainEntry.m_acsHandle,
		(InvokeID_t)RC_INVOKEID,
		(ConnectionID_t FAR *)&connectionID,
		FALSE,
		pPrivateData);
	if (m_nRetCode < 0) 
	{
		char szCode[50]={0};
		sprintf(szCode,"%s|%d",pEInfo->szExtNo,m_nRetCode);
	
        m_MainEntry.m_DataProc.Send_command(iClientId,CMD_HOLDCALL,RC_RESPONSE_FAILED,szCode,strlen(szCode));
		return;
	}
    string eventString;

    m_MainEntry.m_CtiEvent.DisplayString("CSTA REQUEST Hold Call ");
    char lpszTempStr[256] = {0};
    sprintf( lpszTempStr," Call ID: %d  Device ID: %s",pEInfo->nCallID,pEInfo->szExtNo);
    eventString = lpszTempStr;
    m_MainEntry.m_CtiEvent.DisplayString(eventString);

	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::csta_hold_call() " );
	}
}

void CCtiCtrl::CstaClearConnection(int iClientId, PEXTINFO pEInfo, char *lpszGradeFlag)
{
	try
	{
    if( pEInfo == NULL || lpszGradeFlag == NULL )
	{
		TraceEx( "csta clear connection failed, pEInfo == NULL" );
		return;
	}

    ConnectionID_t connectionID;
    memset( connectionID.deviceID, 0, 64 );

	connectionID.callID = pEInfo->nCallID;

    if( m_MainEntry.m_iPBXType == 1 )
    {
        sprintf(connectionID.deviceID, "%s|%s", pEInfo->szExtNo, lpszGradeFlag );
    }
    else
    {
        strcpy (connectionID.deviceID, pEInfo->szExtNo);
    }

	connectionID.devIDType = (ConnectionID_Device_t)(int)STATIC_ID;

    m_nRetCode = cstaClearConnection(m_MainEntry.m_acsHandle,
								(InvokeID_t)RC_INVOKEID,
								(ConnectionID_t FAR *)&connectionID,
								NULL);


	if (m_nRetCode < 0) 
	{
		char szCode[50]={0};
		sprintf(szCode,"%s|%d",pEInfo->szExtNo,m_nRetCode);
		
        m_MainEntry.m_DataProc.Send_command(iClientId,CMD_HANGUP,RC_RESPONSE_FAILED,szCode,strlen(szCode));
		return;
	}
    string eventString;

    m_MainEntry.m_CtiEvent.DisplayString("CSTA REQUEST Clear Connection V2");
    char lpszTempStr[256] = {0};
    sprintf( lpszTempStr, " Call ID: %d  Device ID: %s",pEInfo->nCallID,pEInfo->szExtNo);
    eventString = lpszTempStr;
    m_MainEntry.m_CtiEvent.DisplayString(eventString);
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::csta_clear_connection() " );
	}
}

void CCtiCtrl::CstaAnswerCall(int iClientId, PEXTINFO pE)
{
	try
	{
	if( pE == NULL )
	{
		TraceEx( "csta answer call failed, pE == NULL " );
		return;
	}

	ConnectionID_t connectionID;  

    connectionID.callID = pE->nCallID;
	strcpy (connectionID.deviceID, pE->szExtNo);
    connectionID.devIDType = (ConnectionID_Device_t)(int)STATIC_ID;
    m_nRetCode = cstaAnswerCall (m_MainEntry.m_acsHandle,
            (InvokeID_t)RC_INVOKEID,
            (ConnectionID_t FAR *)&connectionID,
            pPrivateData);
    if (m_nRetCode < 0) 
	{
		char szCode[50]={0};
		sprintf(szCode,"%s|%d",pE->szExtNo,m_nRetCode);
		if( iClientId > 0 )
            m_MainEntry.m_DataProc.Send_command(iClientId,CMD_PICKUP,RC_RESPONSE_FAILED,szCode,strlen(szCode));
		return;
    }
    string eventString;

    m_MainEntry.m_CtiEvent.DisplayString("CSTA REQUEST Answer Call ");
    char lpszTempStr[256] = {0};
    sprintf( lpszTempStr, " Call ID: %d  Device ID: %s",pE->nCallID,pE->szExtNo);
    eventString = lpszTempStr;
    m_MainEntry.m_CtiEvent.DisplayString(eventString);
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::csta_answer_call() " );
	}
}

void CCtiCtrl::CstaRetrieveCall(int iClientId, PEXTINFO pE)
{
	try
	{
	if( pE == NULL )
	{
		TraceEx( "csta retrieve call failed, pE == NULL" );
		return;
	}

	ConnectionID_t connectionID;  

//	connectionID.callID = pE->nCallID;
	connectionID.callID = pE->nHeldCallID;

	strcpy (connectionID.deviceID, pE->szExtNo);
    connectionID.devIDType = (ConnectionID_Device_t)(int)STATIC_ID;
    m_nRetCode = cstaRetrieveCall (m_MainEntry.m_acsHandle,
            (InvokeID_t)RC_INVOKEID,
            (ConnectionID_t FAR *)&connectionID,
            pPrivateData);
    if (m_nRetCode < 0) 
	{
		char szCode[50]={0};
		sprintf(szCode,"%s|%d",pE->szExtNo,m_nRetCode);
		if( iClientId > 0 )
            m_MainEntry.m_DataProc.Send_command(iClientId,CMD_RETRIEVECALL,RC_RESPONSE_FAILED,szCode,strlen(szCode));
		return;
    }
    string eventString;
   
    m_MainEntry.m_CtiEvent.DisplayString("CSTA REQUEST Retrieve Call ");
    char lpszTempStr[256] = {0};
    sprintf( lpszTempStr, " Call ID: %d  Device ID: %s",pE->nCallID,pE->szExtNo);
    eventString = lpszTempStr;
    m_MainEntry.m_CtiEvent.DisplayString(eventString);
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::csta_retrieve_call() " );
	}
}

void CCtiCtrl::CstaConsultationCall(int iClientId, DeviceID_t called, PEXTINFO pE, DeviceID_t orient_calling)
{
	try
	{
    char szLog[256] = {0};
    sprintf( szLog, "csta consultation call v2, iClientId = %d, called = %s, orient_calling = %s", iClientId, called, orient_calling );
    TraceEx( szLog );

	if( pE == NULL )
	{
		TraceEx( "error, csta consultation failed, pE == NULL" );
		return;
	}

	ConnectionID_t connectionID;
	CalledDeviceID_t calledDeviceID;  
	connectionID.callID =  pE->nCallID ;
	strcpy (connectionID.deviceID, pE->szExtNo);
	connectionID.devIDType = STATIC_ID;
	strcpy (calledDeviceID.deviceID, called);
	calledDeviceID.deviceIDType = DEVICE_IDENTIFIER;
	calledDeviceID.deviceIDStatus = ID_NOT_KNOWN;

	PrivateData_t	my_correlatorData;
	
    if( m_MainEntry.m_iSetUUID == 0 )
	{
		my_correlatorData.length = 0;
		my_correlatorData.data[0] = '\0';
		my_correlatorData.vendor[0]= '\0';
	}
	else
	{
		strcpy (my_correlatorData.data, orient_calling);
		my_correlatorData.length = strlen( orient_calling );

		my_correlatorData.vendor[0]= '\0';
	}
	
    m_nRetCode = cstaConsultationCall (m_MainEntry.m_acsHandle,
               (InvokeID_t)				RC_INVOKEID,
               (ConnectionID_t FAR *)	&connectionID,
               (DeviceID_t FAR *) &calledDeviceID.deviceID,
			   (PrivateData_t FAR *) &my_correlatorData);


	if (m_nRetCode < 0)
	{
		char szCode[50]={0};
		sprintf(szCode,"%s|%d",pE->szExtNo,m_nRetCode);
		if( iClientId > 0 )
            m_MainEntry.m_DataProc.Send_command(iClientId,CMD_INITTRANS,RC_RESPONSE_FAILED,szCode,strlen(szCode));
		return;
	}
    string eventString;

    m_MainEntry.m_CtiEvent.DisplayString("CSTA REQUEST Consultation Call V2");
    char lpszTempStr[256] = {0};
    sprintf( lpszTempStr, " Call ID: %d Device ID: %s Called Device: %s",pE->nCallID,pE->szExtNo,called);
    m_MainEntry.m_CtiEvent.DisplayString (eventString);
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::csta_consulation_call() " );
	}
}

void CCtiCtrl::CstaTransferCall(int iClientId, PEXTINFO pE)
{
	try
	{
	if( pE == NULL )
	{
		TraceEx( "error, csta transfer call failed, pE == NULL" );
		return;
	}

	ConnectionID_t heldCall;  
    ConnectionID_t activeCall;

	int iAgentType = -1;
    PAGENTTYPE agent_type = m_MainEntry.m_DataProc.GetAgentType( iClientId );
	if( agent_type != NULL )
		iAgentType = agent_type->iAgentType;

    if( m_MainEntry.m_iPBXAcd == 1 )
	{
		if( agent_type != NULL && agent_type->iAgentType != ivr_type ) 
		{
			heldCall.callID = pE->nHeldCallID;
		}
		else
		{
			heldCall.callID = 0;	//硬排转VDN的情况，此处赋值为0，否则TServer Stream 会异常
			if( pE->nHeldCallID > 0 )
				pE->nCallID = pE->nHeldCallID;
		}
	}
	else
	{
	   heldCall.callID = pE->nHeldCallID;
	}

    char szLog[256] = {0};
    sprintf( szLog, "m_iPBXAcd = %d, heldCall.callID = %d, pE->nHeldCallID = %d, iAgentType = %d, client_id = %d, ext = %s",
             m_MainEntry.m_iPBXAcd, heldCall.callID, pE->nHeldCallID, iAgentType, iClientId, pE->szExtNo );
    TraceEx( szLog );
    
    strcpy (heldCall.deviceID, pE->szExtNo);
    heldCall.devIDType = (ConnectionID_Device_t)(int)STATIC_ID;
    activeCall.callID = pE->nCallID;
    strcpy (activeCall.deviceID, pE->szExtNo);
    activeCall.devIDType = (ConnectionID_Device_t)(int)STATIC_ID;
    m_nRetCode = cstaTransferCall (m_MainEntry.m_acsHandle,
            (InvokeID_t) RC_INVOKEID,
            (ConnectionID_t FAR *)&heldCall,
            (ConnectionID_t FAR *)&activeCall,
            pPrivateData);
    if (m_nRetCode < 0) 
	{
		char szCode[50]={0};
		sprintf(szCode,"%s|%d",pE->szExtNo,m_nRetCode);
		if( iClientId > 0 )
            m_MainEntry.m_DataProc.Send_command(iClientId,CMD_COMPLETETRANS,RC_RESPONSE_FAILED,szCode,strlen(szCode));
		return;
    }
    string eventString;
    
    m_MainEntry.m_CtiEvent.DisplayString("CSTA REQUEST Transfer Call ");
    char lpszTempStr[256] = {0};
    sprintf( lpszTempStr, " Held Call ID: %d Held Device ID: %s Active Call ID: %d Active Device ID: %s",
             pE->nHeldCallID,pE->szExtNo,pE->nCallID,pE->szExtNo);
    eventString = lpszTempStr;
    m_MainEntry.m_CtiEvent.DisplayString (eventString);
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::csta_transfer_call() " );
	}
}

void CCtiCtrl::CstaConferenceCall(int iClientId, PEXTINFO pE)
{
	try
	{
	if( pE == NULL )
	{
		TraceEx( "csta conferen call failed, pE == NULL" );
		return;
	}

	ConnectionID_t heldCall;  
    ConnectionID_t activeCall;

    heldCall.callID = pE->nHeldCallID;
    strcpy (heldCall.deviceID, pE->szExtNo);
    heldCall.devIDType = (ConnectionID_Device_t)(int)STATIC_ID;
    activeCall.callID = pE->nCallID;
    strcpy (activeCall.deviceID, pE->szExtNo);
    activeCall.devIDType = (ConnectionID_Device_t)(int)STATIC_ID;
    m_nRetCode = cstaConferenceCall (m_MainEntry.m_acsHandle,
            (InvokeID_t) RC_INVOKEID,
            (ConnectionID_t FAR *)&heldCall,
            (ConnectionID_t FAR *)&activeCall,
            pPrivateData);
    if (m_nRetCode < 0) 
	{
		char szCode[50]={0};
		sprintf(szCode,"%s|%d",pE->szExtNo,m_nRetCode);
		if( iClientId > 0 )
            m_MainEntry.m_DataProc.Send_command(iClientId,CMD_COMPLETETRANS,RC_RESPONSE_FAILED,szCode,strlen(szCode));
		return;
    }
    string eventString;
    
    m_MainEntry.m_CtiEvent.DisplayString("CSTA REQUEST Conference Call ");
    char lpszTempStr[256] = {0};
    sprintf( lpszTempStr, " Held Call ID: %d Held Device ID: %s Active Call ID: %d Active Device ID: %s",
             pE->nHeldCallID,pE->szExtNo,pE->nCallID,pE->szExtNo);
    m_MainEntry.m_CtiEvent.DisplayString (eventString);
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::csta_conference_call() " );
	}
}

void CCtiCtrl::CstaSingleStepConference(int iClientId, int nJoinType, DeviceID_t DeviceToJoin, PEXTINFO pE)
{
#ifdef _VC6_
	try
	{
	ConnectionID_t		connectionID;

    connectionID.callID = pE->nCallID;
    strcpy (connectionID.deviceID, pE->szExtNo);
    connectionID.devIDType = (ConnectionID_Device_t)(int)STATIC_ID;


	ATTPrivateData_t my_correlatorData;
    memset(my_correlatorData.data, 0, sizeof(my_correlatorData.data));
	my_correlatorData.length = 0;
	//my_correlatorData.vendor[0]=0;
	strcpy(my_correlatorData.vendor,ECS_VENDOR_STRING);

	m_nRetCode = attSingleStepConferenceCall((ATTPrivateData_t  FAR*)&my_correlatorData,
										(ConnectionID_t FAR *)&connectionID,
										(DeviceID_t		FAR*)&DeviceToJoin,
										(ATTParticipationType_t)nJoinType,
										FALSE);

    if (m_nRetCode < 0) 
	{
 		char szCode[50]={0};
		sprintf(szCode,"%s|%d",pE->szExtNo,m_nRetCode);
		if( iClientId > 0 )
            m_MainEntry.m_DataProc.Send_command(iClientId,nJoinType==PT_SILENT?CMD_LISTEN:CMD_INTRUDE,RC_RESPONSE_FAILED,szCode,strlen(szCode));
		return;
    }
    m_nRetCode = cstaEscapeService(m_MainEntry.m_acsHandle,
								(InvokeID_t) RC_INVOKEID,
								(PrivateData_t FAR*)&my_correlatorData);
	 if (m_nRetCode < 0) 
	{
 		char szCode[50]={0};
		sprintf(szCode,"%s|%d",pE->szExtNo,m_nRetCode);
		if( iClientId > 0 )
            m_MainEntry.m_DataProc.Send_command(iClientId,nJoinType==PT_SILENT?CMD_LISTEN:CMD_INTRUDE,RC_RESPONSE_FAILED,szCode,strlen(szCode));
		return;
    }

	/*
	if(pE != NULL)
		PushToActList(pE->szExtNo,nJoinType==PT_SILENT?CMD_LISTEN:CMD_INTRUDE);
	*/
	
    string eventString;
	char str[20]={0};

    m_MainEntry.m_CtiEvent.DisplayString("CSTA REQUEST Single Step Conference: ");
    eventString += m_MainEntry.m_CtiEvent.AddConnectionID (connectionID);
    m_MainEntry.m_CtiEvent.DisplayString (eventString);
  
    char lpszTempStr[256] = {0};
    sprintf( lpszTempStr, "Device To Join: %s", DeviceToJoin);
    eventString = lpszTempStr;
    m_MainEntry.m_CtiEvent.DisplayString (eventString);

    eventString = "Participation type: ";
	sprintf (str, "%ld", nJoinType);
    eventString += str;
    m_MainEntry.m_CtiEvent.DisplayString (eventString);
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::csta_single_step_conference() " );
	}
#endif
}

void CCtiCtrl::CstaMakeCall(int iClientId, DeviceID_t called, char *szExtNo)
{
	try
	{
    if( szExtNo == NULL )
	{
        TraceEx( "error, csta make call  failed, szExtNo == NULL" );
		return;
	}

    string eventString;
	PrivateData_t	my_correlatorData;
	
	my_correlatorData.length = 0;
	my_correlatorData.data [0] = '\0';
	
    m_nRetCode = cstaMakeCall (m_MainEntry.m_acsHandle,
		(InvokeID_t)RC_INVOKEID,
        (DeviceID_t FAR *)szExtNo,
		(DeviceID_t FAR *)called,
		(PrivateData_t FAR *) &my_correlatorData);
		
	
	if (m_nRetCode < 0) 
	{
		char szCode[50]={0};
        sprintf(szCode,"%s|%d",szExtNo,m_nRetCode);
		if( iClientId > 0 )
            m_MainEntry.m_DataProc.Send_command(iClientId,CMD_MAKECALL,RC_RESPONSE_FAILED,szCode,strlen(szCode));
		return;
	}

	/*
	if(pE != NULL)
		PushToActList(pE->szExtNo,CMD_MAKECALL);
	*/

    m_MainEntry.m_CtiEvent.DisplayString ("CSTA REQUEST Make Call V2");
    char lpszTempStr[256] = {0};
    sprintf( lpszTempStr, " from %s  to %s",szExtNo,called);
    eventString = lpszTempStr;
    m_MainEntry.m_CtiEvent.DisplayString (eventString);
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::csta_make_call() " );
	}
}

void CCtiCtrl::CstaMonitorStop(PEXTINFO pE)
{
	try
	{
	if( pE == NULL )
	{
		TraceEx( "error, csta monitor stop failed, pE == NULL" );
		return;
	}

	char strCrossRefID [15];
  
    m_nRetCode = cstaMonitorStop (m_MainEntry.m_acsHandle,
                    (InvokeID_t)RC_INVOKEID,
                    pE->nCrossRefID,
                    pPrivateData);
    if (m_nRetCode < 0) 
	{
		char szCode[50]={0};
		sprintf(szCode,"%s|%d",pE->szExtNo,m_nRetCode);
        int iClientId = m_MainEntry.m_DataProc.GetClientIdByExt( pE->szExtNo );
		if( iClientId > 0 )
            m_MainEntry.m_DataProc.Send_command(iClientId, CMD_UNREGISTERDN,RC_RESPONSE_FAILED,szCode,strlen(szCode));
		return;	
    }
	
    string eventString;
  
    m_MainEntry.m_CtiEvent.DisplayString("CSTA REQUEST Monitor Stop  ");
    sprintf (strCrossRefID, "%ld", pE->nCrossRefID);
    eventString += "Cross Reference ID:";
	eventString += strCrossRefID; 
    m_MainEntry.m_CtiEvent.DisplayString (eventString);
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::csta_monitor_stop() " );
	}
}

int CCtiCtrl::CstaMonitorVDN(const char *szVDN)
{
	try
	{
	if( szVDN == NULL )
		return -1;

	CSTAMonitorFilter_t monFilter;
	ATTPrivateFilter_t attFilter;
    DeviceID_t deviceID;

	CSTAMonitorFilter_t* pFilter= &monFilter;

	pFilter->call = 0;
	pFilter->feature = 0;
	pFilter->agent = 0;
	pFilter->maintenance = 0;
	pFilter->privateFilter = 0;

	attFilter = ATT_ENTERED_DIGITS_FILTER|ATT_CHARGE_ADVICE_FILTER;

	strcpy (deviceID, szVDN);
	ATTPrivateData_t my_correlatorData;
    memset( my_correlatorData.data,0, sizeof(my_correlatorData.data));
	my_correlatorData.length = 0;
	strcpy(my_correlatorData.vendor,ECS_VENDOR_STRING);

    char szLog[256] = {0};
    sprintf( szLog, "begin to monitor VDN, deviceID = %s", deviceID);
    TraceEx( szLog );

    sprintf( szLog, "CstaMonitorVDN PrivateData: vendor=%s Len=%d Data=%s",
             ((PrivateData_t*)pPrivateData)->vendor,((PrivateData_t*)pPrivateData)->length,((PrivateData_t*)pPrivateData)->data);
    TraceEx( szLog );



    m_nRetCode = cstaMonitorCallsViaDevice (m_MainEntry.m_acsHandle,
                    RC_INVOKEID,
                    &deviceID,
                    &monFilter,
                    pPrivateData);      
    if (m_nRetCode < 0) 
	{
        sprintf( szLog, "cstaMonitorVDN Failed errcode=%d, deviceID = %s",
                 m_nRetCode, deviceID);
        TraceEx( szLog );
	}
	else
	{
		TraceEx("CSTA REQUEST Monitor Calls via Device:%s, m_nRetCode = %d ",szVDN, m_nRetCode);
	}

	return m_nRetCode;
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::csta_monitor_vdn() " );
	}
	return -1;
}

void CCtiCtrl::MonitorAllVDN()
{
	try
	{
    for( int i = 0; i < m_MainEntry.m_DataProc.m_nVDNNum; i++ )
	{
        CstaMonitorVDN( (char *)m_MainEntry.m_DataProc.m_strVDN[i].c_str() );
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::monitor_all_vdn() " );
	}
}

void CCtiCtrl::AttQueryAcdInfo(char *device)
{
#ifdef _VC6_
	try
	{
	if( device == NULL )
		return;

	ATTPrivateData_t my_correlatorData;
    memset(my_correlatorData.data, 0, sizeof(my_correlatorData.data));
	my_correlatorData.length = 0;
	//my_correlatorData.vendor[0]=0;
	strcpy(my_correlatorData.vendor,ECS_VENDOR_STRING);

	DeviceID_t deviceID;
	strcpy( deviceID, device );

	RetCode_t nRetCode = attQueryAcdSplit( (ATTPrivateData_t FAR *) &my_correlatorData, &deviceID );
	if( nRetCode < 0 )
	{
		TraceEx( "error, query acd failed, device = %s", device );
	}
	else
	{
		TraceEx( "QueryAcd, device = %s, ret_code = %d", device, nRetCode );
	}

    nRetCode = cstaEscapeService(m_MainEntry.m_acsHandle,
								(InvokeID_t) RC_INVOKEID,
								(PrivateData_t FAR*)&my_correlatorData);

	if (nRetCode < 0) 
	{
		TraceEx( "escape_service failed" );
	}
	else
	{
		TraceEx( "escape_service, device = %s, ret_code = %d", device, nRetCode );
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::att_query_acd_info() " );
	}
#endif
}

void CCtiCtrl::AttQueryTrunkGroup(char *device)
{
#ifdef _VC6_
	try
	{
	if( device == NULL )
		return;

	ATTPrivateData_t my_correlatorData;
    memset( my_correlatorData.data, 0, sizeof(my_correlatorData.data));
    //ZeroMemory(my_correlatorData.data,sizeof(my_correlatorData.data));
	my_correlatorData.length = 0;
	//my_correlatorData.vendor[0]=0;
	strcpy(my_correlatorData.vendor,ECS_VENDOR_STRING);

	DeviceID_t deviceID;
	strcpy( deviceID, device );

	RetCode_t nRetCode = attQueryTrunkGroup( (ATTPrivateData_t FAR *) &my_correlatorData, &deviceID );
	if( nRetCode < 0 )
	{
		TraceEx( "error, query trunk failed, device = %s", device );
	}
	else
	{
		TraceEx( "QueryTrunk, device = %s, ret_code = %d", device, nRetCode );
	}

    nRetCode = cstaEscapeService(m_MainEntry.m_acsHandle,
								(InvokeID_t) RC_INVOKEID,
								(PrivateData_t FAR*)&my_correlatorData);

	if (nRetCode < 0) 
	{
		TraceEx( "query_trunk, escape_service failed" );
	}
	else
	{
		TraceEx( "query_trunk, escape_service, device = %s, ret_code = %d", device, nRetCode );
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, cti_ctrl::att_query_trunk_group() " );
	}
#endif
}

void CCtiCtrl::CstaStartRecord(int iClientId, char *deviceID, char *lpszRecordFileName)
{
    if( iClientId < 0 || deviceID == NULL || lpszRecordFileName == NULL )
        return;

    cstaStartRecord( m_MainEntry.m_acsHandle,
                     (InvokeID_t)RC_INVOKEID,
                     deviceID,
                     lpszRecordFileName );

    char szLog[1024] = {0};
    sprintf( szLog, "csta_start_record, client_id = %d, deive = %s, record_file_name = %s",
             iClientId, deviceID, lpszRecordFileName );
    TraceEx( szLog );
}


void CCtiCtrl::CstaStopRecord(int iClientId, char *deviceID)
{
    if( iClientId < 0 || deviceID == NULL )
        return;

    cstaStopRecord( m_MainEntry.m_acsHandle,
                    (InvokeID_t)RC_INVOKEID,
                    deviceID );

    char szLog[1024] = {0};
    sprintf( szLog, "csta_stop_record, client_id = %d, device = %s", iClientId, deviceID );
    TraceEx( szLog );
}

void CCtiCtrl::StartRequestStateThread()
{
    if( state_request_iThreadId != 0 )
        return;

    if( pthread_create(&state_request_iThreadId, NULL, sysStateThread, NULL) != 0 )
    {
        printf("Create pthread error!\n");
        return;
    }
}

