#include "ctictrl.h"
#include "dataproc.h"
#include "commonenginedll.h"
#include "clientsock.h"

extern CClientSock m_clientSock;
extern CDataProc   m_DataProc;
extern CCommonEngineDll m_CommonEngineDll;

extern void TraceEx( char *szMessage, ...);

CCtiCtrl::CCtiCtrl()
{

}

CCtiCtrl::~CCtiCtrl()
{

}

void CCtiCtrl::cmd_monitor_proc(int iClientId,char *lpszBuffer, string *strInfo)
{
    if( iClientId <= 0 || lpszBuffer == NULL || strInfo == NULL )
        return;

    char szLog[1024] = {0};

    PACKET_HEAD *pHead = (PACKET_HEAD *)lpszBuffer;

    int iClientType = atoi( (char *)strInfo[1].c_str() );
    char device_id[64] = {0};
    strcpy( device_id, (char *)strInfo[0].c_str() );
    //memcpy( device_id, lpszBuffer + sizeof(PACKET_HEAD), pHead->dwSize );

    sprintf( szLog, "cmd_monitor_proc, device_id = %s, client_type = %d", device_id, iClientType );
    TraceEx( szLog );

    if( iClientType == 1 )
    {
        //multi media type
        AddToMultiMediaChannel(pHead, iClientId, iClientType, device_id);
        return;
    }

    char szReasonCode[32] = {0};
    if( m_DataProc.IsBeyondLisence() )
    {
        pHead->cmd_type = SNT_RESPONSE_FAILED;
        strcpy( szReasonCode, "1" );
        TraceEx( "warnning, beyond license");
    }
    else
    {
        if( m_DataProc.IsDNExist( device_id ))
        {
            pHead->cmd_type = SNT_RESPONSE_OK;

            if( !m_DataProc.IsClientIdAlreadyExist(iClientId, iClientType, device_id ) )
            {
                DEVICE_SOCK_INFO *info = new DEVICE_SOCK_INFO;
                if( info != NULL )
                {
                    memset( info, 0, sizeof(DEVICE_SOCK_INFO));
                    info->iClientId = iClientId;
                    info->iClientType = iClientType;
                    strcpy( info->szDevice, device_id );
                    m_DataProc.m_pDevice_Sock_List->push_back( info );


                    memset( szLog, 0, 1024 );
                    sprintf( szLog, "notify, add to device list, client_id = %d, iClientType = %d, device = %s", info->iClientId, info->iClientType, info->szDevice );
                    TraceEx( szLog );
                }
                else
                {
                    pHead->cmd_type = SNT_RESPONSE_FAILED;
                    TraceEx( "error, cmd_monitor device_sock_info malloc memory failed");
                }
            }
        }
        else
        {
            memset( szLog, 0, 1024 );
            sprintf( szLog, "warnning, DN is not in ext_list, device_id = %s", device_id );
            TraceEx( szLog );
            pHead->cmd_type = SNT_RESPONSE_FAILED;
            strcpy( szReasonCode, "2" );
        }
    }

    Send_command( iClientId, pHead->cmd, pHead->cmd_type, szReasonCode, strlen( szReasonCode ) );
}

void CCtiCtrl::cmd_answer_proc(int iClientId,char *lpszBuffer, string *strInfo)
{
    if( iClientId <= 0 || lpszBuffer == NULL || strInfo == NULL )
        return;

    char szLog[512] = {0};
    CALL_INFO *pCallInfo = m_DataProc.FindCallInfoByDeviceId( (char *)strInfo[0].c_str() );
    if( pCallInfo != NULL )
    {
        char szCmd[128] = {0};
        sprintf( szCmd, "bgapi uuid_phone_event %s talk\n\n", pCallInfo->szActiveCallID );

        m_clientSock.SendToClient( szCmd, strlen(szCmd));

        sprintf( szLog, "answer_proc, device = %s, cmd = %s", (char *)strInfo[0].c_str(), szCmd );
        TraceEx( szLog );
    }
    else
    {
        sprintf( szLog, "error, answer_call_proc failed, can not find callinfo, device = %s", (char *)strInfo[0].c_str() );
        TraceEx( szLog );
    }
}

void CCtiCtrl::cmd_hangup_proc(int iClientId,char *lpszBuffer, string *strInfo)
{
    if( iClientId <= 0 || lpszBuffer == NULL || strInfo == NULL )
        return;

    char szLog[512] = {0};
    CALL_INFO *pCallInfo = m_DataProc.FindCallInfoByDeviceId( (char *)strInfo[0].c_str() );
    if( pCallInfo != NULL )
    {
        pCallInfo->iRetrieveFlag = 0;
        pCallInfo->iGradeFlag = atoi( (char *)strInfo[1].c_str() );

        //pCallInfo->szActiveCallID
        char szCmd[128] = {0};
        sprintf( szCmd, "bgapi uuid_kill %s\n\n", pCallInfo->szActiveCallID );

        m_clientSock.SendToClient( szCmd, strlen(szCmd));

        sprintf( szLog, "hangup_proc, iRetrieveFlag = %d, iGradeFlag = %d, device = %s, cmd = %s",
                 pCallInfo->iRetrieveFlag, pCallInfo->iGradeFlag, (char *)strInfo[0].c_str(), szCmd );
        TraceEx( szLog );
    }
    else
    {
        sprintf( szLog, "error, hangup_proc failed, device = %s", (char *)strInfo[0].c_str() );
        TraceEx( szLog );
    }
}

