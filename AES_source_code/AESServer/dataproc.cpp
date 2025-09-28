#include "globaleaesserver.h"
#include "dataproc.h"
#include "ctictrl.h"
#include "commonenginedll.h"
#include "clientsock.h"
#include "urlconvert.h"

extern void TraceEx( char *szMessage, ...);

extern CClientSock m_clientSock;
extern CCommonEngineDll m_CommonEngineDll;
extern CCtiCtrl    m_CtiCtrl;
extern bool    m_bSeriCorrect;
extern int     m_iLisenceNumber;
extern int m_record_all_log;

CDataProc::CDataProc()
{

}

CDataProc::~CDataProc()
{

}

void CDataProc::data_proc_event(char *data)
{
    if( data == NULL )
        return;

    if(m_record_all_log == 1)
        TraceEx( data );

    if( !m_bSeriCorrect )
        return;

    char szLog[1024*20] = {0};

    string strData = "";
    strData = data;
    string strInfo[1024*2] = {""};
    int iCount = DecodeFrame( strInfo, strData, "\n", 1 );

    /*
    for(int i = 0; i < iCount; i++)
    {
        sprintf( szLog, "i = %d, iCount = %d, info = %s", i, iCount, (char *)strInfo[i].c_str());
        TraceEx( szLog );
    }
    */

    string strEventName = GetHeader( iCount, strInfo, "Event-Name: " );
    memset( szLog, 0, 1024*20 );
    sprintf( szLog, "recved, event_name = %s", (char *)strEventName.c_str() );
    TraceEx( szLog );

    if( GetHeader( iCount, strInfo, "Event-Name: " ) == "CHANNEL_APPLICATION")
    {
        string strEventSubclass = GetHeader(iCount, strInfo, "Event-Subclass: " );
        memset( szLog, 0, 1024*20 );
        sprintf( szLog, "recved, event_subclass = %s", (char *)strEventSubclass.c_str() );
        TraceEx( szLog );

        if( GetHeader( iCount, strInfo, "Event-Subclass: ") == "IVR_Ringing_Event" )
        {
            channel_ivr_ringing_event_proc( iCount, strInfo );
            return;
        }

        if( GetHeader( iCount, strInfo, "Event-Subclass: ") == "IVR_RingBack_Event" )
        {
            channel_ivr_ringback_event_proc( iCount, strInfo );
            return;
        }

        if( GetHeader( iCount, strInfo, "Event-Subclass: ") == "Hangup_Event" )
        {
            channel_ivr_hangup_event_proc( iCount, strInfo );
            return;
        }

        if( GetHeader( iCount, strInfo, "Event-Subclass: ") == "Ringing_Event" )
        {
            channel_ext_ringing_event_proc( iCount, strInfo );
            return;
        }

        if( GetHeader( iCount, strInfo, "Event-Subclass: ") == "RingBack_Event" )
        {
            //TraceEx( data );
            channel_ext_ringback_event_proc( iCount, strInfo );
            return;
        }

        if( GetHeader( iCount, strInfo, "Event-Subclass: ") == "Answer_Event" )
        {
//            channel_answer_event_proc( iCount, strInfo );
            return;
        }

        return;
    }

    if( GetHeader( iCount, strInfo, "Event-Name: " ) == "CUSTOM")
    {
        if( GetHeader(iCount, strInfo, "Event-Subclass: " ) == "asr" )
        {
            channel_asr_event_proc( iCount, strInfo, data );
            return;
        }

        return;
    }
    if( GetHeader( iCount, strInfo, "Event-Name: " ) == "PLAYBACK_STOP")
    {
        channel_playback_stop_event_proc(iCount, strInfo);
        return;
    }

    if( GetHeader( iCount, strInfo, "Event-Name: " ) == "DTMF")
    {
        channel_dtmf_event_proc(iCount, strInfo);
        return;
    }

    if( GetHeader( iCount, strInfo, "Event-Name: " ) == "RECORD_STOP")
    {
        TraceEx( "record_stop begin to enter now...");
        channel_record_stop_event_proc(iCount, strInfo);
        return;
    }


    if( GetHeader( iCount, strInfo, "Event-Name: " ) == "CHANNEL_HANGUP")
    {
        channel_hangup_event_proc(iCount, strInfo);
        return;
    }

    if( GetHeader( iCount, strInfo, "Event-Name: " ) == "CHANNEL_PARK")
    {
        channel_park_event_proc(iCount, strInfo);
        return;
    }

    if( GetHeader( iCount, strInfo, "Event-Name: " ) == "CHANNEL_CREATE")
    {
        if( GetHeader( iCount, strInfo, "Answer-State: ") == "ringing")
        {
//            channel_ext_ringing_event_proc(iCount, strInfo);
        }
        return;
    }

    if( GetHeader( iCount, strInfo, "Event-Name: " ) == "CHANNEL_ANSWER")
    {
        channel_answer_event_proc(iCount, strInfo);
        return;
    }

    if( GetHeader( iCount, strInfo, "Event-Subclass: " ) == "callcenter::info")
    {
        if( GetHeader( iCount, strInfo, "CC-Action: ") == "agent-offering")
        {
            return;
        }

        if( GetHeader( iCount, strInfo, "CC-Action: ") == "bridge-agent-start")
        {
            int iii = 0;
            return;
        }

        if( GetHeader( iCount, strInfo, "CC-Action: ") == "bridge-agent-end")
        {
            int iii = 0;
            return;
        }

        if( GetHeader( iCount, strInfo, "CC-Action: ") == "member-queue-start")
        {
            member_queue_start_event_proc(iCount, strInfo);
            return;
        }

        if( GetHeader( iCount, strInfo, "CC-Action: ") == "member-queue-end")
        {
            member_queue_end_event_proc(iCount, strInfo);
            return;
        }
    }

    if( GetHeader( iCount, strInfo, "Event-Name: " ) == "CHANNEL_HOLD")
    {
        channel_hold_event_proc(iCount, strInfo);
        return;
    }

    if( GetHeader( iCount, strInfo, "Event-Name: " ) == "CHANNEL_UNHOLD")
    {
        channel_unhold_event_proc(iCount, strInfo);
        return;
    }
}

int  CDataProc::DecodeFrame(string *strInfo, string &strData, char *lpszSpliteValue, int iSpliteLen)
{
    int i = 0;
    int pos = strData.find( lpszSpliteValue );
    while( pos != string::npos )
    {
        int len = strData.length();
        strInfo[i] = strData.substr( 0, pos );
        strData = strData.substr( pos + iSpliteLen, len - ( strInfo[i].length() + iSpliteLen ) );
        pos = strData.find( lpszSpliteValue );
        i++;
    }
    strInfo[i] = strData;
    return i + 1;
}

string CDataProc::GetHeader(int iCount, string *strInfo, string strValue)
{
    string strResult = "";
    if( strInfo == NULL || iCount < 0 )
        return strResult;

    for(int i = 0; i < iCount; i++)
    {
        int nPos = strInfo[i].find( strValue );
        if( nPos != string::npos )
        {
            char *lp = (char *)strInfo[i].c_str();

            char szTemp[1024*2] = {0};
            memcpy( szTemp, lp, strValue.length() );
            if( strcmp( szTemp, (char *)strValue.c_str() ) == 0 )
            {
                lp += strValue.length();
                strResult = lp;
                break;
            }
        }
    }

    return strResult;
}

string CDataProc::GetChannelName(int iCount, string *strInfo, string strChannelName)
{
    string Channel_Name = GetHeader(iCount, strInfo, strChannelName ); //"Channel-Name: ");
    if( Channel_Name.find( "sofia/internal/") != string::npos )
    {
        Channel_Name = Channel_Name.replace( 0, strlen( "sofia/internal/"), "");
    }
    else if( Channel_Name.find( "sofia/internal_loc/") != string::npos )
    {
        Channel_Name = Channel_Name.replace( 0, strlen( "sofia/internal_loc/"), "");
    }
    else if( Channel_Name.find( "sip:") != string::npos )
    {
        Channel_Name = Channel_Name.replace( 0, strlen( "sip:"), "");
    }
    else if( Channel_Name.find( "gw+") != string::npos )
    {
        Channel_Name = Channel_Name.replace( 0, strlen( "gw+"), "");
    }

    int nPos1 = Channel_Name.find( "%");
    int nPos2 = Channel_Name.find( "@");
    if( nPos1 != string::npos )
    {
        Channel_Name = Channel_Name.substr( 0, nPos1 );
    }
    else if( nPos2 != string::npos )
    {
        Channel_Name = Channel_Name.substr( 0, nPos2 );
    }

    return Channel_Name;
}

string CDataProc::GetDestNumber(int iCount, string *strInfo, string strDestParam, int iReplaceCount, string *strReplaceParam)
{
    string dest_numer = GetHeader(iCount, strInfo, strDestParam);
    if( dest_numer.length() <= 0 )
        return dest_numer;

    for(int i = 0; i < iReplaceCount; i++)
    {
        if( dest_numer.find( strReplaceParam[i]) != string::npos )
        {
            dest_numer = dest_numer.replace( 0, strReplaceParam[i].length(), "" );
        }
    }

    int nPos1 = dest_numer.find( "%");
    int nPos2 = dest_numer.find( "@");
    if( nPos1 != string::npos )
    {
        dest_numer = dest_numer.substr( 0, nPos1 );
    }
    else if( nPos2 != string::npos )
    {
        dest_numer = dest_numer.substr( 0, nPos2 );
    }

    return dest_numer;
}

