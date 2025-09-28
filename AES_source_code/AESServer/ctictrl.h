#ifndef CTICTRL_H
#define CTICTRL_H
#include "globaleaesserver.h"

class CCtiCtrl
{
public:
    CCtiCtrl();
    virtual ~CCtiCtrl();
public:
    void cmd_talk_witch_proc(int iClientId, char *lpszBuffer, string *strInfo);
    void cmd_agent_not_ready_proc(int iClientId, char *lpszBuffer, string *strInfo);
    void cmd_agent_ready_proc(int iClientId, char *lpszBuffer, string *strInfo);
    void cmd_agent_logoff_proc(int iClientId, char *lpszBuffer, string *strInfo);
    void cmd_agent_logon_proc(int iClientId, char *lpszBuffer, string *strInfo);
    void cmd_forcech_proc(int iClientId, char *lpszBuffer, string *strInfo);
    void cmd_intrude_proc(int iClientId, char *lpszBuffer, string *strInfo);
    void cmd_listen_proc(int iClientId, char *lpszBuffer, string *strInfo);
    void cmd_stop_record_proc(int iClientId, char *lpszBuffer, string *strInfo);
    void cmd_start_record_proc(int iClientId, char *lpszBuffer, string *strInfo);
    void cmd_stop_play_proc(int iClientId, char *lpszBuffer, string *strInfo);
    void cmd_start_play_proc(int iClientId, char *lpszBuffer, string *strInfo);
    void cmd_monitor_proc(int iClientId,char *lpszBuffer, string *strInfo);
    void cmd_answer_proc(int iClientId,char *lpszBuffer, string *strInfo);
    void cmd_hangup_proc(int iClientId,char *lpszBuffer, string *strInfo);
    void cmd_makecall_proc(int iClientId,char *lpszBuffer, string *strInfo);
    void cmd_hold_proc(int iClientId,char *lpszBuffer, string *strInfo);
    void cmd_retrieve_proc(int iClientId,char *lpszBuffer, string *strInfo);
    void cmd_init_transfer_proc(int iClientId,char *lpszBuffer, string *strInfo);
    void cmd_complete_transfer_proc(int iClientId,char *lpszBuffer, string *strInfo);
    void cmd_complete_conference_proc(int iClientId,char *lpszBuffer, string *strInfo);
private:
    void AddToMultiMediaChannel(PACKET_HEAD *pHead, int iClientId, int iClientType, char *device_id);
    void Send_command(int iClientId,int cmd,int cmdtype,char *lpszSendBuffer, int data_len);
};

#endif // CTICTRL_H