void CCtiCtrl::cmd_makecall_proc(int iClientId,char *lpszBuffer, string *strInfo)
{
    if( iClientId <= 0 || lpszBuffer == NULL || strInfo == NULL )
        return;

    string strMakeCallCmd = "";

    //ivr make call, need to add later
    //strInfo[0] - srcDN; strInfo[1] - destDN

    //ext make call
    if( strInfo[1].find( "*70") != string::npos || strInfo[1].find( "*73") != string::npos )
    {
        strMakeCallCmd = "bgapi originate {origination_caller_id_name=" + strInfo[0] + ",origination_caller_id_number=" + strInfo[1] + ",origination_callee_id_name=" + strInfo[1] + ",origination_callee_id_number=" + strInfo[0] + "}loopback/" + strInfo[0] + " " + strInfo[1] +"\n\n";
        //EnqueueData = "bgapi originate {origination_caller_id_name=" + tempcmd[1].Trim() + ",origination_caller_id_number=" + tempcmd[2].Trim() + ",origination_callee_id_name=" + tempcmd[2].Trim() + ",origination_callee_id_number=" + tempcmd[1].Trim() + "}loopback/" + tempcmd[1].Trim() + " " + tempcmd[2].Trim();
    }
    else
    {
        if( m_DataProc.IsDNInIVRList( (char *)strInfo[0].c_str() ) )
        {
            if( m_DataProc.IsDestDNInConfList( (char *)strInfo[1].c_str() ))
            {
                strMakeCallCmd = "bgapi originate {origination_caller_id_name=" + strInfo[0] + ",origination_caller_id_number=" + strInfo[0] + ",origination_callee_id_name=" + strInfo[1] + ",origination_callee_id_number=" + strInfo[1] + "}loopback/" + strInfo[0] + " " + strInfo[1] +"\n\n";
            }
            else
            {
                char szLog[1024] = {0};
                sprintf( szLog, "cmd_make_call, ivr make call, ivr_dn = %s, dest_dn = %s", (char *)strInfo[0].c_str(), (char *)strInfo[1].c_str() );
                TraceEx( szLog );
                //IVR分机外呼
                //OutCall|srcDN|destDN
                strMakeCallCmd = "bgapi originate {origination_caller_id_name=" + strInfo[0] + ",origination_caller_id_number=" + strInfo[0] + ",origination_callee_id_name=" + strInfo[1] + ",origination_callee_id_number=" + strInfo[1] + ",IVR_outbound=true,IVR_DN="+strInfo[0]+ ",IVR_OutboundNumber="+strInfo[1]+"}loopback/" + strInfo[1] + " IVR_outbound xml default" + "\n\n";
            }
        }
        else
        {
            strMakeCallCmd = "bgapi originate {origination_caller_id_name=" + strInfo[0] + ",origination_caller_id_number=" + strInfo[0] + ",origination_callee_id_name=" + strInfo[1] + ",origination_callee_id_number=" + strInfo[1] + "}loopback/" + strInfo[0] + " " + strInfo[1] +"\n\n";
            //EnqueueData = "bgapi originate {origination_caller_id_name=" + tempcmd[1].Trim() + ",origination_caller_id_number=" + tempcmd[1].Trim() + ",origination_callee_id_name=" + tempcmd[2].Trim() + ",origination_callee_id_number=" + tempcmd[2].Trim() + "}loopback/" + tempcmd[2].Trim() + " " + tempcmd[1].Trim();
        }
    }

    /*
    //cmd for haigang, marked by liu
    //EnqueueData = "bgapi originate {origination_caller_id_name=" + tempcmd[1].Trim() + ",origination_caller_id_number=" + tempcmd[2].Trim() + ",origination_callee_id_name=" + tempcmd[2].Trim() + ",origination_callee_id_number=" + tempcmd[1].Trim() + "}loopback/" + tempcmd[1].Trim() + " " + tempcmd[2].Trim();
    */

    m_clientSock.SendToClient( (char *)strMakeCallCmd.c_str(), strMakeCallCmd.length() );

    char szLog[1024*2] = {0};
    sprintf( szLog, "make_call, cmd = %s", (char *)strMakeCallCmd.c_str() );
    TraceEx( szLog );
}