void CDataProc::data_proc_from_cti(int iClientId, char *buf)
{
    try
    {
    if( !m_bSeriCorrect )
        return;

    if( iClientId < 0 || buf == NULL )
        return;
    PACKET_HEAD *pHead = (PACKET_HEAD *)buf;

    string strInfo[64] = {""};
    DecodeData( buf, strInfo );

    char szLog[1024] = {0};
    sprintf( szLog, "data_proc_from_cti, cmd = %d", pHead->cmd );
    TraceEx( szLog );

    switch(pHead->cmd)
    {
    case CMD_MONITOR:
        m_CtiCtrl.cmd_monitor_proc( iClientId, buf, strInfo );
        break;
    case CMD_AGENTLOGON:
        m_CtiCtrl.cmd_agent_logon_proc( iClientId, buf, strInfo );
        break;
    case CMD_AGENTLOGOFF:
        m_CtiCtrl.cmd_agent_logoff_proc( iClientId, buf, strInfo );
        break;
    case CMD_AGENTREADY:
        m_CtiCtrl.cmd_agent_ready_proc( iClientId, buf, strInfo );
        break;
    case CMD_AGENTNOTREADY:
        m_CtiCtrl.cmd_agent_not_ready_proc( iClientId, buf, strInfo );
        break;
    case CMD_PICKUP:
        m_CtiCtrl.cmd_answer_proc( iClientId, buf, strInfo );
        break;
    case CMD_HANGUP:
        m_CtiCtrl.cmd_hangup_proc( iClientId, buf, strInfo );
        break;
    case CMD_MAKECALL:
        m_CtiCtrl.cmd_makecall_proc( iClientId, buf, strInfo );
        break;
    case CMD_HOLDCALL:
        m_CtiCtrl.cmd_hold_proc( iClientId, buf, strInfo );
        break;
    case CMD_RETRIEVECALL:
        m_CtiCtrl.cmd_retrieve_proc( iClientId, buf, strInfo );
        break;
    case CMD_INITTRANS:
        m_CtiCtrl.cmd_init_transfer_proc( iClientId, buf, strInfo );
        break;
    case CMD_COMPLETETRANS:
        m_CtiCtrl.cmd_complete_transfer_proc( iClientId, buf, strInfo );
        break;
    case CMD_COMPLETECONF:
        m_CtiCtrl.cmd_complete_conference_proc( iClientId, buf, strInfo );
        break;
    case CMD_LISTEN:
        m_CtiCtrl.cmd_listen_proc( iClientId, buf, strInfo );
        break;
    case CMD_INTRUDE:
        m_CtiCtrl.cmd_intrude_proc( iClientId, buf, strInfo );
        break;
    case CMD_FORCECH:
        m_CtiCtrl.cmd_forcech_proc( iClientId, buf, strInfo );
        break;
    case CMD_START_PLAY:
        m_CtiCtrl.cmd_start_play_proc( iClientId, buf, strInfo );
        break;
    case CMD_STOP_PLAY:
        m_CtiCtrl.cmd_stop_play_proc( iClientId, buf, strInfo );
        break;
   case CMD_START_RECORD:
        m_CtiCtrl.cmd_start_record_proc( iClientId, buf, strInfo );
        break;
    case CMD_STOP_RECORD:
        m_CtiCtrl.cmd_stop_record_proc( iClientId, buf, strInfo );
        break;
    case CMD_TALK_WITH:
        m_CtiCtrl.cmd_talk_witch_proc( iClientId, buf, strInfo );
        break;
    default:
        break;
    }
    }
    catch(...)
    {
        TraceEx( "error, exception, CData::data_proc() " );
    }
//	TraceEx( "pHead->cmd = %d, strInfo[0] = %s, strInfo[1] = %s, iClientId = %d",
//		pHead->cmd, strInfo[0], strInfo[1],  iClientId );

}

void CDataProc::DecodeData(char *buf, string *strInfo)
{
    try
    {
    char buffer[1024*100] = {0};
    PACKET_HEAD *pHead = (PACKET_HEAD *)buf;
    memcpy( buffer, buf + sizeof(PACKET_HEAD), pHead->dwSize );

    string strData = "";
    strData = buffer;
    DecodeFrame( strInfo, strData, "|", 1 );
    }
    catch(...)
    {
        TraceEx( "error, exception, CData::decode_data() " );
    }
}

void CDataProc::OnClientClose(int iClientId)
{
    if( iClientId <= 0 )
        return;

    remove_device_sock_info_by_sockid( iClientId );
}

void CDataProc::remove_device_sock_info_by_sockid(int sock_id)
{
    if( m_pDevice_Sock_List == NULL )
      return;

    if( m_pDevice_Sock_List->size() <= 0 )
        return;

    char szLog[512] = {0};
    sprintf( szLog, "remove device list by clientid = %d", sock_id );
    TraceEx( szLog );

    list<DEVICE_SOCK_INFO *>::iterator it;
    for(it = m_pDevice_Sock_List->begin(); it != m_pDevice_Sock_List->end(); )
    {
        DEVICE_SOCK_INFO *info = *it;
        if( info != NULL )
        {
            if( info->iClientId == sock_id )
            {
                it = m_pDevice_Sock_List->erase( it );

                delete info;
                info = NULL;
                continue;
            }
        }
        ++it;
    }
}


void CDataProc::InitParam()
{
    char szAESIni[64] = {0}, Writed[32] = {0};
    strcpy( szAESIni, "AESServer.ini" );

    QSettings *configIni = new QSettings(szAESIni, QSettings::IniFormat);

    QString strIVRVoiceIP = configIni->value("/IVRSetting/IVRVoiceVirtualPath").toString();

    strIVRVoiceIP = (strIVRVoiceIP.length() > 0)?strIVRVoiceIP:"http://127.0.0.1/ivr/";
    configIni->setValue("/IVRSetting/IVRVoiceVirtualPath", strIVRVoiceIP);
    m_strIVRVoiceVirtualPath = strIVRVoiceIP.toStdString();

    m_iCurrentNum = 0;

    m_nMonitorNum = 0;
#ifdef _VC6_
    m_nMonitorNum = GetPrivateProfileInt("Monitor", "MoniNum", 16, lpFile);
    sprintf(Writed,"%d",m_nMonitorNum);
    WritePrivateProfileString("Monitor", "MoniNum",Writed, lpFile);
#else
    m_nMonitorNum = configIni->value("/Monitor/MoniNum").toInt();
    m_nMonitorNum = (m_nMonitorNum>0)?m_nMonitorNum:16;
    sprintf(Writed,"%d",m_nMonitorNum);
    configIni->setValue("/Monitor/MoniNum", Writed);
#endif

    m_lpstrExtDN = NULL;
    m_lpstrExtDN = new string[m_nMonitorNum];
    if( m_lpstrExtDN == NULL )
    {
        TraceEx( "error, ext_dn malloc failed" );
        return;
    }

    for (int j = 0 ;j<m_nMonitorNum;j++)
    {
        sprintf(Writed,"ExtNo%d",j);

#ifdef _VC6_
        GetPrivateProfileString("Monitor",Writed,"",szTmp,20,lpFile);
        WritePrivateProfileString("Monitor", Writed,szTmp, lpFile);//
#else
        m_lpstrExtDN[j] = "";
        char szSection[32] = {0};
        sprintf( szSection, "/Monitor/%s", Writed );
        QString strExtNo = configIni->value(szSection).toString();
        m_lpstrExtDN[j] = (strExtNo.length()>0)?strExtNo.toStdString():"";
        configIni->setValue(szSection, strExtNo);
#endif
    }

    m_nIvrNum = configIni->value("/CTI/IVRNum").toInt();
    m_nIvrNum = (m_nIvrNum>0)?m_nIvrNum:32;
    sprintf(Writed,"%d",m_nIvrNum);
    configIni->setValue("/CTI/IVRNum", Writed);

    m_lpstrIVRDN = NULL;
    m_lpstrIVRDN = new string[m_nIvrNum];
    if( m_lpstrIVRDN == NULL )
    {
        TraceEx( "error, ivr_dn malloc memory failed!");
        return;
    }

    m_pIVRInfo = NULL;
    m_pIVRInfo = new IVR_INFO[m_nIvrNum];
    if( m_pIVRInfo == NULL )
    {
        TraceEx( "error, ivr_info malloc memory failed!");
        return;
    }


    int i = 0;
    for (i=0; i<m_nIvrNum; i++)
    {
        sprintf(Writed,"Channel%d",i);
#ifdef _VC6_
        GetPrivateProfileString("CTI",Writed,"",m_szCtiDN[i].GetBuffer(20),20,lpFile);
        WritePrivateProfileString("CTI", Writed,m_szCtiDN[i], lpFile);
#else

        m_lpstrIVRDN[i] = "";
        char szSection[32] = {0};
        sprintf( szSection, "/CTI/%s", Writed );
        QString strCtiDN = configIni->value(szSection).toString();
        m_lpstrIVRDN[i] = (strCtiDN.length()>0)?strCtiDN.toStdString():"";
        configIni->setValue(szSection, strCtiDN);

        memset( &m_pIVRInfo[i], 0, sizeof(IVR_INFO) );
        m_pIVRInfo[i].iChannelId = i;
        strcpy( m_pIVRInfo[i].szIVRExt, (char *)m_lpstrIVRDN[i].c_str() );
#endif
    }


    char szbuffer[32] = {0};

    m_iMoniIVRDNNum = configIni->value("/MoniIVRDN/IVRDNNum").toInt();
    m_iMoniIVRDNNum = (m_iMoniIVRDNNum>0)?m_iMoniIVRDNNum:0;
    sprintf(szbuffer,"%d",m_iMoniIVRDNNum);
    configIni->setValue("/MoniIVRDN/IVRDNNum", szbuffer);

    m_lpstrMoniIVRDN = NULL;
    m_lpstrMoniIVRDN = new string[m_iMoniIVRDNNum];
    if( m_lpstrMoniIVRDN == NULL )
    {
        TraceEx( "error, moni_ivr_dn malloc memory failed");
        return;
    }

    for (i=0; i<m_iMoniIVRDNNum; i++)
    {
        sprintf(Writed,"IVRDN%d",i);
        char szSection[32] = {0};
        sprintf( szSection, "/MoniIVRDN/%s", Writed );
        QString strMoniIVRDN = configIni->value(szSection).toString();
        m_lpstrMoniIVRDN[i] = "";
        m_lpstrMoniIVRDN[i] = (strMoniIVRDN.length()>0)?strMoniIVRDN.toStdString():"";
        configIni->setValue(szSection, strMoniIVRDN);
    }

    //conference group
    //m_lpstrConfGrpList
    m_nConfGrpNum = configIni->value("/MoniConfGrp/ConfGrpNum").toInt();
    m_nConfGrpNum = (m_nConfGrpNum>0)?m_nConfGrpNum:0;
    sprintf(Writed,"%d",m_nConfGrpNum);
    configIni->setValue("/MoniConfGrp/ConfGrpNum", Writed);

    m_lpstrConfGrpList = NULL;
    m_lpstrConfGrpList = new string[m_nConfGrpNum];
    if( m_lpstrConfGrpList == NULL )
    {
        TraceEx( "error, conf_grp_list malloc memory failed!");
        return;
    }

    for (i=0; i<m_nConfGrpNum; i++)
    {
        sprintf(Writed,"ConfGrp%d",i);
        char szSection[32] = {0};
        sprintf( szSection, "/MoniConfGrp/%s", Writed );
        QString strConfGrpNo = configIni->value(szSection).toString();
        m_lpstrConfGrpList[i] = "";
        m_lpstrConfGrpList[i] = (strConfGrpNo.length()>0)?strConfGrpNo.toStdString():"";
        configIni->setValue(szSection, strConfGrpNo);
    }



    m_pDevice_Sock_List = NULL;

    delete configIni;


}

