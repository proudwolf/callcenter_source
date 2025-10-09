#ifdef _VC6_
#include "stdafx.h"
#include "TsapiServer.h"
#include "TsapiServerDlg.h"

extern CTsapiServerDlg *g_lpthis;
#endif

#include "GlobalTsapiServer.h"
#include "CtiEvent.h"
#include "mainentry.h"

extern CMainEntry m_MainEntry;

extern void TraceEx( char *buf, ...);

extern char recPrivateData[SIZE_OF_ALCATEL_PRIVATE_DATA];

CCtiEvent::CCtiEvent()
{
    m_bDisplayingSession = true;
	m_pEOriginate = NULL;
}

CCtiEvent::~CCtiEvent()
{
}

int CCtiEvent::CtiEvent(void *wParam, void *lParam)
{
	try
	{
	unsigned short  cstaEventSize;
	unsigned short  numEvents;
	//CErrorDlg errorDlg;
    string eventString;
	char str1 [100];
    char szLog[256] = {0};

	cstaEventSize = sizeof (m_cstaEvent);
	//((PrivateData_t*)recPrivateData)->length = 73;
	((PrivateData_t*)recPrivateData)->length = ATT_MAX_PRIVATE_DATA; // changed by g.l. 29 july 98 for viola.
	((PrivateData_t*)recPrivateData)->data[0] = '\0';
    m_nRetCode = acsGetEventBlock (m_MainEntry.m_acsHandle,
			  (void FAR *)&m_cstaEvent,
			  (unsigned short FAR *)&cstaEventSize,
			  (PrivateData_t FAR *)&recPrivateData,
			  (unsigned short FAR *)&numEvents);
	if (m_nRetCode < 0) 
	{
        sprintf( szLog, "acsGetEventBlock RetCode=%d", m_nRetCode);
        TraceEx( szLog );
		return (0);
	}

    sprintf( szLog, "acsGetEventBlock PrivateData: vendor=%s Len=%d Data=%s",
             ((PrivateData_t*)recPrivateData)->vendor,((PrivateData_t*)recPrivateData)->length,((PrivateData_t*)recPrivateData)->data);

    TraceEx( szLog );


//	TraceEx( "=============CtiEvent m_cstaEvent.eventHeader.eventClass = %d=========", m_cstaEvent.eventHeader.eventClass );

	switch (m_cstaEvent.eventHeader.eventClass) 
	{
	case ACSREQUEST :
      sprintf( szLog, "ACS REQUEST, eventType = %d", m_cstaEvent.eventHeader.eventType );
      TraceEx( szLog );
	  eventString = "ACS REQUEST";
	  switch (m_cstaEvent.eventHeader.eventType) 
	  {
		case ACS_OPEN_STREAM :
		  eventString += "Open Stream";
		  break;
		case ACS_CLOSE_STREAM :
		  eventString += "Close Stream";
		  break;
		case ACS_ABORT_STREAM :
		  eventString += "Abort Stream";
		  break;
		case ACS_UNIVERSAL_FAILURE :
		  eventString += "Universal Failure";
		  break;
		case ACS_KEY_REQUEST :
		  eventString += "Key Request";
		  break;
		case ACS_KEY_REPLY :
		  eventString += "Key Reply";
		  break;
		default :
		  sprintf (str1, "unknowFailureType : %d",
			   m_cstaEvent.eventHeader.eventType);
		  eventString += str1;
		  break;
	  }
	  break;
	case ACSUNSOLICITED :
	  TraceEx( "ACSUNSOLICITED" );
	  AcsUniversalFailureConfEvent (m_cstaEvent.event.acsUnsolicited.u.failureEvent.error, 0);
	  return (0);
	case ACSCONFIRMATION :
      sprintf( szLog, "ACSCONFIRMATION, eventType = %d",  m_cstaEvent.eventHeader.eventType);
      TraceEx( szLog );
	  eventString = "ACS CONFIRM ";
	  switch (m_cstaEvent.eventHeader.eventType) 
	  {
		case ACS_OPEN_STREAM :
		  eventString += "Open Stream";
		  break;
		case ACS_OPEN_STREAM_CONF :
		  AcsOpenStreamConfEvent ();
		  return (0);
		case ACS_CLOSE_STREAM :
		  eventString += "Close Stream";
		  break;
		case ACS_CLOSE_STREAM_CONF :
		  AcsCloseStreamConfEvent ();
		  return (0);
		case ACS_ABORT_STREAM :
		  eventString += "Abort Stream";
		  break;
		case ACS_UNIVERSAL_FAILURE_CONF :
		  AcsUniversalFailureConfEvent (m_cstaEvent.event.acsConfirmation.u.failureEvent.error, 1);
		  return (0);
		case ACS_UNIVERSAL_FAILURE :
		  eventString += "Universal Failure";
		  break;
		case ACS_KEY_REQUEST :
		  eventString += "Key Request";
		  break;
		case ACS_KEY_REPLY :
		  eventString += "Key Reply";
		  break;
		default :
		  sprintf (str1, "unknowFailureType : %d",
			   m_cstaEvent.eventHeader.eventType);
		  eventString += str1;
		  break;
	  }
	  break;
	case CSTAREQUEST :
      sprintf( szLog, "CSTA REQUEST, eventType = %d",  m_cstaEvent.eventHeader.eventType);
      TraceEx( szLog );
	  eventString = "CSTA REQUEST ";
	  switch (m_cstaEvent.eventHeader.eventType) 
	  {
		case CSTA_ESCAPE_SVC_REQ :
		  //CstaEscapeServiceReq ();
		  return (0);
		case CSTA_SYS_STAT_REQ :
		  //CstaSysStatReqEvent ();
		  return (0);
		// Changed by G.L. 10 March for Computing services (_EXT)
		case CSTA_ROUTE_REQUEST_EXT :
		// End Changed
		 // CstaRouteRequestEvent();
		  return (0);
		case CSTA_RE_ROUTE_REQUEST :
		  //CstaReRouteEvent();
		  return (0);
		default :
		  sprintf (str1, "unknow Type : %d",
			   m_cstaEvent.eventHeader.eventType);
		  eventString += str1;
		  break;
	  }
	  break;
	case CSTAUNSOLICITED :
      sprintf( szLog, "CSTA UNSOLICITED RC_EVENT, eventType = %d", m_cstaEvent.eventHeader.eventType );
      TraceEx( szLog );
	  eventString = "CSTA UNSOLICITED RC_EVENT ";
      sprintf( szLog, "======================CtiEvent m_cstaEvent.eventHeader.eventType = %d =============", m_cstaEvent.eventHeader.eventType );
      TraceEx( szLog );

	  switch (m_cstaEvent.eventHeader.eventType) 
	  {
		case CSTA_CALL_CLEARED :
		  CstaCallClearedEvent ();
		  return (0);
		case CSTA_CONFERENCED :
		  CstaConferencedEvent ();
		  return (0);
		case CSTA_CONNECTION_CLEARED :
		  CstaConnectionClearedEvent ();
		  return (0);
		case CSTA_DELIVERED :
		  CstaDeliveredEvent ();
		  return (0);
		case CSTA_DIVERTED :
		  CstaDivertedEvent ();
		  return (0);
		case CSTA_ESTABLISHED :
		  CstaEstablishedEvent ();
		  return (0);
		case CSTA_FAILED :
		  CstaFailedEvent ();
		  return (0);
		case CSTA_HELD :
		  CstaHeldEvent ();
		  return (0);
		case CSTA_NETWORK_REACHED :
		  CstaNetworkReachedEvent ();
		  return (0);
		case CSTA_ORIGINATED :
		  CstaOriginatedEvent ();
		  return (0);
		case CSTA_QUEUED :
		  CstaQueuedEvent ();
		  return (0);
		case CSTA_RETRIEVED :
		  CstaRetrieveEvent ();
		  return (0);
		case CSTA_SERVICE_INITIATED :
		  CstaServiceInitiatedEvent ();
		  return (0);
		case CSTA_TRANSFERRED :
		  CstaTransferredEvent ();
		  return (0);
		case CSTA_CALL_INFORMATION :
		  CstaCallInfoEvent ();
		  return (0);
		case CSTA_DO_NOT_DISTURB :
		  CstaDoNotDisturbEvent ();
		  return (0);
		case CSTA_FORWARDING :
		  CstaForwardingEvent ();
		  return (0);
		case CSTA_MESSAGE_WAITING :
		  CstaMessageWaitingEvent ();
		  return (0);
		case CSTA_LOGGED_ON :
		  CstaLoggedOnEvent ();
		  return (0);
		case CSTA_LOGGED_OFF :
		  CstaLoggedOffEvent ();
		  return (0);
		case CSTA_NOT_READY :
		  CstaNotReadyEvent ();
		  return (0);
		case CSTA_READY :
		  CstaReadyEvent ();
		  return (0);
		case CSTA_WORK_NOT_READY :
		  CstaWorkNotReady ();
		  return (0);
		case CSTA_WORK_READY :
		  CstaWorkReady ();
		  return (0);
		case CSTA_PRIVATE_STATUS :
		  //CstaPrivateStatusEvent();
		  return (0);
		case CSTA_BACK_IN_SERVICE :
		  CstaBackInServiceEvent ();
		  return (0);
		case CSTA_OUT_OF_SERVICE :
		  CstaOutOfServiceEvent ();
		  return (0);
		case CSTA_MONITOR_ENDED :
		  CstaMonitorEnded();
		  return (0);
//		case CSTA_RECORD_EVENT :
//		  CstaRecordEvent();
//		  return (0);
//		case CSTA_STOP_EVENT :
//		  CstaStopEvent();
//		  return (0);
		default :
		  eventString += "unknown type";
		  break;
	  }
	  break;
	case CSTACONFIRMATION :
      sprintf( szLog, "CSTA CONFIRM, eventType = %d", m_cstaEvent.eventHeader.eventType );
      TraceEx( szLog );
	  eventString = "CSTA CONFIRM ";
	  switch (m_cstaEvent.eventHeader.eventType) 
	  {
		case CSTA_ALTERNATE_CALL_CONF :
		  //CstaAlternateCallConfEvent ();
		  return (0);
//		case CSTA_ASSOCIATE_DATA_CONF:
		//CstaAssociateDataConfEvent();
//			return(0);
		case CSTA_ANSWER_CALL_CONF :
		  CstaAnswerCallConfEvent ();
		  return (0);
		case CSTA_CALL_COMPLETION_CONF : 
		  //CstaCallCompletionConfEvent ();
		  return (0);
		case CSTA_CLEAR_CALL_CONF :
		  CstaClearCallConfEvent ();
		  return (0);
		case CSTA_CLEAR_CONNECTION_CONF :
		  CstaClearConnectionConfEvent ();
		  return (0);
		case CSTA_CONFERENCE_CALL_CONF :
		  CstaConferenceCallConfEvent();
		  return (0);
		case CSTA_CONSULTATION_CALL_CONF :
		  CstaConsultationCallConfEvent();
		  return (0);
		case CSTA_DEFLECT_CALL_CONF :
		 // CstaDeflectCallConfEvent ();
		  return (0);
//		case CSTA_PARK_CALL_CONF :
		  //CstaParkCallConfEvent ();
//		  return (0);
		case CSTA_PICKUP_CALL_CONF :
		  //CstaPickupCallConfEvent ();
		  return (0);
		case CSTA_GROUP_PICKUP_CALL_CONF :
		  //CstaGroupPickupCallConfEvent ();
		  return (0);
		case CSTA_HOLD_CALL_CONF :
		  CstaHoldCallConfEvent();
		  return (0);
		case CSTA_MAKE_CALL_CONF :
		  CstaMakeCallConfEvent();
		  return (0);
		case CSTA_MAKE_PREDICTIVE_CALL_CONF :
		  //CstaMakePredictiveCallConfEvent ();
		  return (0);
		case CSTA_QUERY_MWI_CONF :
		  //CstaQueryMsgWaitingIndConfEvent ();
		  return (0);
		case CSTA_QUERY_DND_CONF :
		  //CstaQueryDoNotDisturbConfEvent ();
		  return (0);
		case CSTA_QUERY_FWD_CONF :
		  //CstaQueryForwardingConfEvent ();
		  return (0);
		case CSTA_QUERY_AGENT_STATE_CONF :
		  //CstaQueryAgentStateConfEvent ();
		  return (0);
		case CSTA_QUERY_LAST_NUMBER_CONF :
		  //CstaQueryLastNumberConfEvent ();
		  return (0);
		case CSTA_QUERY_DEVICE_INFO_CONF :
		  //CstaQueryDeviceInfoConfEvent ();
		  return (0);
		case CSTA_RECONNECT_CALL_CONF :
		  //CstaReconnectCallConfEvent ();
		  return (0);
		case CSTA_RETRIEVE_CALL_CONF :
		  CstaRetrieveCallConfEvent ();
		  return (0);
		case CSTA_SET_MWI_CONF :
		  //CstaSetMsgWaitingIndConfEvent ();
		  return (0);
		case CSTA_SET_DND_CONF :
		  //CstaSetDoNotDisturbConfEvent ();
		  return (0);
		case CSTA_SET_FWD_CONF :
		  //CstaSetForwardingConfEvent ();
		  return (0);
		case CSTA_SET_AGENT_STATE_CONF :
		  //CstaSetAgentStateConfEvent ();
		  return (0);
		case CSTA_TRANSFER_CALL_CONF :
		  CstaTransferCallConfEvent ();
		  return (0);
		case CSTA_UNIVERSAL_FAILURE_CONF :
			CstaUniversalFailureConfEvent (m_cstaEvent.event.cstaConfirmation.u.universalFailure.error);
			return (0);
		case CSTA_ROUTE_REGISTER_REQ :
		  eventString += "Route Register Req";
		  break;
		case CSTA_ROUTE_REGISTER_REQ_CONF :
		  //CstaRouteRegisterReqConfEvent();
		  return (0);
		case CSTA_ROUTE_REGISTER_CANCEL :
		  eventString += "Route Register Cancel";
		  break;
		case CSTA_ROUTE_REGISTER_CANCEL_CONF :
		  //CstaRouteRegisterCancelConfEvent();
		  return(0);
		case CSTA_ROUTE_REGISTER_ABORT :
		  eventString += "Route Register Abort";
		  break;
		case CSTA_ROUTE_REQUEST :
		  eventString += "Route Request";
		  break;
		case CSTA_ROUTE_SELECT_REQUEST :
		  eventString += "Route Select Request";
		  break;
		case CSTA_RE_ROUTE_REQUEST :
		  eventString += "Re Route Request";
		  break;
		case CSTA_ROUTE_USED :
		  eventString += "Route Used";
		  break;
		case CSTA_ROUTE_END :
		  eventString += "Route End";
		  break;
		case CSTA_ROUTE_END_REQUEST :
		  eventString += "Route End Request";
		  break;

		// Added by G.L. 23 March 98
//		case CSTA_SEND_DTMF_TONES_CONF :
		  //CstaSendDTMFTonesConfEvent();
//		  return(0);

//		case CSTA_SINGLE_STEP_CONFERENCE_CALL_CONF :
//		  CstaSingleStepConfEvent();
//		  return(0);

//		case CSTA_RECORD_MESSAGE_CALL_CONF :
		  //CstaRecordMessageConfEvent();
//		  return(0);

//		case CSTA_STOP_CALL_CONF :
		  //CstaStopCallConfEvent();
//		  return(0);
		// end added

		case CSTA_ESCAPE_SVC_CONF :
		  //CstaEscapeServiceConfEvent ();
		  return (0);
//		case CSTA_IVR_STATUS_CONF:
		//	CstaIvrStatusConfEvent();
//			return (0);
		case CSTA_SYS_STAT_REQ_CONF :
          m_MainEntry.m_CtiCtrl.CstaSysStatReqConfEvent ();
		  return (0);
		case CSTA_SYS_STAT_START_CONF :
		  //CstaSysStatStartConfEvent ();
		  return (0);
		case CSTA_SYS_STAT_STOP_CONF :
		  //CstaSysStatStopConfEvent ();
		  return (0);
		case CSTA_CHANGE_SYS_STAT_FILTER_CONF :
		  //CstaChangeSysStatFilterConfEvent ();
		  return (0);
		case CSTA_MONITOR_CONF :
		  CstaMonitorConfEvent ();
		  return (0);
		case CSTA_CHANGE_MONITOR_FILTER_CONF :
		  //CstaChangeMonitorFilterConfEvent ();
		  return (0);
		case CSTA_MONITOR_STOP_CONF :
		  CstaMonitorStopConfEvent ();
		  return (0);
		case CSTA_SNAPSHOT_CALL_CONF :
		  //CstaSnapshotCallConfEvent();
		  return(0);
		case CSTA_SNAPSHOT_DEVICE_CONF :
		  //CstaSnapshotDeviceConfEvent();
		  return(0);
		case CSTA_GETAPI_CAPS_CONF :
		  //CstaGetApiCapsConfEvent ();
		  return (0);
		case CSTA_GET_DEVICE_LIST :
		  //CstaGetDeviceListConfEvent ();
		  return (0);
		case CSTA_GET_DEVICE_LIST_CONF :
		  //CstaGetDeviceListConfEvent ();
		  return (0);
		case CSTA_QUERY_CALL_MONITOR_CONF :
		  //CstaQueryCallMonitorConfEvent ();
		  return (0);
		default :
		  sprintf (str1, "unknowType : %d",
			   m_cstaEvent.eventHeader.eventType);
		  eventString += str1;
		  break;
	  }
	  break;
	case CSTAEVENTREPORT :
      sprintf( szLog, "CSTA REPORT, eventType = %d", m_cstaEvent.eventHeader.eventType );
      TraceEx( szLog );
	  eventString = "CSTA REPORT ";
	  switch (m_cstaEvent.eventHeader.eventType) {
		case CSTA_SYS_STAT :
		  //CstaSysStatEvent ();
		  return (0);
		case CSTA_SYS_STAT_ENDED :
		  //CstaSysStatEndedEvent ();
		  return (0);
		case CSTA_ROUTE_REGISTER_ABORT :
		  //CstaRouteRegisterAbortEvent();
		  return (0);
		case CSTA_ROUTE_USED :
		  //CstaRouteUsedEvent();
		  return (0);
		case CSTA_ROUTE_END :
		  //CstaRouteEndEvent();
		  return (0);
		case CSTA_PRIVATE :
		  //CstaPrivateEvent ();
		  return (0);
		default :
		  sprintf (str1, "unknowType : %d",
			   m_cstaEvent.eventHeader.eventType);
		  eventString += str1;
		  break;
	  }
	  break;
	default :
	  sprintf (str1, "unknowClass : %d, unknowType : %d",
		   m_cstaEvent.eventHeader.eventClass, m_cstaEvent.eventHeader.eventType);
	  TraceEx( str1 );
	  eventString = str1;
	  break;
	}
#ifdef _windows_
	DisplayString(eventString);
#else
    TraceEx((char *)eventString.c_str());
#endif
	return (0);
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::cti_event() " );
	}
	return (0);
}