void CCtiCtrl::cmd_hold_proc(int iClientId,char *lpszBuffer, string *strInfo)
{
    if( iClientId <= 0 || lpszBuffer == NULL || strInfo == NULL )
        return;

    CALL_INFO *pCallInfo = m_DataProc.FindCallInfoByDeviceId( (char *)strInfo[0].c_str() );
    if( pCallInfo != NULL )
    {
        pCallInfo->iStartTransferFlag = 0; //add by liu, 2017-5-24
        //EnqueueData = "bgapi uuid_hold " + tempcmd[1].Trim();
        char szHoldCmd[256] = {0};
        sprintf( szHoldCmd, "bgapi uuid_hold %s\n\n", pCallInfo->szActiveCallID );

        m_clientSock.SendToClient( szHoldCmd, strlen( szHoldCmd ) );

        TraceEx( szHoldCmd );
    }
    else
    {
        char szLog[512] = {0};
        sprintf( szLog, "error, hold_proc failed, device = %s", (char *)strInfo[0].c_str() );
        TraceEx( szLog );
    }

}

void CCtiCtrl::cmd_retrieve_proc(int iClientId,char *lpszBuffer, string *strInfo)
{
    if( iClientId <= 0 || lpszBuffer == NULL || strInfo == NULL )
        return;

    char szLog[1024] = {0};
    CALL_INFO *pCallInfo = m_DataProc.FindCallInfoByDeviceId( (char *)strInfo[0].c_str() );
    if( pCallInfo != NULL )
    {
        //EnqueueData = "bgapi uuid_hold off " + tempcmd[1].Trim();
        char szRetrieveCmd[512] = {0};

        if( pCallInfo->iStartTransferFlag == 1 )
        {
            //EnqueueData = "bgapi uuid_recv_dtmf " + tempcmd[2].Trim() + " #|AgentID-" + tempcmd[1].Trim();
            //转接或三方时取回
            sprintf( szRetrieveCmd, "bgapi uuid_recv_dtmf %s  #|AgentID-%s\n\n", pCallInfo->szHeldCallID, (char *)strInfo[0].c_str() );
            m_clientSock.SendToClient( szRetrieveCmd, strlen( szRetrieveCmd ) );

            pCallInfo->iStartTransferFlag = 0;
            pCallInfo->iRetrieveFlag = 1;
            memset( pCallInfo->szActiveCallID, 0, 64 );
            strcpy( pCallInfo->szActiveCallID, pCallInfo->szHeldCallID );
            memset( pCallInfo->szHeldCallID, 0, 64 );

            //产生一个unhold Event事件
            //activeDevice|lCallID
            char lpszSendBuffer[1024*2] = {0};
            sprintf( lpszSendBuffer, "%s|%s", (char *)strInfo[0].c_str(), pCallInfo->szActiveCallID );

            m_DataProc.SendCallEventToClient( (char *)strInfo[0].c_str(), device_type, CMD_EXTONRETRIEVE_EVENT, SNT_EVENT, lpszSendBuffer, strlen(lpszSendBuffer) );
        }
        else
        {
            pCallInfo->iRetrieveFlag = 0; //add by liu, 2017-5-24
            //保持时取回hold-->unhold
            sprintf( szRetrieveCmd, "bgapi uuid_hold off %s\n\n", pCallInfo->szActiveCallID );
            m_clientSock.SendToClient( szRetrieveCmd, strlen( szRetrieveCmd ) );
        }

        sprintf( szLog, "cmd_retrieve_proc, cmd = %s", szRetrieveCmd );
        TraceEx( szLog );
    }
    else
    {
        sprintf( szLog, "error, retrieve_proc failed, device = %s", (char *)strInfo[0].c_str() );
        TraceEx( szLog );
    }
}