bool CDataProc::IsBeyondLisence()
{
    bool bResult = false;
    if( m_iCurrentNum + 1 >= m_iLisenceNumber )
    {
        bResult = true;
        char szLog[128] = {0};
        sprintf( szLog, "beyond lisence num, m_iCurrentNum = %d, m_iLisenceNumber = %d", m_iCurrentNum, m_iLisenceNumber );
        TraceEx( szLog );
    }

    return bResult;
}

bool CDataProc::IsDestDNInConfList(char *szDest)
{
    bool bResult = false;

    if( szDest == NULL )
        return bResult;

    int i = 0;
    for( i = 0; i < m_nConfGrpNum; i++ )
    {
        if( m_lpstrConfGrpList[i] == szDest )
        {
            bResult = true;
            break;
        }
    }

    return bResult;

}

bool CDataProc::IsDNInIVRList(char *szExt)
{
    bool  bResult = false;

    if( szExt == NULL )
        return bResult;

    int i = 0;
    for( i = 0; i < m_nIvrNum; i++)
    {
        if( m_lpstrIVRDN[i] == szExt )
        {
            bResult = true;
            break;
        }
    }

    return bResult;
}

bool CDataProc::IsDNInExtList(char *szExt)
{
    bool  bResult = false;

    if( szExt == NULL )
        return bResult;

    int i = 0;
    for( i = 0; i < m_nIvrNum; i++)
    {
        if( m_lpstrIVRDN[i] == szExt )
        {
            bResult = true;
            break;
        }
    }


    return bResult;
}

bool CDataProc::IsDNExist(char *szExt)
{
    bool  bResult = false;

    if( szExt == NULL )
        return bResult;

    int i = 0;
    for( i = 0; i < m_nMonitorNum; i++)
    {
        if( m_lpstrExtDN[i] == szExt )
        {
            bResult = true;
            return bResult;
        }
    }

    i = 0;
    for( i = 0; i < m_nIvrNum; i++)
    {
        if( m_lpstrIVRDN[i] == szExt )
        {
            bResult = true;
            return bResult;
        }
    }

    i = 0;
    for( i = 0; i < m_iMoniIVRDNNum; i++)
    {
        if( m_lpstrMoniIVRDN[i] == szExt )
        {
            bResult = true;
            return bResult;
        }
    }

    return bResult;
}

int CDataProc::GetClientIdByExt(int iClientType, char *ext, int *iClientArray)
{
    int iTotalNum = 0;

    if( ext == NULL || iClientArray == NULL )
        return iTotalNum;


    if( m_pDevice_Sock_List == NULL )
    {
        m_pDevice_Sock_List = new list<DEVICE_SOCK_INFO *>;
        if( m_pDevice_Sock_List == NULL )
        {
            TraceEx( "error, get_client_id_by_ext, new malloca memory failed");
            return iTotalNum;
        }
    }

    if( m_pDevice_Sock_List->size() <= 0 )
        return iTotalNum;

    list<DEVICE_SOCK_INFO *>::iterator it;
    for(it = m_pDevice_Sock_List->begin(); it != m_pDevice_Sock_List->end(); ++it)
    {
        DEVICE_SOCK_INFO *info = *it;
        if( info != NULL )
        {
            /*
            char szLog[512] = {0};
            sprintf( szLog, "get_client_id_by_ext, szDevice = %s, ext = %s, iClientType = %d, client_type = %d",
                     info->szDevice, ext, info->iClientType, iClientType );
            TraceEx( szLog );
            */
            if( strcmp( info->szDevice, ext ) == 0 && info->iClientType == iClientType )
            {
                iClientArray[iTotalNum] = info->iClientId;
                iTotalNum++;
            }
        }
    }

    return iTotalNum;
}

int CDataProc::SendCallEventToClient(char *ext, int iClientType, int cmd,int cmdtype, char *lpszSendBuffer, int iDataLen)
{
    if( ext == NULL || iClientType < 0 || lpszSendBuffer == NULL || iDataLen <= 0 )
        return -1;

    char buffer[1024*16] = {0};

    PACKET_HEAD Packet_Head;
    memset( &Packet_Head, 0, sizeof( PACKET_HEAD ) );
    Packet_Head.cmd = cmd;
    Packet_Head.cmd_type = cmdtype;
    Packet_Head.dwFlag = PACKET_FLAG;
    Packet_Head.dwSize = iDataLen;

    memcpy( buffer, &Packet_Head, sizeof( PACKET_HEAD ));

    if( iDataLen != 0 )
    {
        memcpy( buffer + sizeof( PACKET_HEAD ), lpszSendBuffer, iDataLen );
    }

    int iClientId[64] = {0};
    int iClientNum = GetClientIdByExt( iClientType, ext, iClientId );
    if( iClientNum > 0 )
    {
        char szLog[256] = {0};
        sprintf( szLog, "notify, send_call_event_to_client, iClientNum = %d, ext = %s, iClientType = %d", iClientNum, ext, iClientType );
        TraceEx( szLog );

        for(int i = 0; i < iClientNum; i++)
        {
            m_CommonEngineDll.SendToClientFunc( iClientId[i], buffer, sizeof(PACKET_HEAD) + iDataLen );

            memset( szLog, 0, 256 );
            sprintf( szLog, "send_call_event_to_client, iClientId = %d, i = %d", iClientId[i], i );
            TraceEx( szLog );
        }
    }
    else
    {
        char szLog[256] = {0};
        sprintf( szLog, "error, can not find client, ext = %s, iClientType = %d, iClientNum = %d", ext, iClientType, iClientNum );
        TraceEx( szLog );
    }

    return iClientNum;
}

void CDataProc::RecordCallInfoByCallID(char *szDeviceId, char *lpszUUID )
{
    if( szDeviceId == NULL || lpszUUID == NULL )
        return;

    char szLog[512] = {0};

    if( !IsDNExist(szDeviceId))
    {
        sprintf( szLog, "warnning, ext dose not exist, device = %s", szDeviceId );
        TraceEx( szLog );
        return;
    }

    sprintf( szLog, "record_call_info, device = %s, uuid = %s", szDeviceId, lpszUUID );
    TraceEx( szLog );

    //CALL_INFO
    CALL_INFO *pCallInfo = m_DNCallInfoMap.Find( szDeviceId );
    if( pCallInfo == NULL )
    {
        pCallInfo = new CALL_INFO;
        if( pCallInfo != NULL )
        {
            memset( pCallInfo, 0, sizeof(CALL_INFO) );
            strcpy( pCallInfo->szDeviceId, szDeviceId );
            strcpy( pCallInfo->szActiveCallID, lpszUUID );

            m_DNCallInfoMap.Add( szDeviceId, pCallInfo );
        }
        else
        {
            TraceEx( "error, call_info malloc memory failed!" );
            return;
        }
    }
    else
    {
        if( pCallInfo->iStartTransferFlag == 1 )
        {
            memset( pCallInfo->szHeldCallID, 0, 64 );
            strcpy( pCallInfo->szHeldCallID, pCallInfo->szActiveCallID );
        }

        memset( pCallInfo->szActiveCallID, 0, 64 );
        strcpy( pCallInfo->szActiveCallID, lpszUUID );
    }

    memset( szLog, 0, 512 );
    sprintf( szLog, "notify, end_record_call_info, device = %s, heldCallId = %s, activeCallId = %s",
             szDeviceId, pCallInfo->szHeldCallID, pCallInfo->szActiveCallID );
    TraceEx( szLog );
}

CALL_INFO *CDataProc::FindCallInfoByUUID(char *uuid)
{
    CALL_INFO *pCallInfo = NULL;
    if( uuid == NULL )
        return pCallInfo;

    pCallInfo = m_DNCallInfoMap.Begin();
    while( pCallInfo != NULL )
    {
        if( strcmp( pCallInfo->szActiveCallID, uuid ) == 0 )
            break;

        pCallInfo = m_DNCallInfoMap.Next();
    }

    return pCallInfo;
}

CALL_INFO *CDataProc::FindCallInfoByDeviceId(char *szDeviceId)
{
    CALL_INFO *pCallInfo = NULL;
    if( szDeviceId == NULL )
        return pCallInfo;

    return m_DNCallInfoMap.Find( szDeviceId );
}

