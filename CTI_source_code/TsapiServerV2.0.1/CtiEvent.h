#ifndef _CTI_EVENT_H_
#define _CTI_EVENT_H_

class CCtiEvent
{
public:
	CCtiEvent();
	virtual ~CCtiEvent();
public:
	void SetOwner(void *lpVoid);
    int  CtiEvent(void *wParam, void *lParam);
    void DisplayString(string eventStr);
    string AddConnectionID(ConnectionID_t connectionID);
public:
	void DisplayPrivateData(ATTPrivateData_t* privateDataPt);
	CSTAEvent_t m_cstaEvent;
private:
	void DealWithSuccessAttConf(int nComd,ATTEvent_t* pEvent);
	void DisplayUserInfo(ATTUserToUserInfo_t *pUserInfo);
	void DisplayATTV4ConnIDList(ATTV4ConnIDList_t *pInfo);
	void DisplayATTV5UserInfo(ATTV5UserToUserInfo_t *pUserInfo);
    string AddExtendedDeviceID(ExtendedDeviceID_t extendedDeviceID);
    string GetEventCause(CSTAEventCause_t cause);
    string GetLocalConnectionInfo(LocalConnectionState_t localConnectionInfo);
    void DisplayUnsolicitedEvent(string eventStr);
	void CstaMonitorStopConfEvent();
	void CstaMonitorConfEvent ();
	void CstaSingleStepConfEvent();
	void CstaUniversalFailureConfEvent(CSTAUniversalFailure_t universalFailure);
	void CstaTransferCallConfEvent();
	void CstaRetrieveCallConfEvent();
	void CstaMakeCallConfEvent();
	void CstaHoldCallConfEvent();
	void CstaConsultationCallConfEvent();
	void CstaConferenceCallConfEvent();
	void CstaClearConnectionConfEvent();
	void CstaClearCallConfEvent();
	void CstaAnswerCallConfEvent();
	void CstaMonitorEnded();
	void CstaOutOfServiceEvent();
	void CstaBackInServiceEvent();
	void CstaWorkReady();
	void CstaWorkNotReady();
	void CstaReadyEvent();
	void CstaNotReadyEvent();
	void CstaLoggedOffEvent();
	void CstaLoggedOnEvent();
	void CstaMessageWaitingEvent();
	void CstaForwardingEvent();
	void CstaDoNotDisturbEvent();
	void CstaCallInfoEvent();
	void CstaTransferredEvent();
	void CstaServiceInitiatedEvent();
	void CstaRetrieveEvent();
	void CstaQueuedEvent();
	void CstaOriginatedEvent();
	void CstaNetworkReachedEvent();
	void CstaHeldEvent();
	void CstaFailedEvent();
	void CstaEstablishedEvent();
	void CstaDivertedEvent();
	void CstaDeliveredEvent();
	void CstaConnectionClearedEvent();
	void CstaConferencedEvent();
	void CstaCallClearedEvent();
	void AcsCloseStreamConfEvent ();
	void AcsOpenStreamConfEvent ();
	void AcsUniversalFailureConfEvent (ACSUniversalFailure_t error, int i);
    void CorrectUnsolicitedRefId(PEXTINFO pEi,ConnectionID_t connectionID,bool bHeld=FALSE);
	
private:
	void GetTrunkNo(char *lpszTrunkGroup, char *lpszTrunkMember, int eventType);
    string AddDeviceID(DeviceID_t deviceID);
    bool m_bDisplayingSession;
	
	RetCode_t m_nRetCode;
	PEXTINFO m_pEOriginate;
};

#endif