void CCtiCtrl::cmd_init_transfer_proc(int iClientId,char *lpszBuffer, string *strInfo)
{
    if( iClientId <= 0 || lpszBuffer == NULL || strInfo == NULL )
        return;

    //BlindTrasfer|srcExt|destExt -------IVR
    //ConsultationCall|srcExt|destExt ------ agent ext
    CALL_INFO *pCallInfo = m_DataProc.FindCallInfoByDeviceId( (char *)strInfo[1].c_str() );
    if( pCallInfo != NULL )
    {
        if( strInfo[0] == "BlindTransfer" )
        {
            //IVR
            string strCmd = "";
            string strActiveCallID = pCallInfo->szActiveCallID;

            strCmd = "bgapi uuid_setvar " + strActiveCallID + " destination_number " + strInfo[2] + "\n\n";
            m_clientSock.SendToClient( (char *)strCmd.c_str(), strCmd.length());

            TraceEx( (char *)strCmd.c_str() );

            strCmd = "";
            strCmd = "bgapi uuid_broadcast " + strActiveCallID + " sleep::500 " + "\n\n";
            m_clientSock.SendToClient( (char *)strCmd.c_str(), strCmd.length());

            TraceEx( (char *)strCmd.c_str() );

            strCmd = "";
            strCmd = "bgapi uuid_transfer " + strActiveCallID + " " + strInfo[2] + "\n\n";
            m_clientSock.SendToClient( (char *)strCmd.c_str(), strCmd.length());

            TraceEx( (char *)strCmd.c_str() );
        }
        else
        {
            pCallInfo->iRingbackFlag = 0;

            //agent ext
            string strCmd = "";
            string strActiveCallID = pCallInfo->szActiveCallID;
            strCmd = "bgapi uuid_broadcast " + strActiveCallID + " export::type=extcall both" + "\n\n";
            m_clientSock.SendToClient( (char *)strCmd.c_str(), strCmd.length());

            TraceEx( (char *)strCmd.c_str() );

            strCmd = "";
            strCmd = "bgapi uuid_broadcast " + strActiveCallID + " export::att_xfer_digits=" + strInfo[2] + "\n\n";
            m_clientSock.SendToClient( (char *)strCmd.c_str(), strCmd.length());

            TraceEx( (char *)strCmd.c_str() );

            if( m_DataProc.IsDestDNInConfList( (char *)strInfo[2].c_str() ) )
            {
                memset( pCallInfo->szHeldCallID, 0, 64 );
                strcpy( pCallInfo->szHeldCallID, pCallInfo->szActiveCallID );
                char szTempLog[1024] = {0};
                sprintf( szTempLog, "---------notify, dest_ext is conference grp no, dest_dn = %s, held_callId = %s,----------",
                         (char *)strInfo[2].c_str(), pCallInfo->szHeldCallID );
                TraceEx( szTempLog );
            }

            //如果为CSTA模式，则将转接方的UUID写入channel变量
            strCmd = "";
            strCmd = "bgapi uuid_broadcast " + strActiveCallID + " export::xfer_caller_uuid=" + pCallInfo->szActiveCallID + "\n\n";
            m_clientSock.SendToClient( (char *)strCmd.c_str(), strCmd.length());

            TraceEx( (char *)strCmd.c_str() );

            strCmd = "";
            strCmd = "bgapi uuid_broadcast " + strActiveCallID + " javascript::covert_att_xfer_digits_by_agent.js" + "\n\n";
            m_clientSock.SendToClient( (char *)strCmd.c_str(), strCmd.length());

            pCallInfo->iStartTransferFlag = 1;

            TraceEx( (char *)strCmd.c_str() );
        }
    }
    else
    {
        char szLog[512] = {0};
        sprintf( szLog, "error, init_transfer_proc failed, device = %s", (char *)strInfo[1].c_str() );
        TraceEx( szLog );
    }
}

void CCtiCtrl::cmd_complete_transfer_proc(int iClientId,char *lpszBuffer, string *strInfo)
{
    if( iClientId <= 0 || lpszBuffer == NULL || strInfo == NULL )
        return;

    char szLog[512] = {0};
    //buf = heldCall|activeCall

    CALL_INFO *pCallInfo = m_DataProc.FindCallInfoByDeviceId( (char *)strInfo[1].c_str() );
    if( pCallInfo != NULL )
    {

        string strHeldCallID = pCallInfo->szHeldCallID;

        string strCmd = "bgapi uuid_recv_dtmf " + strHeldCallID + " *" + "\n\n";

        m_clientSock.SendToClient( (char *)strCmd.c_str(), strCmd.length());

        memset( pCallInfo->szActiveCallID, 0, 64 );
        strcpy( pCallInfo->szActiveCallID, pCallInfo->szHeldCallID );
        memset( pCallInfo->szHeldCallID, 0, 64 );

        sprintf( szLog, "notify, complete_transfer, device = %s, active_call_id = %s, held_call_id = %s, cmd = %s",
                 (char *)strInfo[1].c_str(), pCallInfo->szActiveCallID, pCallInfo->szHeldCallID, (char *)strCmd.c_str() );

        TraceEx( szLog );
    }
    else
    {
        sprintf( szLog, "error, complete_transfer_proc failed, device = %s", (char *)strInfo[1].c_str() );
        TraceEx( szLog );
    }
}
void CCtiCtrl::cmd_complete_conference_proc(int iClientId,char *lpszBuffer, string *strInfo)
{
    if( iClientId <= 0 || lpszBuffer == NULL || strInfo == NULL )
        return;

    char szLog[512] = {0};
    //buf = heldCall|activeCall

    CALL_INFO *pCallInfo = m_DataProc.FindCallInfoByDeviceId( (char *)strInfo[1].c_str() );
    if( pCallInfo != NULL )
    {
        string strHeldCallID = pCallInfo->szHeldCallID;

        string strCmd = "bgapi uuid_recv_dtmf " + strHeldCallID + " 0" + "\n\n";

        m_clientSock.SendToClient( (char *)strCmd.c_str(), strCmd.length());

        TraceEx( (char *)strCmd.c_str() );

        memset( pCallInfo->szActiveCallID, 0, 64 );
        strcpy( pCallInfo->szActiveCallID, pCallInfo->szHeldCallID );
        memset( pCallInfo->szHeldCallID, 0, 64 );

        pCallInfo->iConferenceFlag = 1;
        pCallInfo->iConferenceCount = 3;

        sprintf( szLog, "complete_conference, device = %s, iTransferFlag = %d, iConferenceFlag = %d, iConferenceCount = %d, active_call_id = %s, held_call_id = %s",
                 (char *)strInfo[1].c_str(), pCallInfo->iStartTransferFlag, pCallInfo->iConferenceFlag, pCallInfo->iConferenceCount, pCallInfo->szActiveCallID, pCallInfo->szHeldCallID );
        TraceEx( szLog );
    }
    else
    {
        sprintf( szLog, "error, complete_conference_proc failed, device = %s", (char *)strInfo[1].c_str() );
        TraceEx( szLog );
    }
}