void CDataProc::channel_hangup_event_proc(int iCount, string *strInfo)
{
    string Channel_Name = GetChannelName(iCount, strInfo, "Channel-Name: ");
    string uuid = GetHeader( iCount, strInfo, "Unique-ID: ");
    string dialstatus = GetHeader(iCount, strInfo, "variable_DIALSTATUS: ");
    string strHangupCause = GetHeader(iCount, strInfo, "Hangup-Cause: " );
    string strOtherLegUUID = GetHeader( iCount, strInfo, "Other-Leg-Unique-ID: ");
    string sip_to_user = GetHeader(iCount, strInfo, "variable_sip_to_user: ");

    if (dialstatus == "USER_NOT_REGISTERED" || dialstatus == "INVALID_GATEWAY" || GetHeader(iCount, strInfo, "variable_dial_a_wrong_number: ").length() > 0 )
    {
        //拨打的号码为错号
        //need send unhold_event now...
    }

    char szLog[1024] = {0};
    sprintf( szLog, "-------hangup_event----- has occurred, uuid = %s, Channel_Name = %s, dialstatus = %s, Hangup_Cause = %s, sip_to_user = %s, other_leg_uuid = %s",
             (char *)uuid.c_str(), (char *)Channel_Name.c_str(), (char *)dialstatus.c_str(), (char *)strHangupCause.c_str(),
             (char *)sip_to_user.c_str(), (char *)strOtherLegUUID.c_str() );
    TraceEx( szLog );


    //在三方会议或转接取回时，多余的挂断事件过滤掉
    if( strstr( (char *)sip_to_user.c_str(), "*926" ) != NULL )
    {
        if( strstr( (char *)Channel_Name.c_str(), "*926") == NULL )
        {
            //A--B--C 三方会议中，c挂断了，A和B需要保持通话
            CALL_INFO *pCallInfo = FindCallInfoByDeviceId( (char *)Channel_Name.c_str() );
            if( pCallInfo != NULL && pCallInfo->iConferenceFlag == 1 )
            {
                memset( szLog, 0, 1024 );
                if( pCallInfo->iConferenceCount == 0 )
                {
                    pCallInfo->iRingbackFlag = 0;
                    //发送挂机事件给device
                    sprintf( szLog, "--notify, channel_hangup_event, send hangup_event to device, device = %s ", (char *)Channel_Name.c_str() );
                    char lpszSendBuffer[1024*2] = {0};
                    sprintf( lpszSendBuffer, "%s|%s", (char *)Channel_Name.c_str(), (char *)uuid.c_str() );

                    SendCallEventToClient( (char *)Channel_Name.c_str(), device_type, CMD_EXTHANGUP_EVENT, SNT_EVENT, lpszSendBuffer, strlen(lpszSendBuffer) );

                }
                else
                {
                    pCallInfo->iConferenceCount = 2;

                }

                memset( szLog, 0, 1024 );
                sprintf( szLog, "huangup_event, iConferenceFlag = %d, conference_count = %d, ext = %s",
                         pCallInfo->iConferenceFlag, pCallInfo->iConferenceCount, (char *)Channel_Name.c_str() );
                TraceEx( szLog );
            }
            return;
        }

        //A--B--C，B挂断了，A和C在通话，如果C挂断了，在*926的通道里会有A的UUID，这时要把A也挂断
    }
    else
    {
        if( strstr( (char *)Channel_Name.c_str(), "*926") != NULL )
        {
            //hangup myself uuid now
            memset( szLog, 0, 1024 );

            if( sip_to_user.length() == 0 && strHangupCause == "NORMAL_TEMPORARY_FAILURE" )
            {
                TraceEx( "------notify, channel_hangup_event, hangup_cause = NORMAL_TEMPORARY_FAILURE, and return now..." );
                return;
            }

            sprintf( szLog, "notify, channel_hangup_event, device = %s, other_leg_uuid = %s",
                     (char *)Channel_Name.c_str(), (char *)strOtherLegUUID.c_str() );
            TraceEx( szLog );

            CALL_INFO *pCallInfo = FindCallInfoByUUID( (char *)strOtherLegUUID.c_str() );
            if( pCallInfo != NULL && IsDNInExtList ( pCallInfo->szDeviceId ) )
            {
                memset( szLog, 0, 1024 );
                sprintf( szLog, "notify, channel_hangup_event, no need hangup ext and return now, device_ext = %s, active_id = %s", pCallInfo->szDeviceId, pCallInfo->szActiveCallID );
                TraceEx( szLog );
                return;
            }

            if( pCallInfo != NULL && pCallInfo->iRetrieveFlag == 1 )
            {
                memset( szLog, 0, 1024 );
                sprintf( szLog, "notify, ----*926--------hangup_event don't need send hangup msg to client and return now...,  device = %s, active_call_id = %s, other_leg_uuid = %s",
                         pCallInfo->szDeviceId, pCallInfo->szActiveCallID, (char *)strOtherLegUUID.c_str() );
                TraceEx( szLog );
                return;
            }

            char szCmd[128] = {0};
            sprintf( szCmd, "bgapi uuid_kill %s\n\n", (char *)strOtherLegUUID.c_str() );

            m_clientSock.SendToClient( szCmd, strlen(szCmd));

            TraceEx( szCmd );
            return;
        }
        else
        {
            CALL_INFO *pCallInfo = FindCallInfoByDeviceId( (char *)Channel_Name.c_str() );
            if( pCallInfo != NULL && pCallInfo->iRetrieveFlag == 1 )
            {
                pCallInfo->iRetrieveFlag = 0;
                if( strcmp( pCallInfo->szActiveCallID, (char *)uuid.c_str() ) != 0 )
                {
                    memset( szLog, 0, 1024 );
                    sprintf( szLog, "notify, hangup_event don't need send hangup msg to client and return now...,  channel_name = %s, active_call_id = %s, uuid = %s",
                             (char *)Channel_Name.c_str(), pCallInfo->szActiveCallID, (char *)uuid.c_str() );
                    TraceEx( szLog );
                    return;
                }
            }

            if( pCallInfo != NULL )
                pCallInfo->iRingbackFlag = 0;


            if( strHangupCause == "INCOMPATIBLE_DESTINATION" )
            {
                TraceEx( "--------hangup_event, do not hangup and return now...");
                return;
            }


            char lpszSendBuffer[1024*2] = {0};
            sprintf( lpszSendBuffer, "%s|%s", (char *)Channel_Name.c_str(), (char *)uuid.c_str() );

            int iResult = SendCallEventToClient( (char *)Channel_Name.c_str(), device_type, CMD_EXTHANGUP_EVENT, SNT_EVENT, lpszSendBuffer, strlen(lpszSendBuffer) );
            if( iResult <= 0 )
            {
                memset( szLog, 0, 1024 );
                sprintf( szLog, "warnning, hangup_event, send_call_event_to_client failed, Channel_Name = %s, iResult = %d", (char *)Channel_Name.c_str(), iResult );
                TraceEx( szLog );

                IVR_INFO *pIVRInfo = get_ivr_info_by_uuid( (char *)uuid.c_str() );
                if( pIVRInfo != NULL )
                {
                    memset( lpszSendBuffer, 0, 1024*2 );
                    sprintf( lpszSendBuffer, "%s|%s", pIVRInfo->szIVRExt, (char *)uuid.c_str() );
                    iResult = SendCallEventToClient( pIVRInfo->szIVRExt, device_type, CMD_EXTHANGUP_EVENT, SNT_EVENT, lpszSendBuffer, strlen(lpszSendBuffer) );

                    memset( szLog, 0, 1024 );
                    sprintf( szLog, "last send_1, hangup_event, send_hangup_event_to_cient, send_buffer = %s", lpszSendBuffer );
                    TraceEx( szLog );

                    pIVRInfo->iRunningFlag = 0;
                    memset( pIVRInfo->szCaller, 0, 64 );
                    memset( pIVRInfo->szUUID, 0, 64 );
                }
                else
                {
                    if( strstr( (char *)Channel_Name.c_str(), "loopback") != NULL )
                            return;

                    CALL_INFO *pTheOtherCallInfo = FindCallInfoByUUID( (char *)strOtherLegUUID.c_str() );
                    if( pTheOtherCallInfo != NULL )
                    {
                        pTheOtherCallInfo->iRingbackFlag = 0;

                        memset( lpszSendBuffer, 0, 1024*2 );
                        sprintf( lpszSendBuffer, "%s|%s", pTheOtherCallInfo->szDeviceId, (char *)strOtherLegUUID.c_str() );
                        iResult = SendCallEventToClient( pTheOtherCallInfo->szDeviceId, device_type, CMD_EXTHANGUP_EVENT, SNT_EVENT, lpszSendBuffer, strlen(lpszSendBuffer) );

                        memset( szLog, 0, 1024 );
                        sprintf( szLog, "last send_2, hangup_event, send_hangup_event_to_cient, send_buffer = %s", lpszSendBuffer );
                        TraceEx( szLog );
                    }
                }

                return;
            }
        }
    }

    CALL_INFO *pCallInfo = FindCallInfoByDeviceId( (char *)Channel_Name.c_str() );
    if( pCallInfo == NULL )
    {
        memset( szLog, 0, 1024 );
        sprintf( szLog, "warnning, pCallInfo = NULL, device = %s", (char *)Channel_Name.c_str() );
        TraceEx( szLog );
        return;
    }

    memset( szLog, 0, 1024 );
    sprintf( szLog, "notify, channel_hangup_event, device = %s, iGradeFlag = %d, iStartTransferFlag = %d, iConferenceFlag = %d, iConferenceCount = %d, other_leg_uuid = %s",
             (char *)Channel_Name.c_str(), pCallInfo->iGradeFlag, pCallInfo->iStartTransferFlag, pCallInfo->iConferenceFlag, pCallInfo->iConferenceCount, (char *)strOtherLegUUID.c_str() );
    TraceEx( szLog );

    if( pCallInfo->iStartTransferFlag == 1 )
    {
        pCallInfo->iStartTransferFlag = 0;
        if( pCallInfo->iConferenceFlag == 1 )
        {
            pCallInfo->iConferenceFlag = 0;
            if( pCallInfo->iConferenceCount >= 3 )
            {
                pCallInfo->iConferenceCount = 0;
                TraceEx( "notify, channel_hangup_event, don't need hangup other leg and return now...");
                return;
            }
        }
        else
        {
            TraceEx( "notify, channel_hangup_event, don't need hangup other leg and return now...");
            return;
        }
    }

    IVR_INFO *pIVRInfo = get_ivr_info_by_uuid( (char *)strOtherLegUUID.c_str() );
    if( pIVRInfo != NULL )
    {
        pIVRInfo->iGradeFlag = pCallInfo->iGradeFlag;
    }
    else
    {
        memset( szLog, 0, 1024 );
        sprintf( szLog, "notify, channel_hangup_event,can not find ivr_info, other_leg_uuid = %s ", (char *)strOtherLegUUID.c_str() );
        TraceEx( szLog );
    }

    if( pCallInfo->iGradeFlag == 1 )
    {
        memset( szLog, 0, 1024 );
        sprintf( szLog, "notify, channel_hanup_event, iGradeFlag = %d, and do not need hangup other channel!", pCallInfo->iGradeFlag );
        TraceEx( szLog );
    }
    else
    {
        CALL_INFO *pOtherLegCallInfo = FindCallInfoByUUID( (char *)strOtherLegUUID.c_str() );
        if( pOtherLegCallInfo != NULL && pOtherLegCallInfo->iConferenceFlag == 1 )
        {
            pOtherLegCallInfo->iConferenceCount = 0;
            memset( szLog, 0, 1024 );
            sprintf( szLog, "----notify, channel_hangup_event, find other_leg_call_info, device = %s, other_leg_uuid = %s, conference_count = %d",
                     pOtherLegCallInfo->szDeviceId, (char *)strOtherLegUUID.c_str(), pOtherLegCallInfo->iConferenceCount );
        }

        //hangup other leg channel now
        char szCmd[128] = {0};
        sprintf( szCmd, "bgapi uuid_kill %s\n\n", (char *)strOtherLegUUID.c_str() );

        m_clientSock.SendToClient( szCmd, strlen(szCmd));

        TraceEx( szCmd );
    }

    pCallInfo->iGradeFlag = 0;
}

