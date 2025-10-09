#ifndef _DATAPROC_H_
#define _DATAPROC_H_

#include "GlobalTsapiServer.h"
#include "Lock.h"

class CMainDataProc
{
public:
    CMainDataProc();
    virtual ~CMainDataProc();
public:
    void InitDataProc();
	void WriteAgentExtInfo(char *lpszAgentId, char *lpszExtNo);
	void ReadRegAgentIniFile();
	void SendMsg2Extension(char *SourceNumber, char *DestNumber, char *Param1, char *Param2, char *Param3);
    void SetLocalQueueExt(char *lpszLocalQueueExt);
	void SetQueueExt(char *lpszExtNo);
	void SetLisenceNum(int iLisenceNum);
	void MoniUnlock();
	void MoniLock();
    int GetClientIdByExt(unsigned long dwExt);
	int GetClientIdByExt(char *lpszExt);
	void SetOwner(void *lpVoid);
	void DataProc(int iClientId, char *lpszBuffer);
	void OnClientClose(int iClientId);
public:
    void GetFileCreateTime(char *lpszDateTime);
    void getAdapterInfoWithQt(char *lpszHardWareAddr, char *lpszIPAddr);
    void Cmd_Sendmsg_Proc(int iClientId,char *buf,string *strInfo, bool bAll=FALSE);
    PAGENTTYPE GetAgentTypeByExt(char *lpszExt);
    void FindFile(int iClientId, PACKET_HEAD *pHead, DOWNLOAD_FILE_INFO *pDownload_Info,
                             DOWNLOAD_DATA_INFO *pDownload_Data_Info, char *lpszPathName);
    void send_download_event_to_client(int iClientId, PACKET_HEAD *pHead, int iTotalDownloadDataNum,
            DOWNLOAD_FILE_INFO *pDownloadInfo, DOWNLOAD_DATA_INFO *pDownload_Data_Info);
	void DelExtAgentMap(char *lpszExt, char *lpszAgentId);
	void AddExtAgent(char *lpszExt, char *lpszAgentId);
	PAGENTTYPE GetAgentType(int iClientId);
	void DelIVRCallIDInfo(string ivr_num);
	void AddIVRCallIDInfo(string ivr_num, char *lpszIVRCallId);
	void DelCallIDInfo(string callID);
	void AddCallIDInfo(string callID, char *lpszIVRCallId);
    bool IsVDNNumber(char *lpszVDN);
	void OutQue(string QueDevice, string CallID);
	void AddQueueList(string QueDevice, string CallID, char *callingDeviceId, char *calledDeviceId);
	void LoadVDN();
    bool RecordSysRate();
	PEXTINFO GetPEByExt(char *szExtNo);
    bool CheckSeariNo();
	void SeriProc(char *lpszSeriNo);
    int DecodeFrame(string *strInfo, string &strData, char *lpszSpliteValue, int iSpliteLen);
	void RemonitorAllExt();
	void CloseAllClient();
	void ClearTransferData(PEXTINFO pE,LocalConnectionState_t localConnectionInfo);
	PEXTINFO GetExtInfoFromRefId(CSTAMonitorCrossRefID_t nCrossRefId);
	void Send_command(int iClientId,int cmd,int cmdtype,char *lpszSendBuffer, int data_len);
	void CstaMonitorEvent(int iType);
	void UpdateMsgProc(PMSGINFO pMsgInfo);
	list< PTRANSNODE > m_TransNodeList;
	char	m_lpszCardId[256];
	char	m_lpszSeriCardId[256];
    bool m_bSeriCorrect;
    string m_strVDN[64];
	int		m_nVDNNum;
	LockMap<string, char *> m_IVR_CALLIDMap;
	LockMap<string, char *> m_CallID_IVRIDMap;
	LockMap<string,int> m_ExtSockMap;
    list<string> m_ACDNumList;
    list<string> m_TrunkList;
    char	m_lpszDateTime[128];
private:
    void UnregisterExtFromWS(int iClientId, char *szExt);
    void SendLogoutCmdToVSQL(int iClientId, char *lpszExt);
    void Cmd_Request_Download_Folder_Proc(int iClientId, char *lpszBuffer);
    void Cmd_Download_File_Proc(int iClientId, char *buf);
    void CopyTransferDataToDest(string strSrcExt, string strDestExt);
    void DecodeData(char *buf, string *strInfo);
    void Cmd_SetExtNoEvent_Proc(int iClientId, char *buf, string *strInfo);
	void SendLogoutCmd(int iClientId, char *lpszExt, char *lpszVirtualServer);
    bool IsBeyondLinsenceNum();
	void Cmd_SendFax_Proc(int iClientId, char *buf);
    void SetTransferData(string strExt, string strData);
	