void CCtiCtrl::Send_command(int iClientId,int cmd,int cmdtype,char *lpszSendBuffer, int data_len)
{
    try
    {
    char buffer[1024*16];
    memset( buffer, 0, 1024*16 );

    PACKET_HEAD Packet_Head;
    memset( &Packet_Head, 0, sizeof( PACKET_HEAD ) );
    Packet_Head.cmd = cmd;
    Packet_Head.cmd_type = cmdtype;
    Packet_Head.dwFlag = PACKET_FLAG;
    Packet_Head.dwSize = data_len;

    memcpy( buffer, &Packet_Head, sizeof( PACKET_HEAD ));

    if( data_len != 0 )
    {
        memcpy( buffer + sizeof( PACKET_HEAD ), lpszSendBuffer, data_len );
    }

    m_CommonEngineDll.SendToClientFunc( iClientId, buffer, sizeof(PACKET_HEAD) + data_len );
    }
    catch(...)
    {
        TraceEx( "error, exception, CData::send_command() " );
    }
}

void CCtiCtrl::cmd_start_play_proc(int iClientId, char *lpszBuffer, string *strInfo)
{
    if( iClientId <= 0 || lpszBuffer == NULL || strInfo == NULL )
        return;

    //Ch|FileNum|FileName|FileName2…|FileNameN

    int iCh = atoi( (char *)strInfo[0].c_str() );
    IVR_INFO *pIVRInfo = m_DataProc.get_ivr_info_by_ch( iCh );
    if( pIVRInfo == NULL )
    {
        char szLog[512] = {0};
        sprintf( szLog, "error, start_play_proc failed, pIVRIno = NULL, iCh = %d", iCh );
        TraceEx( szLog );
        return;
    }

    int iFileNum = atoi( (char *)strInfo[1].c_str() );
    if( iFileNum < 0 )
    {
        char szLog[512] = {0};
        sprintf( szLog, "error, start_play_proc failed, iFileNum < 0, iCh = %d", iCh );
        TraceEx( szLog );
        return;
    }

    string strFileList = "";

    if( iFileNum > 1 )
        strFileList = "file_string://";

    for(int i = 0; i < iFileNum; i++)
    {
        if( strstr( (char *)strInfo[i+2].c_str(), "http://" ) == NULL )
        {
            strInfo[i+2] = m_DataProc.m_strIVRVoiceVirtualPath + strInfo[i+2];
        }

        if( i+1 >= iFileNum )
        {
            strFileList += strInfo[i+2];
        }
        else
        {
            strFileList += strInfo[i+2] + "!";
        }
    }

    //multi_file( iFileNum > 1 )
    //bgapi uuid_broadcast f69d3826-8149-11e6-971c-b7bc2a9242dc playback::file_string://http://121.40.128.18/ivr/later.wav!http://121.40.128.18/ivr/holdmusic.wav aleg

    char szCmd[1024*10] = {0};
    sprintf( szCmd, "bgapi uuid_setvar %s playback_terminators #\n\n", pIVRInfo->szUUID );
    m_clientSock.SendToClient( szCmd, strlen( szCmd ));

    char szLog[1024*10] = {0};
    sprintf( szLog, "start_play, ch = %d, cmd = %s", iCh, szCmd );
    TraceEx( szLog );

    memset( szCmd, 0, 1024*10 );
    sprintf( szCmd, "bgapi uuid_broadcast %s playback::%s aleg\n\n", pIVRInfo->szUUID, (char *)strFileList.c_str() );
    m_clientSock.SendToClient( szCmd, strlen( szCmd ));

    memset( szLog, 0, 1024*10 );
    sprintf( szLog, "start_play, ch = %d, cmd = %s", iCh, szCmd );
    TraceEx( szLog );

}

void CCtiCtrl::cmd_stop_play_proc(int iClientId, char *lpszBuffer, string *strInfo)
{
    if( iClientId <= 0 || lpszBuffer == NULL || strInfo == NULL )
        return;

    int iCh = atoi( (char *)strInfo[0].c_str() );
    IVR_INFO *pIVRInfo = m_DataProc.get_ivr_info_by_ch( iCh );
    if( pIVRInfo == NULL )
    {
        char szLog[512] = {0};
        sprintf( szLog, "error, stop_play_proc failed, pIVRIno = NULL, iCh = %d", iCh );
        TraceEx( szLog );
        return;
    }

    char szCmd[1024] = {0};
    sprintf( szCmd, "bgapi uuid_break %s all\n\n", pIVRInfo->szUUID );
    m_clientSock.SendToClient( szCmd, strlen( szCmd ));

    char szLog[1024] = {0};
    sprintf( szLog, "cmd_stop_play, ch = %d, cmd = %s", iCh, szCmd );
    TraceEx( szLog );
}