void CDataProc::channel_park_event_proc(int iCount, string *strInfo)
{
    /*
    for(int i = 0; i < iCount; i++)
    {
        char szLog[1024*10] = {0};
        sprintf( szLog, "park_event, i = %d, iCount = %d, info = %s", i, iCount, (char *)strInfo[i].c_str());
        TraceEx( szLog );
    }
    */

    string uuid = GetHeader(iCount, strInfo, "Unique-ID: " );
    if( GetHeader( iCount, strInfo, "Call-Direction: ") == "inbound"
            && GetHeader( iCount, strInfo, "Event-Calling-Function: ") == "switch_ivr_park" )
    {
        //IVR呼入
        string strCmd = "bgapi uuid_broadcast " + uuid + " event::Event-Subclass=IVR_Ringing_Event,Caller=${caller_id_number},UUID=${uuid},VDN=${destination_number}" + "\n\n";
        m_clientSock.SendToClient( (char *)strCmd.c_str(), strCmd.length() );
        TraceEx( (char *)strCmd.c_str() );

        strCmd = "";
        strCmd = "bgapi uuid_broadcast " + uuid + " unset::park_from_ivr" + "\n\n";
        m_clientSock.SendToClient( (char *)strCmd.c_str(), strCmd.length() );
        TraceEx( (char *)strCmd.c_str() );
    }
    else if( GetHeader( iCount, strInfo, "variable_IVR_outbound: ").length() > 0 )
    {
        //IVR外呼
        char szLog[1024*10] = {0};

        string call_direct = GetHeader( iCount, strInfo, "Call-Direction: ");

        sprintf( szLog, "---detect ivr_outbound now, channel_park_event, call_dire = %s, uuid = %s", (char *)call_direct.c_str(), (char *)uuid.c_str() );
        TraceEx( szLog );

        string IVR_DN = GetHeader(iCount, strInfo, "variable_IVR_DN: ");
        string IVR_OutboundNumber = GetHeader(iCount, strInfo, "variable_IVR_OutboundNumber: ");

        string strCmd = "";
        strCmd = "bgapi uuid_broadcast " + uuid + " event::Event-Subclass=IVR_RingBack_Event,IVR_DN=" + IVR_DN+",UUID=${uuid}"+",IVR_OutboundNumber=" + IVR_OutboundNumber + "\n\n";
        m_clientSock.SendToClient( (char *)strCmd.c_str(), strCmd.length() );
        TraceEx( (char *)strCmd.c_str() );

    }
    else
    {
        char szLog[1024*10] = {0};

        string call_direct = GetHeader( iCount, strInfo, "Call-Direction: ");

        sprintf( szLog, "---channel_park_event, call_dire = %s, uuid = %s", (char *)call_direct.c_str(), (char *)uuid.c_str() );
        TraceEx( szLog );
    }
}

void CDataProc::channel_ext_ringback_event_proc(int iCount, string *strInfo)
{
    string Channel_Name = GetChannelName(iCount, strInfo, "Owner: ");
    string strCallee = GetHeader(iCount, strInfo, "Callee: ");
    string uuid = GetHeader(iCount, strInfo, "UUID: ");
    string other_leg_callee_num = GetHeader(iCount, strInfo, "Other-Leg-Callee-ID-Number: ");
    string Caller_Caller_ID_Number = GetHeader(iCount, strInfo, "Caller-Caller-ID-Number: ");//

    char szLog[1024] = {0};
    sprintf( szLog, "------ringback_event------- hase occurred, Channel_Name = %s, callee = %s, uuid = %s, other_leg_callee_num = %s, Caller_Caller_ID_Number = %s",
             (char *)Channel_Name.c_str(), (char *)strCallee.c_str(), (char *)uuid.c_str(), (char *)other_leg_callee_num.c_str(), (char *)Caller_Caller_ID_Number.c_str() );
    TraceEx( szLog );

    if( Channel_Name == strCallee )
    {
        if( other_leg_callee_num.length() == 0 || other_leg_callee_num == Channel_Name )
        {
            if( Caller_Caller_ID_Number !=  Channel_Name )
            {
                strCallee = Caller_Caller_ID_Number;
            }
            else
            {

            }
        }
        else
        {
            strCallee = other_leg_callee_num;
        }
    }

    if( Channel_Name == strCallee )
    {
        memset( szLog, 0, 1024 );
        sprintf( szLog, "----warnning, ringback_event, channe_name  = strcallee and return now..., channel_name = %s, callee = %s",
                 (char *)Channel_Name.c_str(),(char *)strCallee.c_str() );
        TraceEx( szLog );
        return;
    }

    CALL_INFO *pCallInfo = FindCallInfoByDeviceId( (char *)Channel_Name.c_str() );
    if( pCallInfo != NULL )
    {
        if( pCallInfo->iRingbackFlag == 1 )
        {
            memset( szLog, 0, 1024 );
            sprintf( szLog, "---warinning, ringback_event already exist and return now, device = %s, ring_back_flag = %d",
                     pCallInfo->szDeviceId, pCallInfo->iRingbackFlag );
            TraceEx( szLog );
            return;
        }
    }
    else
    {
        //IVR_ext
        IVR_INFO *pIVRInfo = get_ivr_info_by_device( (char *)Caller_Caller_ID_Number.c_str(), false );
        if( pIVRInfo != NULL )
        {
            pIVRInfo->iRunningFlag = 1;
            memset( pIVRInfo->szUUID, 0, 64 );
            strcpy( pIVRInfo->szUUID, (char *)uuid.c_str() );

            memset( pIVRInfo->szCaller, 0, 64 );
            strcpy( pIVRInfo->szCaller, (char *)Channel_Name.c_str() );

            RecordCallInfoByCallID( (char *)Caller_Caller_ID_Number.c_str(), (char *)uuid.c_str() );


            //send ivr ringback event now...
            char lpszSendBuffer[1024*2] = {0};

            int iCmd = CMD_EXTENSIONRINGBACK_EVENT;

            sprintf( lpszSendBuffer, "%d|%s|%s|%s|%s|%s", iCmd, pIVRInfo->szIVRExt, pIVRInfo->szIVRExt,
                     (char *)Channel_Name.c_str(), pIVRInfo->szUUID, "" );

            char szLog[1024] = {0};
            sprintf( szLog, "------ivr_ringback_event------- hase occurred, lpszSendBuffer = %s", lpszSendBuffer );
            TraceEx( szLog );

            SendCallEventToClient( pIVRInfo->szIVRExt, device_type, iCmd, SNT_EVENT, lpszSendBuffer, strlen(lpszSendBuffer) );

            return;
        }

    }

    RecordCallInfoByCallID( (char *)Channel_Name.c_str(), (char *)uuid.c_str() );

    CALL_INFO *pCall_Info = FindCallInfoByDeviceId( (char *)Channel_Name.c_str() );
    if( pCall_Info != NULL )
    {
        pCall_Info->iRingbackFlag = 1;

        memset( pCall_Info->szCalleeId, 0, 64 );
        strcpy( pCall_Info->szCalleeId, (char *)strCallee.c_str() );
    }

    char lpszSendBuffer[1024*2] = {0};

    int iCmd = CMD_EXTENSIONRINGBACK_EVENT;

    sprintf( lpszSendBuffer, "%d|%s|%s|%s|%s|%s", iCmd, (char *)Channel_Name.c_str(), (char *)Channel_Name.c_str(),
             (char *)strCallee.c_str(), (char *)uuid.c_str(), "" );

    SendCallEventToClient( (char *)Channel_Name.c_str(), device_type, iCmd, SNT_EVENT, lpszSendBuffer, strlen(lpszSendBuffer) );
}

void CDataProc::channel_ext_ringing_event_proc(int iCount, string *strInfo)
{
    string CallerNumber = "", Channel_Name = "", uuid = "";

    CallerNumber = GetChannelName(iCount, strInfo, "Caller: ");
    Channel_Name = GetHeader(iCount, strInfo, "Callee: ");
    if( Channel_Name.length() == 0 )
        Channel_Name = GetHeader(iCount, strInfo, "Caller-Callee-ID-Number: ");

    uuid = GetHeader(iCount, strInfo, "UUID: " );

    string ori_ani = GetHeader(iCount, strInfo, "ori_ani: ");

    char szLog[1024] = {0};
    sprintf( szLog, "------ringing_event------- hase occurred, Channel_Name = %s, caller_numer = %s, orig_ani = %s, uuid = %s",
             (char *)Channel_Name.c_str(), (char *)CallerNumber.c_str(), (char *)ori_ani.c_str(),
             (char *)uuid.c_str());
    TraceEx( szLog );

    RecordCallInfoByCallID( (char *)Channel_Name.c_str(), (char *)uuid.c_str() );

    CALL_INFO *pCallInfo = FindCallInfoByDeviceId( (char *)Channel_Name.c_str() );
    if( pCallInfo != NULL )
        pCallInfo->iRingbackFlag = 0;

    char lpszSendBuffer[1024*2] = {0};

    int iCmd = CMD_CALLERIDCOMING_EVENT;

    sprintf( lpszSendBuffer, "%d|%s|%s|%s|%s|%s", iCmd, (char *)Channel_Name.c_str(), (char *)CallerNumber.c_str(),
             (char *)Channel_Name.c_str(), (char *)uuid.c_str(), "" );

    SendCallEventToClient( (char *)Channel_Name.c_str(), device_type, iCmd, SNT_EVENT, lpszSendBuffer, strlen(lpszSendBuffer) );
}