void CCtiEvent::SetOwner(void *lpVoid)
{
#ifdef _VC6_
	try
	{
	if( lpVoid != NULL )
		g_lpthis = (CTsapiServerDlg *)lpVoid;
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::set_owner() " );
	}
#endif
}

void CCtiEvent::AcsUniversalFailureConfEvent(ACSUniversalFailure_t error, int i)
{
	try
	{
    string eventString;
	char str1 [40];

	if (i)
	eventString = "ACS CONFIRMATION Failure ";
	else
	eventString = "ACS UNSOLITED Failure ";
	switch (error) {
	case TSERVER_STREAM_FAILED :
		{
			eventString += "TSERVER_STREAM_FAILED";

            TraceEx( "%s, and restart now...", (char *)eventString.c_str());
			exit( 0 );	//add by liu, 2010-12-08 14:37
		}
	  break;
	case TSERVER_NO_THREAD :
	  eventString += "TSERVER_NO_THREAD";
	  break;
	case TSERVER_BAD_DRIVER_ID :
	  eventString += "TSERVER_BAD_DRIVER_ID";
	  break;
	case TSERVER_DEAD_DRIVER :
	  eventString += "TSERVER_DEAD_DRIVER";
	  break;
	case TSERVER_MESSAGE_HIGH_WATER_MARK :
	  eventString += "TSERVER_MESSAGE_HIGH_WATER_MARK";
	  break;
	case TSERVER_FREE_BUFFER_FAILED :
	  eventString += "TSERVER_FREE_BUFFER_FAILED";
	  break;
	case TSERVER_SEND_TO_DRIVER :
	  eventString += "TSERVER_SEND_TO_DRIVER";
	  break;
	case TSERVER_RECEIVE_FROM_DRIVER :
	  eventString += "TSERVER_RECEIVE_FROM_DRIVER";
	  break;
	case TSERVER_REGISTRATION_FAILED :
	  eventString += "TSERVER_REGISTRATION_FAILED";
	  break;
	case TSERVER_SPX_FAILED :
	  eventString += "TSERVER_SPX_FAILED";
	  break;
	case TSERVER_TRACE :
	  eventString += "TSERVER_TRACE";
	  break;
	case TSERVER_NO_MEMORY :
	  eventString += "TSERVER_NO_MEMORY";
	  break;
	case TSERVER_ENCODE_FAILED :
	  eventString += "TSERVER_ENCODE_FAILED";
	  break;
	case TSERVER_DECODE_FAILED :
	  eventString += "TSERVER_DECODE_FAILED";
	  break;
	case TSERVER_BAD_CONNECTION :
	  eventString += "TSERVER_BAD_CONNECTION";
	  break;
	case TSERVER_BAD_PDU :
	  eventString += "TSERVER_BAD_PDU";
	  break;
	case TSERVER_NO_VERSION :
	  eventString += "TSERVER_NO_VERSION";
	  break;
	case TSERVER_ECB_MAX_EXCEEDED :
	  eventString += "TSERVER_ECB_MAX_EXCEEDED";
	  break;
	case TSERVER_NO_ECBS :
	  eventString += "TSERVER_NO_ECBS";
	  break;
	case TSERVER_NO_SDB :
	  eventString += "TSERVER_NO_SDB";
	  break;
	case TSERVER_NO_SDB_CHECK_NEEDED :
	  eventString += "TSERVER_NO_SDB_CHECK_NEEDED";
	  break;
	case TSERVER_SDB_CHECK_NEEDED :
	  eventString += "TSERVER_SDB_CHECK_NEEDED";
	  break;
	case TSERVER_BAD_SDB_LEVEL :
	  eventString += "TSERVER_BAD_SDB_LEVEL";
	  break;
	case TSERVER_BAD_SERVERID :
	  eventString += "TSERVER_BAD_SERVERID";
      TraceEx ((char *)eventString.c_str());
      m_MainEntry.Abortstream ();
	  return;
	case TSERVER_BAD_STREAM_TYPE :
	  eventString += "TSERVER_BAD_STREAM_TYPE";
      TraceEx ((char *)eventString.c_str());
      m_MainEntry.Abortstream ();
	  return;
	case TSERVER_BAD_PASSWORD_OR_LOGIN :
	  eventString += "TSERVER_BAD_PASSWORD_OR_LOGIN";
      TraceEx ((char *)eventString.c_str());
      m_MainEntry.Abortstream ();
	  return;
	case TSERVER_NO_USER_RECORD :
	  eventString += "TSERVER_NO_USER_RECORD";
      TraceEx ((char *)eventString.c_str());
      m_MainEntry.Abortstream ();
	  return;
	case TSERVER_NO_DEVICE_RECORD :
	  eventString += "TSERVER_NO_DEVICE_RECORD";
	  break;
	case TSERVER_DEVICE_NOT_ON_LIST :
	  eventString += "TSERVER_DEVICE_NOT_ON_LIST";
	  break;
	case TSERVER_USERS_RESTRICTED_HOME :
	  eventString += "TSERVER_USERS_RESTRICTED_HOME";
	  break;
	case TSERVER_NO_AWAYPERMISSION :
	  eventString += "TSERVER_NO_AWAYPERMISSION";
	  break;
	case TSERVER_NO_HOMEPERMISSION :
	  eventString += "TSERVER_NO_HOMEPERMISSION";
	  break;
	case TSERVER_NO_AWAY_WORKTOP :
	  eventString += "TSERVER_NO_AWAY_WORKTOP";
	  break;
	case TSERVER_BAD_DEVICE_RECORD :
	  eventString += "TSERVER_BAD_DEVICE_RECORD";
	  break;
	case TSERVER_DEVICE_NOT_SUPPORTED :
	  eventString += "TSERVER_DEVICE_NOT_SUPPORTED";
      m_MainEntry.m_DataProc.CstaMonitorEvent( RC_RESPONSE_FAILED );
	  break;
	case TSERVER_INSUFFICIENT_PERMISSION :
	  eventString += "TSERVER_INSUFFICIENT_PERMISSION";
	  break;
	case TSERVER_NO_RESOURCE_TAG :
	  eventString += "TSERVER_NO_RESOURCE_TAG";
	  break;
	case TSERVER_INVALID_MESSAGE :
	  eventString += "TSERVER_INVALID_MESSAGE";
	  break;
	case TSERVER_EXCEPTION_LIST :
	  eventString += "TSERVER_EXCEPTION_LIST";
	  break;
	case TSERVER_NOT_ON_OAM_LIST :
	  eventString += "TSERVER_NOT_ON_OAM_LIST";
	  break;
	case TSERVER_PBX_ID_NOT_IN_SDB :
	  eventString += "TSERVER_PBX_ID_NOT_IN_SDB";
	  break;
	case TSERVER_USER_LICENSES_EXCEEDED :
	  eventString += "TSERVER_USER_LICENSES_EXCEEDED";
	  break;
	case TSERVER_OAM_DROP_CONNECTION :
	  eventString += "TSERVER_OAM_DROP_CONNECTION";
	  break;
	case TSERVER_NO_VERSION_RECORD :
	  eventString += "TSERVER_NO_VERSION_RECORD";
	  break;
	case TSERVER_OLD_VERSION_RECORD :
	  eventString += "TSERVER_OLD_VERSION_RECORD";
	  break;
	case TSERVER_BAD_PACKET :
	  eventString += "TSERVER_BAD_PACKET";
	  break;
	case TSERVER_OPEN_FAILED :
	  eventString += "TSERVER_OPEN_FAILED";
	  break;
	case TSERVER_OAM_IN_USE :
	  eventString += "TSERVER_OAM_IN_USE";
	  break;
	case TSERVER_DEVICE_NOT_ON_HOME_LIST :
	  eventString += "TSERVER_DEVICE_NOT_ON_HOME_LIST";
	  break;
	case TSERVER_DEVICE_NOT_ON_CALL_CONTROL_LIST :
	  eventString += "TSERVER_DEVICE_NOT_ON_CALL_CONTROL_LIST";
	  break;
	case TSERVER_DEVICE_NOT_ON_AWAY_LIST :
	  eventString += "TSERVER_DEVICE_NOT_ON_AWAY_LIST";
	  break;
	case TSERVER_DEVICE_NOT_ON_ROUTE_LIST :
	  eventString += "TSERVER_DEVICE_NOT_ON_AWAY_LIST";
	  break;
	case TSERVER_DEVICE_NOT_ON_MONITOR_DEVICE_LIST :
	  eventString += "TSERVER_DEVICE_NOT_ON_MONITOR_DEVICE_LIST";
	  break;
	case TSERVER_DEVICE_NOT_ON_MONITOR_CALL_DEVICE_LIST :
	  eventString += "TSERVER_DEVICE_NOT_ON_MONITOR_CALL_DEVICE_LIST";
	  break;
	case TSERVER_NO_CALL_CALL_MONITOR_PERMISSION :
	  eventString += "TSERVER_NO_CALL_CALL_MONITOR_PERMISSION";
	  break;
	case TSERVER_HOME_DEVICE_LIST_EMPTY :
	  eventString += "TSERVER_HOME_DEVICE_LIST_EMPTY";
	  break;
	case TSERVER_CALL_CONTROL_LIST_EMPTY :
	  eventString += "TSERVER_CALL_CONTROL_LIST_EMPTY";
	  break;
	case TSERVER_AWAY_LIST_EMPTY :
	  eventString += "TSERVER_AWAY_LIST_EMPTY";
	  break;
	case TSERVER_ROUTE_LIST_EMPTY :
	  eventString += "TSERVER_ROUTE_LIST_EMPTY";
	  break;
	case TSERVER_MONITOR_DEVICE_LIST_EMPTY :
	  eventString += "TSERVER_MONITOR_DEVICE_LIST_EMPTY";
	  break;
	case TSERVER_MONITOR_CALL_DEVICE_LIST_EMPTY :
	  eventString += "TSERVER_MONITOR_CALL_DEVICE_LIST_EMPTY";
	  break;
	case TSERVER_USER_AT_HOME_WORKTOP :
	  eventString += "TSERVER_USER_AT_HOME_WORKTOP";
	  break;
	case TSERVER_DEVICE_LIST_EMPTY :
	  eventString += "TSERVER_DEVICE_LIST_EMPTY";
	  break;
	case TSERVER_BAD_GET_DEVICE_LEVEL :
	  eventString += "TSERVER_BAD_GET_DEVICE_LEVEL";
	  break;
	case TSERVER_DRIVER_UNREGISTERED :
	  eventString += "TSERVER_DRIVER_UNREGISTERED";
	  break;
	case TSERVER_NO_ACS_STREAM :
	  eventString += "TSERVER_NO_ACS_STREAM";
	  break;
	case TSERVER_DROP_OAM :
	  eventString += "TSERVER_DROP_OAM";
	  break;
	case TSERVER_ECB_TIMEOUT :
	  eventString += "TSERVER_ECB_TIMEOUT";
	  break;
	case TSERVER_BAD_ECB :
	  eventString += "TSERVER_BAD_ECB";
	  break;
	case TSERVER_ADVERTISE_FAILED :
	  eventString += "TSERVER_ADVERTISE_FAILED";
	  break;
	case TSERVER_NETWARE_FAILURE :
	  eventString += "TSERVER_NETWARE_FAILURE";
	  break;
	case TSERVER_TDI_QUEUE_FAULT :
	  eventString += "TSERVER_TDI_QUEUE_FAULT";
	  break;
	case TSERVER_DRIVER_CONGESTION :
	  eventString += "TSERVER_DRIVER_CONGESTION";
	  break;
	case TSERVER_NO_TDI_BUFFERS :
	  eventString += "TSERVER_NO_TDI_BUFFERS";
	  break;
	case TSERVER_OLD_INVOKEID :
	  eventString += "TSERVER_OLD_INVOKEID";
	  break;
	case TSERVER_HWMARK_TO_LARGE :
	  eventString += "TSERVER_HWMARK_TO_LARGE";
	  break;
	case TSERVER_SET_ECB_TO_LOW :
	  eventString += "TSERVER_SET_ECB_TO_LOW";
	  break;
	case TSERVER_NO_RECORD_IN_FILE :
	  eventString += "TSERVER_NO_RECORD_IN_FILE";
	  break;
	case TSERVER_ECB_OVERDUE :
	  eventString += "TSERVER_ECB_OVERDUE";
	  break;
	case DRIVER_DUPLICATE_ACSHANDLE :
	  eventString += "DRIVER_DUPLICATE_ACSHANDLE";
      TraceEx ((char *)eventString.c_str());
      m_MainEntry.Abortstream ();
	  return;
	case DRIVER_INVALID_ACS_REQUEST :
	  eventString += "DRIVER_INVALID_ACS_REQUEST";
	  break;
	case DRIVER_ACS_HANDLE_REJECTION :
	  eventString += "DRIVER_ACS_HANDLE_REJECTION";
	  break;
	case DRIVER_INVALID_CLASS_REJECTION :
	  eventString += "DRIVER_INVALID_CLASS_REJECTION";
	  break;
	case DRIVER_GENERIC_REJECTION :
	  eventString += "DRIVER_GENERIC_REJECTION";
	  break;
	case DRIVER_RESOURCE_LIMITATION :
	  eventString += "DRIVER_RESOURCE_LIMITATION";
	  break;
	case DRIVER_ACSHANDLE_TERMINATION :
	  eventString += "DRIVER_ACSHANDLE_TERMINATION";
      TraceEx ((char *)eventString.c_str());
      m_MainEntry.Abortstream ();
	  return;
	case DRIVER_LINK_UNAVAILABLE :
	  eventString += "DRIVER_LINK_UNAVAILABLE";
	  break;
	case DRIVER_OAM_IN_USE :
	  eventString += "DRIVE_OAM_IN_USE";
	  break;
	case TSERVER_VERSION_MISMATCH:
	  eventString += "TSERVER_VERSION_MISMATCH";
	  break;
	default :
	  sprintf (str1, "%d", error);
	  eventString += str1;
	  break;
	}
    TraceEx ((char *)eventString.c_str());
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::acs_universal_failure_conf_event() " );
	}
}

void CCtiEvent::AcsOpenStreamConfEvent()
{
	try
	{
	ACSOpenStreamConfEvent_t *pEvent;
    string eventString;

//	eventString = "Tsapi Server. Connected to ";
//	eventString += g_lpthis->m_sServerName;
//	g_lpthis->SetWindowText(eventString);
	pEvent = &m_cstaEvent.event.acsConfirmation.u.acsopen;
	eventString = "ACS CONFIRM Open Stream";
    TraceEx( (char *)eventString.c_str() );
	eventString = "           API Version: ";
	eventString += pEvent->apiVer;
    TraceEx( (char *)eventString.c_str() );
	eventString = "           Library Version: ";
	eventString += pEvent->libVer;
    TraceEx( (char *)eventString.c_str() );
	eventString = "           TSERVER Version: ";
	eventString += pEvent->tsrvVer;
    TraceEx( (char *)eventString.c_str() );
	eventString = "           Driver Version: ";
	eventString += pEvent->drvrVer;
    TraceEx( (char *)eventString.c_str() );
    //theApp.m_bPhoneOn = true;
    m_MainEntry.m_bOpenedStream = true;
	/*YB 240800+*/
#ifdef _VC6_
	::KillTimer(NULL, g_lpthis->my_reconnectTimer);
#else
#endif
    m_MainEntry.m_bAutoReconnect = false;

#ifdef _VC6_
	/*YB 240800-*/
    (CWnd*)g_lpthis->GetDlgItem(IDC_BTN_START)->EnableWindow(!m_MainEntry.m_bOpenedStream);
    (CWnd*)g_lpthis->GetDlgItem(IDC_BTN_END)->EnableWindow(m_MainEntry.m_bOpenedStream);
#else
    TraceEx( "tsap_server is running now...");
#endif

	// Start the AutoKeepalive;
    m_MainEntry.AutoKeepalive();
    m_MainEntry.m_CtiCtrl.StartRequestStateThread();
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::acs_open_stream_conf_event() " );
	}
}

