#ifndef DATAPROC_H
#define DATAPROC_H
#include "globaleaesserver.h"
#include "Lock.h"

class CDataProc
{
public:
    CDataProc();
    virtual ~CDataProc();
public:
    void utf8_to_gbk(char *lpszInputData, char *lpszOutputData);
    bool IsDestDNInConfList(char *szDest);
    bool IsDNInIVRList(char *szExt);
    bool IsClientIdAlreadyExist(int iClientId, int iClientType, char *device);
    IVR_INFO *get_ivr_info_by_ch(int iCh);
    int SendCallEventToClient(char *ext, int iClientType, int cmd,int cmdtype, char *lpszSendBuffer, int iDataLen);
    CALL_INFO *FindCallInfoByUUID(char *uuid);
    CALL_INFO *FindCallInfoByDeviceId(char *szDeviceId);
    int GetClientIdByExt(int iClientType, char *ext, int *iClientArray);
    bool IsDNExist(char *szExt);
    bool IsBeyondLisence();
    void InitParam();
    void DecodeData(char *buf, string *strInfo);
    void OnClientClose(int iClientId);
    void data_proc_from_cti(int iClientId, char *buf);
    void data_proc_event(char *data);
    string GetDestNumber(int iCount, string *strInfo, string strDestParam, int iReplaceCount, string *strReplaceParam);
    int  DecodeFrame(string *strInfo, string &strData, char *lpszSpliteValue, int iSpliteLen);
    string GetChannelName(int iCount, string *strInfo, string strChannelName);
    string GetHeader(int iCount, string *strInfo, string strValue);
public:
    list<DEVICE_SOCK_INFO *> *m_pDevice_Sock_List;
    string m_strIVRVoiceVirtualPath;
private:
    void get_asr_result_proc(char *data, string &strRequestId, int &iSentenceId, int &iBeginTime, int &iCurrentTime, string &strText);
    void remove_device_sock_info_by_sockid(int sock_id);
    bool IsDNInExtList(char *szExt);
    bool is_ivr_running(char *lpszUUID, char *ivr_device);
    IVR_INFO *get_ivr_info_by_caller(char *caller);
    IVR_INFO *get_ivr_info_by_device(char *device, bool bNeedJudgeRunningFlag);
    IVR_INFO *get_ivr_info_by_uuid(char *lpszUUID);
    IVR_INFO *get_idle_ivr_channel();
    void member_queue_end_event_proc(int iCount, string *strInfo);
    void member_queue_start_event_proc(int iCount, string *strInfo);
    void channel_asr_event_proc(int iCount, string *strInfo, char *data );
    void channel_ext_ringback_event_proc(int iCount, string *strInfo);
    void channel_playback_stop_event_proc(int iCount, string *strInfo);
    void channel_record_stop_event_proc(int iCount, string *strInfo);
    void channel_dtmf_event_proc(int iCount, string *strInfo);
    void channel_ivr_hangup_event_proc(int iCount, string *strInfo);
    void channel_ivr_ringback_event_proc(int iCount, string *strInfo);
    void channel_ivr_ringing_event_proc(int iCount, string *strInfo);
    void channel_unhold_event_proc(int iCount, string *strInfo);
    void channel_hold_event_proc(int iCount, string *strInfo);
    void channel_answer_event_proc(int iCount, string *strInfo);
    void channel_ext_ringing_event_proc(int iCount, string *strInfo);
    void channel_park_event_proc(int iCount, string *strInfo);
    void channel_hangup_event_proc(int iCount, string *strInfo);
    void RecordCallInfoByCallID(char *szDeviceId, char *lpszUUID );
private:
    int     m_iCurrentNum;

    LockMap<string, CALL_INFO *>    m_DNCallInfoMap;
    LockMap<string, CALLEEID_INFO *> m_CalleeIdInfoMap;

    int     m_nConfGrpNum;
    string  *m_lpstrConfGrpList;
    int     m_nMonitorNum;
    string  *m_lpstrExtDN;
    int     m_nIvrNum;
    string  *m_lpstrIVRDN;
    int		m_iMoniIVRDNNum;
    string  *m_lpstrMoniIVRDN;
    IVR_INFO *m_pIVRInfo;
};

#endif // DATAPROC_H