void CDataProc::channel_answer_event_proc(int iCount, string *strInfo)
{
    string CallerNumber = "", DestinationNumber = "", ani = "";
    string Caller_Orig_Caller_ID_Number = "";
    string UUID = GetHeader(iCount, strInfo, "Unique-ID: " );
    string Channel_Name = GetChannelName(iCount, strInfo, "Channel-Name: ");

    int iReplaceCount = 3;
    string stringReplaceParam[3] = {"sip:", "gw+", "*926" };
    CallerNumber = GetDestNumber( iCount, strInfo, "Caller-Caller-ID-Number: ", iReplaceCount, stringReplaceParam );
    DestinationNumber = GetDestNumber( iCount, strInfo, "Caller-Destination-Number: ", iReplaceCount, stringReplaceParam );
    Caller_Orig_Caller_ID_Number = GetDestNumber( iCount, strInfo, "Caller-Orig-Caller-ID-Number: ", iReplaceCount, stringReplaceParam );

    char szLog[1024] = {0};
    sprintf( szLog, "------answer_event------- hase occurred, Channel_Name = %s, caller_numer = %s, dest_number = %s, orig_caller_numer = %s, UUID = %s       ",
             (char *)Channel_Name.c_str(), (char *)CallerNumber.c_str(), (char *)DestinationNumber.c_str(), (char *)Caller_Orig_Caller_ID_Number.c_str(),
             (char *)UUID.c_str() );
    TraceEx( szLog );

    if( DestinationNumber == "Local_Extension" )
    {
        CALL_INFO *pCallInfo = FindCallInfoByDeviceId( (char *)Channel_Name.c_str() );
        if( pCallInfo != NULL && pCallInfo->iConferenceFlag != 1 )
        {
            TraceEx( "warnning, do not send answer_event to client, because dest_num = Local_Extension, and return now...");
            return;
        }
    }

    if( Channel_Name == CallerNumber && Channel_Name == DestinationNumber &&  Channel_Name == Caller_Orig_Caller_ID_Number )
    {
        TraceEx("warnning, do not send answer_event to client, because 4 param_value is the same value adn return now...");
        return;
    }

    bool bIVROutboundFlag = false;

    if( IsDNInIVRList((char *)Caller_Orig_Caller_ID_Number.c_str()))
    {
        bIVROutboundFlag =  true;
    }

    int iIO = 0;
    char dest_ext[64] = {0}, dest_ext_other[64] = {0};
    if( DestinationNumber == "IVR_outbound" || ( !IsDNExist( (char *)Channel_Name.c_str() ) && !IsDNExist( (char *)CallerNumber.c_str()) && !IsDNExist( (char *)DestinationNumber.c_str())) )
    {
        bIVROutboundFlag = true;
    }

    if( bIVROutboundFlag )
    {
        //------answer_event------- hase occurred, Channel_Name = loopback/6006-a, caller_numer = 6006, dest_number = IVR_outbound, orig_caller_numer = 5002, UUID = 44e5445a-f8fb-11e6-bbba-9b54c792bfd5
        //IVR外呼，通话建立事件

        if( DestinationNumber != "IVR_outbound" )
        {
            CALLEEID_INFO *pCalleeIdInfo = new CALLEEID_INFO;
            if( pCalleeIdInfo != NULL )
            {
                memset( pCalleeIdInfo, 0, sizeof(CALLEEID_INFO) );
                strcpy( pCalleeIdInfo->szUUID, (char *)UUID.c_str() );
                strcpy( pCalleeIdInfo->szCalleeId, (char *)DestinationNumber.c_str() );

                m_CalleeIdInfoMap.Add( UUID, pCalleeIdInfo );
                TraceEx( "-------add_calleeid map--------");
            }
        }

        iIO = 1;
        DestinationNumber = CallerNumber;
        CallerNumber = Caller_Orig_Caller_ID_Number;
        strcpy( dest_ext, (char *)Caller_Orig_Caller_ID_Number.c_str() );

        memset( szLog, 0, 1024 );
        sprintf( szLog, "answer_event, IVR_OUTBOUND, dest_number = %s, caller_num = %s, iIO = %d", (char *)DestinationNumber.c_str(),
                 (char *)CallerNumber.c_str(), iIO );
        TraceEx( szLog );
    }
    else
    {
        if( strstr( (char *)Channel_Name.c_str(), "*926" ) != NULL )
        {
            Channel_Name = CallerNumber;
            DestinationNumber = Caller_Orig_Caller_ID_Number;
        }

        if( Channel_Name == CallerNumber )
            iIO = 1;

        if( iIO == 0 && DestinationNumber.length() == 0 )
            DestinationNumber = Channel_Name;

        strcpy( dest_ext, (char *)Channel_Name.c_str() );

        CALL_INFO *pCallInfo = FindCallInfoByDeviceId( (char *)Channel_Name.c_str() );
        if( pCallInfo != NULL && pCallInfo->iRingbackFlag == 1 )
        {
            DestinationNumber = pCallInfo->szCalleeId;
        }

        CALL_INFO *pCallInfo_caller_number = FindCallInfoByDeviceId( (char *)CallerNumber.c_str() );
        if( strcmp(dest_ext, (char *)CallerNumber.c_str() ) != 0 && pCallInfo_caller_number != NULL )
        {
            memset( dest_ext_other, 0, 64 );
            strcpy( dest_ext_other, (char *)CallerNumber.c_str() );
        }
    }

    if( DestinationNumber == "Local_Extension" )
        return;

    //callingDevice|answeringDevice|calledDevice|lCallID|iIO
    char lpszSendBuffer[1024*2] = {0};

    sprintf( lpszSendBuffer, "%s|%s|%s|%s|%d", (char *)CallerNumber.c_str(), (char *)DestinationNumber.c_str(),
             (char *)DestinationNumber.c_str(), (char *)UUID.c_str(), iIO );

    TraceEx( lpszSendBuffer );

    SendCallEventToClient( dest_ext, device_type, CMD_CALLESTABLISHED_EVENT, SNT_EVENT, lpszSendBuffer, strlen(lpszSendBuffer) );

    if( strlen( dest_ext_other ) > 0 )
    {
        char lpszSendBuffer_other[1024*2] = {0};
        int iIO_other = 1;

        sprintf( lpszSendBuffer_other, "%s|%s|%s|%s|%d", (char *)CallerNumber.c_str(), (char *)DestinationNumber.c_str(),
                 (char *)DestinationNumber.c_str(), (char *)UUID.c_str(), iIO_other );

        SendCallEventToClient( dest_ext_other, device_type, CMD_CALLESTABLISHED_EVENT, SNT_EVENT, lpszSendBuffer_other, strlen(lpszSendBuffer_other) );
    }
}

void CDataProc::channel_hold_event_proc(int iCount, string *strInfo)
{
    string Channel_Name = GetChannelName(iCount, strInfo, "Channel-Name: ");
    string Callee = GetHeader(iCount, strInfo, "Caller-Destination-Number: ");
    string UUID = GetHeader(iCount, strInfo, "Unique-ID: " );

    string OtherLegUUID = GetHeader(iCount, strInfo, "Other-Leg-Unique-ID: ");

    char szLog[1024] = {0};
    sprintf( szLog, "------hold_event------- hase occurred, Channel_Name = %s, Callee = %s, UUID = %s, OtherLegUUID = %s",
             (char *)Channel_Name.c_str(), (char *)Callee.c_str(), (char *)UUID.c_str(), (char *)OtherLegUUID.c_str() );
    TraceEx( szLog );

    //activeDevice|heldDevice|lCallID
    char lpszSendBuffer[1024*2] = {0};
    sprintf( lpszSendBuffer, "%s|%s|%s", (char *)Channel_Name.c_str(), (char *)Callee.c_str(), (char *)UUID.c_str());

    SendCallEventToClient( (char *)Channel_Name.c_str(), device_type, CMD_EXTONHOLD_EVENT, SNT_EVENT, lpszSendBuffer, strlen(lpszSendBuffer) );
}

void CDataProc::channel_unhold_event_proc(int iCount, string *strInfo)
{
    string Channel_Name = GetChannelName(iCount, strInfo, "Channel-Name: ");
    string Callee = GetHeader(iCount, strInfo, "Caller-Destination-Number: ");
    string UUID = GetHeader(iCount, strInfo, "Unique-ID: " );

    string OtherLegUUID = GetHeader(iCount, strInfo, "Other-Leg-Unique-ID: ");

    char szLog[1024] = {0};
    sprintf( szLog, "------unhold_event------- hase occurred, Channel_Name = %s, Callee = %s, UUID = %s, OtherLegUUID = %s",
             (char *)Channel_Name.c_str(), (char *)Callee.c_str(), (char *)UUID.c_str(), (char *)OtherLegUUID.c_str() );
    TraceEx( szLog );

    //activeDevice|lCallID
    char lpszSendBuffer[1024*2] = {0};
    sprintf( lpszSendBuffer, "%s|%s", (char *)Channel_Name.c_str(), (char *)UUID.c_str());

    SendCallEventToClient( (char *)Channel_Name.c_str(), device_type, CMD_EXTONRETRIEVE_EVENT, SNT_EVENT, lpszSendBuffer, strlen(lpszSendBuffer) );
}

void CDataProc::channel_ivr_ringback_event_proc(int iCount, string *strInfo)
{
    string IVR_DN = GetChannelName(iCount, strInfo, "IVR_DN: ");
    string IVR_OutboundNumber = GetChannelName(iCount, strInfo, "IVR_OutboundNumber: ");
    string UUID = GetHeader(iCount, strInfo, "UUID: " );

    char szLog[1024] = {0};
    sprintf( szLog, "ivr_ringback_event, ivr_dn = %s, ivr_outbound_number = %s, uuid = %s",
             (char *)IVR_DN.c_str(), (char *)IVR_OutboundNumber.c_str(), (char *)UUID.c_str() );
    TraceEx( szLog );

    IVR_INFO *pIVRInfo = get_ivr_info_by_device( (char *)IVR_DN.c_str(), false );
    if( pIVRInfo != NULL )
    {
        pIVRInfo->iRunningFlag = 1;
        memset( pIVRInfo->szUUID, 0, 64 );
        strcpy( pIVRInfo->szUUID, (char *)UUID.c_str() );

        memset( pIVRInfo->szCaller, 0, 64 );
        strcpy( pIVRInfo->szCaller, (char *)IVR_OutboundNumber.c_str() );

        RecordCallInfoByCallID( (char *)IVR_DN.c_str(), (char *)UUID.c_str() );


        //send ivr ringback event now...
        char lpszSendBuffer[1024*2] = {0};

        int iCmd = CMD_EXTENSIONRINGBACK_EVENT;

        sprintf( lpszSendBuffer, "%d|%s|%s|%s|%s|%s", iCmd, pIVRInfo->szIVRExt, pIVRInfo->szIVRExt,
                 (char *)IVR_OutboundNumber.c_str(), pIVRInfo->szUUID, "" );

        char szLog[1024] = {0};
        sprintf( szLog, "------ivr_ringback_event------- hase occurred, lpszSendBuffer = %s", lpszSendBuffer );
        TraceEx( szLog );

        SendCallEventToClient( pIVRInfo->szIVRExt, device_type, iCmd, SNT_EVENT, lpszSendBuffer, strlen(lpszSendBuffer) );
    }
    else
    {
        memset( szLog, 0, 1024 );
        sprintf( szLog, "error, can not find IVR_INFO, ivr_dn = %s", (char *)IVR_DN.c_str() );
        TraceEx( szLog );
    }
}