void CCtiEvent::AcsCloseStreamConfEvent()
{
	try
	{
	TraceEx ("ACS CONFIRM Close Stream");
    //theApp.m_bPhoneOn = false;
    m_MainEntry.m_bOpenedStream = false;
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::acs_close_stream_conf_event() " );
	}
}

void CCtiEvent::CstaCallClearedEvent()
{
	try
	{
	CSTACallClearedEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.callCleared;
	
    PEXTINFO pE = m_MainEntry.m_DataProc.GetExtInfoFromRefId(m_cstaEvent.event.cstaUnsolicited.monitorCrossRefId);
	CorrectUnsolicitedRefId(pE,pEvent->clearedCall);
    m_MainEntry.m_DataProc.ClearTransferData(pE,pEvent->localConnectionInfo);

    if( m_MainEntry.m_iPBXAcd == 1 )
	{
		char call_ID [16] = {0};
		sprintf (call_ID, "%ld", pEvent->clearedCall.callID);

        m_MainEntry.m_DataProc.DelCallIDInfo( call_ID );
	}
#ifdef _windows_
	DisplayString("CSTA UNSOLICITED RC_EVENT Call Cleared  ");
	eventString += AddConnectionID (pEvent->clearedCall);
	DisplayUnsolicitedEvent (eventString);
	eventString = "               ";
	eventString += GetLocalConnectionInfo (pEvent->localConnectionInfo);
	eventString += "   ";
	eventString += GetEventCause (pEvent->cause);
	DisplayString (eventString);
#else
    TraceEx( "CSTA UNSOLICITED RC_EVENT Call Cleared  ");
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_call_cleared_event() " );
	}
}

void CCtiEvent::CstaConferencedEvent()
{
	try
	{
	CSTAConferencedEvent_t *pEvent;
    string eventString;
	ConnectionID_t *pConnection;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.conferenced;
    PEXTINFO pE = m_MainEntry.m_DataProc.GetExtInfoFromRefId(m_cstaEvent.event.cstaUnsolicited.monitorCrossRefId);

	DisplayString("CSTA UNSOLICITED RC_EVENT Conferenced  Old Held Call: ");
#ifdef _windows_
	eventString += AddConnectionID (pEvent->primaryOldCall);
	DisplayUnsolicitedEvent (eventString);
	eventString = "                          Old Active Call: ";
	eventString += AddConnectionID (pEvent->secondaryOldCall);
	DisplayString (eventString);
	eventString = "                          Controller ";
	eventString += AddExtendedDeviceID (pEvent->confController);
	DisplayString (eventString);
	eventString = "                          Added party ";
	eventString += AddExtendedDeviceID (pEvent->addedParty);
	DisplayString (eventString);
#endif
	for (int i =0; i < pEvent->conferenceConnections.count; i++) 
	{
		pConnection = &pEvent->conferenceConnections.connection [i].party;

#ifdef _windows_
		eventString = "        Conference party: ";
		eventString += AddConnectionID (pEvent->conferenceConnections.connection [i].party);
		DisplayString (eventString);
		eventString = "                 Static Device: ";
		eventString += AddExtendedDeviceID (pEvent->conferenceConnections.connection [i].staticDevice);
		DisplayString (eventString);
#endif
		
		if( pE != NULL && strcmp(pEvent->conferenceConnections.connection [i].staticDevice.deviceID,pE->szExtNo)==0)
			CorrectUnsolicitedRefId(pE,pEvent->conferenceConnections.connection [i].party);
	}

#ifdef _windows_
	eventString = "               ";
	eventString += GetLocalConnectionInfo (pEvent->localConnectionInfo);
	eventString += "   ";
	eventString += GetEventCause (pEvent->cause);
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_conferenced_event() " );
	}
}

void CCtiEvent::CstaConnectionClearedEvent()
{
	try
	{
	CSTAConnectionClearedEvent_t *pEvent;
    string eventString;
    char szLog[256] = {0};
	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.connectionCleared;

    PEXTINFO pE = m_MainEntry.m_DataProc.GetExtInfoFromRefId(m_cstaEvent.event.cstaUnsolicited.monitorCrossRefId);
	if( pE != NULL )
	{
        sprintf( szLog, "connection_clear_event, ext = %s", pE->szExtNo );
        TraceEx( szLog );
        int iClientId = m_MainEntry.m_DataProc.GetClientIdByExt( pE->szExtNo );
		if( iClientId > 0 )
		{
			pE->iConnect = 0;
			if( strcmp( pE->szExtNo , pEvent->releasingDevice.deviceID ) == 0 )
			{
                m_MainEntry.m_DataProc.Send_command(iClientId,CMD_EXTHANGUP_EVENT,RC_EVENT,pE->szExtNo,strlen(pE->szExtNo));
                sprintf( szLog, "connetion_event_clear, client_id = %d, ext = %s", iClientId, pE->szExtNo );
                TraceEx( szLog );

				CorrectUnsolicitedRefId(pE,pEvent->droppedConnection);
			}
			else
			{
//				m_MainEntry.m_DataProc.Send_command(iClientId,CMD_CALLDISCONNECT_EVENT,RC_EVENT,pE->szExtNo,strlen(pE->szExtNo));
			}
		}
		else
		{
			TraceEx( "warnning, can not find cient_id, ext = %s", pE->szExtNo );
		}
	}
    m_MainEntry.m_DataProc.ClearTransferData(pE,pEvent->localConnectionInfo);

    if( m_MainEntry.m_iPBXAcd == 1 )
	{
		char call_ID [16] = {0};
		sprintf (call_ID, "%ld", pEvent->droppedConnection.callID);
        m_MainEntry.m_DataProc.OutQue( pEvent->releasingDevice.deviceID, call_ID );

        m_MainEntry.m_DataProc.DelIVRCallIDInfo( pEvent->releasingDevice.deviceID );
	}
	/*
	//////////////////////////////////////////////////////////////
	if( pE == NULL )
	{
		//add by liu, 2013-08-15 14:00
        int iClientId = m_MainEntry.m_DataProc.GetClientIdByExt( pEvent->releasingDevice.deviceID );
		if( iClientId > 0 )
		{
			TraceEx( "pU = NULL, find clientid = %d, releasing_deviceId = %s", iClientId, pEvent->releasingDevice.deviceID );
            m_MainEntry.m_DataProc.Send_command(iClientId,CMD_EXTHANGUP_EVENT,RC_EVENT,pEvent->releasingDevice.deviceID,strlen(pEvent->releasingDevice.deviceID));
		}
		else
		{
			TraceEx( "warnning, cant find clientid, connection_cleared_event, iClientId = %d, releasing_device = %s", iClientId, pEvent->releasingDevice.deviceID );
		}
	}
	/////////////////////////////////////////////////////////////
	*/

	DisplayString ("CSTA UNSOLICITED RC_EVENT Connection Cleared  ");

#ifdef _windows_
	eventString += AddConnectionID (pEvent->droppedConnection);
	DisplayUnsolicitedEvent (eventString);
	eventString = "                Releasing Device: ";
	eventString += AddExtendedDeviceID (pEvent->releasingDevice);
	DisplayString (eventString);
	eventString = "               ";
	eventString += GetLocalConnectionInfo (pEvent->localConnectionInfo);
	eventString += "   ";
	eventString += GetEventCause (pEvent->cause);
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_connection_cleared_event() " );
	}
}

void CCtiEvent::CstaDeliveredEvent()
{
	try
	{
    char szLog[256] = {0};

    if( !m_MainEntry.m_DataProc.m_bSeriCorrect )
	{
        sprintf( szLog, "error, CtiEvent m_bSeriCorrect = %d", m_MainEntry.m_DataProc.m_bSeriCorrect );
        TraceEx( szLog );
		return;
    }

	CSTADeliveredEvent_t *pEvent;
    string eventString;

    pEvent = &m_cstaEvent.event.cstaUnsolicited.u.delivered;

	PEXTINFO pE = NULL;
	if( strlen( pEvent->alertingDevice.deviceID ) < 5 && strlen( pEvent->callingDevice.deviceID ) < 5 )
	{
        pE = m_MainEntry.m_DataProc.GetExtInfoFromRefId(m_cstaEvent.event.cstaUnsolicited.monitorCrossRefId);
		if( pE == NULL )
		{
            sprintf( szLog, "error, cstaDeliveredEvent failed, cant find pE, monitorCrossRefId = %d",  m_cstaEvent.event.cstaUnsolicited.monitorCrossRefId);
            TraceEx( szLog );
		}
	}
	else
	{
        pE = m_MainEntry.m_DataProc.GetPEByExt( pEvent->alertingDevice.deviceID );
		if( pE == NULL )
		{
            sprintf( szLog, "warnning, cant find pE, alertingDevice = %s", pEvent->alertingDevice.deviceID );
            TraceEx( szLog );

            pE = m_MainEntry.m_DataProc.GetExtInfoFromRefId(m_cstaEvent.event.cstaUnsolicited.monitorCrossRefId);
			if( pE == NULL )
			{
                sprintf( szLog, "error, cstaDeliveredEvent failed, cant find pE, monitorCrossRefId = %d",  m_cstaEvent.event.cstaUnsolicited.monitorCrossRefId);
                TraceEx( szLog );
			}
		}
	}


    bool bRingEvent = false;
	char szBuf[1024]={0};
	int iClientId = 0;
	if( pE != NULL )
	{
        iClientId = m_MainEntry.m_DataProc.GetClientIdByExt( pE->szExtNo );
		if( iClientId > 0 )
		{

			char trunkGroup[32] = {0}, trunkMember[32] = {0};
			GetTrunkNo( trunkGroup, trunkMember, ATT_DELIVERED );
			
			pE->iConnect = 0;
			
			if( strcmp(pEvent->alertingDevice.deviceID,pE->szExtNo ) == 0 )
			{
				sprintf(szBuf ,"%s|%s|%s|%s|%s" ,pE->szExtNo,pEvent->callingDevice.deviceID,pEvent->calledDevice.deviceID, trunkGroup, trunkMember);

                if( m_MainEntry.m_iPBXAcd != 1 )
                    m_MainEntry.m_DataProc.Send_command(iClientId,CMD_CALLERIDCOMING_EVENT,RC_EVENT,szBuf,strlen(szBuf));

				CorrectUnsolicitedRefId(pE,pEvent->connection);

				sprintf( pE->CallingDeviceID.deviceID, "%s", pEvent->callingDevice.deviceID );

                bRingEvent = true;
                sprintf( szLog, "ringing event, deviceID = %s, CallingDeviceID = %s, szBuf = %s, iConnect = %d, bRingEvent = %d", pEvent->alertingDevice.deviceID, pE->CallingDeviceID.deviceID, szBuf, pE->iConnect, bRingEvent );
                TraceEx( szLog );
			}
			else
			{
				sprintf(szBuf ,"%s|%s|%s|%s" ,pE->szExtNo,pEvent->calledDevice.deviceID, trunkGroup, trunkMember);
                m_MainEntry.m_DataProc.Send_command(iClientId,CMD_EXTENSIONRINGBACK_EVENT,RC_EVENT,szBuf,strlen(szBuf));

                sprintf( szLog, "ringback event, deviceID = %s, szBuf = %s, iConnect = %d", pEvent->alertingDevice.deviceID, szBuf, pE->iConnect );
                TraceEx( szLog );
			}	
		}
	}

    if( m_MainEntry.m_iPBXAcd == 1 )
	{
		char callID[16] = {0};
		sprintf (callID, "%ld", pEvent->connection.callID);

        if( m_MainEntry.m_DataProc.IsVDNNumber( pEvent->alertingDevice.deviceID ) )
		{
            char *lpszIVRCallId = m_MainEntry.m_DataProc.m_IVR_CALLIDMap.Find( pEvent->callingDevice.deviceID );

			if( lpszIVRCallId != NULL )
			{
                m_MainEntry.m_DataProc.AddCallIDInfo( callID, lpszIVRCallId );
				
                sprintf( szLog, "add_callid_info, callID = %s, lpszIVRCallId = %s, ivr_no = %s", callID, lpszIVRCallId, pEvent->callingDevice.deviceID );
                TraceEx( szLog );

                m_MainEntry.m_DataProc.m_IVR_CALLIDMap.Remove( pEvent->callingDevice.deviceID );

				delete[] lpszIVRCallId;
				lpszIVRCallId = NULL;
			}
			/*
            else if( m_MainEntry.m_iNoIVR == 1 )
			{
				char lpszCallID[64] = {0};
				
				SYSTEMTIME ct;
				GetLocalTime(&ct);
								
				sprintf( lpszCallID, "%04d%02d%02d%02d%02d%02d%03d%03d", 
					ct.wYear,ct.wMonth,ct.wDay,ct.wHour,ct.wMinute,ct.wSecond,ct.wMilliseconds, 9 );

                m_MainEntry.m_DataProc.AddCallIDInfo( callID, lpszCallID );

                TraceEx( "add_callid_info, m_iNoIVR = %d, callID = %s, lpszCallD = %s", m_MainEntry.m_iNoIVR, callID, lpszCallID );
			}
			*/
		}
		else
		{
			if( !bRingEvent )
				return;

			char lpszTempCallID[64] = {0}, lpszTempCalledID[64] = {0};

            if( m_MainEntry.m_iNoIVR == 1 )
			{
				strcpy( lpszTempCallID, callID );
				strcpy( lpszTempCalledID, pEvent->lastRedirectionDevice.deviceID );
			}
			else
			{
                char *lpszIVRCallId = m_MainEntry.m_DataProc.m_CallID_IVRIDMap.Find( callID );
				if( lpszIVRCallId != NULL )
				{
					strcpy( lpszTempCallID, lpszIVRCallId );
                    m_MainEntry.m_DataProc.m_CallID_IVRIDMap.Remove( callID );
					delete[] lpszIVRCallId;
					lpszIVRCallId = NULL;

				}

				strcpy( lpszTempCalledID, pEvent->calledDevice.deviceID );
			}

			//坐席分机震铃，发送ivr_callid至客户端
			//param1 = ivr_call_id, param2 = lpszIVRCallId, param3 = IVR分机号^VDN
            int iDestId = m_MainEntry.m_DataProc.m_ExtSockMap.Find( pEvent->alertingDevice.deviceID );
			if( iDestId > 0 )
			{
				char szInBuff[1024] = {0};
				sprintf(szInBuff,"%s|%s|%s|%s|%s^%s",
					pEvent->alertingDevice.deviceID,pEvent->callingDevice.deviceID,
					"ivr_call_id",lpszTempCallID,pEvent->callingDevice.deviceID,
					lpszTempCalledID);

                m_MainEntry.m_DataProc.Send_command( iDestId, CMD_EXTENSIONMSG_EVENT, RC_EVENT, szInBuff, strlen(szInBuff) );
                sprintf( szLog, "TsapiServer, send ivr_callid to client, iDestId = %d, szInBuff = %s, strlen(szInBuffer) = %d", iDestId, szInBuff, strlen(szInBuff) );
                TraceEx( szLog );
			}
		}

		if( bRingEvent )
            m_MainEntry.m_DataProc.Send_command(iClientId,CMD_CALLERIDCOMING_EVENT,RC_EVENT,szBuf,strlen(szBuf));

	}

	DisplayString ("CSTA UNSOLICITED RC_EVENT Delivered V2 ");
#ifdef _windows_
	eventString += AddConnectionID (pEvent->connection);
	DisplayUnsolicitedEvent (eventString);

	eventString = "               Alerting Device: ";
	eventString += AddExtendedDeviceID (pEvent->alertingDevice);
	DisplayString (eventString);
	eventString = "               Calling Device: ";
	eventString += AddExtendedDeviceID (pEvent->callingDevice);
	DisplayString (eventString);
	eventString = "               Called Device: ";
	eventString += AddExtendedDeviceID (pEvent->calledDevice);
	DisplayString (eventString);
	eventString = "               Last Redirection Device: ";
	eventString += AddExtendedDeviceID (pEvent->lastRedirectionDevice);
	DisplayString (eventString);

	eventString = "               ";
	eventString += GetLocalConnectionInfo (pEvent->localConnectionInfo);
	eventString += "   ";
	eventString += GetEventCause (pEvent->cause);
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_delivered_event() " );
	}

}

void CCtiEvent::CstaDivertedEvent()
{
	try
	{
	CSTADivertedEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.diverted;

	char szExtNo[32] = {0};
	strcpy( szExtNo, (char *)pEvent->divertingDevice.deviceID );

	if( (char)pEvent->cause == EC_CALL_NOT_ANSWERED )
	{
        int iClientId = m_MainEntry.m_DataProc.GetClientIdByExt( szExtNo );
		if( iClientId > 0 )
            m_MainEntry.m_DataProc.Send_command(iClientId,CMD_EXTHANGUP_EVENT,RC_EVENT,szExtNo,strlen(szExtNo));
	}


	DisplayString ("CSTA UNSOLICITED RC_EVENT Diverted");
#ifdef _windows_
	eventString += AddConnectionID (pEvent->connection);
	DisplayUnsolicitedEvent (eventString);
	eventString = "               Diverting Device: ";
	eventString += AddExtendedDeviceID (pEvent->divertingDevice);
	DisplayString (eventString);
	eventString = "               New Destination: ";
	eventString += AddExtendedDeviceID (pEvent->newDestination);
	DisplayString (eventString);
	eventString = "               ";
	eventString += GetLocalConnectionInfo (pEvent->localConnectionInfo);
	eventString += "   ";
	eventString += GetEventCause (pEvent->cause);
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_deverted_event() " );
	}
}