void CCtiCtrl::cmd_start_record_proc(int iClientId, char *lpszBuffer, string *strInfo)
{
    if( iClientId <= 0 || lpszBuffer == NULL || strInfo == NULL )
        return;

    char szUUID[256] = {0};
    string recFilePathName = strInfo[1]; //"/mnt/voicemail/" + strInfo[1];

    char szCmd[1024*2] = {0};

    if( strInfo[2] == "CTICtrl")
    {
        CALL_INFO *pCallInfo = m_DataProc.FindCallInfoByDeviceId( (char *)strInfo[0].c_str() );
        if( pCallInfo == NULL )
        {
            char szLog[512] = {0};
            sprintf( szLog, "error, start_record_proc failed, pCallInfo = NULL, device = %d", (char *)strInfo[0].c_str() );
            TraceEx( szLog );
            return;
        }

        strcpy( szUUID, pCallInfo->szActiveCallID );

        sprintf( szCmd, "bgapi uuid_setvar %s playback_terminators %s#\n\n", szUUID, (char *)recFilePathName.c_str() );
        m_clientSock.SendToClient( szCmd, strlen( szCmd ));

        TraceEx( szCmd );

        //uuid_record,<uuid>[start|stop|mask|unmask] <path> [<limit>],Record sessionaudio,mod_commands
        memset( szCmd, 0, 1024*2 );
        //sprintf( szCmd, "bgapi uuid_broadcast %s record::%s\n\n", szUUID, (char *)recFilePathName.c_str() );
        sprintf( szCmd, "bgapi uuid_record %s start %s\n\n", szUUID, (char *)recFilePathName.c_str() );
        m_clientSock.SendToClient( szCmd, strlen( szCmd ));
    }
    else
    {
        //Ch|recFileName
        int iCh = atoi( (char *)strInfo[0].c_str() );
        IVR_INFO *pIVRInfo = m_DataProc.get_ivr_info_by_ch( iCh );
        if( pIVRInfo == NULL )
        {
            char szLog[512] = {0};
            sprintf( szLog, "error, start_record_proc failed, pIVRIno = NULL, iCh = %d", iCh );
            TraceEx( szLog );
            return;
        }

        strcpy( szUUID, pIVRInfo->szUUID );

        sprintf( szCmd, "bgapi uuid_setvar %s playback_terminators %s#\n\n", szUUID, (char *)recFilePathName.c_str() );
        m_clientSock.SendToClient( szCmd, strlen( szCmd ));

        TraceEx( szCmd );

        memset( szCmd, 0, 1024*2 );
        sprintf( szCmd, "bgapi uuid_broadcast %s record::%s  aleg\n\n", szUUID, (char *)recFilePathName.c_str() );
        m_clientSock.SendToClient( szCmd, strlen( szCmd ));
    }

    char szLog[1024*2] = {0};
    sprintf( szLog, "cmd_start_record, param = %s, cmd = %s", (char *)strInfo[0].c_str(), szCmd );
    TraceEx( szLog );
}

void CCtiCtrl::cmd_stop_record_proc(int iClientId, char *lpszBuffer, string *strInfo)
{
    if( iClientId <= 0 || lpszBuffer == NULL || strInfo == NULL )
        return;

    char szUUID[256] = {0};
    if( strInfo[1] == "CTICtrl")
    {
        CALL_INFO *pCallInfo = m_DataProc.FindCallInfoByDeviceId( (char *)strInfo[0].c_str() );
        if( pCallInfo == NULL )
        {
            char szLog[512] = {0};
            sprintf( szLog, "error, start_record_proc failed, pCallInfo = NULL, device = %d", (char *)strInfo[0].c_str() );
            TraceEx( szLog );
            return;
        }

        strcpy( szUUID, pCallInfo->szActiveCallID );
    }
    else
    {
        int iCh = atoi( (char *)strInfo[0].c_str() );
        IVR_INFO *pIVRInfo = m_DataProc.get_ivr_info_by_ch( iCh );
        if( pIVRInfo == NULL )
        {
            char szLog[512] = {0};
            sprintf( szLog, "error, stop_record_proc failed, pIVRIno = NULL, iCh = %d", iCh );
            TraceEx( szLog );
            return;
        }

        strcpy( szUUID, pIVRInfo->szUUID );
    }

    char szCmd[1024] = {0};
    sprintf( szCmd, "bgapi uuid_recv_dtmf %s #\n\n", szUUID );

    m_clientSock.SendToClient( szCmd, strlen( szCmd ));

    char szLog[1024] = {0};
    sprintf( szLog, "cmd_stop_record, param = %s, cmd = %s", (char *)strInfo[0].c_str(), szCmd );
    TraceEx( szLog );
}