void CDataProc::channel_ivr_ringing_event_proc(int iCount, string *strInfo)
{
    string strCaller = GetChannelName(iCount, strInfo, "Caller: ");
    string UUID = GetHeader(iCount, strInfo, "UUID: " );

    //Callee为转入IVR流程的VDN而不是IVR_Channel
//    string strCalleeId = GetHeader(iCount, strInfo, "VDN: " );

    char szIVRDevice[64] = {0};
    if( is_ivr_running( (char *)UUID.c_str(), szIVRDevice ) )
    {
        char szLog[512] = {0};
        sprintf( szLog, "notify, ivr_channel is running and kill the uuid now, ivr_device = %s, uuid = %s",
                 szIVRDevice, (char *)UUID.c_str() );
        TraceEx( szLog );

        IVR_INFO *pIVRInfo = get_ivr_info_by_device( szIVRDevice, true );
        if( pIVRInfo != NULL && pIVRInfo->iGradeFlag == 1 )
        {
            memset( szLog, 0, 512 );
            sprintf( szLog, "notify, ivr_info->iGradeFlag = %d, do not kill the uuid and continue by ivr_flow", pIVRInfo->iGradeFlag );
            TraceEx( szLog );

            pIVRInfo->iGradeFlag = 0;
        }
        else
        {
            char szCmd[128] = {0};
            sprintf( szCmd, "bgapi uuid_kill %s\n\n", (char *)UUID.c_str() );

            m_clientSock.SendToClient( szCmd, strlen(szCmd));

            TraceEx( szCmd );
        }

        return;
    }

    IVR_INFO *pIVRInfo = get_idle_ivr_channel();
    if( pIVRInfo != NULL )
    {
        pIVRInfo->iRunningFlag = 1;
        memset( pIVRInfo->szCaller, 0, 64 );
        memset( pIVRInfo->szUUID, 0, 64 );

        strcpy( pIVRInfo->szCaller, (char *)strCaller.c_str() );
        strcpy( pIVRInfo->szUUID, (char *)UUID.c_str() );

        RecordCallInfoByCallID( pIVRInfo->szIVRExt, pIVRInfo->szUUID );

        //send ivr ringing event now...
        char lpszSendBuffer[1024*2] = {0};

        int iCmd = CMD_CALLERIDCOMING_EVENT;

        CALLEEID_INFO *pCalleeIdInfo = m_CalleeIdInfoMap.Find( UUID );
        if( pCalleeIdInfo != NULL )
        {
            sprintf( lpszSendBuffer, "%d|%s|%s|%s|%s|%s", iCmd, pIVRInfo->szIVRExt, pIVRInfo->szCaller,
                     pCalleeIdInfo->szCalleeId, pIVRInfo->szUUID, "" );

            m_CalleeIdInfoMap.Remove( UUID );

            delete pCalleeIdInfo;
            pCalleeIdInfo = NULL;
        }
        else
        {
            sprintf( lpszSendBuffer, "%d|%s|%s|%s|%s|%s", iCmd, pIVRInfo->szIVRExt, pIVRInfo->szCaller,
                     pIVRInfo->szIVRExt, pIVRInfo->szUUID, "" );
        }

        char szLog[1024] = {0};
        sprintf( szLog, "------ivr_ringing_event------- hase occurred, lpszSendBuffer = %s", lpszSendBuffer );
        TraceEx( szLog );

        SendCallEventToClient( pIVRInfo->szIVRExt, device_type, iCmd, SNT_EVENT, lpszSendBuffer, strlen(lpszSendBuffer) );
    }
    else
    {
        TraceEx( "error, can not find idle ive channel now...");
    }
}

IVR_INFO *CDataProc::get_idle_ivr_channel()
{
    IVR_INFO *pIVRInfo = NULL;

    for(int i = 0; i < m_nIvrNum; i++)
    {
        if( m_pIVRInfo[i].iRunningFlag == 0 )
        {
            pIVRInfo = &m_pIVRInfo[i];
            break;
        }
    }

    return pIVRInfo;
}

IVR_INFO *CDataProc::get_ivr_info_by_uuid(char *lpszUUID)
{
    IVR_INFO *pIVRInfo = NULL;
    if( lpszUUID == NULL )
        return pIVRInfo;

    for(int i = 0; i < m_nIvrNum; i++)
    {
        if( m_pIVRInfo[i].iRunningFlag == 1 && strcmp( m_pIVRInfo[i].szUUID, lpszUUID) == 0 )
        {
            pIVRInfo = &m_pIVRInfo[i];
            break;
        }
    }

    return pIVRInfo;
}

bool CDataProc::is_ivr_running(char *lpszUUID, char *ivr_device)
{
    bool bResult = false;
    if( lpszUUID == NULL || ivr_device == NULL )
        return bResult;

    for(int i = 0; i < m_nIvrNum; i++)
    {
        if( m_pIVRInfo[i].iRunningFlag == 1 && strcmp( m_pIVRInfo[i].szUUID, lpszUUID) == 0 )
        {
            strcpy( ivr_device, m_pIVRInfo[i].szIVRExt );
            bResult = true;
            break;
        }
    }

    return bResult;
}

void CDataProc::channel_ivr_hangup_event_proc(int iCount, string *strInfo)
{
    string UUID = GetHeader(iCount, strInfo, "UUID: " );
    IVR_INFO *pIVRInfo = get_ivr_info_by_uuid( (char *)UUID.c_str());
    if( pIVRInfo != NULL )
    {
        pIVRInfo->iRunningFlag = 0;
        memset( pIVRInfo->szCaller, 0, 64 );
        memset( pIVRInfo->szUUID, 0, 64 );

        char lpszSendBuffer[1024*2] = {0};
        sprintf( lpszSendBuffer, "%s|%s", pIVRInfo->szIVRExt, (char *)UUID.c_str() );

        SendCallEventToClient( pIVRInfo->szIVRExt, device_type, CMD_EXTHANGUP_EVENT, SNT_EVENT, lpszSendBuffer, strlen(lpszSendBuffer) );

        char szLog[1024] = {0};
        sprintf( szLog, "notify, ---------ivr_hangup_event-------- has occured, lpszSendBuffer = %s", lpszSendBuffer );
        TraceEx( szLog );
    }
    else
    {
        char szLog[1024] = {0};
        sprintf( szLog, "error, ivr_hangup_event can not find pIVRInfo..., uuid = %s", (char *)UUID.c_str() );
        TraceEx( szLog );
    }
}

void CDataProc::channel_dtmf_event_proc(int iCount, string *strInfo)
{
    string DTMF_Digit = "";
    string uuid = GetHeader(iCount, strInfo, "Unique-ID: ");
    DTMF_Digit = GetHeader(iCount, strInfo, "DTMF-Digit: ");
    string DTMF_Source= GetHeader(iCount, strInfo, "DTMF-Source: ");

    char msg_log[1024] = {0};
    sprintf( msg_log, "dtmf_event, dtmf_source = %s, dtmf_value = %s", (char *)DTMF_Source.c_str(), (char *)DTMF_Digit.c_str() );
    TraceEx( msg_log );

    if (DTMF_Source != "APP")
    {
        IVR_INFO *pIVRInfo = get_ivr_info_by_uuid( (char *)uuid.c_str());
        if (pIVRInfo != NULL)
        {
            //if(GetHeader(iCount, strInfo, "DTMF-Source: ") == "RTP")
            if(DTMF_Source == "RTP" || DTMF_Source == "INBAND_AUDIO")
            {
                char buf[1024*2] = {0};

                char szLog[1024] = {0};

                sprintf( szLog, "dtmf_len = %d, dtmf_value = %s", strlen( (char *)DTMF_Digit.c_str() ), (char *)DTMF_Digit.c_str() );
                TraceEx( szLog );

                if( strlen( (char *)DTMF_Digit.c_str() ) != 1 || DTMF_Digit == "" )
                {
                    DTMF_Digit = "#"; //add by liu, 2017-12-20 11:55
                    TraceEx( "set dtmf_digt = #");
                }

                sprintf( buf, "%s|%d|%s", pIVRInfo->szIVRExt, pIVRInfo->iChannelId, (char *)DTMF_Digit.c_str() );

                SendCallEventToClient( pIVRInfo->szIVRExt, multi_media_type, CMD_RECV_DTMF, SNT_EVENT, buf, strlen(buf) );

                memset( szLog, 0, 1024 );
                sprintf( szLog, "notify, ---------ivr_dtmf_event-------- has occured--, buf = %s", buf );
                TraceEx( szLog );
            }
        }
        else
        {
            TraceEx( "error, dtmf_event, can not find ivr_info");
        }

    }
}

void CDataProc::channel_record_stop_event_proc(int iCount, string *strInfo)
{
    //DN|Ch
    string uuid = GetHeader(iCount, strInfo, "Unique-ID: ");
    IVR_INFO *pIVRInfo = get_ivr_info_by_uuid( (char *)uuid.c_str());
    if (pIVRInfo != NULL)
    {
        char buf[1024] = {0};
        sprintf( buf, "%s|%d", pIVRInfo->szIVRExt, pIVRInfo->iChannelId );

        SendCallEventToClient( pIVRInfo->szIVRExt, multi_media_type, CMD_END_RECORD, SNT_EVENT, buf, strlen(buf) );

        char szLog[1024] = {0};
        sprintf( szLog, "notify, ---------ivr_record_stop_event-------- has occured, buf = %s", buf );
        TraceEx( szLog );
    }
    else
    {
        TraceEx( "error, ivr_record_stop_event can not find pIVRInfo...");
    }
}

void CDataProc::channel_playback_stop_event_proc(int iCount, string *strInfo)
{
    //DN|Ch|reasonCode
    //reasonCode的定义如下：
    //1：正常结束放音。
    //2：接收到DTMF打断放音

    string uuid = GetHeader(iCount, strInfo, "Unique-ID: ");
    string Playback_Status = GetHeader(iCount, strInfo, "Playback-Status: ");
    int reasonCode = 0;
    if (Playback_Status == "done")
    {
        reasonCode = 1;
    }
    else
    {
        reasonCode = 2;
    }

    IVR_INFO *pIVRInfo = get_ivr_info_by_uuid( (char *)uuid.c_str());
    if (pIVRInfo != NULL)
    {
        char buf[1024] = {0};
        sprintf( buf, "%s|%d|%d", pIVRInfo->szIVRExt, pIVRInfo->iChannelId, reasonCode );

        SendCallEventToClient( pIVRInfo->szIVRExt, multi_media_type, CMD_END_PLAY, SNT_EVENT, buf, strlen(buf) );

        char szLog[1024] = {0};
        sprintf( szLog, "notify, ---------ivr_playback_stop_event-------- has occured, buf = %s", buf );
        TraceEx( szLog );
    }
    else
    {
        TraceEx( "error, ivr_playback_stop_event can not find pIVRInfo...");
    }
}

IVR_INFO *CDataProc::get_ivr_info_by_ch(int iCh)
{
    IVR_INFO *pIVRInfo = NULL;
    if( iCh < 0 || iCh >= m_nIvrNum )
        return pIVRInfo;

    pIVRInfo = &m_pIVRInfo[iCh];
    return pIVRInfo;
}