void CCtiEvent::CstaEstablishedEvent()
{
	try
	{
	CSTAEstablishedEvent_t *pEvent;
    string eventString;
    char szLog[256] = {0};

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.established;

    sprintf( szLog, "established_event, monitorCrossRefId = %d", m_cstaEvent.event.cstaUnsolicited.monitorCrossRefId );
    TraceEx( szLog );

    PEXTINFO pE = m_MainEntry.m_DataProc.GetExtInfoFromRefId(m_cstaEvent.event.cstaUnsolicited.monitorCrossRefId);
	if( pE != NULL && strcmp(pEvent->answeringDevice.deviceID,pE->szExtNo) == 0)
		CorrectUnsolicitedRefId(pE,pEvent->establishedConnection);

	char SourceNum[32] = {0};

	if( pE != NULL )
	{
        int iClientId = m_MainEntry.m_DataProc.GetClientIdByExt( pE->szExtNo );
		if( iClientId > 0 )
		{
			strcpy( SourceNum, pE->szExtNo );
			char szBuf[1024]={0}, szAnsweringDeviceId[64] = {0};
			if( strlen( pEvent->answeringDevice.deviceID ) == 0 )
			{
				strcpy( szAnsweringDeviceId, pEvent->calledDevice.deviceID );
				if( strcmp( SourceNum, pEvent->callingDevice.deviceID ) != 0 )
				{
					//正常情况下，pE->szExtNo 和 pEvent->callingDevice.deviceID应该相同
					//yum项目呼出时候这里会有异常产生，pE->szExtNo 和 pEvent->callingDevice.deviceID不相等，原因：还未检查出来
					strcpy( SourceNum, pEvent->callingDevice.deviceID );

                    sprintf( szLog, "warnning, call_established, SourceNum changed now, SourceNum = %s, pE->szExtNo = %s",
                             SourceNum, pE->szExtNo );

                    TraceEx( szLog );
				}
			}
			else
			{
				strcpy( szAnsweringDeviceId, pEvent->answeringDevice.deviceID );
			}
			sprintf(szBuf ,"%s|%s|%s|%d" ,SourceNum,pEvent->callingDevice.deviceID,
				szAnsweringDeviceId,pEvent->answeringDevice.deviceIDType);
			
			TraceEx( "CMD_CALLESTABLISHED_EVENT, buf = %s", szBuf );

			//CMD_CALLESTABLISHED_EVENT, buf = 6008|6001|6008|0
			//notify, CMD_CALLESTABLISHED_EVENT, buf = 6008|6008|6001|0

            if( m_MainEntry.m_iPBXAcd == 1 && strlen( pEvent->callingDevice.deviceID ) < 5 && m_MainEntry.m_iPBXType == 0 )
			{
//				if( strlen( pEvent->calledDevice.deviceID ) > 5 )
				{
					sprintf(szBuf ,"%s|%s|%s|%d" ,SourceNum, szAnsweringDeviceId, pEvent->callingDevice.deviceID,
						pEvent->answeringDevice.deviceIDType);

					TraceEx( "notify, CMD_CALLESTABLISHED_EVENT, buf = %s", szBuf );

                    m_MainEntry.m_DataProc.Send_command(iClientId,CMD_CALLESTABLISHED_EVENT,RC_EVENT,szBuf,strlen(szBuf));
				}
//				else
//				{
//					TraceEx( "established, not send this msg, m_iPBXAcd = %d", m_MainEntry.m_iPBXAcd );
//				}
			}
			else
			{
                m_MainEntry.m_DataProc.Send_command(iClientId,CMD_CALLESTABLISHED_EVENT,RC_EVENT,szBuf,strlen(szBuf));
			}

		}
		else
		{
			TraceEx( "established_event, can not find client_id" );
		}
		
	}
	else
	{
		TraceEx( "established, error, can not find pE now..." );
	}

	DisplayString ("CSTA UNSOLICITED RC_EVENT Established V2 ");
#ifdef _windows_
	eventString += AddConnectionID (pEvent->establishedConnection);
	DisplayUnsolicitedEvent (eventString);

	eventString = "               Answering Device: ";
	eventString += AddExtendedDeviceID (pEvent->answeringDevice);
	DisplayString (eventString);
	eventString = "               Calling Device: ";
	eventString += AddExtendedDeviceID (pEvent->callingDevice);
	DisplayString (eventString);
	eventString = "               Called Device: ";
	eventString += AddExtendedDeviceID (pEvent->calledDevice);
	DisplayString (eventString);
	eventString = "               Last Redirection Device: ";
	eventString += AddExtendedDeviceID (pEvent->lastRedirectionDevice);
	DisplayString (eventString);

	eventString = "               ";
	eventString += GetLocalConnectionInfo (pEvent->localConnectionInfo);
	eventString += "   ";
	eventString += GetEventCause (pEvent->cause);
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_established_event() " );
	}
}

void CCtiEvent::CstaFailedEvent()
{
	try
	{
	CSTAFailedEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.failed;

    PEXTINFO pE = m_MainEntry.m_DataProc.GetExtInfoFromRefId(m_cstaEvent.event.cstaUnsolicited.monitorCrossRefId);
	if( pE != NULL && strcmp(pEvent->failingDevice.deviceID,pE->szExtNo) == 0)
		CorrectUnsolicitedRefId(pE,pEvent->failedConnection);

    m_MainEntry.m_DataProc.ClearTransferData(pE,pEvent->localConnectionInfo);

	if( pE != NULL )
	{
        int iClientId = m_MainEntry.m_DataProc.GetClientIdByExt( pE->szExtNo );
		if( iClientId > 0 )
		{
            m_MainEntry.m_DataProc.Send_command(iClientId,CMD_FAILEDCONNECTION_EVENT,RC_EVENT,pE->szExtNo,strlen(pE->szExtNo));
		}
		
	}

	DisplayString ("CSTA UNSOLICITED RC_EVENT Failed  Connection ");
#ifdef _windows_
	eventString += AddConnectionID (pEvent->failedConnection);
	DisplayUnsolicitedEvent (eventString);
	eventString = "                Failing Device: ";
	eventString += AddExtendedDeviceID (pEvent->failingDevice);
	DisplayString (eventString);
	eventString = "                Called Device: ";
	eventString += AddExtendedDeviceID (pEvent->calledDevice);
	DisplayString (eventString);
	eventString = "               ";
	eventString += GetLocalConnectionInfo (pEvent->localConnectionInfo);
	eventString += "   ";
	eventString += GetEventCause (pEvent->cause);
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_failed_event() " );
	}
}

void CCtiEvent::CstaHeldEvent()
{
	try
	{
	CSTAHeldEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.held;

    PEXTINFO pEi = m_MainEntry.m_DataProc.GetExtInfoFromRefId(m_cstaEvent.event.cstaUnsolicited.monitorCrossRefId);
	if( pEi != NULL && strcmp( pEvent->holdingDevice.deviceID,pEi->szExtNo)==0 )
        CorrectUnsolicitedRefId(pEi,pEvent->heldConnection,true);

    int iClientId = m_MainEntry.m_DataProc.GetClientIdByExt( pEi->szExtNo );
	if( iClientId > 0 )
        m_MainEntry.m_DataProc.Send_command( iClientId, CMD_EXTONHOLD_EVENT,RC_EVENT,pEi->szExtNo,strlen(pEi->szExtNo));

	DisplayString ("CSTA UNSOLICITED RC_EVENT Held  ");
#ifdef _windows_
	eventString += AddConnectionID (pEvent->heldConnection);
	DisplayUnsolicitedEvent (eventString);

	eventString = "               Holding Device: ";
	eventString += AddExtendedDeviceID (pEvent->holdingDevice);
	DisplayString (eventString);
	eventString = "               ";
	eventString += GetLocalConnectionInfo (pEvent->localConnectionInfo);
	eventString += "   ";
	eventString += GetEventCause (pEvent->cause);
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_held_event() " );
	}
}

void CCtiEvent::CstaNetworkReachedEvent()
{
	try
	{
	CSTANetworkReachedEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.networkReached;


	DisplayString ("CSTA UNSOLICITED RC_EVENT Network Reached  ");
#ifdef _windows_
	eventString += AddConnectionID (pEvent->connection);
	DisplayUnsolicitedEvent (eventString);
	eventString = "                 Trunk Used: ";
	eventString += AddExtendedDeviceID (pEvent->trunkUsed);
	DisplayString (eventString);
	eventString = "                 Called Device: ";
	eventString += AddExtendedDeviceID (pEvent->calledDevice);
	DisplayString (eventString);
	eventString = "               ";
	eventString += GetLocalConnectionInfo (pEvent->localConnectionInfo);
	eventString += "   ";
	eventString += GetEventCause (pEvent->cause);
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_network_reached_event() " );
	}
}

void CCtiEvent::CstaOriginatedEvent()
{
	try
	{
	CSTAOriginatedEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.originated;

    PEXTINFO pE = m_MainEntry.m_DataProc.GetExtInfoFromRefId(m_cstaEvent.event.cstaUnsolicited.monitorCrossRefId);
	if( pE != NULL && strcmp(pEvent->callingDevice.deviceID,pE->szExtNo) == 0)
		CorrectUnsolicitedRefId(pE,pEvent->originatedConnection);

	m_pEOriginate = pE;

	if( pE != NULL )
	{
        int iClientId = m_MainEntry.m_DataProc.GetClientIdByExt( pE->szExtNo );
		if( iClientId > 0 )
            m_MainEntry.m_DataProc.Send_command(iClientId,CMD_EXTDIALING_EVENT,RC_EVENT,pE->szExtNo,strlen(pE->szExtNo));
	}

	DisplayString ("CSTA UNSOLICITED RC_EVENT Originated  ");
#ifdef _windows_
	eventString += AddConnectionID (pEvent->originatedConnection);
	DisplayUnsolicitedEvent (eventString);
	eventString = "               Calling Device: ";
	eventString += AddExtendedDeviceID (pEvent->callingDevice);
	DisplayString (eventString);
	eventString = "               Called Device: ";
	eventString += AddExtendedDeviceID (pEvent->calledDevice);
	DisplayString (eventString);

	eventString = "               ";
	eventString += GetLocalConnectionInfo (pEvent->localConnectionInfo);
	eventString += "   ";
	eventString += GetEventCause (pEvent->cause);
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_originated_event() " );
	}
}

void CCtiEvent::CstaQueuedEvent()
{
	try
	{
	CSTAQueuedEvent_t *pEvent;
    string eventString;
	char nQueue [25]; 

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.queued;

    if( m_MainEntry.m_iPBXAcd == 1 )
	{
//		switch ((char)pEvent->localConnectionInfo) 
//		{
//			case CS_CONNECT:
//				{
                    if( m_MainEntry.m_iNoIVR == 0 )
					{
						char call_ID [16] = {0};
						sprintf (call_ID, "%ld", pEvent->queuedConnection.callID);
                        m_MainEntry.m_DataProc.AddQueueList( pEvent->queue.deviceID, call_ID, pEvent->callingDevice.deviceID, pEvent->calledDevice.deviceID );
					}
//				}
//				break;
//		}
	}

    if( m_MainEntry.m_iNoIVR == 1 )
	{
		char call_ID [16] = {0};
		sprintf (call_ID, "%ld", pEvent->queuedConnection.callID);
        m_MainEntry.m_DataProc.AddQueueList( pEvent->queue.deviceID, call_ID, pEvent->callingDevice.deviceID, pEvent->lastRedirectionDevice.deviceID );
	}

	DisplayString ("CSTA UNSOLICITED RC_EVENT Queued V2 ");
#ifdef _windows_
	eventString += AddConnectionID (pEvent->queuedConnection);
	DisplayUnsolicitedEvent (eventString);

	eventString = "               Queue: ";
	eventString += AddExtendedDeviceID (pEvent->queue);
	DisplayString (eventString);
	eventString = "               Calling Device: ";
	eventString += AddExtendedDeviceID (pEvent->callingDevice);
	DisplayString (eventString);
	eventString = "               Called Device: ";
	eventString += AddExtendedDeviceID (pEvent->calledDevice);
	DisplayString (eventString);
	eventString = "               Last Redirection Device: ";
	eventString += AddExtendedDeviceID (pEvent->lastRedirectionDevice);
	DisplayString (eventString);
	eventString = "               Calls queued:";
	sprintf (nQueue, " %d", pEvent->numberQueued);
	eventString += nQueue;
	DisplayString (eventString);
	
	eventString = "               ";
	eventString += GetLocalConnectionInfo (pEvent->localConnectionInfo);
	eventString += "   ";
	eventString += GetEventCause (pEvent->cause);
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_queued_event() " );
	}

}

void CCtiEvent::CstaRetrieveEvent()
{
	try
	{
    char szLog[256] = {0};

	CSTARetrievedEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.retrieved;

    PEXTINFO pE = m_MainEntry.m_DataProc.GetExtInfoFromRefId(m_cstaEvent.event.cstaUnsolicited.monitorCrossRefId);

	if( pE != NULL && strcmp( pEvent->retrievingDevice.deviceID,pE->szExtNo) == 0)
		CorrectUnsolicitedRefId(pE,pEvent->retrievedConnection);
	if( pE != NULL )
	{
        int iClientId = m_MainEntry.m_DataProc.GetClientIdByExt( pE->szExtNo );
		if( iClientId > 0 )
            m_MainEntry.m_DataProc.Send_command(iClientId,CMD_EXTONRETRIEVE_EVENT,RC_EVENT,pE->szExtNo,strlen(pE->szExtNo));
	}
    else
    {
        sprintf( szLog, "error, csat_retrieve_event, pE==NULL, retrieving_device = %s, monitorCrossRefId = ld",
                 pEvent->retrievingDevice.deviceID, m_cstaEvent.event.cstaUnsolicited.monitorCrossRefId);
        TraceEx( szLog );
    }

	DisplayString ("CSTA UNSOLICITED RC_EVENT Retrieved  ");

#ifdef _windows_
	eventString += AddConnectionID (pEvent->retrievedConnection);
    DisplayUnsolicitedEvent (eventString);
	eventString = "                  Retrieving Device: ";
	eventString += AddExtendedDeviceID (pEvent->retrievingDevice);
	DisplayString (eventString);
    eventString = "               ";
	eventString += GetLocalConnectionInfo (pEvent->localConnectionInfo);
	eventString += "   ";
	eventString += GetEventCause (pEvent->cause);
    DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_retrieve_event() " );
	}
}

void CCtiEvent::CstaServiceInitiatedEvent()
{
	try
	{
	CSTAServiceInitiatedEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.serviceInitiated;
    PEXTINFO pE = m_MainEntry.m_DataProc.GetExtInfoFromRefId(m_cstaEvent.event.cstaUnsolicited.monitorCrossRefId);
	if( pE != NULL )
	{
		CorrectUnsolicitedRefId(pE, pEvent->initiatedConnection);
	}
	else
	{
		TraceEx( "===========CstaServiceInitiatedEvent error, pE == NULL============" );
	}

	if( pE != NULL )
	{
        int iClientId = m_MainEntry.m_DataProc.GetClientIdByExt( pE->szExtNo );
		if( iClientId > 0 )
            m_MainEntry.m_DataProc.Send_command(iClientId,CMD_EXTPICKUP_EVENT,RC_EVENT,pE->szExtNo,strlen(pE->szExtNo));
	}

	DisplayString ("CSTA UNSOLICITED RC_EVENT Service Initiated  ");
#ifdef _windows_
	eventString += AddConnectionID (pEvent->initiatedConnection);
	DisplayUnsolicitedEvent (eventString);

	eventString = "               ";
	eventString += GetLocalConnectionInfo (pEvent->localConnectionInfo);
	eventString += "   ";
	eventString += GetEventCause (pEvent->cause);
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_service_initiated_event() " );
	}
}