	int CheckTheSameExtReged(char *lpszExtNo, int iClientType);
    void Cmd_Start_Record_Proc(int iClientId, char *buf, string *strInfo);
    void Cmd_Stop_Record_Proc(int iClientId, char *buf, string *strInfo);
    void Cmd_ExtRegister_Proc(int iClientId, char *buf, string *strInfo);
    void Cmd_AgentOper_Proc(int iClientId, PACKET_HEAD *pHead, char *buf, string *strInfo);
    void Cmd_ExtHoldCall_Proc(int iClientId, char *buf, string *strInfo);
    void Cmd_ExtRetrieveCall_Proc(int iClientId, char *buf, string *strInfo);
    void Cmd_ExtPickup_Proc(int iClientId, char *buf, string *strInfo);
    void Cmd_ExtHangup_Proc(int iClientId, char *buf, string *strInfo);
    void Cmd_MakeCall_Proc(int iClientId, char *buf, string *strInfo);
    void Cmd_InitTrans_Proc(int iClientId, char *buf, string *strInfo);
    void Cmd_CompleteTrans_Proc(int iClientId, char *buf, string *strInfo);
    void Cmd_CompleteConf_Proc(int iClientId, char *buf, string *strInfo);
    void Cmd_IntrudeExt_Proc(int iClientId, char *buf, string *strInfo);
    void Cmd_ListenExt_Proc(int iClientId, char *buf, string *strInfo);
    void Cmd_UnregisterExt_Proc(int iClientId, char *buf, string *strInfo);
    void Cmd_SetTransData_Proc(int iClientId, char *buf, string *strInfo);
    void Cmd_GetTransData_Proc(int iClientId, char *buf, string *strInfo);
	void Cmd_HeartBeat_Proc(int iClientId, char *buf);
    void GetIPAndPort(int iClientId, string &strIPAddr, string &strPort);
	void DelFromListCtrl(int iClientId);
	void AddToListCtrl(char *szExt, int iClientId, list<PSOCKEXT> *pSockExtList);
	void AddToExtSock(char *szExt, int iClientId);
private:
    int QueryVersion(char *lpszFileName, PMSGINFO pMsgInfo, int iClientId);
    void GetFileCreateTime();
    REGEXTINFOLIST *FindRegExtInfoBySock(int iSock);
    bool IsRegisterContinue(int iClientId);
	void ReloadListCtrl(int iClientId);
	void RemoveExtFromVServer(char *ext, int iClientId);
	int WriteLimitDay(int iRunDayNum);
	int ReadLimitDay();
    void send_msg_to_agent_type(int iAgentType, string *strInfo);
	void RemoveFromAgentTypeList(int iClientId, int iAgentType);
	void CtiOwnerMsg(char *lpszSource, char *lpszDest, char *Param1, char *Param2, char *Param3);
	PAGENTTYPE FindFreeAgentType();
	void AddAgentType(int iClientId, int iAgentType, char *ext);
	void DelCrossIdInfo(PEXTINFO pEInfo);
	void AddCrossIdInfo(PEXTINFO pEInfo, char *szExtNo);
	void DelExtInfo(char *szExtNo);
	void AddExtInfo(char *szExtNo);
    void Cmd_DownloadStart(int iClientId, char *lpszBuffer, string *strInfo);
    void Cmd_QueryVersion(int iClientId, char *lpszBuffer, string *strInfo);
    AGENT_STATE *GetAgentStateByExt(char *lpszExt);
	
    list<REGEXTINFOLIST *>  m_RegExtInfoList;
	LockMap<string, char *> m_ExtAgentMap;
	list<string> m_RegExtList;
	LockMap<string,int> m_ExtFlagMap;
	LockMap<long,char *> m_CrossIdExtMap;
	LockMap<string,PEXTINFO> m_ExtInfoMap;
	LockMap<int, list<PSOCKEXT> *> m_SockExtListMap;
	LockMap<int, char *> m_ExtDataMap;
	LockMap<int, list<PAGENTTYPE> *> m_AgentTypeListMap;
	LockMap<string, list<PQUEUEINFO> *> m_QueueListMap;
	//LockMap<string, char *> m_GlbCallIdDataMap;
    list<AGENT_STATE *> m_AgentStateList;

#ifdef _VC6_
	CRITICAL_SECTION m_MoniCS;
#else
    QMutex          m_MoniCS;
#endif

	int		m_iCurrentClientNum;
	int		m_iMaxClientNum;
	int		m_nMoniNum;
	int		m_nIvrNum;
	char	m_szQueueExt[32];
    char    m_szLocalQueueExt[32];
	AGENTTYPE	m_AgentType[MAX_AGENTNUM];
    char	m_lpUpdateFile[512];

	char	m_lpszDefaultCard[32];
    bool	m_bCheckSer;
	int		m_iLimitDay;

	char	m_szTempDateTime[128];
	int		m_iRunDayNum;
	char	m_lpszSysFile[256];

	AGENT_EXT_INFO m_AgentExtInfo[MAX_AGENTNUM];
};

#endif