IVR_INFO *CDataProc::get_ivr_info_by_caller(char *caller)
{
    IVR_INFO *pIVRInfo = NULL;
    if( caller == NULL )
        return pIVRInfo;

    for(int i = 0; i < m_nIvrNum; i++)
    {
        if( m_pIVRInfo[i].iRunningFlag == 1 && strcmp( m_pIVRInfo[i].szCaller, caller) == 0 )
        {
            pIVRInfo = &m_pIVRInfo[i];
            break;
        }
    }

    return pIVRInfo;
}

IVR_INFO *CDataProc::get_ivr_info_by_device(char *device, bool bNeedJudgeRunningFlag)
{
    IVR_INFO *pIVRInfo = NULL;
    if( device == NULL )
        return pIVRInfo;

    for(int i = 0; i < m_nIvrNum; i++)
    {
        if( bNeedJudgeRunningFlag )
        {
            if( m_pIVRInfo[i].iRunningFlag == 1 && strcmp( m_pIVRInfo[i].szIVRExt, device) == 0 )
            {
                pIVRInfo = &m_pIVRInfo[i];
                break;
            }
        }
        else
        {
            if( strcmp( m_pIVRInfo[i].szIVRExt, device) == 0 )
            {
                pIVRInfo = &m_pIVRInfo[i];
                break;
            }
        }
    }

    return pIVRInfo;
}

bool CDataProc::IsClientIdAlreadyExist(int iClientId, int iClientType, char *device)
{
    bool bResult = false;
    if( device == NULL )
        return bResult;
    if( m_pDevice_Sock_List == NULL )
    {
        m_pDevice_Sock_List = new list<DEVICE_SOCK_INFO *>;
        if( m_pDevice_Sock_List == NULL )
        {
            TraceEx( "error, is_client_id_already_exist, new malloca memory failed");
            return bResult;
        }
    }

    if( m_pDevice_Sock_List->size() == 0 )
        return bResult;

    list<DEVICE_SOCK_INFO *>::iterator it;
    for(it = m_pDevice_Sock_List->begin(); it != m_pDevice_Sock_List->end(); ++it)
    {
        DEVICE_SOCK_INFO *info = *it;
        if( info != NULL )
        {
            if( strcmp( info->szDevice, device ) == 0 && info->iClientType == iClientType
                    && info->iClientId == iClientId )
            {
                bResult = true;
                break;
            }
        }
    }

    return bResult;
}

void CDataProc::member_queue_start_event_proc(int iCount, string *strInfo)
{
    string uuid = GetHeader(iCount, strInfo, "CC-Member-Session-UUID: " );
    string callerid = GetHeader(iCount, strInfo, "CC-Member-CID-Number: ");
    string QueueName = GetHeader(iCount, strInfo, "CC-Queue: ");
    string numberQueue = GetHeader(iCount, strInfo, "CC-Count: ");
    string Caller_Destination_Number = GetHeader(iCount, strInfo, "Caller-Destination-Number: ");
    //MemberQueueStart(QueueNum, UUID, callerid, CC_Count, Caller_Destination_Number);

    //buf = member-queue-start|queue|UUID|callerid|calledid|numberQueue

    char lpszSendBuffer[1024*2] = {0}, queue_ext[32] = {0};
    strcpy( queue_ext, "5001" );

    int iCmd = CMD_QUEUE_IN;

    sprintf( lpszSendBuffer, "member-queue-start|%s|%s|%s|%s|%s", (char *)QueueName.c_str(), (char *)uuid.c_str(),
             (char *)callerid.c_str(), (char *)Caller_Destination_Number.c_str(), (char *)numberQueue.c_str() );

    SendCallEventToClient( queue_ext, device_type, iCmd, SNT_EVENT, lpszSendBuffer, strlen(lpszSendBuffer) );

    char szLog[1024*2]  = {0};
    sprintf( szLog, "in_queue_event, buf = %s", lpszSendBuffer );
    TraceEx( szLog );
}

void CDataProc::member_queue_end_event_proc(int iCount, string *strInfo)
{
    //buf = member-queue-end|queue|UUID|callerid|calledid|numberQueu

    string uuid = GetHeader(iCount, strInfo, "CC-Member-Session-UUID: ");
    string callerid = GetHeader(iCount, strInfo, "CC-Member-CID-Number: ");
    string QueueName = GetHeader(iCount, strInfo, "CC-Queue: ");
//    string Caller_Destination_Number = GetHeader(iCount, strInfo, "Caller-Destination-Number: ");

    string numberQueue = GetHeader(iCount, strInfo, "CC-Count: ");

    string DN = GetHeader(iCount, strInfo, "Other-Leg-Destination-Number: ");
    string Cause = GetHeader(iCount, strInfo, "CC-Cause: ");

//    if( Cause == "Cancel" )
//    {
        char lpszSendBuffer[1024*2] = {0}, queue_ext[32] = {0};
        strcpy( queue_ext, "5001" );

        int iCmd = CMD_QUEUE_OUT;

        sprintf( lpszSendBuffer, "member-queue-end|%s|%s|%s|%s|%s", (char *)QueueName.c_str(), (char *)uuid.c_str(),
                 (char *)callerid.c_str(), (char *)DN.c_str(), (char *)numberQueue.c_str() );

        SendCallEventToClient( queue_ext, device_type, iCmd, SNT_EVENT, lpszSendBuffer, strlen(lpszSendBuffer) );

//    }

        char szLog[1024*2]  = {0};
        sprintf( szLog, "out_queue_event, buf = %s", lpszSendBuffer );
        TraceEx( szLog );
}

void CDataProc::get_asr_result_proc(char *data, string &strRequestId, int &iSentenceId, int &iBeginTime, int &iCurrentTime, string &strText)
{
    if( data == NULL )
        return;

    QByteArray byteArray = data;
    QJsonParseError jsonError;
    QJsonDocument doucment = QJsonDocument::fromJson(byteArray, &jsonError);  // 转化为 JSON 文档
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError))
    {  // 解析未发生错误
        if (doucment.isObject())
        {  // JSON 文档为对象
            QJsonObject object = doucment.object();  // 转化为对象
            if (object.contains("request_id"))
            {
                QJsonValue value = object.value("request_id");
                if (value.isString())
                {
                    strRequestId = value.toString().toStdString();
                }
            }

            if (object.contains("result"))
            {
                QJsonValue value = object.value("result");
                if (value.isObject())
                {  // result 的 value 是对象
                    QJsonObject obj = value.toObject();
                    if (obj.contains("sentence_id"))
                    {
                        QJsonValue value = obj.value("sentence_id");
                        if (value.isString())
                        {
                            iSentenceId = value.toInt();
                        }
                    }

                    if (obj.contains("begin_time"))
                    {
                        QJsonValue value = obj.value("begin_time");
                        if (value.isString())
                        {
                            iBeginTime = value.toInt();

                        }
                    }

                    if (obj.contains("current_time"))
                    {
                        QJsonValue value = obj.value("current_time");
                        if (value.isString())
                        {
                            iCurrentTime = value.toInt();
                        }
                    }

                    if (obj.contains("text"))
                    {
                        QJsonValue value = obj.value("text");
                        if (value.isString())
                        {
                            strText = value.toString().toStdString();
                        }
                    }

                }
            }
        }
    }
    else
    {
        TraceEx( "---------get_asr_result-----can not process json-------");
    }
}

void CDataProc::utf8_to_gbk(char *lpszInputData, char *lpszOutputData)
{
    if( lpszInputData == NULL || lpszOutputData == NULL )
        return;

    QTextCodec *utf8 = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(utf8);
 //   QTextCodec::setCodecForStrings(utf8);

    QTextCodec* gbk = QTextCodec::codecForName("gbk");

    QString strInput= lpszInputData;

    //utf8 -> gbk
    //1. utf8 -> unicode
    QString strUnicode= utf8->toUnicode(strInput.toLocal8Bit().data());
    //2. unicode -> gbk, 得到QByteArray
    QByteArray gb_bytes= gbk->fromUnicode(strUnicode);
    lpszOutputData =  gb_bytes.data(); //获取其char *
}

void CDataProc::channel_asr_event_proc(int iCount, string *strInfo, char *data )
{

    char szLog[1024*20]  = {0};

    TraceEx( "-----------channel_asr_event_proc-------" );

    string Channel_Name = GetChannelName(iCount, strInfo, "Channel: ");
    sprintf( szLog, "channel_name = %s", (char *)Channel_Name.c_str());
    TraceEx( szLog );

    if( Channel_Name.find( "loopback/") != string::npos )
    {
        Channel_Name = Channel_Name.replace( 0, strlen( "loopback/"), "");

        int nPos = Channel_Name.find( "-a");
        if( nPos != string::npos )
        {
            Channel_Name = Channel_Name.substr( 0, nPos );
        }
    }

    string ASR_Response = GetHeader(iCount, strInfo, "ASR-Response: ");

    CUrlConvert url_decoder;
    string strASRDecoder = url_decoder.UrlDecode_GBK( ASR_Response );

    string strRequestId = "", strText = "";
    int iSentenceId = 0, iBeginTime = 0, iCurrentTime = 0;

    get_asr_result_proc( (char *)strASRDecoder.c_str(), strRequestId, iSentenceId, iBeginTime, iCurrentTime, strText );

    memset( szLog, 0, 1024*20 );
    sprintf( szLog, "ASR_response, Channel_Name = %s, request_id = %s, iSentence_id = %d, iBeginTime = %d, iCurrentTime = %d, strText = %s",
             (char *)Channel_Name.c_str(), (char *)strRequestId.c_str(), iSentenceId, iBeginTime, iCurrentTime, (char *)strText.c_str()  );

    TraceEx( szLog );

    IVR_INFO *pIVRInfo = get_ivr_info_by_caller( (char *)Channel_Name.c_str() );
    if( pIVRInfo == NULL )
    {
        memset( szLog, 0, 1024*20 );
        sprintf( szLog, "error, asr_response, can not find ivr_info, caller = %s", (char *)Channel_Name.c_str() );
        TraceEx( szLog );
        return;
    }

    //send ivr asr event now...
    char lpszSendBuffer[1024*2] = {0};

    int iCmd = CMD_ASR_RESPONSE_EVENT;
    sprintf( lpszSendBuffer, "%d|%s|%s|%s|%s|%s", iCmd, pIVRInfo->szIVRExt, (char *)strText.c_str(),
             pIVRInfo->szIVRExt, pIVRInfo->szUUID, "" );

    memset( szLog, 0, 1024*20 );
    sprintf( szLog, "------ivr_asr_event------- hase occurred, lpszSendBuffer = %s", lpszSendBuffer );
    TraceEx( szLog );

    SendCallEventToClient( pIVRInfo->szIVRExt, multi_media_type, iCmd, SNT_EVENT, lpszSendBuffer, strlen(lpszSendBuffer) );

}