void CCtiEvent::CstaTransferredEvent()
{
	try
	{
    char szLog[256] = {0};
	CSTATransferredEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.transferred;
	
    PEXTINFO pE = m_MainEntry.m_DataProc.GetExtInfoFromRefId(m_cstaEvent.event.cstaUnsolicited.monitorCrossRefId);
	if( pE != NULL && strcmp(pEvent->transferringDevice.deviceID , pE->szExtNo) == 0)
		CorrectUnsolicitedRefId(pE, pEvent->primaryOldCall);

	if( pE != NULL )
	{
        int iClientId = m_MainEntry.m_DataProc.GetClientIdByExt( pE->szExtNo );
		if( iClientId > 0 )
		{
            char lpszEventData[1024] = {0};
            sprintf( lpszEventData, "%s|%s|%s", pE->szExtNo, pEvent->transferringDevice.deviceID, pEvent->transferredDevice.deviceID );

            m_MainEntry.m_DataProc.Send_command(iClientId,CMD_EXTTRANSFERED_EVENT,RC_EVENT,lpszEventData,strlen(lpszEventData) + 1);

            TraceEx( "--------CstaTransferredEvent-------, strEventData = %s", lpszEventData );
		}
	}

	char lpszCallingDevice[128] = {0}, lpszCalledDevice[128] = {0};

	DisplayString ("CSTA UNSOLICITED RC_EVENT Transferred Primary Call  ");
#ifdef _windows_
	eventString += AddConnectionID (pEvent->primaryOldCall);
	DisplayUnsolicitedEvent (eventString);
	eventString = "                   Secondary Call  ";
	eventString += AddConnectionID (pEvent->secondaryOldCall);
	DisplayString (eventString);
	eventString = "                   Transferring Device: ";
	eventString += AddExtendedDeviceID (pEvent->transferringDevice);
	DisplayString (eventString);
	eventString = "                   Transferred Device: ";
	eventString += AddExtendedDeviceID (pEvent->transferredDevice);
	DisplayString (eventString);
#endif
	for (int i = 0; i < pEvent->transferredConnections.count; i++) 
	{
#ifdef _windows_
		eventString = "           Party  ";
		eventString += AddConnectionID (pEvent->transferredConnections.connection [i].party);
		DisplayString (eventString);
		eventString = "                 Static Device: ";
		eventString += AddExtendedDeviceID (pEvent->transferredConnections.connection [i].staticDevice);
		DisplayString (eventString);
#endif
		if( pEvent->transferredConnections.connection [i].staticDevice.deviceIDType == EXPLICIT_PUBLIC_NATIONAL 
			|| pEvent->transferredConnections.connection [i].staticDevice.deviceIDType == EXPLICIT_PUBLIC_SUBSCRIBER )
		{
			sprintf( lpszCallingDevice, "%s", pEvent->transferredConnections.connection [i].party.deviceID );
		}
		else if( pEvent->transferredConnections.connection [i].staticDevice.deviceIDType == EXPLICIT_PRIVATE_LOCAL_NUMBER )
		{
			sprintf( lpszCalledDevice, "%s", pEvent->transferredConnections.connection [i].party.deviceID );
		}
	}

    if( m_MainEntry.m_iPBXAcd == 1 )
	{
        int iClientId = m_MainEntry.m_DataProc.GetClientIdByExt( lpszCalledDevice );
		if( iClientId > 0 )
		{
            PEXTINFO pE_1 = m_MainEntry.m_DataProc.GetPEByExt( lpszCalledDevice );
			if( pE_1 != NULL )
			{
				if( pE_1->iConnect != 1 )
				{
					char szBuf[1024]={0};
					sprintf(szBuf ,"%s|%s|%s|%d" ,lpszCalledDevice,lpszCallingDevice,
					lpszCalledDevice,0);	
				
                    m_MainEntry.m_DataProc.Send_command(iClientId,CMD_CALLESTABLISHED_EVENT,RC_EVENT,szBuf,strlen(szBuf));

                    sprintf( szLog, "m_iPBXAcd = %d, CMD_CALLESTABLISHED_EVENT, szBuf = %s, iConnect = %d", m_MainEntry.m_iPBXAcd, szBuf, pE_1->iConnect );
                    TraceEx( szLog );

					pE_1->iConnect = 1;
				}
				else
				{
                    sprintf( szLog, "not send call_established event now, iConnect = %d", pE_1->iConnect );
                    TraceEx( szLog );
				}
				
			}
			else
			{
                sprintf( szLog, "cant find pE_1, lpszCalledDevice = %s", lpszCalledDevice );
                TraceEx( szLog );
			}
		}
		else
		{
            sprintf( szLog, "error, cant find client_id, ext = %s", lpszCalledDevice );
            TraceEx( szLog );
		}
	
	}
#ifdef _windows_
	eventString = "               ";
	eventString += GetLocalConnectionInfo (pEvent->localConnectionInfo);
	eventString += "   ";
	eventString += GetEventCause (pEvent->cause);
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_transferred_event() " );
	}
}

void CCtiEvent::CstaCallInfoEvent()
{
	try
	{
	CSTACallInformationEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.callInformation;

	DisplayString ("CSTA UNSOLICITED RC_EVENT Call Information  ");
#ifdef _windows_
	eventString += AddConnectionID (pEvent->connection);
	DisplayUnsolicitedEvent (eventString);
	eventString = "                Device: ";
	eventString += AddExtendedDeviceID (pEvent->device);
	DisplayString (eventString);
	eventString = "                Account Information: ";
	eventString += pEvent->accountInfo;
	DisplayString (eventString);
	eventString = "                Authorization Code: ";
	eventString += pEvent->authorisationCode;
	DisplayString (eventString);                                 
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_call_info_event() " );
	}
}

void CCtiEvent::CstaDoNotDisturbEvent()
{
	try
	{
	CSTADoNotDisturbEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.doNotDisturb; 
	DisplayString ("CSTA UNSOLICITED RC_EVENT Do Not Disturb : ");
#ifdef _windows_
	DisplayUnsolicitedEvent (eventString);
	eventString = "         Device: ";
	eventString += AddExtendedDeviceID (pEvent->device);
	eventString += "    State: ";
	if (pEvent->doNotDisturbOn)
	eventString += "On";
	else
	eventString += "Off";      
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_do_not_distrub_event() " );
	}
	
}

void CCtiEvent::CstaForwardingEvent()
{
	try
	{
	CSTAForwardingEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.forwarding; 
	//DisplayUnsolicitedEvent ("CSTA UNSOLICITED RC_EVENT Forwarding :");
	DisplayString ("CSTA UNSOLICITED RC_EVENT Forwarding :");
#ifdef _windows_
	eventString = "         Device: ";
	eventString += AddExtendedDeviceID (pEvent->device);
	DisplayString (eventString);
	eventString = "         Type: ";

	switch (pEvent->forwardingInformation.forwardingType) {
	case FWD_IMMEDIATE :
	  eventString += "IMMEDIATE";
	  break;
	case FWD_BUSY :
	  eventString += "BUSY";
	  break;
	case FWD_NO_ANS :
	  eventString += "NO ANSWER";
	  break;
	case FWD_BUSY_INT :
	  eventString += "BUSY INTERNAL";
	  break;
	case FWD_BUSY_EXT :
	  eventString += "BUSY EXTERNAL";
	  break;
	case FWD_NO_ANS_INT :
	  eventString += "NO ANSWER INTERNAL";
	  break;
	case FWD_NO_ANS_EXT :
	  eventString += "NO ANSWER EXTERNAL";
	  break;
	default :
	  eventString += "unknown";
	  break;
	}
	eventString += "   State: ";
	if (pEvent->forwardingInformation.forwardingOn)
	eventString += "ON";
	else
	eventString += "OFF";
	eventString += "   Forwarded to: ";
	eventString += pEvent->forwardingInformation.forwardDN;
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_forwarding_event() " );
	}
}

void CCtiEvent::CstaMessageWaitingEvent()
{
	try
	{
	CSTAMessageWaitingEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.messageWaiting; 
#ifdef _windows_
	DisplayString ("CSTA UNSOLICITED RC_EVENT Message Waiting : ");
	DisplayUnsolicitedEvent (eventString);
	eventString = "         For Device: ";
	eventString += AddExtendedDeviceID (pEvent->deviceForMessage);

	DisplayString (eventString);
	eventString = "         From Device: ";
	eventString += AddExtendedDeviceID (pEvent->invokingDevice);
	DisplayString (eventString);
	eventString += "        State: ";
	if (pEvent->messageWaitingOn)
	eventString += "On";
	else
	eventString += "Off";      
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_message_wariting_event() " );
	}
}

void CCtiEvent::CstaLoggedOnEvent()
{
	try
	{
	CSTALoggedOnEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.loggedOn;
	DisplayString ("CSTA UNSOLICITED RC_EVENT Logged On V2 ");
#ifdef _windows_
	eventString += AddExtendedDeviceID (pEvent->agentDevice);
	DisplayUnsolicitedEvent (eventString);
	eventString = "              Agent ID: ";
	eventString += pEvent->agentID;
	DisplayString (eventString);
	eventString = "              Agent Group: ";
	eventString += pEvent->agentGroup;
	DisplayString (eventString);
	eventString = "              Agent Password: ";
	eventString += pEvent->password;
	
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_logged_on_event() " );
	}
}

void CCtiEvent::CstaLoggedOffEvent()
{
	try
	{
	CSTALoggedOffEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.loggedOff;
#ifdef _windows_
	DisplayString ("CSTA UNSOLICITED RC_EVENT Logged Off V2 ");
	eventString += AddExtendedDeviceID (pEvent->agentDevice);
	DisplayUnsolicitedEvent (eventString);
	eventString = "              Agent ID: ";
	eventString += pEvent->agentID;
	DisplayString (eventString);
	eventString = "              Agent Group: ";
	eventString += pEvent->agentGroup;
	
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_logged_off_event() " );
	}
}

void CCtiEvent::CstaNotReadyEvent()
{
	try
	{
	CSTANotReadyEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.notReady;
	DisplayString ("CSTA UNSOLICITED RC_EVENT Not Ready V2 ");
#ifdef _windows_
	eventString += AddExtendedDeviceID (pEvent->agentDevice);
	DisplayUnsolicitedEvent (eventString);
	eventString = "                Agent ID: ";
	eventString += pEvent->agentID;
	
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_not_ready_event() " );
	}
}

void CCtiEvent::CstaReadyEvent()
{
	try
	{
	CSTAReadyEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.ready;
	DisplayString ("CSTA UNSOLICITED RC_EVENT Ready V2 ");
#ifdef _windows_
	eventString += AddExtendedDeviceID (pEvent->agentDevice);
	DisplayUnsolicitedEvent (eventString);
	eventString = "                Agent ID: ";
	eventString += pEvent->agentID;
	//eventString += GetEventCause (pEvent->cause);
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_ready_event() " );
	}
}

void CCtiEvent::CstaWorkNotReady()
{
	try
	{
	CSTAWorkNotReadyEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.workNotReady;
	DisplayString ("CSTA UNSOLICITED RC_EVENT Work Not Ready  ");
#ifdef _windows_
	eventString += AddExtendedDeviceID (pEvent->agentDevice);
	DisplayUnsolicitedEvent (eventString);
	eventString = "                Agent ID: ";
	eventString += pEvent->agentID;
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_work_not_ready() " );
	}
}

void CCtiEvent::CstaWorkReady()
{
	try
	{
	CSTAWorkNotReadyEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.workNotReady;
	DisplayString ("CSTA UNSOLICITED RC_EVENT Work Not Ready  ");

#ifdef _windows_
	eventString += AddExtendedDeviceID (pEvent->agentDevice);
	DisplayUnsolicitedEvent (eventString);
	eventString = "                Agent ID: ";
	eventString += pEvent->agentID;
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_work_ready() " );
	}
}