void CCtiCtrl::AddToMultiMediaChannel(PACKET_HEAD *pHead, int iClientId, int iClientType, char *device_id)
{
    if( pHead == NULL ||iClientId <= 0 || device_id == NULL )
        return;

    char szReasonCode[32] = {0};
    pHead->cmd_type = SNT_RESPONSE_OK;
    if( !m_DataProc.IsClientIdAlreadyExist(iClientId, iClientType, device_id ) )
    {
        DEVICE_SOCK_INFO *info = new DEVICE_SOCK_INFO;
        if( info != NULL )
        {
            pHead->cmd_type = SNT_RESPONSE_OK;
            memset( info, 0, sizeof(DEVICE_SOCK_INFO));
            info->iClientId = iClientId;
            info->iClientType = iClientType;
            strcpy( info->szDevice, device_id );
            m_DataProc.m_pDevice_Sock_List->push_back( info );

            char szLog[1024] = {0};
            sprintf( szLog, "notify, add to multi_medial device list, client_id = %d, iClientType = %d, device = %s", info->iClientId, info->iClientType, info->szDevice );
            TraceEx( szLog );
        }
        else
        {
            strcpy( szReasonCode, "2" );
            pHead->cmd_type = SNT_RESPONSE_FAILED;
            TraceEx( "error, add muliti media device_sock_info malloc memory failed");
        }
    }

    Send_command( iClientId, pHead->cmd, pHead->cmd_type, szReasonCode, strlen( szReasonCode ) );
}

void CCtiCtrl::cmd_listen_proc(int iClientId, char *lpszBuffer, string *strInfo)
{
    if( iClientId <= 0 || lpszBuffer == NULL || strInfo == NULL )
        return;

    //activeCallId|activeDN|joinedDN
    string activeDN = strInfo[1];
    string joinedDN = strInfo[2];

    string cmd = "bgapi originate {origination_callee_id_number=" + activeDN + ",listen=true}user/" + joinedDN + " *70" + activeDN + "\n\n";
    m_clientSock.SendToClient( (char *)cmd.c_str(), cmd.length() );

    char szLog[1024] = {0};
    sprintf( szLog, "cmd_listen, cmd = %s", (char *)cmd.c_str() );
    TraceEx( szLog );
}

void CCtiCtrl::cmd_intrude_proc(int iClientId, char *lpszBuffer, string *strInfo)
{
    if( iClientId <= 0 || lpszBuffer == NULL || strInfo == NULL )
        return;

    //activeCallId|activeDN|joinedDN
    string activeDN = strInfo[1];
    string joinedDN = strInfo[2];
    string cmd = "bgapi originate {origination_callee_id_number=" + activeDN + ",Intrude=true}user/" + joinedDN + " *70" + activeDN + "\n\n";
    m_clientSock.SendToClient( (char *)cmd.c_str(), cmd.length() );

    char szLog[1024] = {0};
    sprintf( szLog, "cmd_intrude, cmd = %s", (char *)cmd.c_str() );
    TraceEx( szLog );
}

void CCtiCtrl::cmd_forcech_proc(int iClientId, char *lpszBuffer, string *strInfo)
{

}

void CCtiCtrl::cmd_agent_not_ready_proc(int iClientId, char *lpszBuffer, string *strInfo)
{
    //SetAgentStatus |AgentID|DN|agentGroup|Status

    string cmd = "bgapi callcenter_config agent set status " + strInfo[1] + " 'On Break'" +  "\n\n";
    m_clientSock.SendToClient( (char *)cmd.c_str(), cmd.length() );

    char szLog[1024] = {0};
    sprintf( szLog, "cmd_agent_not_ready, cmd = %s", (char *)cmd.c_str() );
    TraceEx( szLog );

    cmd = "";
    cmd = "bgapi callcenter_config agent set reason_code " + strInfo[1] + " 1" + "\n\n";
    m_clientSock.SendToClient( (char *)cmd.c_str(), cmd.length() );

    memset( szLog, 0, 1024 );
    sprintf( szLog, "cmd_agent_not_ready, cmd = %s", (char *)cmd.c_str() );
    TraceEx( szLog );}

void CCtiCtrl::cmd_agent_ready_proc(int iClientId, char *lpszBuffer, string *strInfo)
{
    //SetAgentStatus |AgentID|DN|agentGroup|Status

    string cmd = "bgapi callcenter_config agent set status " + strInfo[1] + " 'Available'" + "\n\n";
    m_clientSock.SendToClient( (char *)cmd.c_str(), cmd.length() );

    char szLog[1024] = {0};
    sprintf( szLog, "cmd_agent_ready, cmd = %s", (char *)cmd.c_str() );
    TraceEx( szLog );

    cmd = "";
    cmd = "bgapi callcenter_config agent set reason_code " + strInfo[1] + " 0" + "\n\n";
    m_clientSock.SendToClient( (char *)cmd.c_str(), cmd.length() );

    memset( szLog, 0, 1024 );
    sprintf( szLog, "cmd_agent_ready, cmd = %s", (char *)cmd.c_str() );
    TraceEx( szLog );
}

void CCtiCtrl::cmd_agent_logoff_proc(int iClientId, char *lpszBuffer, string *strInfo)
{
    //Logoff|AgentID|DN|Queue|Position
    string cmd = "api callcenter_config agent set reason_code " + strInfo[1] + " -2\n\n";
    m_clientSock.SendToClient( (char *)cmd.c_str(), cmd.length() );

    char szLog[1024] = {0};
    sprintf( szLog, "cmd_agent_logoff, cmd = %s", (char *)cmd.c_str() );
    TraceEx( szLog );

    cmd = "";
    cmd = "api callcenter_config agent del " + strInfo[1] + " |" + strInfo[1] + "\n\n";
    m_clientSock.SendToClient( (char *)cmd.c_str(), cmd.length() );

    memset( szLog, 0, 1024 );
    sprintf( szLog, "cmd_agent_logoff, cmd = %s", (char *)cmd.c_str() );
    TraceEx( szLog );
}

