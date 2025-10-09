#ifndef _CTI_CTRL_H_
#define _CTI_CTRL_H_

#include "GlobalTsapiServer.h"

class CCtiCtrl
{
public:
	CCtiCtrl();
	virtual ~CCtiCtrl();
public:
    void StartRequestStateThread();
	void AttQueryTrunkGroup(char *device);
	void AttQueryAcdInfo(char *device);
    void CstaStopRecord(int iClientId, char *deviceID);
    void CstaStartRecord(int iClientId, char *deviceID, char *lpszRecordFileName);
	int CstaMonitorVDN(const char *szVDN);
	void CstaMonitorStop(PEXTINFO pE);
    void CstaMakeCall(int iClientId, DeviceID_t called,char *szExtNo);
	void CstaSingleStepConference(int iClientId, int nJoinType,DeviceID_t DeviceToJoin,PEXTINFO pE);
	void CstaConferenceCall(int iClientId,PEXTINFO pE);
	void CstaTransferCall(int iClientId,PEXTINFO pE);
	void CstaConsultationCall(int iClientId,DeviceID_t called,PEXTINFO pE, DeviceID_t orient_calling);
	void CstaRetrieveCall(int iClientId, PEXTINFO pE);
	void CstaAnswerCall(int iClientId, PEXTINFO pE);
    void CstaClearConnection(int iClientId, PEXTINFO pEInfo, char *lpszGradeFlag);
	void CstaHoldCall(int iClientId, PEXTINFO pEInfo);
	void CstaSetAgentState(int nAgentCmd, char *szExtNo, const char *sAgentID,const char *sAgentGroup,const char *sAgentPassword, int iClientId);
	int CstaMonitorDevice (const char *szExtNo);
	void CstaSysStatReqConfEventTimer();
	void CstaSysStatReq ();
#ifdef _windows_
	void SetOwner(void *lpVoid);
#endif
    bool AcsOpenStream (StreamType_t apiLevel, bool bReconnect);
	void CstaSysStatReqConfEvent ();
private:
	void MonitorAllVDN();
	void TryToReconnect();
	void AcsCloseStream();
private:
    LPACSNOTIFYEVENT m_lpfnAcsNotifyEvent;
    string AddSystemStatus(SystemStatus_t systemStatus);
	RetCode_t m_nRetCode;
	Level_t m_nACSLevel;
    PrivateData_t *pPrivateData;

    string m_sProgName;
    unsigned int my_setTimer;
	int		m_iState;	// 1 - 正常， 0 - 不正常
};

#endif