void CCtiEvent::CstaBackInServiceEvent()
{
	try
	{
	CSTABackInServiceEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.backInService;
	DisplayString ("CSTA UNSOLICITED RC_EVENT Back In Service For Device: ");
#ifdef _windows_
	eventString += pEvent->device;
	eventString += " ";
	eventString += GetEventCause (pEvent->cause);
	DisplayUnsolicitedEvent (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_back_in_service_event() " );
	}
}

void CCtiEvent::CstaOutOfServiceEvent()
{
	try
	{
	CSTAOutOfServiceEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaUnsolicited.u.outOfService;
	DisplayString ("CSTA UNSOLICITED RC_EVENT Out Of Service For Device: ");
#ifdef _windows_
	eventString += pEvent->device;
	eventString += " ";
	eventString += GetEventCause (pEvent->cause);
	DisplayUnsolicitedEvent (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_out_of_service_event() " );
	}
}

void CCtiEvent::CstaMonitorEnded()
{
	try
	{
    string eventString;

	DisplayString ("CSTA UNSOLICITED RC_EVENT Monitor Ended   ");
#ifdef _windows_
	eventString += GetEventCause (m_cstaEvent.event.cstaUnsolicited.u.monitorEnded.cause);
	DisplayUnsolicitedEvent (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_monitor_ended() " );
	}
}

void CCtiEvent::CstaAnswerCallConfEvent()
{
	try
	{
	DisplayString ("CSTA CONFIRM Answer Call");
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_answer_call_conf_event() " );
	}
}

void CCtiEvent::CstaClearCallConfEvent()
{
	try
	{
	DisplayString ("CSTA CONFIRM Clear Call");
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_clear_call_conf_event() " );
	}
}

void CCtiEvent::CstaClearConnectionConfEvent()
{
	try
	{
	DisplayString ("CSTA CONFIRM Clear Connection");
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_clear_connection_conf_event() " );
	}
}

void CCtiEvent::CstaConferenceCallConfEvent()
{
	try
	{
	CSTAConferenceCallConfEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaConfirmation.u.conferenceCall;
	eventString = "CSTA CONFIRM Conference Call  ";
#ifdef _windows_
	eventString += AddConnectionID (pEvent->newCall);
	DisplayString (eventString);
	for (int i = 0; i < pEvent->connList.count; i++) 
	{
		eventString = "           Party ";
		eventString += AddConnectionID (pEvent->connList.connection [i].party);
		DisplayString (eventString);
		eventString = "          Static Device: ";
		eventString += AddExtendedDeviceID (pEvent->connList.connection [i].staticDevice);
		DisplayString (eventString);
	}
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_conference_call_conf_event() " );
	}
}

void CCtiEvent::CstaConsultationCallConfEvent()
{
	try
	{
    string eventString;

	eventString = "CSTA CONFIRM Consultation Call  ";
#ifdef _windows_
	eventString += AddConnectionID (m_cstaEvent.event.cstaConfirmation.u.consultationCall.newCall);
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_consultation_call_conf_event() " );
	}
}

void CCtiEvent::CstaHoldCallConfEvent()
{
	try
	{
	DisplayString ("CSTA CONFIRM Hold Call");
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_hold_call_conf_event() " );
	}
}

void CCtiEvent::CstaMakeCallConfEvent()
{
	try
	{
    string eventString;
	eventString = "CSTA CONFIRM Make Call  ";
#ifdef _windows_
	eventString += AddConnectionID (m_cstaEvent.event.cstaConfirmation.u.makeCall.newCall);
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_make_call_conf_event() " );
	}
}

void CCtiEvent::CstaRetrieveCallConfEvent()
{
	try
	{
	DisplayString ("CSTA CONFIRM Retrieve Call");
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_retrieve_call_conf_event() " );
	}
}

void CCtiEvent::CstaTransferCallConfEvent()
{
	try
	{
	CSTATransferCallConfEvent_t *pEvent;
    string eventString;

	pEvent = &m_cstaEvent.event.cstaConfirmation.u.transferCall;
#ifdef _windows_
	eventString = "CSTA CONFIRM Transfert Call  ";
	eventString += AddConnectionID (pEvent->newCall);
	DisplayString (eventString);
	for (int i = 0; i < pEvent->connList.count; i++) 
	{
		eventString ="           Party ";
		eventString += AddConnectionID (pEvent->connList.connection [i].party);
		DisplayString (eventString);
		eventString = "          Static Device: ";
		eventString += AddExtendedDeviceID (pEvent->connList.connection [i].staticDevice);
		DisplayString (eventString);
	}
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_transfer_call_conf_event() " );
	}
}

void CCtiEvent::CstaUniversalFailureConfEvent(CSTAUniversalFailure_t universalFailure)
{
	try
	{
    string eventString;
	char str1 [40];

	eventString = "CSTA CONFIRM Failure ";
	switch (universalFailure) 
	{
	case GENERIC_UNSPECIFIED :
	  eventString += "GENERIC_UNSPECIFIED";
	  break;
	case GENERIC_OPERATION :
	  eventString += "GENERIC_OPERATION";
	  break;
	case REQUEST_INCOMPATIBLE_WITH_OBJECT :
	  eventString += "REQUEST_INCOMPATIBLE_WITH_OBJECT";
	  break;
	case VALUE_OUT_OF_RANGE :
	  eventString += "VALUE_OUT_OF_RANGE";
	  break;
	case OBJECT_NOT_KNOWN :
	  eventString += "OBJECT_NOT_KNOWN";
	  break;
	case INVALID_CALLING_DEVICE :
	  eventString += "INVALID_CALLING_DEVICE";
	  break;
	case INVALID_CALLED_DEVICE :
	  eventString += "INVALID_CALLED_DEVICE";
	  break;
	case INVALID_FORWARDING_DESTINATION :
	  eventString += "INVALID_FORWARDING_DESTINATION";
	  break;
	case PRIVILEGE_VIOLATION_ON_SPECIFIED_DEVICE :
	  eventString += "PRIVILEGE_VIOLATION_ON_SPECIFIED_DEVICE";
	  break;
	case PRIVILEGE_VIOLATION_ON_CALLED_DEVICE :
	  eventString += "PRIVILEGE_VIOLATION_ON_CALLED_DEVICE";
	  break;
	case PRIVILEGE_VIOLATION_ON_CALLING_DEVICE :
	  eventString += "PRIVILEGE_VIOLATION_ON_CALLING_DEVICE";
	  break;
	case INVALID_CSTA_CALL_IDENTIFIER :
	  eventString += "INVALID_CSTA_CALL_IDENTIFIER";
	  break;
	case INVALID_CSTA_DEVICE_IDENTIFIER :
	  eventString += "INVALID_CSTA_DEVICE_IDENTIFIER";
      m_MainEntry.m_DataProc.CstaMonitorEvent( RC_RESPONSE_FAILED );
	  break;
	case INVALID_CSTA_CONNECTION_IDENTIFIER :
	  eventString += "INVALID_CSTA_CONNECTION_IDENTIFIER";
	  break;
	case INVALID_DESTINATION :
	  eventString += "INVALID_DESTINATION";
	  break;
	case INVALID_FEATURE :
	  eventString += "INVALID_FEATURE";
	  break;
	case INVALID_ALLOCATION_STATE :
	  eventString += "INVALID_ALLOCATION_STATE";
	  break;
	case INVALID_CROSS_REF_ID :
	  eventString += "INVALID_CROSS_REF_ID";
	  break;
	case INVALID_OBJECT_TYPE :
	  eventString += "INVALID_OBJECT_TYPE";
	  break;
	case SECURITY_VIOLATION :
	  eventString += "SECURITY_VIOLATION";
	  break;
	case GENERIC_STATE_INCOMPATIBILITY :
	  eventString += "GENERIC_STATE_INCOMPATIBILITY";
	  break;
	case INVALID_OBJECT_STATE :
	  eventString += "INVALID_OBJECT_STATE";
	  break;
	case INVALID_CONNECTION_ID_FOR_ACTIVE_CALL :
	  eventString += "INVALID_CONNECTION_ID_FOR_ACTIVE_CALL";
	  break;
	case NO_ACTIVE_CALL :
	  eventString += "NO_ACTIVE_CALL";
	  break;
	case NO_HELD_CALL :
	  eventString += "NO_HELD_CALL";
	  break;
	case NO_CALL_TO_CLEAR :
	  eventString += "NO_CALL_TO_CLEAR";
	  break;
	case NO_CONNECTION_TO_CLEAR :
	  eventString += "NO_CONNECTION_TO_CLEAR";
	  break;
	case NO_CALL_TO_ANSWER :
	  eventString += "NO_CALL_TO_ANSWER";
	  break;
	case NO_CALL_TO_COMPLETE :
	  eventString += "NO_CALL_TO_COMPLETE";
	  break;
	case GENERIC_SYSTEM_RESOURCE_AVAILABILITY :
	  eventString += "GENERIC_SYSTEM_RESOURCE_AVAILABILITY";
	  break;
	case SERVICE_BUSY :
	  eventString += "SERVICE_BUSY";
	  break;
	case RESOURCE_BUSY :
	  eventString += "RESOURCE_BUSY";
	  break;
	case RESOURCE_OUT_OF_SERVICE :
	  eventString += "RESOURCE_OUT_OF_SERVICE";
	  break;
	case NETWORK_BUSY :
	  eventString += "NETWORK_BUSY";
	  break;
	case NETWORK_OUT_OF_SERVICE :
	  eventString += "NETWORK_OUT_OF_SERVICE";
	  break;
	case OVERALL_MONITOR_LIMIT_EXCEEDED :
	  eventString += "OVERALL_MONITOR_LIMIT_EXCEEDED";
	  break;
	case CONFERENCE_MEMBER_LIMIT_EXCEEDED :
	  eventString += "CONFERENCE_MEMBER_LIMIT_EXCEEDED";
	  break;
	case GENERIC_SUBSCRIBED_RESOURCE_AVAILABILITY :
	  eventString += "GENERIC_SUBSCRIBED_RESOURCE_AVAILABILITY";
	  break;
	case OBJECT_MONITOR_LIMIT_EXCEEDED :
	  eventString += "OBJECT_MONITOR_LIMIT_EXCEEDED";
	  break;
	case EXTERNAL_TRUNK_LIMIT_EXCEEDED :
	  eventString += "EXTERNAL_TRUNK_LIMIT_EXCEEDED";
	  break;
	case OUTSTANDING_REQUEST_LIMIT_EXCEEDED :
	  eventString += "OUTSTANDING_REQUEST_LIMIT_EXCEEDED";
	  break;
	case GENERIC_PERFORMANCE_MANAGEMENT :
	  eventString += "GENERIC_PERFORMANCE_MANAGEMENT";
	  break;
	case PERFORMANCE_LIMIT_EXCEEDED :
	  eventString += "PERFORMANCE_LIMIT_EXCEEDED";
	  break;
	case SEQUENCE_NUMBER_VIOLATED :
	  eventString += "SEQUENCE_NUMBER_VIOLATED";
	  break;
	case TIME_STAMP_VIOLATED :
	  eventString += "TIME_STAMP_VIOLATED";
	  break;
	case PAC_VIOLATED :
	  eventString += "PAC_VIOLATED";
	  break;
	case SEAL_VIOLATED :
	  eventString += "SEAL_VIOLATED";
	  break;
	case GENERIC_UNSPECIFIED_REJECTION :
	  eventString += "GENERIC_UNSPECIFIED_REJECTION";
	  break;
	case GENERIC_OPERATION_REJECTION :
		{
		  eventString += "GENERIC_OPERATION_REJECTION";
		  TraceEx( "GENERIC_OPERATION_REJECTION, and exit now" );
		  exit( 0 );	//add by liu, 2011-05-03
		}
	  break;
	case DUPLICATE_INVOCATION_REJECTION :
	  eventString += "DUPLICATE_INVOCATION_REJECTION";
	  break;
	case UNRECOGNIZED_OPERATION_REJECTION :
	  eventString += "UNRECOGNIZED_OPERATION_REJECTION";
	  break;
	case MISTYPED_ARGUMENT_REJECTION :
	  eventString += "MISTYPED_ARGUMENT_REJECTION";
	  break;
	case RESOURCE_LIMITATION_REJECTION :
	  eventString += "RESOURCE_LIMITATION_REJECTION";
	  break;
	case ACS_HANDLE_TERMINATION_REJECTION :
	  eventString += "ACS_HANDLE_TERMINATION_REJECTION";
	  break;
	case SERVICE_TERMINATION_REJECTION :
	  eventString += "SERVICE_TERMINATION_REJECTION";
	  break;
	case REQUEST_TIMEOUT_REJECTION :
	  eventString += "REQUEST_TIMEOUT_REJECTION";
	  break;
	case REQUESTS_ON_DEVICE_EXCEEDED_REJECTION :
	  eventString += "REQUESTS_ON_DEVICE_EXCEEDED_REJECTION";
	  break;
	default :
	  sprintf (str1, "%d", universalFailure);
	  eventString += str1;
	  break;
	}
	DisplayString (eventString);
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_universal_failure_conf_event() " );
	}
}

void CCtiEvent::CstaSingleStepConfEvent()
{
	try
	{
	DisplayString ("CSTA CONFIRM Single Step Conference");
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_single_step_conf_event() " );
	}
}

void CCtiEvent::CstaMonitorConfEvent()
{
	try
	{
	char refIdStr [15];
    string eventString;

    m_MainEntry.m_DataProc.CstaMonitorEvent( RC_RESPONSE_OK );

	sprintf (refIdStr, "%ld", m_cstaEvent.event.cstaConfirmation.u.monitorStart.monitorCrossRefID);
	eventString = "CSTA CONFIRMATION Set Monitor - Cross Reference ID: ";
    DisplayString (eventString);
#ifdef _windows_
    eventString += refIdStr;
	eventString = "           Call: ";
	sprintf (refIdStr, "%X ", m_cstaEvent.event.cstaConfirmation.u.monitorStart.monitorFilter.call);
	eventString +=  refIdStr;
	eventString += "  Feature: ";
	sprintf (refIdStr, "%X ", m_cstaEvent.event.cstaConfirmation.u.monitorStart.monitorFilter.feature);
	eventString +=  refIdStr;
	eventString += "  Agent: ";
	sprintf (refIdStr, "%X ", m_cstaEvent.event.cstaConfirmation.u.monitorStart.monitorFilter.agent);
	eventString +=  refIdStr;
	eventString += "  Maintenance: ";
	sprintf (refIdStr, "%X ", m_cstaEvent.event.cstaConfirmation.u.monitorStart.monitorFilter.maintenance);
	eventString +=  refIdStr;
	eventString += "  Private: ";
	sprintf (refIdStr, "%lX ", m_cstaEvent.event.cstaConfirmation.u.monitorStart.monitorFilter.privateFilter);
	eventString +=  refIdStr;
	DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_monitor_conf_event() " );
	}
}

void CCtiEvent::CstaMonitorStopConfEvent()
{
	try
	{
	DisplayString ("CSTA CONFIRMATION Monitor Stop");
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::csta_monitor_stop_conf_event() " );
	}
}

void CCtiEvent::DisplayString(string eventStr)
{
	try
	{
    TraceEx((char *)eventStr.c_str());
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::display_string() " );
	}
}

string CCtiEvent::AddConnectionID(ConnectionID_t connectionID)
{
	try
	{
    string eventString;
	char callID [15];

	eventString = "Call ID: ";
	sprintf (callID, "%ld", connectionID.callID);
	eventString += callID;
	eventString += " Device ID: ";
	eventString += connectionID.deviceID;
  

	switch (connectionID.devIDType) 
	{
	case STATIC_ID :
		eventString += "  Static ID";
		break;
	case DYNAMIC_ID :
		eventString += "  Dynamic ID";
		break;
	default :
		eventString += "  Unknown Type";
		break;
	}
	return (eventString);
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::add_connection_id() " );
	}
	return "";
}

void CCtiEvent::DisplayUnsolicitedEvent(string eventStr)
{
	try
	{
	char crossRefID [20];
    string newStr;

	if (m_bDisplayingSession) 
	{
        newStr = "";
		sprintf (crossRefID, "%d ", m_cstaEvent.event.cstaUnsolicited.monitorCrossRefId);
		newStr = "Cross Ref ID:";
		newStr += crossRefID;
		newStr += eventStr;
		DisplayString(newStr);
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::diplay_unsolicited_event() " );
	}
}

string CCtiEvent::GetLocalConnectionInfo(LocalConnectionState_t localConnectionInfo)
{
	try
	{
    string eventString;
	char str1 [15];

	eventString = "Connection State: ";
	switch ((char)localConnectionInfo) 
	{
	case CS_NULL :
	  eventString += "NULL";
	  break;
	case CS_INITIATE :
	  eventString += "INITIATE";
	  break;
	case CS_ALERTING :
	  eventString += "ALERTING";
	  break;
	case CS_CONNECT :
	  eventString += "CONNECT";
	  break;
	case CS_HOLD :
	  eventString += "HOLD";
	  break;
	case CS_QUEUED :
	  eventString += "QUEUED";
	  break;
	case CS_FAIL :
	  eventString += "FAIL";
	  break;
	case CS_NONE :
	  eventString += "NONE";
	  break;
	default :
	  sprintf (str1, "%ld", localConnectionInfo);
	  eventString += str1;
	  break;
	}
	return (eventString);
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::get_local_connection_info() " );
	}
	return "";
}

string CCtiEvent::GetEventCause(CSTAEventCause_t cause)
{
	try
	{
    string eventString;
	char str1 [15];

	eventString = "Event Cause: ";
	switch ((char)cause) 
	{
	  case EC_NONE :
		eventString += "NONE";
		break;
	  case EC_ACTIVE_MONITOR :
		eventString += "ACTIVE MONITOR";
		break;
	  case EC_ALTERNATE :
		eventString += "ALTERNATE";
		break;
	  case EC_BUSY :
		eventString += "BUSY";
		break;
	  case EC_CALL_BACK :
		eventString += "CALL BACK";
		break;
	  case EC_CALL_CANCELLED :
		eventString += "CALL CANCELLED";
		break;
	  case EC_CALL_FORWARD_ALWAYS :
		eventString += "CALL FORWARD ALWAYS";
		break;
	  case EC_CALL_FORWARD_BUSY :
		eventString += "CALL FORWARD BUSY";
		break;
	  case EC_CALL_FORWARD_NO_ANSWER :
		eventString += "CALL FORWARD NO ANSWER";
		break;
	  case EC_CALL_FORWARD :
		eventString += "CALL FORWARD";
		break;
	  case EC_CALL_NOT_ANSWERED :
		eventString += "CALL NOT ANSWERED";
		break;
	  case EC_CALL_PICKUP :
		eventString += "CALL PICKUP";
		break;
	  case EC_CAMP_ON :
		eventString += "CAMP ON";
		break;
	  case EC_DEST_NOT_OBTAINABLE :
		eventString += "DESTINATION NOT OBTAINABLE";
		break;
	  case EC_DO_NOT_DISTURB :
		eventString += "DO NOT DISTURB";
		break;
	  case EC_INCOMPATIBLE_DESTINATION :
		eventString += "INCOMPATIBLE DESTINATION";
		break;
	  case EC_INVALID_ACCOUNT_CODE :
		eventString += "INVALID ACCOUNT CODE";
		break;
	  case EC_KEY_CONFERENCE :
		eventString += "KEY CONFERENCE";
		break;
	  case EC_LOCKOUT :
		eventString += "LOCKOUT";
		break;
	  case EC_MAINTENANCE :
		eventString += "MAINTENANCE";
		break;
	  case EC_NETWORK_CONGESTION :
		eventString += "NETWORK CONGESTION";
		break;
	  case EC_NETWORK_NOT_OBTAINABLE :
		eventString += "NETWORK NOT OBTAINABLE";
		break;
	  case EC_NEW_CALL :
		eventString += "NEW CALL";
		break;
	  case EC_NO_AVAILABLE_AGENTS :
		eventString += "NO AVAILABLE AGENTS";
		break;
	  case EC_OVERRIDE :
		eventString += "OVERRIDE";
		break;
	  case EC_PARK :
		eventString += "PARK";
		break;
	  case EC_OVERFLOW :
		eventString += "OVERFLOW";
		break;
	  case EC_RECALL :
		eventString += "RECALL";
		break;
	  case EC_REDIRECTED :
		eventString += "REDIRECTED";
		break;
	  case EC_REORDER_TONE :
		eventString += "RECORDER TONE";
		break;
	  case EC_RESOURCES_NOT_AVAILABLE :
		eventString += "RESOURCES NOT AVAILABLE";
		break;
	  case EC_SILENT_MONITOR :
		eventString += "SILENT MONITOR";
		break;
	  case EC_TRANSFER :
		eventString += "TRANSFER";
		break;
	  case EC_TRUNKS_BUSY :
		eventString += "TRUNKS BUSY";
		break;
	  case EC_VOICE_UNIT_INITIATOR :
		eventString += "VOICE UNIT INITIATOR";
		break;
	/*
	  case 	EC_BLOCKED :
		eventString += "	BLOCKED";
		break;
	  case 	EC_CHARACTER_COUNT_REACHED :
		eventString += "";
		break;
	  case 	EC_CONSULTATION :
		eventString += "CHARACTER COUNT REACHED";
		break;
	  case EC_DISTRIBUTED :
		eventString += "DISTRIBUTED";
		break;
	  case 	EC_DTMF_TONE_DETECTED :
		eventString += "DTMF TONE DETECTED";
		break;
	  case 	EC_DURATION_EXCEEDED :
		eventString += "DURATION EXCEEDED";
		break;
	  case 	EC_END_OF_DATA_DETECTED :
		eventString += "END OF DATA DETECTED";
		break;
	  case 	EC_ENTERING_DISTRIBUTION :
		eventString += "ENTERING DISTRIBUTION";
		break;
	  case EC_FORCED_PAUSE :
		eventString += "FORCED PAUSE";
		break;
	  case 	EC_MAKE_CALL :
		eventString += "MAKE CALL";
		break;
	  case 	EC_MESSAGE_SIZE_EXECEEDED :
		eventString += "MESSAGE SIZE EXECEEDED";
		break;
	  case 	EC_NETWORK_SIGNAL :
		eventString += "NETWORK SIGNAL";
		break;
	  case 	EC_NEXT_MESSAGE :
		eventString += "NEXT MESSAGE";
		break;
	  case EC_NORMAL_CLEARING :
		eventString += "NORMAL CLEARING";
		break;	
	  case EC_NO_SPEECH_DETECTED :
		eventString += "NO SPEECH DETECTED";
		break;	
	  case 	EC_NUMBER_CHANGED :
		eventString += "NUMBER CHANGED";
		break;
	  case 	EC_SINGLE_STEP_CONFERENCE :
		eventString += "SINGLE STEP CONFERENCE";
		break;
	  case 	EC_SINGLE_STEP_TRANSFER :
		eventString += "SINGLE STEP TRANSFER";
		break;
	  case 	EC_SPEECH_DETECTED :
		eventString += "SPEECH DETECTED";
		break;
	  case 	EC_SWITCH_TERMINATED :
		eventString += "SWITCH TERMINATED";
		break;
	  case 	EC_TERMINATION_CHARACTER_RECEIVED :
		eventString += "ERMINATION CHARACTER RECEIVED";
		break;
	  case 	EC_TIMEOUT :
		eventString += "TIMEOUT";
		break;
	*/
	default :
	  sprintf (str1, "%ld", cause);
	  eventString += str1;
	  break;
	}
	return (eventString);
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::get_event_cause() " );
	}
	return "";
}

string CCtiEvent::AddExtendedDeviceID(ExtendedDeviceID_t extendedDeviceID)
{
	try
	{
    string eventString;

	eventString = "Device ID: ";
	eventString += extendedDeviceID.deviceID;
	eventString += "  Type: ";
	switch (extendedDeviceID.deviceIDType) 
	{
	case DEVICE_IDENTIFIER :
	  eventString += "DEVICE IDENTIFIER";
	  break;
	case IMPLICIT_PUBLIC :
	  eventString += "IMPLICIT PUBLIC";
	  break;
	case EXPLICIT_PUBLIC_UNKNOWN :
	  eventString += "EXPLICIT PUBLIC UNKNOWN";
	  break;
	case EXPLICIT_PUBLIC_INTERNATIONAL :
	  eventString += "EXPLICIT PUBLIC INTERNATIONAL";
	  break;
	case EXPLICIT_PUBLIC_NATIONAL :
	  eventString += "EXPLICIT PUBLIC NATIONAL";
	  break;
	case EXPLICIT_PUBLIC_NETWORK_SPECIFIC :
	  eventString += "EXPLICIT PUBLIC NETWORK SPECIFIC";
	  break;
	case EXPLICIT_PUBLIC_SUBSCRIBER :
	  eventString += "EXPLICIT PUBLIC SUBSCRIBER";
	  break;
	case EXPLICIT_PUBLIC_ABBREVIATED :
	  eventString += "EXPLICIT PUBLIC ABBREVIATED";
	  break;
	case IMPLICIT_PRIVATE :
	  eventString += "IMPLICIT PRIVATE";
	  break;
	case EXPLICIT_PRIVATE_UNKNOWN :
	  eventString += "EXPLICIT UNKNOWN";
	  break;
	case EXPLICIT_PRIVATE_LEVEL3_REGIONAL_NUMBER :
	  eventString += "EXPLICIT PRIVATE LEVEL 3 REGIONAL NUMBER";
	  break;
	case EXPLICIT_PRIVATE_LEVEL2_REGIONAL_NUMBER :
	  eventString += "EXPLICIT PRIVATE LEVEL 2 REGIONAL NUMBER";
	  break;
	case EXPLICIT_PRIVATE_LEVEL1_REGIONAL_NUMBER :
	  eventString += "EXPLICIT PRIVATE LEVEL 1 REGIONAL NUMBER";
	  break;
	case EXPLICIT_PRIVATE_PTN_SPECIFIC_NUMBER :
	  eventString += "EXPLICIT PRIVATE PTN SPECIFIC NUMBER";
	  break;
	case EXPLICIT_PRIVATE_LOCAL_NUMBER :
	  eventString += "EXPLICIT PRIVATE LOCAL NUMBER";
	  break;
	case EXPLICIT_PRIVATE_ABBREVIATED :
	  eventString += "EXPLICIT PRIVATE ABBREVIATED";
	  break;
	case OTHER_PLAN :
	  eventString += "OTHER PLAN";
	  break;
	case TRUNK_IDENTIFIER :
	  eventString += "TRUNK IDENTIFIER";
	  break;
	case TRUNK_GROUP_IDENTIFIER :
	  eventString += "TRUNK GROUP IDENTIFIER";
	  break;
	default :
	  eventString += "Unknown Type";
	  break;
	}
	eventString += "  Status: ";
	switch (extendedDeviceID.deviceIDStatus) {
	case ID_PROVIDED :
	  eventString += "ID PROVIDED";
	  break;
	case ID_NOT_KNOWN :
	  eventString += "ID NOT KNOWN";
	  break;
	case ID_NOT_REQUIRED :
	  eventString += "ID NOT REQUIRED";
	  break;
	default :
	  eventString += "Unknown status";
	  break;
	}
	return (eventString);
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::add_extendes_device_id() " );
	}
	return "";
}

void CCtiEvent::CorrectUnsolicitedRefId(PEXTINFO pEi, ConnectionID_t connectionID, bool bHeld)
{
	try
	{
	if( pEi != NULL )
	{
		if(bHeld)
		{
			if( pEi->nHeldCallID != connectionID.callID )
				pEi->nHeldCallID = connectionID.callID;
		}
		else
		{
			if( pEi->nCallID != connectionID.callID )
				pEi->nCallID = connectionID.callID;
		}
		
		if( connectionID.devIDType == DYNAMIC_ID && 
			strcmp( pEi->szDynamicID , connectionID.deviceID ) != 0 )
			strcpy( pEi->szDynamicID , connectionID.deviceID );
		
		/*
		if (((PrivateData_t*)recPrivateData)->length != 0)
		{			
			CstaCommonArguments_t *cstaCommonArgumentsPt;
			cstaCommonArgumentsPt = (CstaCommonArguments_t*)((char *)(((PrivateData_t*)recPrivateData)->data + 1));
			
			// get first pointer

			AlcatelCstaPrivObject_t* alcPrivObjFirstPt;
			alcPrivObjFirstPt = (AlcatelCstaPrivObject_t*) cstaCommonArgumentsPt->privateData.privateHeap;

			for (int i=0; i < cstaCommonArgumentsPt->privateData.numberOfPrivateData; i++)
			{
				AlcatelCstaPrivObject_t* alcPrivObjPt;
				alcPrivObjPt = (AlcatelCstaPrivObject_t*)((char*) cstaCommonArgumentsPt->privateData.privateHeap +
							cstaCommonArgumentsPt->privateData.AlcCstaPrivPtOffsetArray[i]);

				if( alcPrivObjPt->objectId == globalCallID_Id )
				{
					char globalcallid[MAX_LENGTH_OCTET_STRING];
					char buffer[2*MAX_LENGTH_OCTET_STRING];
					memset(globalcallid,0, MAX_LENGTH_OCTET_STRING);
					memset(buffer,0, 2*MAX_LENGTH_OCTET_STRING);
					memcpy(globalcallid, 	(char *)&(alcPrivObjPt->u.globalCallID.octetString), alcPrivObjPt->u.globalCallID.length);				
					for (int j=0 ; j < alcPrivObjPt->u.globalCallID.length; j++)
					{
						sprintf (&buffer[2*j], "% 02x",(unsigned char ) globalcallid[j]);
					}
					if( strcmp( pEi->szGlobalCallid , buffer ) != 0 )
					{
						memset( pEi->szGlobalCallid, 0, 128 );
						strcpy( pEi->szGlobalCallid , buffer );
					}
				}
			}
		}
		*/
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::correct_unsolicited_ref_id() " );
	}
}

void CCtiEvent::DisplayPrivateData(ATTPrivateData_t *privateDataPt)
{
#ifdef _VC6_
	try
	{
    string eventString;
	unsigned long j = 0;
	unsigned long i = 0;

	ATTEvent_t attEvent;
	//ATTPrivateData_t RecData;

	m_nRetCode=attPrivateData(privateDataPt,&attEvent);
	if (m_nRetCode < 0) 
	{
		TraceEx("attPrivateData RetCode=%d", m_nRetCode);
		return ;
	}

    eventString += "PRIVATE DATA RECEIVED - Vendor: ";
    eventString += privateDataPt->vendor;
    DisplayString (eventString);

	switch(attEvent.eventType) 
	{
	case ATTV5_CLEAR_CONNECTION:
		DisplayATTV5UserInfo(&attEvent.u.v5clearConnectionReq.userInfo);
		break;
	case ATTV5_CONSULTATION_CALL:
		DisplayATTV5UserInfo(&attEvent.u.v5consultationCallReq.userInfo);
		break;
	case ATTV5_MAKE_CALL:
		DisplayATTV5UserInfo(&attEvent.u.v5makeCallReq.userInfo);
		break;
	case ATTV5_DIRECT_AGENT_CALL:
		DisplayATTV5UserInfo(&attEvent.u.v5directAgentCallReq.userInfo);
		break;
	case ATTV5_MAKE_PREDICTIVE_CALL:
		DisplayATTV5UserInfo(&attEvent.u.v5makePredictiveCallReq.userInfo);
		break;
	case ATTV5_SUPERVISOR_ASSIST_CALL:
		DisplayATTV5UserInfo(&attEvent.u.v5supervisorAssistCallReq.userInfo);
		break;
	case ATTV5_RECONNECT_CALL:
		DisplayATTV5UserInfo(&attEvent.u.v5reconnectCallReq.userInfo);
		break;
	case ATTV4_SENDDTMF_TONE:
		{
			ATTV4SendDTMFTone_t *pInfo=&attEvent.u.v4sendDTMFToneReq;
			eventString = "ATTV4SendDTMFTone RECEIVED  ";
			DisplayString (eventString);

			eventString = "SenderInfo ";
			eventString += AddConnectionID(pInfo->sender);
			DisplayString (eventString);

			DisplayATTV4ConnIDList(&pInfo->receivers);

            char lpszTempStr[1024] = {0};
            sprintf( lpszTempStr, "DTMFToneInfo Tones=%s High=%d Low=%d",pInfo->tones,pInfo->toneDuration,pInfo->pauseDuration);
            eventString = lpszTempStr;
            DisplayString (eventString);
		}
		break;
	case ATT_SENDDTMF_TONE_CONF:
		eventString = "Event Type:       ATT_SENDDTMF_TONE_CONF";
		DisplayString (eventString);
		break;
	case ATTV4_SET_AGENT_STATE:
		eventString = "Event Type:       ATTV4_SET_AGENT_STATE";
		DisplayString (eventString);
		break;
	case ATT_QUERY_ACD_SPLIT:
		eventString = "Event Type:       ATT_QUERY_ACD_SPLIT";
		DisplayString (eventString);
		break;
	case ATT_QUERY_ACD_SPLIT_CONF:
		{
			eventString = "Event Type:       ATT_QUERY_ACD_SPLIT_CONF";
			DisplayString (eventString);

            string strDevice = "";
            if( m_MainEntry.m_DataProc.m_ACDNumList.size() > 0 )
			{
                strDevice = m_MainEntry.m_DataProc.m_ACDNumList.front();
                m_MainEntry.m_DataProc.m_ACDNumList.pop_front();
			}
			//queryAcdSplitReq
            char lpszTempStr[1024] = {0};
            sprintf( lpszTempStr, "Device:: %s", (char *)strDevice.c_str());//attEvent.u.queryAcdSplitReq.device );
            eventString = lpszTempStr;
			DisplayString (eventString);

            sprintf( lpszTempStr, "Available Agents: %d", attEvent.u.queryAcdSplit.availableAgents );
            eventString = lpszTempStr;
			DisplayString (eventString);

            sprintf( lpszTempStr, "Calls In Queue: %d", attEvent.u.queryAcdSplit.callsInQueue );
            eventString = lpszTempStr;
			DisplayString (eventString);

            sprintf( lpszTempStr, "Agents Logged In: %d", attEvent.u.queryAcdSplit.agentsLoggedIn );
            eventString = lpszTempStr;
			DisplayString (eventString);

            char lpszParam[1024] = {0};
            sprintf( lpszParam, "%d^%d^%d", attEvent.u.queryAcdSplit.availableAgents, attEvent.u.queryAcdSplit.callsInQueue, attEvent.u.queryAcdSplit.agentsLoggedIn );

            m_MainEntry.m_DataProc.SendMsg2Extension( "CTI", "VSQL", "ACDQuery", (char *)strDevice.c_str(), lpszParam );
//			g_lpthis->DisplayACDInfo( attEvent.u.queryAcdSplit.availableAgents,
//				attEvent.u.queryAcdSplit.callsInQueue,
//				attEvent.u.queryAcdSplit.agentsLoggedIn );
		}
		break;
	case ATT_QUERY_AGENT_LOGIN:
		eventString = "Event Type:       ATT_QUERY_AGENT_LOGIN";
		DisplayString (eventString);
		break;
	case ATT_QUERY_AGENT_LOGIN_CONF:
		eventString = "Event Type:       ATT_QUERY_AGENT_LOGIN_CONF";
		DisplayString (eventString);
		break;
	case ATT_QUERY_AGENT_LOGIN_RESP:
		eventString = "Event Type:       ATT_QUERY_AGENT_LOGIN_RESP";
		DisplayString (eventString);
		break;
	case ATT_QUERY_AGENT_STATE:
		eventString = "Event Type:       ATT_QUERY_AGENT_STATE";
		DisplayString (eventString);
		break;
	case ATTV4_QUERY_AGENT_STATE_CONF:
		eventString = "Event Type:       ATTV4_QUERY_AGENT_STATE_CONF";
		DisplayString (eventString);
		break;
	case ATT_QUERY_CALL_CLASSIFIER:
		eventString = "Event Type:       ATT_QUERY_CALL_CLASSIFIER";
		DisplayString (eventString);
		break;
	case ATT_QUERY_CALL_CLASSIFIER_CONF:
		eventString = "Event Type:       ATT_QUERY_CALL_CLASSIFIER_CONF";
		DisplayString (eventString);
		break;
	case ATTV4_QUERY_DEVICE_INFO_CONF:
		eventString = "Event Type:       ATTV4_QUERY_DEVICE_INFO_CONF";
		DisplayString (eventString);
		break;
	case ATT_QUERY_MWI_CONF:
		eventString = "Event Type:       ATT_QUERY_MWI_CONF";
		DisplayString (eventString);
		break;
	case ATT_QUERY_STATION_STATUS:
		eventString = "Event Type:       ATT_QUERY_STATION_STATUS";
		DisplayString (eventString);
		break;
	case ATT_QUERY_STATION_STATUS_CONF:
		eventString = "Event Type:       ATT_QUERY_STATION_STATUS_CONF";
		DisplayString (eventString);
		break;
	case ATT_QUERY_TOD:
		eventString = "Event Type:       ATT_QUERY_TOD";
		DisplayString (eventString);
		break;
	case ATT_QUERY_TOD_CONF:
		eventString = "Event Type:       ATT_QUERY_TOD_CONF";
		DisplayString (eventString);
		break;
	case ATT_QUERY_TG:
		eventString = "Event Type:       ATT_QUERY_TG";
		DisplayString (eventString);
		break;
	case ATT_QUERY_TG_CONF:
		{
			eventString = "Event Type:       ATT_QUERY_TG_CONF";
			DisplayString (eventString);

            string strDevice = "";
            if( m_MainEntry.m_DataProc.m_TrunkList.size() > 0 )
			{
                strDevice = m_MainEntry.m_DataProc.m_TrunkList.front();
                m_MainEntry.m_DataProc.m_TrunkList.pop_front();
			}

            char lpszTempStr[1024] = {0};
            sprintf( lpszTempStr, "device: %s", (char *)strDevice.c_str() );
            eventString = lpszTempStr;
			DisplayString (eventString );

            sprintf( lpszTempStr, "Idle Trunks: %d", attEvent.u.queryTg.idleTrunks );
            eventString = lpszTempStr;
			DisplayString (eventString);

            sprintf( lpszTempStr, "Used Trunks: %d", attEvent.u.queryTg.usedTrunks );
            eventString = lpszTempStr;
			DisplayString (eventString);

            char lpszParam[1024] = {0};
            sprintf( lpszParam, "%d^%d", attEvent.u.queryTg.idleTrunks, attEvent.u.queryTg.usedTrunks );

            m_MainEntry.m_DataProc.SendMsg2Extension( "CTI", "VSQL", "TrunkQuery", (char *)strDevice.c_str(), lpszParam );
			
		}
		break;
	case ATTV4_SNAPSHOT_DEVICE_CONF:
		eventString = "Event Type:       ATTV4_SNAPSHOT_DEVICE_CONF";
		DisplayString (eventString);
		break;
	case ATTV4_MONITOR_FILTER:
		eventString = "Event Type:       ATTV4_MONITOR_FILTER";
		DisplayString (eventString);
		break;
	case ATTV4_MONITOR_CONF:
		eventString = "Event Type:       ATTV4_MONITOR_CONF";
		DisplayString (eventString);
		break;
	case ATT_MONITOR_STOP_ON_CALL:
		eventString = "Event Type:       ATT_MONITOR_STOP_ON_CALL";
		DisplayString (eventString);
		break;
	case ATT_MONITOR_STOP_ON_CALL_CONF:
		eventString = "Event Type:       ATT_MONITOR_STOP_ON_CALL_CONF";
		DisplayString (eventString);
		break;
	case ATTV4_MONITOR_CALL_CONF:
		eventString = "Event Type:       ATTV4_MONITOR_CALL_CONF";
		DisplayString (eventString);
		break;
	case ATT_CALL_CLEARED:
		eventString = "Event Type:       ATT_CALL_CLEARED";
		DisplayString (eventString);
		break;
	case ATTV3_CONFERENCED:
		eventString = "Event Type:       ATTV3_CONFERENCED";
		DisplayString (eventString);
		break;
	case ATTV5_CONNECTION_CLEARED:
		eventString = "Event Type:       ATTV5_CONNECTION_CLEARED";
		DisplayString (eventString);
		break;
	case ATTV3_DELIVERED:
		eventString = "Event Type:       ATTV3_DELIVERED";
		DisplayString (eventString);
		break;
	case ATT_ENTERED_DIGITS:
		eventString = "Event Type:       ATT_ENTERED_DIGITS";
		DisplayString (eventString);
		break;
	case ATTV3_ESTABLISHED:
		eventString = "Event Type:       ATTV3_ESTABLISHED";
		DisplayString (eventString);
		break;
	case ATTV4_NETWORK_REACHED:
		eventString = "Event Type:       ATTV4_NETWORK_REACHED";
		DisplayString (eventString);
		break;
	case ATTV3_TRANSFERRED:
		eventString = "Event Type:       ATTV3_TRANSFERRED";
		DisplayString (eventString);
		break;
	case ATTV4_ROUTE_REQUEST:
		eventString = "Event Type:       ATTV4_ROUTE_REQUEST";
		DisplayString (eventString);
		break;
	case ATTV5_ROUTE_SELECT:
		eventString = "Event Type:       ATTV5_ROUTE_SELECT";
		DisplayString (eventString);
		break;
	case ATT_ROUTE_USED:
		eventString = "Event Type:       ATT_ROUTE_USED";
		DisplayString (eventString);
		break;
	case ATT_SYS_STAT:
		eventString = "Event Type:       ATT_SYS_STAT";
		DisplayString (eventString);
		break;
	case ATTV3_LINK_STATUS:
		eventString = "Event Type:       ATTV3_LINK_STATUS";
		DisplayString (eventString);
		break;
	case ATTV5_ORIGINATED:
		eventString = "Event Type:       ATTV5_ORIGINATED";
		DisplayString (eventString);
		break;
	case ATT_LOGGED_ON:
		eventString = "Event Type:       ATT_LOGGED_ON";
		DisplayString (eventString);
		break;
	case ATT_QUERY_DEVICE_NAME:
		eventString = "Event Type:       ATT_QUERY_DEVICE_NAME";
		DisplayString (eventString);
		break;
	case ATTV4_QUERY_DEVICE_NAME_CONF:
		eventString = "Event Type:       ATTV4_QUERY_DEVICE_NAME_CONF";
		DisplayString (eventString);
		break;
	case ATT_QUERY_AGENT_MEASUREMENTS:
		eventString = "Event Type:       ATT_QUERY_AGENT_MEASUREMENTS";
		DisplayString (eventString);
		break;
	case ATT_QUERY_AGENT_MEASUREMENTS_CONF:
		eventString = "Event Type:       ATT_QUERY_AGENT_MEASUREMENTS_CONF";
		DisplayString (eventString);
		break;
	case ATT_QUERY_SPLIT_SKILL_MEASUREMENTS:
		eventString = "Event Type:       ATT_QUERY_SPLIT_SKILL_MEASUREMENTS";
		DisplayString (eventString);
		break;
	case ATT_QUERY_SPLIT_SKILL_MEASUREMENTS_CONF:
		eventString = "Event Type:       ATT_QUERY_SPLIT_SKILL_MEASUREMENTS_CONF";
		DisplayString (eventString);
		break;
	case ATT_QUERY_TRUNK_GROUP_MEASUREMENTS:
		eventString = "Event Type:       ATT_QUERY_TRUNK_GROUP_MEASUREMENTS";
		DisplayString (eventString);
		break;
	case ATT_QUERY_TRUNK_GROUP_MEASUREMENTS_CONF:
		eventString = "Event Type:       ATT_QUERY_TRUNK_GROUP_MEASUREMENTS_CONF";
		DisplayString (eventString);
		break;
	case ATT_QUERY_VDN_MEASUREMENTS:
		eventString = "Event Type:       ATT_QUERY_VDN_MEASUREMENTS";
		DisplayString (eventString);
		break;
	case ATT_QUERY_VDN_MEASUREMENTS_CONF:
		eventString = "Event Type:       ATT_QUERY_VDN_MEASUREMENTS_CONF";
		DisplayString (eventString);
		break;
	case ATTV4_CONFERENCED:
		eventString = "Event Type:       ATTV4_CONFERENCED";
		DisplayString (eventString);
		break;
	case ATTV4_DELIVERED:
		eventString = "Event Type:       ATTV4_DELIVERED";
		DisplayString (eventString);
		break;
	case ATTV4_ESTABLISHED:
		eventString = "Event Type:       ATTV4_ESTABLISHED";
		DisplayString (eventString);
		break;
	case ATTV4_TRANSFERRED:
		eventString = "Event Type:       ATTV4_TRANSFERRED";
		DisplayString (eventString);
		break;
	case ATTV4_LINK_STATUS:
		eventString = "Event Type:       ATTV4_LINK_STATUS";
		DisplayString (eventString);
		break;
	case ATTV4_GETAPI_CAPS_CONF:
		eventString = "Event Type:       ATTV4_GETAPI_CAPS_CONF";
		DisplayString (eventString);
		break;
	case ATT_SINGLE_STEP_CONFERENCE_CALL:
		eventString = "Event Type:       ATT_SINGLE_STEP_CONFERENCE_CALL";
		DisplayString (eventString);
		break;
	case ATT_SINGLE_STEP_CONFERENCE_CALL_CONF:
		eventString = "Event Type:       ATT_SINGLE_STEP_CONFERENCE_CALL_CONF";
		DisplayString (eventString);
		break;
	case ATT_SELECTIVE_LISTENING_HOLD:
		eventString = "Event Type:       ATT_SELECTIVE_LISTENING_HOLD";
		DisplayString (eventString);
		break;
	case ATT_SELECTIVE_LISTENING_HOLD_CONF:
		eventString = "Event Type:       ATT_SELECTIVE_LISTENING_HOLD_CONF";
		DisplayString (eventString);
		break;
	case ATT_SELECTIVE_LISTENING_RETRIEVE:
		eventString = "Event Type:       ATT_SELECTIVE_LISTENING_RETRIEVE";
		DisplayString (eventString);
		break;
	case ATT_SELECTIVE_LISTENING_RETRIEVE_CONF:
		eventString = "Event Type:       ATT_SELECTIVE_LISTENING_RETRIEVE_CONF";
		DisplayString (eventString);
		break;
	case ATT_SENDDTMF_TONE:
		eventString = "Event Type:       ATT_SENDDTMF_TONE";
		DisplayString (eventString);
		break;
	case ATT_SNAPSHOT_DEVICE_CONF:
		eventString = "Event Type:       ATT_SNAPSHOT_DEVICE_CONF";
		DisplayString (eventString);
		break;
	case ATT_LINK_STATUS:
		eventString = "Event Type:       ATT_LINK_STATUS";
		DisplayString (eventString);
		break;
	case ATT_SET_BILL_RATE:
		eventString = "Event Type:       ATT_SET_BILL_RATE";
		DisplayString (eventString);
		break;
	case ATT_SET_BILL_RATE_CONF:
		eventString = "Event Type:       ATT_SET_BILL_RATE_CONF";
		DisplayString (eventString);
		break;
	case ATT_QUERY_UCID:
		eventString = "Event Type:       ATT_QUERY_UCID";
		DisplayString (eventString);
		break;
	case ATT_QUERY_UCID_CONF:
//		DealWithSuccessAttConf(CMD_ATTQUERYUCID,&attEvent);
		eventString = "Event Type:       ATT_QUERY_UCID_CONF";
		DisplayString (eventString);
		eventString = "Universal Call Id : ";
		eventString += attEvent.u.queryUCID.ucid;
		DisplayString (eventString);
		break;
	case ATTV5_CONFERENCED:
		eventString = "Event Type:       ATTV5_CONFERENCED";
		DisplayString (eventString);
		break;
	case ATT_LOGGED_OFF:
		eventString = "Event Type:       ATT_LOGGED_OFF";
		DisplayString (eventString);
		break;
	case ATTV5_DELIVERED:
		eventString = "Event Type:       ATTV5_DELIVERED";
		DisplayString (eventString);
		break;
	case ATTV5_ESTABLISHED:
		eventString = "Event Type:       ATTV5_ESTABLISHED";
		DisplayString (eventString);
		break;
	case ATTV5_TRANSFERRED:
		eventString = "Event Type:       ATTV5_TRANSFERRED";
		DisplayString (eventString);
		break;
	case ATTV5_ROUTE_REQUEST:
		eventString = "Event Type:       ATTV5_ROUTE_REQUEST";
		DisplayString (eventString);
		break;
	case ATT_CONSULTATION_CALL_CONF:
		eventString = "Event Type:       ATT_CONSULTATION_CALL_CONF";
		DisplayString (eventString);
		break;
	case ATT_MAKE_CALL_CONF:
		eventString = "Event Type:       ATT_MAKE_CALL_CONF";
		DisplayString (eventString);
		break;
	case ATT_MAKE_PREDICTIVE_CALL_CONF:
		eventString = "Event Type:       ATT_MAKE_PREDICTIVE_CALL_CONF";
		DisplayString (eventString);
		break;
	case ATTV5_SET_AGENT_STATE:
		eventString = "Event Type:       ATTV5_SET_AGENT_STATE";
		DisplayString (eventString);
		break;
	case ATTV5_QUERY_AGENT_STATE_CONF:
		eventString = "Event Type:       ATTV5_QUERY_AGENT_STATE_CONF";
		DisplayString (eventString);
		break;
	case ATT_QUERY_DEVICE_NAME_CONF:
		eventString = "Event Type:       ATT_QUERY_DEVICE_NAME_CONF";
		DisplayString (eventString);
		break;
	case ATT_CONFERENCE_CALL_CONF:
		eventString = "Event Type:       ATT_CONFERENCE_CALL_CONF";
		DisplayString (eventString);
		break;
	case ATT_TRANSFER_CALL_CONF:
		eventString = "Event Type:       ATT_TRANSFER_CALL_CONF";
		DisplayString (eventString);
		break;
	case ATT_MONITOR_FILTER:
		eventString = "Event Type:       ATT_MONITOR_FILTER";
		DisplayString (eventString);
		break;
	case ATT_MONITOR_CONF:
    {
		eventString = "Event Type:       ATT_MONITOR_CONF";
		DisplayString (eventString);
		eventString = "Server: Monitor Filter";
		DisplayString (eventString);
        char lpszTempStr[1024] = {0};
        sprintf( lpszTempStr, "Used Filter: %d",attEvent.u.monitorStart.usedFilter);
        eventString = lpszTempStr;
		DisplayString (eventString);
    }
		break;
	case ATT_MONITOR_CALL_CONF:
		eventString = "Event Type:       ATT_MONITOR_CALL_CONF";
		DisplayString (eventString);
		break;
	case ATT_SERVICE_INITIATED:
		eventString = "Event Type:       ATT_SERVICE_INITIATED";
		DisplayString (eventString);
		break;
	case ATT_CHARGE_ADVICE:
		eventString = "Event Type:       ATT_CHARGE_ADVICE";
		DisplayString (eventString);
		break;
	case ATT_GETAPI_CAPS_CONF:
		eventString = "Event Type:       ATT_GETAPI_CAPS_CONF";
		DisplayString (eventString);
		break;
	case ATT_QUERY_DEVICE_INFO_CONF:
		eventString = "Event Type:       ATT_QUERY_DEVICE_INFO_CONF";
		DisplayString (eventString);
		break;
	case ATT_SET_ADVICE_OF_CHARGE:
		eventString = "Event Type:       ATT_SET_ADVICE_OF_CHARGE";
		DisplayString (eventString);
		break;
	case ATT_SET_ADVICE_OF_CHARGE_CONF:
		eventString = "Event Type:       ATT_SET_ADVICE_OF_CHARGE_CONF";
		DisplayString (eventString);
		break;
	case ATT_NETWORK_REACHED:
		eventString = "Event Type:       ATT_NETWORK_REACHED";
		DisplayString (eventString);
		break;
	case ATT_SET_AGENT_STATE:
		eventString = "Event Type:       ATT_SET_AGENT_STATE";
		DisplayString (eventString);
		break;
	case ATT_SET_AGENT_STATE_CONF:
		eventString = "Event Type:       ATT_SET_AGENT_STATE_CONF";
		DisplayString (eventString);
		break;
	case ATT_QUERY_AGENT_STATE_CONF:
		eventString = "Event Type:       ATT_QUERY_AGENT_STATE_CONF";
		DisplayString (eventString);
		break;
	case ATT_ROUTE_REQUEST:
		eventString = "Event Type:       ATT_ROUTE_REQUEST";
		DisplayString (eventString);
		break;
	case ATT_TRANSFERRED:
		eventString = "Event Type:       ATT_TRANSFERRED";
		DisplayString (eventString);
		break;
	case ATT_CONFERENCED:
		eventString = "Event Type:       ATT_CONFERENCED";
		DisplayString (eventString);
		break;
	case ATT_CLEAR_CONNECTION:
		eventString = "Event Type:       ATT_CLEAR_CONNECTION";
		DisplayString (eventString);
		break;
	case ATT_CONSULTATION_CALL:
		eventString = "Event Type:       ATT_CONSULTATION_CALL";
		DisplayString (eventString);
		break;
	case ATT_MAKE_CALL:
		eventString = "Event Type:       ATT_MAKE_CALL";
		DisplayString (eventString);
		break;
	case ATT_DIRECT_AGENT_CALL:
		eventString = "Event Type:       ATT_DIRECT_AGENT_CALL";
		DisplayString (eventString);
		break;
	case ATT_MAKE_PREDICTIVE_CALL:
		eventString = "Event Type:       ATT_MAKE_PREDICTIVE_CALL";
		DisplayString (eventString);
		break;
	case ATT_SUPERVISOR_ASSIST_CALL:
		eventString = "Event Type:       ATT_SUPERVISOR_ASSIST_CALL";
		DisplayString (eventString);
		break;
	case ATT_RECONNECT_CALL:
		eventString = "Event Type:       ATT_RECONNECT_CALL";
		DisplayString (eventString);
		break;
	case ATT_CONNECTION_CLEARED:
		eventString = "Event Type:       ATT_CONNECTION_CLEARED";
		DisplayString (eventString);
		break;
	case ATT_ROUTE_SELECT:
		eventString = "Event Type:       ATT_ROUTE_SELECT";
		DisplayString (eventString);
		break;
	case ATT_DELIVERED:
    {
		eventString = "Event Type:       ATT_DELIVERED";
		DisplayString (eventString);
		eventString = "Server: Delivered";
		DisplayString (eventString);

        char lpszTempStr[1024] = {0};
        sprintf( lpszTempStr, "TrunkGroup: %s", attEvent.u.deliveredEvent.trunkGroup );
        eventString = lpszTempStr;
		DisplayString (eventString);

        sprintf( lpszTempStr, "TrunkMember: %s", attEvent.u.deliveredEvent.trunkMember );
        eventString = lpszTempStr;
        DisplayString (eventString);
    }
		break;
	case ATT_ESTABLISHED:
    {
		eventString = "Event Type:       ATT_ESTABLISHED";
		DisplayString (eventString);
		eventString = "Server: Established";
		DisplayString (eventString);

        char lpszTempStr[1024] = {0};
        sprintf( lpszTempStr, "TrunkGroup: %s", attEvent.u.deliveredEvent.trunkGroup );
        eventString = lpszTempStr;
		DisplayString (eventString);

        sprintf( lpszTempStr, "TrunkMember: %s", attEvent.u.deliveredEvent.trunkMember );
        eventString = lpszTempStr;
		DisplayString (eventString);
    }
		break;
	case ATT_ORIGINATED:
		{
            eventString = "Event Type:       ATT_ORIGINATED";
            DisplayString (eventString);
            eventString = "Server: Originated";
            DisplayString (eventString);

            char lpszTempStr[1024] = {0};
            sprintf( lpszTempStr, "Logical Agent: %s",attEvent.u.originatedEvent.logicalAgent);
            eventString = lpszTempStr;
            DisplayString (eventString);
            DisplayUserInfo(&attEvent.u.originatedEvent.userInfo);

            if( m_MainEntry.m_bGetAgentId )
            {
                if( m_pEOriginate != NULL )
                {
                    TraceEx( "ext_no = %s", m_pEOriginate->szExtNo );

                    m_MainEntry.m_DataProc.WriteAgentExtInfo( (char *)attEvent.u.originatedEvent.logicalAgent, m_pEOriginate->szExtNo );
                }
            }
        }
		break;
	default:
		break;
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::display_private_data() " );
	}
#endif
}

string CCtiEvent::AddDeviceID(DeviceID_t deviceID)
{
	try
	{
    string eventString;

	eventString = "Device ID: ";
	eventString += deviceID;
	return (eventString);
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::add_device_id() " );
	}
	return "";
}

void CCtiEvent::DisplayATTV5UserInfo(ATTV5UserToUserInfo_t *pUserInfo)
{
	try
	{
    string eventString;
	eventString = "ATTV5UserInfo RECEIVED  ";
	DisplayString (eventString);
#ifdef _windows_
    char lpszTempStr[1024] = {0};
    sprintf( lpszTempStr, "UUIProtocolType=%d",pUserInfo->type);
    eventString = lpszTempStr;
    DisplayString (eventString);

    sprintf( lpszTempStr, "UUIUserData Len=%d Content=%s",pUserInfo->data.length,pUserInfo->data.value);
    eventString = lpszTempStr;
    DisplayString (eventString);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::display_attv5_user_info() " );
	}
}

void CCtiEvent::DisplayATTV4ConnIDList(ATTV4ConnIDList_t *pInfo)
{
	try
	{
    string eventString;
	eventString = "ATTV4ConnIDList RECEIVED  ";
	DisplayString (eventString);
#ifdef _windows_
	for (int i = 0; i < pInfo->count; i++) 
	{
		eventString = "           Party  ";
		eventString += AddConnectionID (pInfo->party[i]);
		DisplayString (eventString);
	}
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::display_attv4_connId_list() " );
	}
}

void CCtiEvent::DealWithSuccessAttConf(int nComd,ATTEvent_t* pEvent)
{
}

void CCtiEvent::DisplayUserInfo(ATTUserToUserInfo_t *pUserInfo)
{
	try
	{
    string eventString;
	eventString = "UserInfo RECEIVED  ";
	DisplayString (eventString);
#ifdef _windows_
    char lpszTempStr[1024] = {0};
    sprintf( lpszTempStr, "UUIProtocolType=%d",pUserInfo->type);
    eventString = lpszTempStr;
    DisplayString ((char *)eventString.c_str());

    sprintf( lpszTempStr, "UUIUserData Len=%d Content=%s",pUserInfo->data.length,pUserInfo->data.value);
    eventString = lpszTempStr;
    DisplayString ((char *)eventString.c_str());
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::diplay_user_info() " );
	}
}

void CCtiEvent::GetTrunkNo(char *lpszTrunkGroup, char *lpszTrunkMember, int eventType)
{
#ifdef _VC6_
	try
	{
	if( lpszTrunkGroup == NULL || lpszTrunkMember == NULL )
		return;

	if (((ATTPrivateData_t*)recPrivateData)->length != 0)
	{
		//TrunkGroup and TrunkMember
		ATTEvent_t attEvent;
	
		m_nRetCode=attPrivateData((ATTPrivateData_t*)recPrivateData,&attEvent);
		if (m_nRetCode < 0) 
		{
			TraceEx("error, GetTrunkNo, attPrivateData RetCode=%d", m_nRetCode);
			return ;
		}

		if( attEvent.eventType == eventType )
		{
			sprintf( lpszTrunkGroup, "%s", attEvent.u.deliveredEvent.trunkGroup );
			sprintf( lpszTrunkMember, "%s", attEvent.u.deliveredEvent.trunkMember );

			TraceEx( "GetTrunkNo, trunkGroup = %s, trunkMember = %s, eventType = %d", lpszTrunkGroup, lpszTrunkMember, eventType );
		}

	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CCtiEvent::get_trunk_no() " );
	}
#endif
}