void CCtiCtrl::cmd_agent_logon_proc(int iClientId, char *lpszBuffer, string *strInfo)
{
    //Login|AgentID|DN|Queue|Position
    string cmd = "bgapi sofia status profile internal reg " + strInfo[2] + "@ AgentId-" + strInfo[1] + "\n\n";
    m_clientSock.SendToClient( (char *)cmd.c_str(), cmd.length() );

    char szLog[1024] = {0};
    sprintf( szLog, "cmd_agent_logon, cmd = %s", (char *)cmd.c_str() );
    TraceEx( szLog );

    cmd = "bgapi callcenter_config agent add " + strInfo[1] + " callback" + "\n\n";
    m_clientSock.SendToClient( (char *)cmd.c_str(), cmd.length() );


    cmd = "bgapi callcenter_config agent set contact " + strInfo[1] + " [origination_caller_id_name=" + strInfo[2] + ",call_timeout=0]user/" + strInfo[2] + "\n\n";
    m_clientSock.SendToClient( (char *)cmd.c_str(), cmd.length() );


    cmd = "bgapi callcenter_config agent set max_no_answer " + strInfo[1] + " 0\n\n";
    m_clientSock.SendToClient( (char *)cmd.c_str(), cmd.length() );

    cmd = "bgapi callcenter_config agent set status " + strInfo[1] + " 'Logged Out'" + "\n\n";
    m_clientSock.SendToClient( (char *)cmd.c_str(), cmd.length() );


    cmd = "bgapi callcenter_config agent set reject_delay_time " + strInfo[1] + " 10\n\n";
    m_clientSock.SendToClient( (char *)cmd.c_str(), cmd.length() );

    cmd = "bgapi callcenter_config agent set busy_delay_time " + strInfo[1] + " 60\n\n";
    m_clientSock.SendToClient( (char *)cmd.c_str(), cmd.length() );


    cmd = "bgapi callcenter_config agent set wrap_up_time " + strInfo[1] + " 5\n\n";
    m_clientSock.SendToClient( (char *)cmd.c_str(), cmd.length() );

    cmd = "bgapi callcenter_config agent set reason_code " + strInfo[1] + " -1\n\n";
    m_clientSock.SendToClient( (char *)cmd.c_str(), cmd.length() );

    //Login|AgentID|DN|Queue|Position
    if( strInfo->length() > 4 && strInfo[4].length() > 0 )
    {
        cmd = "bgapi callcenter_config tier add " + strInfo[3] + " " + strInfo[1] + " 1 " + strInfo[4] + "\n\n";
        m_clientSock.SendToClient( (char *)cmd.c_str(), cmd.length() );

    }
    else
    {
        cmd = "bgapi callcenter_config tier add " + strInfo[3] + " " + strInfo[1] + " 1 1\n\n";
        m_clientSock.SendToClient( (char *)cmd.c_str(), cmd.length() );
    }
}

void CCtiCtrl::cmd_talk_witch_proc(int iClientId, char *lpszBuffer, string *strInfo)
{
    //src_ext|dest_ext

    int iSrcCh = atoi( (char *)strInfo[0].c_str() );
    int iDestCh = atoi ( (char *)strInfo[1].c_str() );

    char szLog[512] = {0};
    sprintf( szLog, "cmd_talk_with_proc, src_ch = %d, dest_ch = %d", iSrcCh, iDestCh );
    TraceEx( szLog );

    IVR_INFO *pSrcIVRInfo = m_DataProc.get_ivr_info_by_ch( iSrcCh );
    if( pSrcIVRInfo == NULL )
    {
        memset( szLog, 0, 512 );
        sprintf( szLog, "error, cmd_talk_with_proc, can not find pSrcIVRInfo, src_ivr_ch = %d", iSrcCh );
        TraceEx( szLog );
        return;
    }

    IVR_INFO *pDestIVRInfo = m_DataProc.get_ivr_info_by_ch( iDestCh );
    if( pDestIVRInfo == NULL )
    {
        memset( szLog, 0, 512 );
        sprintf( szLog, "error, cmd_talk_with_proc, can not find pDestIVRInfo, dest_ivr_ch = %d", iDestCh );
        TraceEx( szLog );
        return;
    }

    char szCmd[512] = {0};
    sprintf( szCmd, "bgapi uuid_bridge %s %s\n\n", pSrcIVRInfo->szUUID, pDestIVRInfo->szUUID );

    m_clientSock.SendToClient( szCmd, strlen( szCmd ) );

    memset( szLog, 0, 512 );
    sprintf( szLog, "cmd_talk_with_proc, cmd = %s", szCmd );
    TraceEx( szLog );
}
