#include "DataProc.h"
#include "AutoLock.h"
#include "GlobalTsapiServer.h"
#include "mainentry.h"

#ifdef _windows_
#else
//#include <sys/socket.h>
#endif

extern void TraceEx( char *buf, ...);

extern CMainEntry m_MainEntry;

void *DownloadFolderThread(void *lpVoid)
{
    char *lpszBuffer = (char *)lpVoid;
    if( lpszBuffer == NULL )
        return 0;

    PACKET_HEAD *pHead = (PACKET_HEAD *)lpszBuffer;
    if( pHead == NULL )
        return 0;

    DOWNLOAD_FILE_INFO *pDownload_Info = (DOWNLOAD_FILE_INFO *)(lpszBuffer + sizeof(PACKET_HEAD));
    if( pDownload_Info == NULL )
        return 0;

    DOWNLOAD_DATA_INFO *pDownload_Data_Info = (DOWNLOAD_DATA_INFO *)(lpszBuffer + sizeof(PACKET_HEAD) + sizeof(DOWNLOAD_FILE_INFO) );
    if( pDownload_Data_Info == NULL )
        return 0;

    int iClientId = 0;
    memcpy( &iClientId, lpszBuffer + sizeof(PACKET_HEAD) + pHead->dwSize, sizeof(int) );

    char szLog[1024] = {0};
    int iTotalDataNum = 1;
    for(int i = 0; i < iTotalDataNum; i++)
    {
        sprintf( szLog, "notify, down_load_folder_thread, lpszRemoteFilePathName = %s", pDownload_Data_Info->lpszRemoteFilePathName );
        TraceEx( szLog );

        memset( pDownload_Data_Info->lpszRemoteFileFolder, 0, MAX_PATH);
        strcpy( pDownload_Data_Info->lpszRemoteFileFolder, pDownload_Data_Info->lpszRemoteFilePathName );

        pDownload_Info->iOrientPos = 0; //借用此字段，作为文件夹下文件的总数量
        m_MainEntry.m_DataProc.FindFile(  iClientId, pHead, pDownload_Info, pDownload_Data_Info, pDownload_Data_Info->lpszRemoteFileFolder );

        //文件夹下文件名传输完毕
        char szTotalFileNum[32] = {0};
        sprintf( szTotalFileNum, "%d", pDownload_Info->iOrientPos );
        m_MainEntry.m_DataProc.Send_command( iClientId,  CMD_DOWNLOAD_FILE_FIND_END, RC_EVENT, szTotalFileNum, strlen( szTotalFileNum ) );

        sprintf( szLog, "send comd_download_file_end to client, iClientId = %d, szTotalFileNum = %s", iClientId, szTotalFileNum );
        TraceEx( szLog );

    }

    delete[] lpszBuffer;
    lpszBuffer = NULL;

    return 0;
}

CMainDataProc::CMainDataProc()
{
}

CMainDataProc::~CMainDataProc()
{
	m_RegExtList.clear();

#ifdef _VC6_
	DeleteCriticalSection( &m_MoniCS );
#endif
}

void CMainDataProc::DataProc(int iClientId, char *lpszBuffer)
{
	try
	{
	if( iClientId < 0 || lpszBuffer == NULL )
		return;
	PACKET_HEAD *pHead = (PACKET_HEAD *)lpszBuffer;

    string strInfo[16] = {""};
	DecodeData( lpszBuffer, strInfo );

	switch(pHead->cmd)
	{
		case CMD_REGISTERDN:
			Cmd_ExtRegister_Proc(iClientId,lpszBuffer, strInfo);
			break;
		case CMD_AGENTLOGON:
		case CMD_AGENTLOGOFF:
		case CMD_AGENTREADY:
		case CMD_AGENTNOTREADY:
			Cmd_AgentOper_Proc(iClientId, pHead, lpszBuffer, strInfo);
			break;
		case CMD_SENDMSGTOEXT:
		case CMD_SENDMSG2ALL:
            Cmd_Sendmsg_Proc(iClientId,lpszBuffer,strInfo, pHead->cmd==CMD_SENDMSG2ALL?true:false);
			break;
		case CMD_HOLDCALL:
			Cmd_ExtHoldCall_Proc(iClientId, lpszBuffer, strInfo);
			break;
		case CMD_RETRIEVECALL:
			Cmd_ExtRetrieveCall_Proc(iClientId, lpszBuffer, strInfo);
			break;
		case CMD_PICKUP:
			Cmd_ExtPickup_Proc(iClientId, lpszBuffer, strInfo);
			break;
		case CMD_HANGUP:
			Cmd_ExtHangup_Proc(iClientId, lpszBuffer, strInfo);
			break;
		case CMD_MAKECALL:
			Cmd_MakeCall_Proc(iClientId, lpszBuffer, strInfo);
			break;
		case CMD_INITTRANS:
			Cmd_InitTrans_Proc(iClientId, lpszBuffer, strInfo);
			break;
		case CMD_COMPLETETRANS:
			Cmd_CompleteTrans_Proc(iClientId, lpszBuffer, strInfo);
			break;
		case CMD_COMPLETECONF:
			Cmd_CompleteConf_Proc(iClientId, lpszBuffer, strInfo);
			break;
		case CMD_LISTEN:
			Cmd_ListenExt_Proc(iClientId, lpszBuffer, strInfo);
			break;
		case CMD_INTRUDE:
			Cmd_IntrudeExt_Proc(iClientId, lpszBuffer, strInfo);
			break;
		case CMD_FORCECH:
			break;
		case CMD_UNREGISTERDN:
			Cmd_UnregisterExt_Proc(iClientId, lpszBuffer, strInfo);
			break;
		case CMD_SETTRANSDATA:
			Cmd_SetTransData_Proc(iClientId, lpszBuffer, strInfo);
			break;
		case CMD_GETTRANSDATA:
			Cmd_GetTransData_Proc(iClientId, lpszBuffer, strInfo);
			break;
		case CMD_HEARTBEAT:
			Cmd_HeartBeat_Proc(iClientId, lpszBuffer);
			break;
		case CMD_SENDFAX:
			Cmd_SendFax_Proc(iClientId, lpszBuffer);
			break;
		case CMD_SETOTHEREVENT:
			Cmd_SetExtNoEvent_Proc(iClientId, lpszBuffer, strInfo);
			break;
		case CMD_REQUERYFILEVER:
			Cmd_QueryVersion(iClientId, lpszBuffer, strInfo);
			break;
		case CMD_DOWNLOADFILE:
            Cmd_Download_File_Proc(iClientId, lpszBuffer);
//			Cmd_DownloadStart(iClientId, lpszBuffer, strInfo);
			break;
        case CMD_REQUEST_DOWNLOAD_FOLDER:
            Cmd_Request_Download_Folder_Proc(iClientId, lpszBuffer);
            break;
        case CMD_START_RECORD:
            Cmd_Start_Record_Proc(iClientId, lpszBuffer, strInfo);
            break;
        case CMD_STOP_RECORD:
            Cmd_Stop_Record_Proc(iClientId, lpszBuffer, strInfo);
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

void CMainDataProc::OnClientClose(int iClientId)
{
	try
	{
	if( iClientId <= 0 )
		return;

	PAGENTTYPE pAgentType = GetAgentType( iClientId );
	if( pAgentType != NULL )
	{
		RemoveFromAgentTypeList( iClientId, pAgentType->iAgentType );
		memset( pAgentType, 0, sizeof(AGENTTYPE) );
	}

	list<PSOCKEXT> *plist = m_SockExtListMap.Find( iClientId );
	if( plist != NULL )
	{
		list<PSOCKEXT>::iterator it;
		for( it = plist->begin(); it != plist->end(); ++it )
		{
			PSOCKEXT pSockExt = *it;
			if( pSockExt != NULL )
			{
				char *lpszExtNo = NULL;
				lpszExtNo = (char *)pSockExt->strExtNo.c_str();
				if( lpszExtNo != NULL )
				{
					m_ExtSockMap.Remove( lpszExtNo );

					DelExtInfo( lpszExtNo );

					if( lpszExtNo[0] == 'v' || lpszExtNo[0] == 'V' )
					{
                        if( m_MainEntry.m_iCTIType == trans_station_cti_type )
                        {
                            SendLogoutCmdToVSQL( iClientId, lpszExtNo );
                        }
					}
					else
					{
						m_iCurrentClientNum--;
						if( m_iCurrentClientNum <= 0 )
							m_iCurrentClientNum = 0;
						if( strlen( m_szQueueExt ) > 0 )
							SendLogoutCmd( iClientId, lpszExtNo, "VServer" );
					}
                    char szLog[256] = {0};
                    sprintf( szLog, "OnClientClose, remove extno = %s, m_iCurrentClientNum = %d",
                             lpszExtNo, m_iCurrentClientNum );
                    TraceEx( szLog );
				}

				delete pSockExt;
				pSockExt = NULL;
			}
			
		}

		plist->clear();

		delete plist;
		plist = NULL;

		m_SockExtListMap.Remove( iClientId );
	}

	DelFromListCtrl(iClientId );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::on_client_close() " );
	}
}

void CMainDataProc::Cmd_AgentOper_Proc(int iClientId, PACKET_HEAD *pHead, char *buf, string *strInfo)
{
	try
	{
    m_MainEntry.m_CtiCtrl.CstaSetAgentState( pHead->cmd, (char *)strInfo[0].c_str(), (char *)strInfo[1].c_str(),
            (char *)strInfo[2].c_str(), (char *)strInfo[3].c_str(), iClientId );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_agent_oper_proc() " );
	}
}

void CMainDataProc::Cmd_CompleteConf_Proc(int iClientId, char *buf, string *strInfo)
{
	try
	{
    m_MainEntry.m_CtiCtrl.CstaConferenceCall(iClientId, m_ExtInfoMap.Find( (char *)strInfo[0].c_str() ) );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_complete_conf_proc() " );
	}
}

void CMainDataProc::Cmd_CompleteTrans_Proc(int iClientId, char *buf, string *strInfo)
{
	try
	{
    m_MainEntry.m_CtiCtrl.CstaTransferCall(iClientId, m_ExtInfoMap.Find( (char *)strInfo[0].c_str() ) );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_complete_trans_proc() " );
	}
}

void CMainDataProc::Cmd_ExtHangup_Proc(int iClientId, char *buf, string *strInfo)
{
	try
	{
    m_MainEntry.m_CtiCtrl.CstaClearConnection( iClientId, m_ExtInfoMap.Find( (char *)strInfo[0].c_str() ), (char *)strInfo[1].c_str() );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_exthangup_proc() " );
	}
}

void CMainDataProc::Cmd_ExtHoldCall_Proc(int iClientId, char *buf, string *strInfo)
{
	try
	{
    m_MainEntry.m_CtiCtrl.CstaHoldCall( iClientId, m_ExtInfoMap.Find( (char *)strInfo[0].c_str() ) );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_ext_hold_call_proc() " );
	}
}

void CMainDataProc::Cmd_ExtPickup_Proc(int iClientId, char *buf, string *strInfo)
{
	try
	{
    m_MainEntry.m_CtiCtrl.CstaAnswerCall( iClientId, m_ExtInfoMap.Find( (char  *)strInfo[0].c_str() ) );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_ext_pickup_proc() " );
	}
}

void CMainDataProc::Cmd_ExtRegister_Proc(int iClientId, char *buf, string *strInfo)
{
	try
	{
    char szLog[512] = {0};
    if( !m_bSeriCorrect )
    {
        sprintf( szLog, "error, register notify,  m_bSeriCorrect = %d", m_bSeriCorrect );
        TraceEx( szLog );
        return;
    }

	if( !IsRegisterContinue( iClientId ) )
		return;

	PACKET_HEAD *pHead = (PACKET_HEAD *)buf;

	char szExtNo[32] = {0};
    strcpy( szExtNo, (char *)strInfo[0].c_str() );

    int iClientType = atoi( (char *)strInfo[1].c_str() );
    int iWSType = atoi( (char *)strInfo[2].c_str() );

    sprintf( szLog, "TsapiServer, Cmd_ExtRegister_Proc, iClientId = %d, szExtNo = %s, iType = %d, iWSType = %d",
             iClientId, szExtNo, iClientType, iWSType );

    TraceEx( szLog );

	if( strlen( szExtNo ) == 0 )
	{
		TraceEx( "error, cmd_reigster_proc failed and not register and return now, iClientId = %d, szExtNo = %s, iType = %d",
			iClientId, szExtNo, iClientType );
		return;
	}

	AddAgentType( iClientId, iClientType, szExtNo );

    bool bOk = false, bSameExt = false, bVExt = false, bRegister = false;

	int client_id = CheckTheSameExtReged( szExtNo, iClientType );
	if( client_id > 0 )
	{
		if( iClientType == mutl_agent_type )
		{
            sprintf( szLog, "warnning, same ext in line now and not register, szExtNo = %s, inner_client_id = %d, iClientId = %d, iType = %d",
                     szExtNo, client_id, iClientId, iClientType );
            TraceEx( szLog );
			return;
		}

		PAGENTTYPE agent_type = GetAgentType(client_id);
		if( agent_type != NULL && agent_type->iAgentType == mutl_agent_type )
		{
			//unregister the ext
            m_MainEntry.m_CtiCtrl.CstaMonitorStop(m_ExtInfoMap.Find( szExtNo ) );

			RemoveExtFromVServer( szExtNo, client_id );
            usleep( 100*1000 );
			ReloadListCtrl( client_id );

            bRegister = true;

            sprintf( szLog, "notify, csta_monitor_stop ext = %s, client_id = %d, iAgentType = %d,request_client_id = %d, request_client_type = %d",
                     szExtNo, client_id, agent_type->iAgentType, iClientId, iClientType );
            TraceEx( szLog );
		}
		else
		{
            bRegister = false;
            bSameExt = true;
			pHead->cmd_type = RC_RESPONSE_FAILED;
            sprintf( szLog, "warnning, same extno in line now, szExtNo = %s, bSameExt = %d", szExtNo, bSameExt );
            TraceEx( szLog );
		}
	}
	else
	{
        bRegister = true;
	}

	if( bRegister )
	{
		if( szExtNo[0] == 'v' || szExtNo[0] == 'V' )
		{
            bOk = true;
            bVExt = true;
			pHead->cmd_type = RC_RESPONSE_OK;

            m_ExtSockMap.Add(szExtNo, iClientId);
            if( bVExt )
            {
                AddToExtSock( szExtNo, iClientId );
                m_MainEntry.SendToClientFunc( iClientId, (char *)pHead, sizeof(PACKET_HEAD) + pHead->dwSize );
            }
		}
		else
		{
			if( IsBeyondLinsenceNum() )
			{
				//超过lisence限制
				pHead->cmd_type = RC_REG_EXT_BEYOND_LISENCE;//RC_RESPONSE_FAILED;
			}
			else
			{
                m_RegExtList.push_back( szExtNo );

                m_ExtSockMap.Add(szExtNo, iClientId);

                m_ExtFlagMap.Remove(szExtNo);
                int iFirstRegister = 1;
                m_ExtFlagMap.Add(szExtNo, iFirstRegister);
                sprintf( szLog, "m_RegExtList.size() = %d, reg_ext = %s, csta_monitor_device success",
                         m_RegExtList.size(), szExtNo );
                TraceEx( szLog );

                if( m_MainEntry.m_CtiCtrl.CstaMonitorDevice( szExtNo ) >= 0 )
				{
                    bOk = true;
				}
				else
				{
                    //分机注册失败
					pHead->cmd_type = RC_REG_EXT_FAILED;//RC_RESPONSE_FAILED;
					m_RegExtList.pop_back();
                    m_ExtFlagMap.Remove(szExtNo);
				}
			}
		}
	}

	if( !bOk )
	{
        sprintf( szLog, "TsapiServer, Cmd_ExtRegister_Proc, register failed, bOk = %d, iClientId = %d, szExtNo = %s",
                 bOk, iClientId, szExtNo );
        TraceEx( szLog );

        m_MainEntry.SendToClientFunc( iClientId, (char *)pHead, sizeof(PACKET_HEAD) + pHead->dwSize );

		if( bSameExt )
		{
            if( iWSType != ws_type )
            {
                m_MainEntry.CloseSockFunc( iClientId );
                sprintf( szLog, "CloseSockFunc now, iClientId = %d", iClientId );
                TraceEx( szLog );
            }
		}
	}
	else
	{
        /*
        sprintf( szLog,  "&&&&&&&&&&&&& begin to add map, ext = %s, client_id = %d",szExtNo, iClientId );
        TraceEx( szLog );
		m_ExtSockMap.Add(szExtNo, iClientId);
		if( bVExt )
		{
			AddToExtSock( szExtNo, iClientId );
            m_MainEntry.SendToClientFunc( iClientId, (char *)pHead, sizeof(PACKET_HEAD) + pHead->dwSize );
		}
        */
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_ext_register_proc() " );
	}
}

void CMainDataProc::Cmd_ExtRetrieveCall_Proc(int iClientId, char *buf, string *strInfo)
{
	try
	{
    m_MainEntry.m_CtiCtrl.CstaRetrieveCall(iClientId, m_ExtInfoMap.Find( (char *)strInfo[0].c_str() ) );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_ext_retrieve_call_proc() " );
	}
}

void CMainDataProc::Cmd_GetTransData_Proc(int iClientId, char *buf, string *strInfo)
{
	try
	{
    char szLog[256] = {0};
    sprintf( szLog, "Cmd_GetTransData_Proc, iClientId = %d, buf = %s, strInfo[0] = %s", iClientId, buf, (char *)strInfo[0].c_str() );
    TraceEx( szLog );

	char szInBuff[1024*100] = {0};
	char *lpszExtData = NULL;
    lpszExtData = m_ExtDataMap.Find( atoi( (char *)strInfo[0].c_str() ) );
	if( lpszExtData != NULL )
	{	
        string strText = lpszExtData;
        if( strText.length() > 0 )
		{
            sprintf(szInBuff,"%s|%s", (char *)strInfo[0].c_str(), (char *)strText.c_str());

			Send_command(iClientId, CMD_RETTRANSDATA_EVENT, RC_EVENT, szInBuff, strlen(szInBuff));

			TraceEx( "Cmd_GetTransData_Proc, %s", szInBuff );
		}
		else
		{
            TraceEx( "Cmd_GetTransData_Proc, strText.GetLength()  = 0, strText = %s", (char *)strText.c_str() );

            Send_command(iClientId, CMD_RETTRANSDATA_EVENT, RC_EVENT, (char *)strInfo[0].c_str(), strInfo[0].length() );
		}
	}
	else
	{
        Send_command(iClientId, CMD_RETTRANSDATA_EVENT, RC_EVENT, (char *)strInfo[0].c_str(), strInfo[0].length() );
		TraceEx( "Cmd_GetTransData_Proc, lpszExtData == NULL" );
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_get_trans_data_proc() " );
	}

	/*
	char szInBuff[1024*100] = {0};
	PEXTINFO pE = m_ExtInfoMap.Find( (LPSTR &)strInfo[0] );
	if( pE == NULL )
	{
		TraceEx( "Cmd_GetTransData_Proc failed, pE == NULL" );
		return;
	}

	char *lpszExtData = NULL;
	lpszExtData = m_GlbCallIdDataMap.Find( pE->szGlobalCallid );
	if( lpszExtData != NULL )
	{	
        string strText = lpszExtData;
		if( strText.GetLength() > 0 )
		{
			sprintf(szInBuff,"%s|%s",strInfo[0],strText);

			Send_command(iClientId, CMD_RETTRANSDATA_EVENT, SNT_EVENT, szInBuff, strlen(szInBuff));

			TraceEx( "Cmd_GetTransData_Proc, %s", szInBuff );
		}
		else
		{
			TraceEx( "Cmd_GetTransData_Proc, strText.GetLength()  = 0, strText = %s", strText );

			Send_command(iClientId, CMD_RETTRANSDATA_EVENT, SNT_EVENT, (LPSTR &)strInfo[0], strInfo[0].GetLength() );
		}
	}
	else
	{
		Send_command(iClientId, CMD_RETTRANSDATA_EVENT, SNT_EVENT, (LPSTR &)strInfo[0], strInfo[0].GetLength() );
		TraceEx( "Cmd_GetTransData_Proc, lpszExtData == NULL" );
	}
	*/
}

void CMainDataProc::Cmd_InitTrans_Proc(int iClientId, char *buf, string *strInfo)
{
	try
	{
    char szLog[256] = {0};
	DeviceID_t szCalled, szOrientCalling;
	memset( szOrientCalling, 0, 64 );
    strcpy( szOrientCalling, (char *)strInfo[2].c_str() );
    strcpy(szCalled, (char *)strInfo[1].c_str());
    CopyTransferDataToDest( strInfo[0], strInfo[1] );
    sprintf( szLog, "begin to transfer, strInfo[0] = %s, strInfo[1] = %s, strInfo[2] = %s", (char *)strInfo[0].c_str(), (char *)strInfo[1].c_str(), (char *)strInfo[2].c_str() );
    TraceEx( szLog );
    m_MainEntry.m_CtiCtrl.CstaConsultationCall(iClientId, szCalled, m_ExtInfoMap.Find( (char *)strInfo[0].c_str() ), szOrientCalling );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_init_trans_proc() " );
	}
}

void CMainDataProc::Cmd_IntrudeExt_Proc(int iClientId, char *buf, string *strInfo)
{
	try
	{
    PEXTINFO pE = m_MainEntry.m_DataProc.m_ExtInfoMap.Find( (char *)strInfo[0].c_str() );

	if(pE != NULL)
	{
        PEXTINFO pEi = m_MainEntry.m_DataProc.m_ExtInfoMap.Find( (char *)strInfo[1].c_str() );
		
		if(pEi != NULL)
            m_MainEntry.m_CtiCtrl.CstaSingleStepConference(iClientId,PT_ACTIVE,pE->szExtNo,pEi);
		else
            Send_command(iClientId,CMD_INTRUDE,RC_RESPONSE_FAILED,(char *)strInfo[0].c_str(),strInfo[0].length());
	}
	else
	{
        Send_command(iClientId,CMD_INTRUDE,RC_RESPONSE_FAILED,(char *)strInfo[0].c_str(),strInfo[0].length());
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_intrude_ext_proc() " );
	}
}

void CMainDataProc::Cmd_ListenExt_Proc(int iClientId, char *buf, string *strInfo)
{
	try
	{
    PEXTINFO pE = m_ExtInfoMap.Find( (char *)strInfo[0].c_str() );

	if(pE != NULL)
	{
        PEXTINFO pEi = m_ExtInfoMap.Find( (char *)strInfo[1].c_str() );
		
		if(pEi != NULL)
            m_MainEntry.m_CtiCtrl.CstaSingleStepConference(iClientId,PT_SILENT,pE->szExtNo,pEi);
		else
            Send_command(iClientId,CMD_LISTEN,RC_RESPONSE_FAILED,(char *)strInfo[0].c_str(),strInfo[0].length());
	}
	else
	{
        Send_command(iClientId,CMD_LISTEN,RC_RESPONSE_FAILED,(char *)strInfo[0].c_str(),strInfo[0].length());
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_listen_ext_proc() " );
	}
}

void CMainDataProc::Cmd_MakeCall_Proc(int iClientId, char *buf, string *strInfo)
{
	try
	{
	DeviceID_t szCalled;
    strcpy(szCalled, (char *)strInfo[1].c_str());

    char szExtNo[64] = {0};
    if( m_MainEntry.m_iCtrlSystemFlag == 1 )
    {
        //调度系统
         strcpy( szExtNo, (char *)strInfo[0].c_str() );
    }
    else
    {
        PEXTINFO pE = m_ExtInfoMap.Find( (char *)strInfo[0].c_str() );
        if( pE == NULL )
        {
            TraceEx( "error, csta make call  failed, pE == NULL" );
            return;
        }

        strcpy( szExtNo, pE->szExtNo );
    }

    m_MainEntry.m_CtiCtrl.CstaMakeCall(iClientId, szCalled, szExtNo );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_make_call_proc() " );
	}
}

void CMainDataProc::Cmd_Sendmsg_Proc(int iClientId,char *buf, string *strInfo, bool bAll)
{
	try
	{
    char szLog[1024*100] = {0};
    sprintf( szLog, "CMainDataProc::Cmd_Sendmsg_Proc, iClientId = %d, buf = %s, strInfo[0] = %s, strInfo[1] = %s, strInfo[2] = %s, strInfo[3] = %s, strInfo[4] = %s, bAll = %d",
             iClientId, buf, (char *)strInfo[0].c_str(), (char *)strInfo[1].c_str(), (char *)strInfo[2].c_str(),
            (char *)strInfo[3].c_str(), (char *)strInfo[4].c_str(), bAll );
    TraceEx( szLog );

	char szInBuff[1024*100] = {0};

	if( bAll && strInfo[1] == "AgentStatus" )
	{
        if( m_MainEntry.m_iCTIType != trans_station_cti_type && iClientId > 0 )
        {
            sprintf(szInBuff,"%s|%s|%s|%s|%s", (char *)strInfo[0].c_str(), (char *)strInfo[0].c_str(),
                    (char *)strInfo[1].c_str(), (char *)strInfo[2].c_str(), (char *)strInfo[3].c_str());

            Send_command( iClientId, CMD_EXTENSIONMSG_EVENT, RC_EVENT, szInBuff, strlen(szInBuff) );

            sprintf( szLog, "TsapiServer, Cmd_SendMsg_Proc, szInBuff = %s, strlen(szInBuffer) = %d, iDestId = %d, ext = %s",
                     szInBuff, strlen(szInBuff), iClientId, (char *)strInfo[0].c_str() );
            TraceEx( szLog );
        }
	}

    //recv form ocx/websocket/client message
    if( m_MainEntry.m_iCTIType  == loadbalance_cti_type && iClientId > 0 )
    {
        if( strInfo[1] == "RequestAgentState" )
        {
            //请求坐席状态，直接从loadbalance_cti中获取，然后返回到client端
            PAGENT_STATE pAgentState = GetAgentStateByExt( (char *)strInfo[1].c_str() );
            if( pAgentState != NULL )
            {
                char szState[32] = {0};
                sprintf( szState, "%d", pAgentState->iCurrentState );

                sprintf(szInBuff,"%s|%s|%s|%s|%s", (char *)strInfo[1].c_str(), (char *)strInfo[0].c_str(),
                        "AgentStatus",szState, "");

                Send_command( iClientId, CMD_EXTENSIONMSG_EVENT, RC_EVENT, szInBuff, strlen(szInBuff) );
            }
            else
            {
                sprintf( szLog, "error, can not find agent_state, ext = %s", (char *)strInfo[1].c_str() );
                TraceEx( szLog );
            }

        }
        else
        {
            //直接把消息发送给trans_sation_cti中转CTI
            PACKET_HEAD *pHead = (PACKET_HEAD *)buf;
            if( pHead != NULL )
                m_MainEntry.m_ocxInterface.SendCmdFunc( pHead->cmd, buf + sizeof( PACKET_HEAD ), pHead->dwSize );
        }

        return;
    }

	if( bAll )
	{
        if( strInfo[1] == "AgentStatus" || strInfo[1] == "ACD" )
		{
            for(int i = 0; i < m_MainEntry.m_nMoniExtNum; i++)
			{
                int iDestId = m_ExtSockMap.Find( (char *)m_MainEntry.m_strMoniExt[i].c_str() );
				if( iDestId > 0 )
				{
                    sprintf(szInBuff,"%s|%s|%s|%s|%s", (char *)m_MainEntry.m_strMoniExt[i].c_str(),
                            (char *)strInfo[0].c_str(),(char *)strInfo[1].c_str(),(char *)strInfo[2].c_str(),
                            (char *)strInfo[3].c_str());

                    int iCmd_Event = CMD_EXTENSIONMSG_EVENT;
                    if( m_MainEntry.m_iCTIType == trans_station_cti_type )
                    {
                        iCmd_Event = CMD_SENDMSG2ALL;
                    }

                    Send_command( iDestId, iCmd_Event, RC_EVENT, szInBuff, strlen(szInBuff) );

				}
				else
				{

                    sprintf( szLog, "Error AgentStatus, cant find destid, ext = %s", (char *)m_MainEntry.m_strMoniExt[i].c_str() );
                    TraceEx( szLog );
				}
			}

            if( m_MainEntry.m_iCTIType != trans_station_cti_type && strInfo[1] == "AgentStatus" )
            {
                //在非中转CTI中记录下当前分机的状态
                PAGENT_STATE pAgentState = GetAgentStateByExt( (char *)strInfo[0].c_str() );
                if( pAgentState == NULL )
                {
                    pAgentState = new AGENT_STATE;
                    if( pAgentState != NULL )
                    {
                        memset( pAgentState, 0, sizeof( AGENT_STATE ) );
                        strcpy( pAgentState->szDeviceId, (char *)strInfo[0].c_str() );
                    }
                    else
                    {
                        TraceEx( "error, malloc memory agent_state failed");
                    }
                }

                if( pAgentState != NULL )
                    pAgentState->iCurrentState = atoi( (char *)strInfo[2].c_str() );

            }
			return;
		}
		else if( strInfo[1] == "RequestAgentState" )
		{
			//szInBuff = V778|7056|RequestAgentState||, strlen(szInBuffer) = 29
            bool bMoniExtension = false;
            for(int i = 0; i < m_MainEntry.m_nMoniExtNum; i++)
			{
                if( m_MainEntry.m_strMoniExt[i] == strInfo[0] )
				{
                    bMoniExtension = true;
					break;
				}
			}
			if( !bMoniExtension )	//普通坐席不请求坐席状态
				return;
		}

        if( m_SockExtListMap.Size() > 0 )
        {
            list<PSOCKEXT> *pSockExtList = m_SockExtListMap.Begin();
            while( pSockExtList != NULL )
            {
                if( pSockExtList->size() > 0 )
                {
                    list<PSOCKEXT>::iterator it;
                    for( it = pSockExtList->begin(); it != pSockExtList->end(); ++it )
                    {
                        PSOCKEXT pSockExt = *it;
                        if( pSockExt != NULL )
                        {
                            PAGENTTYPE pAgentType = GetAgentType( pSockExt->iClientId );
                            if( pAgentType != NULL && pAgentType->iAgentType == msg_engine_type )
                            {
                                //IVR 分机号不用发送
                                continue;
                            }

                            char *lpszExtNo = NULL;
                            lpszExtNo = (char *)pSockExt->strExtNo.c_str();
                            if( lpszExtNo != NULL )
                            {
                                if( strcmp( lpszExtNo , "v6001" ) == 0 )
                                {
                                    //不用发送消息给v6001
                                    continue;
                                }

                                int iCmd_Event = CMD_EXTENSIONMSG_EVENT;
                                if( m_MainEntry.m_iCTIType == trans_station_cti_type )
                                {
                                    iCmd_Event = CMD_SENDMSG2ALL;
                                }

                                bool bNeed_Send_Cmd = true;
                                if( iClientId < 0 && m_MainEntry.m_iCTIType == loadbalance_cti_type )
                                {
                                    if( strInfo[2] == "AgentLogin" || strInfo[2] == "AgentLogout" )
                                    {
                                        //iClientId = -1, buf = 3Uw, strInfo[0] = VLBCTI_4, strInfo[1] = 6004, strInfo[2] = AgentLogin, strInfo[3] = 1004, strInfo[4] = AgentReady, bAll = 1
                                        PAGENTTYPE pSrcAgentType = GetAgentTypeByExt( (char *)strInfo[1].c_str() );
                                        if( pSrcAgentType != NULL && pSrcAgentType->iClientId > 0 )
                                        {
                                            bNeed_Send_Cmd = true;
                                        }
                                        else
                                        {
                                            bNeed_Send_Cmd = false;
                                        }
                                    }

                                    sprintf(szInBuff,"%s|%s|%s|%s|%s", lpszExtNo, (char *)strInfo[1].c_str(),
                                            (char *)strInfo[2].c_str(),(char *)strInfo[3].c_str(),(char *)strInfo[4].c_str());

                                    char szTempLog[1024*100] = {0};
                                    sprintf( szTempLog, "dest_client = %d, dest_ext = %s, %s, need_send_cmd = %d",
                                             pSockExt->iClientId, lpszExtNo, szInBuff, bNeed_Send_Cmd );
                                    TraceEx( szTempLog );

                                }
                                else
                                {
                                    sprintf(szInBuff,"%s|%s|%s|%s|%s", lpszExtNo, (char *)strInfo[0].c_str(),
                                            (char *)strInfo[1].c_str(),(char *)strInfo[2].c_str(),(char *)strInfo[3].c_str());
                                }

                                if( bNeed_Send_Cmd )
                                    Send_command( pSockExt->iClientId, iCmd_Event, RC_EVENT, szInBuff, strlen(szInBuff) );

                            }
                        }
                    }
                }

                pSockExtList = m_SockExtListMap.Next();
            }
        }

	}
	else
	{
		if( strInfo[1] == "CTI" )
		{

            CtiOwnerMsg( (char *)strInfo[0].c_str(), (char *)strInfo[1].c_str(), (char *)strInfo[2].c_str(),
                    (char *)strInfo[3].c_str(), (char *)strInfo[4].c_str() );

			return;
		}

		if( strInfo[2] == "IVR_CALLID" )
        {
            AddIVRCallIDInfo( (char *)strInfo[0].c_str(), (char *)strInfo[3].c_str() );
        }


        if( m_MainEntry.m_iCTIType == trans_station_cti_type && iClientId > 0 )
        {
            //中转CTI，发送消息给某个分机，应该转发给所有的loabbalance_cti，然后在loadbalance_cti中决定是发送给某个坐席分机
            for(int i = 0; i < m_MainEntry.m_nMoniExtNum; i++)
            {
                int iDestId = m_ExtSockMap.Find( (char *)m_MainEntry.m_strMoniExt[i].c_str() );
                if( iDestId > 0 )
                {

                    sprintf(szInBuff,"%s|%s|%s|%s|%s", (char *)(char *)strInfo[0].c_str(),
                            (char *)strInfo[1].c_str(),(char *)strInfo[2].c_str(),(char *)strInfo[3].c_str(),
                            (char *)strInfo[4].c_str());

                    TraceEx( szInBuff );

                    Send_command( iDestId, CMD_EXTENSIONMSG_EVENT, RC_EVENT, szInBuff, strlen(szInBuff) );

                }
                else
                {
                    sprintf( szLog, "Error AgentStatus, cant find destid, ext = %s", (char *)m_MainEntry.m_strMoniExt[i].c_str() );
                    TraceEx( szLog );
                }
            }

            return;
        }


        int iDestId = 0;
        if( strcmp( m_szQueueExt, (char *)strInfo[1].c_str() ) == 0 && m_MainEntry.m_iCTIType != normal_cti_type )
        {
            iDestId = m_ExtSockMap.Find( m_szLocalQueueExt );
        }
        else
        {
            if( iClientId < 0 && m_MainEntry.m_iCTIType == loadbalance_cti_type )
            {
                if( strInfo[2] == "AgentLogin" )
                {
                    bool bNeedSend = false;
                    //iClientId = -1, buf = 3Uw, strInfo[0] = 6001, strInfo[1] = VSQL, strInfo[2] = AgentLogin, strInfo[3] = 1001, strInfo[4] = AgentReady, bAll = 0
                    PAGENTTYPE pSrcAgentType = GetAgentTypeByExt( (char *)strInfo[0].c_str() );
                    if( pSrcAgentType != NULL && pSrcAgentType->iClientId > 0 )
                    {
                        bNeedSend = true;
                    }
                    else
                    {
                        bNeedSend = false;
                    }

                    if( !bNeedSend )
                    {
                        sprintf( szLog, "warnning, do not need send this msg, need_send = %d, agent_ext = %s",
                                 bNeedSend, (char *)strInfo[0].c_str() );
                        TraceEx( szLog );
                        return;
                    }
                }
            }

            iDestId = m_ExtSockMap.Find( (char *)strInfo[1].c_str() );
        }

		if( iDestId > 0 )
		{
			//TsapiServer, Cmd_SendMsg_Proc, szInBuff = 7301|8015|AgentNotReady|883688|, strlen(szInBuffer) = 31

            sprintf(szInBuff,"%s|%s|%s|%s|%s",(char *)strInfo[1].c_str(),(char *)strInfo[0].c_str(),
                    (char *)strInfo[2].c_str(),(char *)strInfo[3].c_str(),(char *)strInfo[4].c_str());

			Send_command( iDestId, CMD_EXTENSIONMSG_EVENT, RC_EVENT, szInBuff, strlen(szInBuff) );


            sprintf( szLog, "TsapiServer, Cmd_SendMsg_Proc, szInBuff = %s, strlen(szInBuffer) = %d", szInBuff, strlen(szInBuff) );
            TraceEx( szLog );
		}
		else
		{
            if( iClientId > 0 )
                Send_command( iClientId, CMD_SENDMSGTOEXT, RC_RESPONSE_FAILED, (char *)strInfo[0].c_str(), strInfo[0].length() );

		}
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_send_msg_proc() " );
	}
}

void CMainDataProc::Cmd_SetTransData_Proc(int iClientId, char *buf, string *strInfo)
{
	try
	{
	SetTransferData( strInfo[0], strInfo[1] );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_set_trans_data_proc() " );
	}
}

void CMainDataProc::UnregisterExtFromWS(int iClientId, char *szExt)
{
    if( iClientId <= 0 || szExt == NULL )
        return;

    char szLog[1024] = {0};
    sprintf( szLog, "unregister_ext_from_ws, iClientId = %d, szExt = %s", iClientId, szExt );
    TraceEx( szLog );

    try
    {
    list<PSOCKEXT> *plist = m_SockExtListMap.Find( iClientId );
    if( plist != NULL )
    {
        list<PSOCKEXT>::iterator it;
        for( it = plist->begin(); it != plist->end(); ++it )
        {
            PSOCKEXT pSockExt = *it;
            if( pSockExt != NULL )
            {
                char *lpszExtNo = NULL;
                lpszExtNo = (char *)pSockExt->strExtNo.c_str();
                if( lpszExtNo != NULL && strcmp( szExt, lpszExtNo ) == 0 )
                {
                    m_ExtSockMap.Remove( lpszExtNo );

                    DelExtInfo( lpszExtNo );

                    if( lpszExtNo[0] == 'v' || lpszExtNo[0] == 'V' )
                    {
                        if( strlen( m_szQueueExt ) > 0 )
                            SendLogoutCmd( iClientId, lpszExtNo, "VServer" );
                    }
                    else
                    {
                        m_iCurrentClientNum--;
                        if( m_iCurrentClientNum <= 0 )
                            m_iCurrentClientNum = 0;
                        if( strlen( m_szQueueExt ) > 0 )
                            SendLogoutCmd( iClientId, lpszExtNo, "VServer" );
                    }

                    plist->erase( it );

                    memset( szLog, 0, 1024 );
                    sprintf( szLog, "unregister_ext_from_ws, remove extno = %s, m_iCurrentClientNum = %d",
                             lpszExtNo, m_iCurrentClientNum );
                    TraceEx( szLog );

                    delete pSockExt;
                    pSockExt = NULL;

                    break;
                }
            }
        }

        AddToListCtrl( "", iClientId, plist );
    }
    }
    catch(...)
    {
        TraceEx( "error, exception, CData::unregister_ext_from_ws" );
    }

}

void CMainDataProc::Cmd_UnregisterExt_Proc(int iClientId, char *buf, string *strInfo)
{
	try
	{
    m_MainEntry.m_CtiCtrl.CstaMonitorStop(m_ExtInfoMap.Find( (char *)strInfo[0].c_str() ) );
	
	PACKET_HEAD *pHead = (PACKET_HEAD *)buf;
	pHead->cmd_type = RC_RESPONSE_OK;
    m_MainEntry.SendToClientFunc( iClientId, (char *)pHead, sizeof(PACKET_HEAD) + pHead->dwSize );

    int iType = atoi( (char *)strInfo[1].c_str() );
    if( iType == ws_type )
    {
        UnregisterExtFromWS( iClientId, (char *)strInfo[0].c_str() );
    }
    else
    {
        OnClientClose( iClientId );
        m_MainEntry.CloseSockFunc( iClientId );
    }
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_unregister_ext_proc() " );
	}
}

void CMainDataProc::SetOwner(void *lpVoid)
{
#ifdef _VC6_
	try
	{
	if( lpVoid == NULL )
		return;
	g_lpthis = (CTsapiServerDlg *)lpVoid;
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::set_owner() " );
	}
#endif
}

int CMainDataProc::CheckTheSameExtReged(char *lpszExtNo, int iClientType)
{
	try
	{
    char szLog[256] = {0};
    if( m_MainEntry.m_iKickedOff == 0 )
	{
		if( iClientType == agent_type || iClientType == agent_monitor_type )
			return m_ExtSockMap.Find( lpszExtNo );
	}
	
	int iClientId = m_ExtSockMap.Find( lpszExtNo );
	if( iClientId > 0 )
	{
		char szInBuff[256] = {0};
		sprintf(szInBuff,"%s|%s|%s|%s|%s",lpszExtNo,lpszExtNo,"ForceLogout","","");
		Send_command( iClientId, CMD_EXTENSIONMSG_EVENT, RC_EVENT, szInBuff, strlen(szInBuff) );

        sprintf( szLog, "check_same_ext, client_id = %d, szInBuff = %s", iClientId, szInBuff );
        TraceEx( szLog );

		OnClientClose( iClientId );
        m_MainEntry.CloseSockFunc( iClientId );
	}
	return 0;
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::check_the_same_ext_reged() " );
	}
	return 0;
}

int CMainDataProc::GetClientIdByExt(char *lpszExt)
{
	try
	{
	if( lpszExt == NULL )
		return 0;
	return m_ExtSockMap.Find( lpszExt );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::get_clientid_by_ext() " );
	}
	return 0;
}

void CMainDataProc::Send_command(int iClientId,int cmd,int cmdtype,char *lpszSendBuffer, int data_len)
{
	try
	{
    char buffer[1024*100];
    memset( buffer, 0, 1024*100 );

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

    m_MainEntry.SendToClientFunc( iClientId, buffer, sizeof(PACKET_HEAD) + data_len );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::send_command() " );
	}
}

int  CMainDataProc::DecodeFrame(string *strInfo, string &strData, char *lpszSpliteValue, int iSpliteLen)
{
    int i = 0;
    int pos = strData.find( lpszSpliteValue );
    while( pos >= 0 )
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

void CMainDataProc::AddToExtSock(char *szExt, int iClientId)
{
	try
	{
	list<PSOCKEXT> *pSockExtList = m_SockExtListMap.Find(iClientId);
	if( pSockExtList == NULL )
	{
		pSockExtList = new list<PSOCKEXT>;
		if( pSockExtList == NULL )
			return;
	}

	PSOCKEXT pSockExt = new SOCKEXT;
	if( pSockExt == NULL )
		return;
	
	pSockExt->strExtNo =  szExt;
	pSockExt->iClientId = iClientId;
	m_SockExtListMap.Add(iClientId, pSockExtList );	

	pSockExtList->push_back( pSockExt );

    char szLog[256] = {0};
    sprintf( szLog, "CMainDataProc::AddToExtSock, szExt = %s, iClientId = %d", szExt, iClientId );
    TraceEx( szLog );

	AddToListCtrl( szExt, iClientId, pSockExtList );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::add_to_ext_sock() " );
	}
}

void CMainDataProc::AddToListCtrl(char *szExt, int iClientId, list<PSOCKEXT> *pSockExtList)
{
	try
	{

#ifdef _VC6_
    string strIPAddr = "", strPort = "", strSock = "";
	GetIPAndPort( iClientId,  strIPAddr, strPort );
    char szClientId[32] = {0};
    sprintf( szClientId, "%d", iClientId );
    strSock = szClientId;

    string strNum = "", strTime = "", strExt = "";
	strNum.Format( "%d", pSockExtList->size() );
	CTime t = CTime::GetCurrentTime();
	strTime.Format( "%4d-%2d-%2d, %2d:%2d:%2d", t.GetYear(), t.GetMonth(), t.GetDay(),
		t.GetHour(), t.GetMinute(), t.GetSecond() );
#endif



 //   2015-12-12 12:12:12

    string strExt = "";
	list<PSOCKEXT>::iterator it;
    for( it = pSockExtList->begin(); it != pSockExtList->end(); ++it )
	{
		PSOCKEXT pSockExt = *it;
		if( pSockExt != NULL )
		{
			char *lpszExtNo = NULL;
			lpszExtNo = (char *)pSockExt->strExtNo.c_str();
			if( lpszExtNo != NULL )
			{
                if( strExt.length() == 0 )
				{
					strExt = lpszExtNo;
				}
				else
				{
					strExt += ",";
					strExt += lpszExtNo;
				}
			}
		}		
	}

#ifdef _VC6_
    bool bFind = false;
	for(int iItem = 0; iItem < g_lpthis->m_wndlist.GetItemCount(); iItem++)
	{
		if( g_lpthis->m_wndlist.GetItemText( iItem, col_sock ) == strSock )
		{
			g_lpthis->m_wndlist.SetItemText( iItem, col_num, (LPSTR &)strNum );
			g_lpthis->m_wndlist.SetItemText( iItem, col_time, (LPSTR &)strTime );
			g_lpthis->m_wndlist.SetItemText( iItem, col_extno, (LPSTR &)strExt );

            bFind = true;
			break;
		}
	}

	if( !bFind )
	{
		int nItemCount = g_lpthis->m_wndlist.GetItemCount();
        string strItemCount = "";
		strItemCount.Format( "%d", nItemCount );
		int iItemIndex = g_lpthis->m_wndlist.InsertItem( nItemCount, (LPSTR &)strItemCount );

		g_lpthis->m_wndlist.SetItemText( iItemIndex, col_ip, (LPSTR &)strIPAddr );
		g_lpthis->m_wndlist.SetItemText( iItemIndex, col_port, (LPSTR &)strPort );
		g_lpthis->m_wndlist.SetItemText( iItemIndex, col_num, (LPSTR &)strNum );
		g_lpthis->m_wndlist.SetItemText( iItemIndex, col_time, (LPSTR &)strTime );
		g_lpthis->m_wndlist.SetItemText( iItemIndex, col_extno, (LPSTR &)strExt );
		g_lpthis->m_wndlist.SetItemText( iItemIndex, col_sock, (LPSTR &)strSock );

	}
#else

    QDateTime current_dt = QDateTime::currentDateTime();
    QString strDT = current_dt.toString("yyyy-MM-dd hh:mm:ss ddd");

    REGEXTINFOLIST *regExtInfo = FindRegExtInfoBySock( iClientId );
    if( regExtInfo == NULL )
    {
        regExtInfo = new REGEXTINFOLIST;
        if( regExtInfo != NULL )
        {
            regExtInfo->iClientId = iClientId;
            regExtInfo->iNum = pSockExtList->size();
            regExtInfo->iPort = 0;
            memset( regExtInfo->lpszIPAddr, 0, 32 );
            regExtInfo->strExt = strExt;
            memset( regExtInfo->szTime, 0, 32 );
            strcpy( regExtInfo->szTime, (char *)strDT.toStdString().c_str() );

            m_RegExtInfoList.push_back( regExtInfo );
        }
    }
    else
    {
            regExtInfo->iNum = pSockExtList->size();
            regExtInfo->strExt = "";
            regExtInfo->strExt = strExt;
            memset( regExtInfo->szTime, 0, 32 );
            strcpy( regExtInfo->szTime, (char *)strDT.toStdString().c_str() );

    }
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::add_to_list_ctrl() " );
	}
}

REGEXTINFOLIST *CMainDataProc::FindRegExtInfoBySock(int iSock)
{
    REGEXTINFOLIST *regExtInfo = NULL;
    return regExtInfo;
}

void CMainDataProc::DelFromListCtrl(int iClientId)
{
#ifdef _VC6_
	try
	{
    string strSock = "";
	strSock.Format( "%d", iClientId );
	
	for(int iItem = 0; iItem < g_lpthis->m_wndlist.GetItemCount(); iItem++)
	{
		if( g_lpthis->m_wndlist.GetItemText( iItem, col_sock ) == strSock )
		{
			g_lpthis->m_wndlist.DeleteItem( iItem );
			break;
		}
	}

	for(int i = iItem; i < g_lpthis->m_wndlist.GetItemCount(); i++)
	{
        string strIndex = "";
		strIndex.Format( "%d", i );
		g_lpthis->m_wndlist.SetItemText( i, col_no, (LPSTR &)strIndex );
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::del_from_list_ctrl() " );
	}
#endif
}

void CMainDataProc::GetIPAndPort(int iClientId, string &strIPAddr, string &strPort)
{
#ifdef _VC6_
	try
	{
	sockaddr_in sa;
	int nSa = sizeof(sa);
	getpeername( iClientId, (struct sockaddr *)&sa, &nSa );
	strIPAddr.Format( "%s", inet_ntoa( sa.sin_addr ) );
	strPort.Format( "%d", sa.sin_port );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::get_ip_and_port() " );
	}
#endif
}

void CMainDataProc::Cmd_HeartBeat_Proc(int iClientId, char *buf)
{
	
}

int CMainDataProc::GetClientIdByExt(unsigned long dwExt)
{
	try
	{
    char szExt[32] = {0};
    sprintf( szExt, "%d", dwExt );
    return m_ExtSockMap.Find( szExt );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::get_clientId_byext() " );
	}
	return 0;
}

void CMainDataProc::MoniLock()
{
	try
	{
#ifdef _VC6_
	EnterCriticalSection( &m_MoniCS );
#else
    QMutexLocker locker(&m_MoniCS);
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::moni_lock() " );
	}
}

void CMainDataProc::MoniUnlock()
{
	try
	{
#ifdef _VC6_
	LeaveCriticalSection( &m_MoniCS );
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::moni_unlock() " );
	}
}

void CMainDataProc::SetTransferData(string strExt, string strData)
{
	try
	{
	char *lpData = NULL;
    int iExt = atoi( (char *)strExt.c_str() );
	
	lpData = m_ExtDataMap.Find( iExt );
	if( lpData != NULL )
	{
		delete[] lpData;
		lpData = NULL;
	}
	m_ExtDataMap.Remove( iExt );
	
    lpData = new char[ strData.length() + 1 ];
	if( lpData != NULL )
	{
        memset( lpData, 0, strData.length() + 1 );
        strcpy( lpData, (char *)strData.c_str() );
		m_ExtDataMap.Add( iExt, lpData );
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::set_transfer_data() " );
	}
	/*
	char *lpData = NULL;
	
	PEXTINFO pE = m_ExtInfoMap.Find( (LPSTR &)strExt );
	if( pE == NULL )
	{
		TraceEx( "SetTransferData failed, pE == NULL" );
		return;
	}

	lpData = m_GlbCallIdDataMap.Find( pE->szGlobalCallid );
	if( lpData != NULL )
	{
		delete[] lpData;
		lpData = NULL;
	}
	m_GlbCallIdDataMap.Remove( pE->szGlobalCallid );

	lpData = new char[ strData.GetLength() + 1 ];
	if( lpData != NULL )
	{
		memset( lpData, 0, strData.GetLength() + 1 );
		strcpy( lpData, (LPSTR &)strData );
		m_GlbCallIdDataMap.Add( pE->szGlobalCallid, lpData );
	}
	*/
}

bool CMainDataProc::IsBeyondLinsenceNum()
{
	try
	{
    char szLog[256] = {0};
    bool bResult = false;
	if( m_iCurrentClientNum >= m_iMaxClientNum )
	{
        bResult = true;
        sprintf( szLog, "BeyondLinsenceNum, m_iCurrentClientNum = %d, m_iMaxClientNum = %d",
                 m_iCurrentClientNum, m_iMaxClientNum );
        TraceEx( szLog );
	}

    sprintf( szLog, "IsBeyondLinsenceNum, m_iMaxClientNum = %d, m_iCurrentClientNum = %d, bResult = %d",
             m_iMaxClientNum, m_iCurrentClientNum, bResult );

    TraceEx( szLog );

	return bResult;
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::is_beyond_license_num() " );
	}
    return false;
}

void CMainDataProc::SetLisenceNum(int iLisenceNum)
{
	try
	{
	m_iMaxClientNum = iLisenceNum;
    char szLog[256] = {0};
    sprintf( szLog, "SetLisenceNum, m_iMaxClientNum = %d", m_iMaxClientNum );
    TraceEx( szLog );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::set_license_num() " );
	}
}

void CMainDataProc::Cmd_SendFax_Proc(int iClientId, char *buf)
{
	try
	{
	PACKET_HEAD *pHead = (PACKET_HEAD *)buf;
	char szDestExt[16] = {0};
	memcpy( szDestExt, buf + sizeof(PACKET_HEAD) + 1, 16 );

	int iDestClientId = GetClientIdByExt( szDestExt );

    //string str = "";
    char lpszLogStr[512] = {0};
    sprintf( lpszLogStr, "Cmd_SendFax_Proc, iDestClientId = %d, iDataLen = %d, szDestExt = %s", iDestClientId, sizeof(PACKET_HEAD) + pHead->dwSize, szDestExt );
    //str.Format( "Cmd_SendFax_Proc, iDestClientId = %d, iDataLen = %d, szDestExt = %s", iDestClientId, sizeof(PACKET_HEAD) + pHead->dwSize, szDestExt );

	if( iDestClientId > 0 )
	{
        m_MainEntry.SendToClientFunc( iDestClientId, buf, sizeof(PACKET_HEAD) + pHead->dwSize );
	}

    TraceEx( lpszLogStr );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_send_fax_proc() " );
	}
}

void CMainDataProc::SetLocalQueueExt(char *lpszLocalQueueExt)
{
    try
    {
    memset( m_szLocalQueueExt, 0, 32 );
    if( lpszLocalQueueExt != NULL )
    {
        strcpy( m_szLocalQueueExt, lpszLocalQueueExt );
    }
    }
    catch(...)
    {
        TraceEx( "error, exception, CData::set_queue_ext() " );
    }
}

void CMainDataProc::SetQueueExt(char *lpszExtNo)
{
	try
	{
	memset( m_szQueueExt, 0, 32 );
	if( lpszExtNo != NULL )
	{
		strcpy( m_szQueueExt, lpszExtNo );
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::set_queue_ext() " );
	}
}

void CMainDataProc::SendLogoutCmdToVSQL(int iClientId, char *lpszExt)
{
    if( iClientId < 0 || lpszExt == NULL )
        return;

    char buf[256] = {0}, szLog[512] = {0};
    sprintf( buf, "%s|%s|AgentLogout|%s", "VSQL", lpszExt, "VServer" );

    int iDestId = m_ExtSockMap.Find( "VSQL" );
    if( iDestId > 0 )
    {
        Send_command( iDestId, CMD_EXTENSIONMSG_EVENT, RC_EVENT, buf, strlen(buf) );

        sprintf( szLog, "SendLogoutCmdToVSQL, iDestId = %d, lpszExt = %s, buf = %s", iDestId, lpszExt, buf );
        TraceEx( szLog );
    }
    else
    {
        sprintf( szLog, "SendLogoutCmdToVSQL failed, iDestId = %d", iDestId );
        TraceEx( szLog );
    }
}

void CMainDataProc::SendLogoutCmd(int iClientId, char *lpszExt, char *lpszVirtualServer)
{
	try
	{
	if( iClientId < 0 || lpszExt == NULL || lpszVirtualServer == NULL )
		return;

    char szLog[256] = {0};
	if( strcmp( lpszExt, m_szQueueExt ) == 0 )
	{
        sprintf( szLog, "warnning, SendLogoutCmd failed and return now, lpszExt = %s, m_szQueueExt = %s",
                 lpszExt, m_szQueueExt );
        TraceEx( szLog );
		return;
	}

	char *agent_id = m_ExtAgentMap.Find( lpszExt );
	if( agent_id != NULL )
	{
        sprintf( szLog, "send_logout_cmd, agent_id = %s, ext = %s", agent_id, lpszExt );
        TraceEx( szLog );
        m_MainEntry.m_CtiCtrl.CstaSetAgentState( CMD_AGENTLOGOFF, lpszExt, agent_id, "1", "", iClientId );
	}

	char buf[256] = {0};
	sprintf( buf, "%s|%s|AgentLogout|%s", m_szQueueExt, lpszExt, lpszVirtualServer );

    int iDestId = 0;
    if( m_MainEntry.m_iCTIType != normal_cti_type )
    {
        iDestId = m_ExtSockMap.Find( m_szLocalQueueExt );
    }
    else
    {

        iDestId = m_ExtSockMap.Find( m_szQueueExt );
    }

	if( iDestId > 0 )
	{
		Send_command( iDestId, CMD_EXTENSIONMSG_EVENT, RC_EVENT, buf, strlen(buf) );

        sprintf( szLog, "SendLogoutCmd, iDestId = %d, lpszExt = %s, buf = %s", iDestId, lpszExt, buf );
        TraceEx( szLog );
	}
	else
	{
        sprintf( szLog, "SendLogoutCmd failed, iDestId = %d", iDestId );
        TraceEx( szLog );
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::send_logout_cmd() " );
	}
}

void CMainDataProc::Cmd_SetExtNoEvent_Proc(int iClientId, char *buf, string *strInfo)
{
	try
	{
	//buffer = szExtNo|nFlag|bOnEvent
    char szLog[256] = {0};
    sprintf( szLog, "SetExtNoEvent, szExtNo = %s, nFlag = %s, bOnEvent = %s",
             (char *)strInfo[0].c_str(), (char *)strInfo[1].c_str(), (char *)strInfo[2].c_str() );
    TraceEx( szLog );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_set_ext_no_event_proc() " );
	}
}

void CMainDataProc::DecodeData(char *buf, string *strInfo)
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

void CMainDataProc::UpdateMsgProc(PMSGINFO pMsgInfo)
{
	try
	{
	if( pMsgInfo == NULL )
		return;

	char *buf = new char[pMsgInfo->FileInfo.lFileSize + 32];
	if( buf != NULL )
	{
        unsigned char byType = pMsgInfo->iMsgType;
		memset( buf, 0, pMsgInfo->FileInfo.lFileSize + 32 );
		memcpy( buf, &byType, 1 );
		memcpy( buf + 1, pMsgInfo->FileInfo.lpszData, pMsgInfo->FileInfo.lFileSize );

		Send_command( pMsgInfo->iClientId, CMD_DOWNLOADFILE, RC_RESPONSE_OK, buf, pMsgInfo->FileInfo.lFileSize + 1 );

		delete[] buf;
		buf = NULL;
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::update_msg_proc() " );
	}
}

void CMainDataProc::Cmd_QueryVersion(int iClientId, char *lpszBuffer, string *strInfo)
{
#ifdef _VC6_
	try
	{
	int iCmdType = RC_RESPONSE_OK;
	MSGINFO msgInfo;
	memset( &msgInfo, 0, sizeof(MSGINFO) );
	
	if( g_lpthis->QueryVersionFunc == NULL )
	{
		iCmdType = RC_RESPONSE_FAILED;
	}
	else
	{
		if( g_lpthis->QueryVersionFunc((LPSTR &)strInfo[0], &msgInfo, iClientId) < 0 )
			iCmdType = RC_RESPONSE_FAILED;
	}
	
	char szBuf[1024] = {0};
	sprintf(szBuf ,"%s|%s|%d" ,strInfo[0], msgInfo.FileInfo.szVersion, msgInfo.FileInfo.lFileSize);
	Send_command(iClientId, CMD_REQUERYFILEVER, iCmdType, szBuf, strlen(szBuf));

	TraceEx( "QueryVersion, iClientId = %d, Result = %s", iClientId, szBuf );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_query_version() " );
	}
#endif

    char szLog[512] = {0};
    try
    {
    int iCmdType = RC_RESPONSE_OK;
    MSGINFO msgInfo;
    memset( &msgInfo, 0, sizeof(MSGINFO) );

    char lpszFileName[256] = {0};
    QueryVersion( lpszFileName, &msgInfo, iClientId );

    char szBuf[1024] = {0};
    sprintf(szBuf ,"%s|%s|%d" , lpszFileName, msgInfo.FileInfo.szVersion, msgInfo.FileInfo.lFileSize);
    Send_command(iClientId, CMD_REQUERYFILEVER, iCmdType, szBuf, strlen(szBuf));

    sprintf( szLog, "QueryVersion, iClientId = %d, Result = %s", iClientId, szBuf );
    TraceEx( szLog );
    }
    catch(...)
    {
        TraceEx( "error, exception, CData::cmd_query_version() " );
    }

}

int CMainDataProc::QueryVersion(char *lpszFileName, PMSGINFO pMsgInfo, int iClientId)
{
    char szLog[512] = {0};
    int iResult = -1;
    char szFileName[128] = {0}, szFilePathName[128] = {0};
    memset( pMsgInfo, 0, sizeof(MSGINFO) );

    QSettings *configIni = new QSettings(m_lpUpdateFile, QSettings::IniFormat);

    QString strFileNum = configIni->value("/Set/FileNum").toString();
    int iFileNum = atoi( (char *)strFileNum.toStdString().c_str() );

    sprintf( szLog, "QueryVersion, lpszFileName = %s, iFileNum = %d, iClientId = %d", lpszFileName, iFileNum, iClientId );
    TraceEx( szLog );

    iFileNum = 1;
    for(int i = 0; i < iFileNum; i++)
    {
        char szFileIndex[16] = {0};
        strcpy( szFileIndex, "File" );

        sprintf( szLog, " i = %d, szFileIndex = %s", i, szFileIndex );
        TraceEx( szLog );

        char szIndex[8] = {0};
        sprintf( szIndex, "%d", i );
        strcat( szFileIndex, szIndex );

        sprintf( szLog, " i = %d, szFileIndex = %s, szIndex = %s", i, szFileIndex, szIndex );
        TraceEx( szLog );

        memset( szFileName, 0, 128 );
        char szSection[128] = {0};
        sprintf( szSection, "/%s/FileName", szFileIndex );
        QString strFileName = configIni->value(szSection).toString();
        strcpy( szFileName, (char *)strFileName.toStdString().c_str() );

        sprintf( szLog, "QueryVersion, i = %d, szFileIndex = %s, szFileName = %s, lpszFileName = %s",
                 i, szFileIndex, szFileName, lpszFileName );

        TraceEx( szLog );

        //if( strcmp( szFileName, lpszFileName ) == 0 )
        {
            pMsgInfo->iClientId = iClientId;
            strcpy( pMsgInfo->FileInfo.lpszFileName, lpszFileName );

            memset( szFilePathName, 0, 128 );

            char szPathNameSection[128] = {0};
            sprintf( szPathNameSection, "/%s/FilePathName", szFileIndex );
            QString strFilePathName = configIni->value(szPathNameSection).toString();
            strcpy( szFilePathName, (char *)strFilePathName.toStdString().c_str() );

            pMsgInfo->FileInfo.lFileSize = 10;//updatefile.GetFileLen( szFilePathName );

            char szVerSetion[128] = {0};
            sprintf( szVerSetion, "/%s/Version", szFileIndex );
            QString strVersion = configIni->value(szVerSetion).toString();
            strcpy( pMsgInfo->FileInfo.szVersion, (char *)strVersion.toStdString().c_str() );

            pMsgInfo->iMsgType = 0;

            strcpy( lpszFileName, szFilePathName );
            sprintf( szLog, "QueryVersion find now, szFilePathName = %s, szVersion = %s, lFileSize = %d",
                     szFilePathName, pMsgInfo->FileInfo.szVersion, pMsgInfo->FileInfo.lFileSize );
            TraceEx( szLog );

            iResult = 0;

            break;
        }
    }
    return iResult;
}

void CMainDataProc::Cmd_DownloadStart(int iClientId, char *lpszBuffer, string *strInfo)
{
#ifdef _VC6_
	try
	{
	if( g_lpthis->DownloadStartFunc != NULL )
		g_lpthis->DownloadStartFunc( (LPSTR &)strInfo[0], iClientId );

	TraceEx( "DownloadStart, iClientId = %d, lpszFileName = %s", iClientId, strInfo[0] );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::cmd_download_start() " );
	}
#endif
}

void CMainDataProc::CstaMonitorEvent(int iType)
{
	try
	{
        char szLog[256] = {0};
        if( m_RegExtList.size() <= 0 )
        {
            sprintf( szLog, "CstaMonitorEvent failed, m_RegExtList.size() = %d", m_RegExtList.size() );
            TraceEx( szLog );
            return;
        }

        while( m_RegExtList.size() > 0 )
        {

            int iClientId = 0;
            string strExtNo;


            strExtNo =  m_RegExtList.front();

            int iFlag = m_ExtFlagMap.Find(strExtNo);

            sprintf( szLog, "csta_monitor_event, iflag = %d, m_RegExtList.size() = %d, front = %s", iFlag, m_RegExtList.size(), (char *)strExtNo.c_str() );
            TraceEx( szLog );

            if( strExtNo.length() > 0 )
            {
                iClientId = m_ExtSockMap.Find( strExtNo );
                if( iClientId > 0 && iFlag == 1 )
                {
                    Send_command( iClientId, CMD_REGISTERDN, iType, (char *)strExtNo.c_str(), strExtNo.length() );
                }
                else
                {
                    sprintf( szLog, "error, can not find client_id, m_ext_sock_map, extno = %s", (char *)strExtNo.c_str() );
                    TraceEx( szLog );
                }
            }

            m_RegExtList.pop_front();

            if( iType == RC_RESPONSE_OK )
            {
                m_iCurrentClientNum++;
                AddExtInfo((char *)strExtNo.c_str());

                if( iFlag == 1 )
                    AddToExtSock((char *)strExtNo.c_str(), iClientId);

                sprintf( szLog, "CstaMonitorEvent, register successful, iClientId = %d, szExtNo = %s, m_iCurrentClientNum = %d",
                         iClientId, (char *)strExtNo.c_str(), m_iCurrentClientNum );
                TraceEx( szLog );
            }
            else if( iType == RC_RESPONSE_FAILED )
            {
                m_ExtSockMap.Remove( (char *)strExtNo.c_str() );
                sprintf( szLog, "CstaMonitorEvent, register failed, iClientId = %d, szExtNo = %s",
                         iClientId, (char *)strExtNo.c_str() );
                TraceEx( szLog );
            }
        }

	}
	catch(...)
	{
		TraceEx( "error, exception, CData::csta_monitor_event() " );
	}
}

void CMainDataProc::AddExtInfo(char *szExtNo)
{
	try
	{
	PEXTINFO pEInfo = m_ExtInfoMap.Find(szExtNo);
	if( pEInfo == NULL )
	{
		pEInfo = new EXTINFO;
		if( pEInfo != NULL )
		{
			memset( pEInfo, 0, sizeof(EXTINFO) );
			strcpy( pEInfo->szExtNo, szExtNo );
            pEInfo->nCrossRefID = m_MainEntry.m_CtiEvent.m_cstaEvent.event.cstaConfirmation.u.monitorStart.monitorCrossRefID;

			AddCrossIdInfo( pEInfo, szExtNo );

			m_ExtInfoMap.Add(szExtNo, pEInfo);
		}
		else
		{
			TraceEx( "AddExtInfo failed, new pEinfo failed" );
		}
	}
	else
	{
		DelCrossIdInfo( pEInfo );

        pEInfo->nCrossRefID = m_MainEntry.m_CtiEvent.m_cstaEvent.event.cstaConfirmation.u.monitorStart.monitorCrossRefID;

		AddCrossIdInfo( pEInfo, szExtNo );
        char szLog[256] = {0};
        sprintf( szLog, "AddExtInfo, readd successful, szExtNo = %s", szExtNo );
        TraceEx( szLog );
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::add_ext_info() " );
	}
}

void CMainDataProc::DelExtInfo(char *szExtNo)
{
	try
	{
	PEXTINFO pEInfo = m_ExtInfoMap.Find(szExtNo);
	if( pEInfo != NULL )
	{
		DelCrossIdInfo( pEInfo );

		m_ExtInfoMap.Remove( szExtNo );
		delete pEInfo;
		pEInfo = NULL;
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::del_ext_info() " );
	}
}

PEXTINFO CMainDataProc::GetExtInfoFromRefId(CSTAMonitorCrossRefID_t nCrossRefId)
{
	try
	{
	char *szExtNo = m_CrossIdExtMap.Find( nCrossRefId );
	if( szExtNo == NULL || strlen( szExtNo ) == 0 )
		return NULL;

	return m_ExtInfoMap.Find( szExtNo );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::get_extinfo_from_refid() " );
	}
	return NULL;
}

void CMainDataProc::ClearTransferData(PEXTINFO pE, LocalConnectionState_t localConnectionInfo)
{
	try
	{
	if( pE == NULL )
	{
		TraceEx( "ClearTransferData failed, pE == NULL" );
		return;
	}

    char szLog[256] = {0};
	if( (char)localConnectionInfo == CS_NULL )
	{
		char *lpData = NULL;
		int iExtNo = atoi( pE->szExtNo );
		lpData = m_ExtDataMap.Find( iExtNo );
		if( lpData != NULL )
		{
			m_ExtDataMap.Remove( iExtNo );
			delete[] lpData;
			lpData = NULL;

            sprintf( szLog, "Extno hangup and clear transfer data now, iExtNo = %d", iExtNo );
            TraceEx( szLog );
		}

		/*
		char *lpData = NULL;
		lpData = m_GlbCallIdDataMap.Find( pE->szGlobalCallid );
		if( lpData != NULL )
		{
			delete[] lpData;
			lpData = NULL;
		}
		m_GlbCallIdDataMap.Remove( pE->szGlobalCallid );
		*/
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::clear_transfer_data() " );
	}
}

void CMainDataProc::CloseAllClient()
{
	try
	{
	TraceEx( "begin to close all client" );
#ifdef _VC6_
	for(int iItem = 0; iItem < g_lpthis->m_wndlist.GetItemCount(); iItem++)
	{
        string strSock = g_lpthis->m_wndlist.GetItemText( iItem, col_sock );

		TraceEx( "begin to close the sock = %s", strSock );
        m_MainEntry.CloseSockFunc( atoi( (LPSTR &)strSock ) );
		Sleep( 100 );
	}
#else
    char szLog[256] = {0};
    list<REGEXTINFOLIST *>::iterator it;
    for( it = m_RegExtInfoList.begin(); it != m_RegExtInfoList.end(); ++it )
    {
        REGEXTINFOLIST *regExtInfo = *it;
        if( regExtInfo != NULL )
        {
            sprintf( szLog, "begin to close the sock = %d", regExtInfo->iClientId );
            TraceEx( szLog );
            m_MainEntry.CloseSockFunc( regExtInfo->iClientId );
            usleep( 100*1000 );
        }
    }
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::close_all_client() " );
	}
}

void CMainDataProc::AddCrossIdInfo(PEXTINFO pEInfo, char *szExtNo)
{
	try
	{
	if( pEInfo == NULL || szExtNo == NULL )
		return;

	char *lpszExt = new char[256];
	if( lpszExt != NULL )
	{
		memset( lpszExt, 0, 256 );
        strcpy( lpszExt, szExtNo );
        char szLog[256] = {0};
        sprintf( szLog, "-------------------------add_cross_id_info,ext = %s, nCrossRefID = %ld--------------------", lpszExt, pEInfo->nCrossRefID);
        TraceEx( szLog );
		m_CrossIdExtMap.Add(pEInfo->nCrossRefID, lpszExt );
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::add_cross_id_info() " );
	}
}

void CMainDataProc::DelCrossIdInfo(PEXTINFO pEInfo)
{
	try
	{
	if( pEInfo == NULL )
		return;

	char *szExt = m_CrossIdExtMap.Find( pEInfo->nCrossRefID );
	if( szExt != NULL )
	{
		m_CrossIdExtMap.Remove( pEInfo->nCrossRefID );
		delete[] szExt;
		szExt = NULL;
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::del_crossid_info() " );
	}
}

void CMainDataProc::RemonitorAllExt()
{
	try
	{
	TraceEx( "=================begin to RemonitorAllExt================" );
    char szLog[256] = {0};
	int iRemonitor = 2;
#ifdef _VC6_
    for(int iItem = 0; iItem < g_lpthis->m_wndlist.GetItemCount(); iItem++)
#else
    if( m_RegExtInfoList.size() <= 0 )
        return;

    list<REGEXTINFOLIST *>::iterator it;
    for( it = m_RegExtInfoList.begin(); it != m_RegExtInfoList.end();  )
#endif
	{
        int iClientId = 0;
#ifdef _VC6_
        string strSock = g_lpthis->m_wndlist.GetItemText( iItem, col_sock );
        iClientId = atoi( strSock );
#else
        REGEXTINFOLIST *regExtInfo = *it;
        if( regExtInfo == NULL )
        {
            it = m_RegExtInfoList.erase( it );
            continue;
        }
        else
        {
            iClientId = regExtInfo->iClientId;
        }
#endif
		list<PSOCKEXT> *pSockExtList = m_SockExtListMap.Find(iClientId);
		if( pSockExtList != NULL )
		{
			list<PSOCKEXT>::iterator it;
			for( it = pSockExtList->begin(); it != pSockExtList->end(); ++it )
			{
				PSOCKEXT pSockExt = *it;
				if( pSockExt != NULL )
				{
					char *lpszExtNo = NULL;
					lpszExtNo = (char *)pSockExt->strExtNo.c_str();
					if( lpszExtNo != NULL && lpszExtNo[0] != 'v' && lpszExtNo[0] != 'V' )
					{
                        sprintf( szLog, "begin to monitor the ext = %s", lpszExtNo );
                        TraceEx( szLog );

						m_RegExtList.push_back( lpszExtNo );
                        if( m_MainEntry.m_CtiCtrl.CstaMonitorDevice( lpszExtNo ) >= 0 )
						{
							m_iCurrentClientNum--;
							if( m_iCurrentClientNum <= 0 )
								m_iCurrentClientNum = 0;
							m_ExtFlagMap.Remove(lpszExtNo);
							m_ExtFlagMap.Add(lpszExtNo, iRemonitor);
						}
						else
						{
							m_RegExtList.pop_back();
						}
#ifdef _VC6_
						Sleep( 50 );
#else
                        usleep( 50*1000 );
#endif
					}
				}		
			}
		}
        ++it;
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::remonitor_all_ext() " );
	}
}

void CMainDataProc::CopyTransferDataToDest(string strSrcExt, string strDestExt)
{
	try
	{
    char szLog[256] = {0};
	char *lpszData = NULL;
    lpszData = m_ExtDataMap.Find( atoi( (char *)strSrcExt.c_str() ) );
	if( lpszData != NULL )
	{
        string strSrcData = lpszData;
		
        int iDestExt = atoi( (char *)strDestExt.c_str() );
		char *lpszDestData = NULL;
		lpszDestData = m_ExtDataMap.Find( iDestExt );
		if( lpszDestData != NULL )
		{
			delete[] lpszDestData;
			lpszDestData = NULL;
		}
		m_ExtDataMap.Remove( iDestExt );
		
        lpszDestData = new char[ strSrcData.length() + 1 ];
		if( lpszDestData != NULL )
		{
            memset( lpszDestData, 0, strSrcData.length() + 1 );
            strcpy( lpszDestData, (char *)strSrcData.c_str() );
			m_ExtDataMap.Add( iDestExt, lpszDestData );
		}
		
        sprintf( szLog, "CopyTransferDataToDest, strSrcExt = %s, strDestExt = %s, strSrcData = %s",
                 (char *)strSrcExt.c_str(), (char *)strDestExt.c_str(), (char *)strSrcData.c_str() );
        TraceEx( szLog );
	}
	else
	{
		TraceEx( "CopyTransferDataToDest, lpszData == NULL" );
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::copy_transfer_data_to_dest() " );
	}
}

void CMainDataProc::AddAgentType(int iClientId, int iAgentType, char *ext)
{
	try
	{
	if( ext == NULL )
		return;

	PAGENTTYPE pAgentType = GetAgentType(iClientId);
	if( pAgentType != NULL )
	{
		pAgentType->iAgentType = iAgentType;
	}
	else
	{
		pAgentType = FindFreeAgentType();
		if( pAgentType != NULL )
		{
			pAgentType->iClientId = iClientId;
			pAgentType->iAgentType = iAgentType;
		}
	}

	memset( pAgentType->szDeviceId, 0, 32 );
	strcpy( pAgentType->szDeviceId, ext );

	list<PAGENTTYPE> *pAgentTypeList = m_AgentTypeListMap.Find( iAgentType );
	if( pAgentTypeList == NULL )
	{
		pAgentTypeList = new list<PAGENTTYPE>;
		if( pAgentTypeList == NULL )
			return;

		m_AgentTypeListMap.Add( iAgentType, pAgentTypeList );
	}

	pAgentTypeList->push_back( pAgentType );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::add_agent_type() " );
	}
}

PAGENTTYPE CMainDataProc::GetAgentTypeByExt(char *lpszExt)
{
    try
    {
    if( lpszExt == NULL )
        return NULL;

    PAGENTTYPE pAgentType = NULL;

    for(int i = 0; i < MAX_AGENTNUM; i++)
    {
        if( strcmp( m_AgentType[i].szDeviceId, lpszExt ) == 0 )
        {
            pAgentType = &m_AgentType[i];
            break;
        }
    }
    return pAgentType;
    }
    catch(...)
    {
        TraceEx( "error, exception, CData::get_agent_type() " );
    }
    return NULL;
}

PAGENTTYPE CMainDataProc::GetAgentType(int iClientId)
{
	try
	{
	PAGENTTYPE pAgentType = NULL;
	for(int i = 0; i < MAX_AGENTNUM; i++)
	{
		if( m_AgentType[i].iClientId == iClientId )
		{
			pAgentType = &m_AgentType[i];
			break;
		}
	}
	return pAgentType;
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::get_agent_type() " );
	}
	return NULL;
}

PAGENTTYPE CMainDataProc::FindFreeAgentType()
{
	try
	{
	PAGENTTYPE pAgentType = NULL;
	for(int i = 0; i < MAX_AGENTNUM; i++)
	{
		if( m_AgentType[i].iUsed == 0 )
		{
			pAgentType = &m_AgentType[i];
			pAgentType->iUsed = 1;
			break;
		}
	}
	return pAgentType;
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::find_free_agent_type() " );
	}
	return NULL;
}

void CMainDataProc::CtiOwnerMsg(char *lpszSource, char *lpszDest, char *Param1, char *Param2, char *Param3)
{
}

void CMainDataProc::GetFileCreateTime(char *lpszDateTime)
{
#ifdef _windows_
    QFileInfo info( "encrptinterface.dll" );
#else
    QFileInfo info( "libencrptinterface.so" );
#endif

    QDateTime t = info.created();
    QString strDTHour = t.toString("hh");
    QString strDTMin = t.toString("mm");
    QString strDTSenc = t.toString("ss");

    sprintf( lpszDateTime, "%s%s%s", (char *)strDTHour.toStdString().c_str(),(char *)strDTMin.toStdString().c_str(),
             (char *)strDTSenc.toStdString().c_str() );


}

void CMainDataProc::getAdapterInfoWithQt(char *lpszHardWareAddr, char *lpszIPAddr)
{
    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();

    foreach(QNetworkInterface interfaceItem, interfaceList)
    {
        if(interfaceItem.flags().testFlag(QNetworkInterface::IsUp)
                &&interfaceItem.flags().testFlag(QNetworkInterface::IsRunning)
                &&interfaceItem.flags().testFlag(QNetworkInterface::CanBroadcast)
                &&interfaceItem.flags().testFlag(QNetworkInterface::CanMulticast)
                &&!interfaceItem.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
            QList<QNetworkAddressEntry> addressEntryList=interfaceItem.addressEntries();
            foreach(QNetworkAddressEntry addressEntryItem, addressEntryList)
            {
                if(addressEntryItem.ip().protocol()==QAbstractSocket::IPv4Protocol)
                {
                    /*
                    qDebug()<<"------------------------------------------------------------";
                    qDebug()<<"Adapter Name:"<<interfaceItem.name();
                    qDebug()<<"Adapter Address:"<<interfaceItem.hardwareAddress();
                    qDebug()<<"IP Address:"<<addressEntryItem.ip().toString();
                    qDebug()<<"IP Mask:"<<addressEntryItem.netmask().toString();
                    */

                    string strInfo[16] = {""};
                    string strData = interfaceItem.hardwareAddress().toStdString();
                    DecodeFrame( strInfo, strData, ":", 1 );
                    sprintf( lpszHardWareAddr, "%s%s%s", (char *)strInfo[1].c_str(), (char *)strInfo[3].c_str(), (char *)strInfo[5].c_str());


                    string strIPInfo[16] = {""};
                    strData = addressEntryItem.ip().toString().toStdString();
                    DecodeFrame( strIPInfo, strData, ".", 1 );
                    sprintf( lpszIPAddr, "%s.%s", (char *)strIPInfo[2].c_str(), (char *)strIPInfo[3].c_str() );

                    return;
                }
            }
        }
    }
}

void CMainDataProc::SeriProc(char *lpszSeriNo)
{
	try
	{
	if( lpszSeriNo == NULL )
		return;

    char szLog[1024] = {0};

    char lpszHardWareAddr[64] = {0},lpszIPAddr[32] = {0};
    getAdapterInfoWithQt(lpszHardWareAddr, lpszIPAddr);

    char lpszCreateDateTime[64] = {0};
    GetFileCreateTime( lpszCreateDateTime );

    memset( m_lpszSeriCardId, 0, 256 );

    char lpszOutput[256] = {0};
    encrpt_decode( lpszSeriNo, strlen(lpszSeriNo), lpszOutput );

    string strInfo[16] = {""}, strData = "";
    strData = lpszOutput;
    DecodeFrame( strInfo, strData, ",", 1 );

    char lpszHardWareAddr_decoder[64] = {0}, lpszIPAdddr_decoder[64] = {0}, lpszCreateDateTime_decoder[64] = {0};
    strcpy( lpszHardWareAddr_decoder, (char *)strInfo[0].c_str() );
    strcpy( lpszIPAdddr_decoder, (char *)strInfo[1].c_str() );
    strcpy( lpszCreateDateTime_decoder, (char *)strInfo[2].c_str() );


    /*
    sprintf( szLog, "seri_proc, lpszHardWareAddr = %s, lpszIPAdddr = %s, lpszCreateDateTime = %s", lpszHardWareAddr, lpszIPAddr, lpszCreateDateTime );
    TraceEx( szLog );

    memset( szLog, 0, 1024 );
    sprintf( szLog, "seri_proc, lpszHardWareAddr_decoder = %s, lpszIPAdddr_decoder = %s, lpszCreateDateTime_decoder = %s", lpszHardWareAddr_decoder, lpszIPAdddr_decoder, lpszCreateDateTime_decoder );
    TraceEx( szLog );
    */

    if( (strcmp( lpszHardWareAddr, lpszHardWareAddr_decoder ) != 0 ) || strcmp( lpszIPAddr, lpszIPAdddr_decoder ) != 0
            || strcmp( lpszCreateDateTime, lpszCreateDateTime_decoder ) != 0 || strlen( lpszSeriNo ) == 0 )
    {
        m_bSeriCorrect = false;
    }
    else
    {
        m_bSeriCorrect = true;
    }

    if( m_bSeriCorrect )
    {
        SetLisenceNum( atoi( (char *)strInfo[3].c_str() ) );
        m_iLimitDay = atoi( (char *)strInfo[4].c_str() );

    }
    else
    {
        TraceEx( "序列号验证错误，请与软件开发商联系!" );
        exit( 0 );
    }

	}
	catch(...)
	{
		TraceEx( "error, exception, CData::seri_proc() " );
	}
}

bool CMainDataProc::CheckSeariNo()
{
    //return true;
	try
	{
        /*
        char szLog[512] = {0};
        sprintf(szLog, "check_seari_no, m_lpszSeariCardId = %s, m_lpszCardId = %s", m_lpszSeriCardId, m_lpszCardId );
        TraceEx( szLog );
        */

    if( !m_bSeriCorrect )
	{
		TraceEx( "序列号验证错误，请与软件开发商联系!" );
	}

    }
	catch(...)
	{
		TraceEx( "error, exception, CData::check_seari_no() " );
    }
    return m_bSeriCorrect;
}

void CMainDataProc::RemoveFromAgentTypeList(int iClientId, int iAgentType)
{
	try
	{
	list<PAGENTTYPE> *pAgentTypeList = m_AgentTypeListMap.Find( iAgentType );
	if( pAgentTypeList == NULL )
		return;

	list<PAGENTTYPE>::iterator it;
	for(it = pAgentTypeList->begin(); it != pAgentTypeList->end(); ++it)
	{
		PAGENTTYPE pAgent_type = *it;
		if( pAgent_type == NULL )
			continue;

		if( pAgent_type->iClientId == iClientId )
		{
			it = pAgentTypeList->erase( it );
			if( iAgentType == agent_monitor_type || iAgentType == record_type )
			{
				//若是网关和录音则继续直到删除完毕(一个程序绑定多个分机的情况)
				continue;
			}
			else
			{
				//一个程序绑定一个分机的情况
				break;
			}
		}
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::remove_from_agent_type_list() " );
	}
}

void CMainDataProc::send_msg_to_agent_type(int iAgentType, string *strInfo)
{
	try
	{
    char szLog[256] = {0};
	list<PAGENTTYPE> *pAgentTypeList = m_AgentTypeListMap.Find( iAgentType );
	if( pAgentTypeList == NULL )
	{
        sprintf( szLog, "error, send_msg failed, can not find list, iAgentType = %d", iAgentType );
        TraceEx( szLog );
		return;
	}
	
	char szInBuff[1024*100] = {0};
	list<PAGENTTYPE>::iterator it;
	for(it = pAgentTypeList->begin(); it != pAgentTypeList->end(); ++it)
	{
		PAGENTTYPE pAgent_type = *it;
		if( pAgent_type == NULL )
			continue;

		if( pAgent_type->iClientId > 0 )
		{
            sprintf(szInBuff,"%s|%s|%s|%s|%s", pAgent_type->szDeviceId, (char *)strInfo[0].c_str(),
                    (char *)strInfo[1].c_str(),(char *)strInfo[2].c_str(),(char *)strInfo[3].c_str());
			Send_command( pAgent_type->iClientId, CMD_EXTENSIONMSG_EVENT, RC_EVENT, szInBuff, strlen(szInBuff) );

            sprintf( szLog, "TsapiServer, Cmd_SendMsg_Proc, szInBuff = %s, strlen(szInBuffer) = %d, iDestId = %d, ext = %s",
                     szInBuff, strlen(szInBuff), pAgent_type->iClientId, pAgent_type->szDeviceId );
            TraceEx( szLog );
		}
		else
		{
            sprintf( szLog, "Error AgentStatus, cant find destid, ext = %s", pAgent_type->szDeviceId );
            TraceEx( szLog );
		}
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::send_msg_to_agent_type() " );
	}
}

PEXTINFO CMainDataProc::GetPEByExt(char *szExtNo)
{
	try
	{
	if( szExtNo == NULL )
		return NULL;
	return m_ExtInfoMap.Find( szExtNo );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::get_pe_by_ext() " );
	}
	return NULL;
}

bool CMainDataProc::RecordSysRate()
{
	try
	{
    bool bResult = true;

#ifdef _VC6_
	SYSTEMTIME ct;
	GetLocalTime(&ct);

	sprintf( m_szTempDateTime, "%4d-%2d-%2d 0:0:0", ct.wYear, ct.wMonth, ct.wDay );
#else
    QDateTime current_dt = QDateTime::currentDateTime();
    QString strDT = current_dt.toString("yyyy-MM-dd");
    sprintf( m_szTempDateTime, "%s", (char *)strDT.toStdString().c_str() );
#endif

    char szLog[512] = {0};
    sprintf( szLog, "record_sys_rate, m_lpszDateTime = %s, m_szTempDateTime = %s", m_lpszDateTime, m_szTempDateTime );
    TraceEx( szLog );

	if( strcmp( m_lpszDateTime, m_szTempDateTime ) != 0 && strlen( m_lpszDateTime ) > 0 )
	{
        TraceEx( "recrod_sys_rate, notify, begin to write limie day");
		m_iRunDayNum++;
		WriteLimitDay( m_iRunDayNum );
        TraceEx( "record_sys_rate, notify, write limite day end");
	}

    memset( szLog, 0, 512 );
    sprintf( szLog, "record_sys_rate, m_iRunnda = %d, lda = %d", m_iRunDayNum, m_iLimitDay);
    TraceEx( szLog );
	if( m_iRunDayNum >= m_iLimitDay )
	{
        //m_bSeriCorrect = false;
        bResult = false;
	}

#ifdef _VC6_
	sprintf( m_lpszDateTime, "%4d-%2d-%2d 0:0:0", ct.wYear, ct.wMonth, ct.wDay );
#else
    sprintf( m_lpszDateTime, "%s", (char *)strDT.toStdString().c_str() );
#endif

	return bResult;
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::record_sys_rate() " );
	}
    return true;
}

int CMainDataProc::ReadLimitDay()
{
	try
	{
	char lpszRunDayNum[32] = {0};
#ifdef _VC6_
	GetPrivateProfileString( "Setting", "systime", "0", lpszRunDayNum, 32, m_lpszSysFile );
	m_iRunDayNum = atoi( lpszRunDayNum );
	WritePrivateProfileString("Setting", "systime",lpszRunDayNum, m_lpszSysFile);
#else
    QSettings *configIni = new QSettings(m_lpszSysFile, QSettings::IniFormat);

    QString strRunDayNum = configIni->value("/Setting/systime").toString();
    m_iRunDayNum = atoi( (char *)strRunDayNum.toStdString().c_str() );
    configIni->setValue("/Setting/systime", strRunDayNum);

    delete configIni;
#endif
	return m_iRunDayNum;
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::read_limit_day() " );
	}
	return 0;
}

int CMainDataProc::WriteLimitDay(int iRunDayNum)
{
	try
	{
	char lpszRunDayNum[32] = {0};
	sprintf( lpszRunDayNum, "%d", iRunDayNum );
#ifdef _VC6_
	WritePrivateProfileString("Setting", "systime",lpszRunDayNum, m_lpszSysFile);
#else
    QSettings *configIni = new QSettings(m_lpszSysFile, QSettings::IniFormat);

    configIni->setValue("/Setting/systime", lpszRunDayNum);

    delete configIni;
#endif
	return 0;
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::write_limit_day() " );
	}
	return 0;
}

void CMainDataProc::RemoveExtFromVServer(char *ext, int iClientId)
{
	try
	{
	if( ext == NULL || iClientId <= 0 )
		return;

    char szLog[256] = {0};
    sprintf( szLog, "remove_ext_from_VServer, ext = %s, client_id = %d", ext, iClientId );
    TraceEx( szLog );

	list<PSOCKEXT> *plist = m_SockExtListMap.Find( iClientId );
	if(  plist == NULL )
	{
        sprintf( szLog, "error, cant find plist, client_id = %d", iClientId );
        TraceEx( szLog );
		return;
	}
	
	list<PSOCKEXT>::iterator it;
	for( it = plist->begin(); it != plist->end(); ++it )
	{
		PSOCKEXT pSockExt = *it;
		if( pSockExt != NULL )
		{
			char *lpszExtNo = NULL;
			lpszExtNo = (char *)pSockExt->strExtNo.c_str();
			if( lpszExtNo != NULL && strcmp( lpszExtNo, ext ) == 0 )
			{
				m_ExtSockMap.Remove( lpszExtNo );

				DelExtInfo( lpszExtNo );

				if( lpszExtNo[0] == 'v' || lpszExtNo[0] == 'V' )
				{
				}
				else
				{
					m_iCurrentClientNum--;
					if( m_iCurrentClientNum <= 0 )
						m_iCurrentClientNum = 0;
					if( strlen( m_szQueueExt ) > 0 )
						SendLogoutCmd( iClientId, lpszExtNo, "" );
				}
                sprintf( szLog, "remove_ext_from_VServer, remove extno = %s, m_iCurrentClientNum = %d",
                         lpszExtNo, m_iCurrentClientNum );
                TraceEx( szLog );
				
				plist->erase( it );

				delete pSockExt;
				pSockExt = NULL;

				break;
			}
		}		
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::remove_ext_from_VServer() " );
	}
}

void CMainDataProc::ReloadListCtrl(int iClientId)
{
	try
	{
	if( iClientId <= 0 )
		return;

    char szLog[256] = {0};
	list<PSOCKEXT> *pSockExtList = m_SockExtListMap.Find( iClientId );
	if(  pSockExtList == NULL )
	{
        sprintf( szLog, "error, reload_list_ctrl, cant find pSockExtList, client_id = %d", iClientId );
        TraceEx( szLog );
		return;
	}

    string strSock = "";
    char szTempSock[32] = {0};
    sprintf( szTempSock, "%d", iClientId );
    strSock = szTempSock;

    string strNum = "", strExt = "";
    char szTempNum[32] = {0};
    int iListSize = pSockExtList->size();
    sprintf( szTempNum, "%d", pSockExtList->size() );
    strNum = szTempNum;

	list<PSOCKEXT>::iterator it;
	for( it = pSockExtList->begin(); it != pSockExtList->end(); ++it )
	{
		PSOCKEXT pSockExt = *it;
		if( pSockExt != NULL )
		{
			char *lpszExtNo = NULL;
			lpszExtNo = (char *)pSockExt->strExtNo.c_str();
			if( lpszExtNo != NULL )
			{
                if( strExt.length() == 0 )
				{
					strExt = lpszExtNo;
				}
				else
				{
					strExt += ",";
					strExt += lpszExtNo;
				}
			}
		}		
	}

#ifdef _VC6_
	for(int iItem = 0; iItem < g_lpthis->m_wndlist.GetItemCount(); iItem++)
	{
		if( g_lpthis->m_wndlist.GetItemText( iItem, col_sock ) == strSock )
		{
			g_lpthis->m_wndlist.SetItemText( iItem, col_num, (LPSTR &)strNum );
			g_lpthis->m_wndlist.SetItemText( iItem, col_extno, (LPSTR &)strExt );

			break;
		}
	}
#else
    if( m_RegExtInfoList.size() <= 0 )
        return;

    list<REGEXTINFOLIST *>::iterator reg_it;
    for( reg_it = m_RegExtInfoList.begin(); reg_it != m_RegExtInfoList.end(); ++reg_it )
    {
        REGEXTINFOLIST *regExtInfo = *reg_it;
        if( regExtInfo != NULL && regExtInfo->iClientId == iClientId )
        {
            regExtInfo->iNum = iListSize;
            regExtInfo->strExt = strExt;
        }
    }
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::reload_list_ctrl() " );
	}
}

bool CMainDataProc::IsRegisterContinue(int iClientId)
{
	try
	{
    char szLog[256] = {0};
    bool bResult = true;
    if( m_MainEntry.m_iMainCTIRunning == 1 && m_MainEntry.m_iServerFlag == 1 && m_MainEntry.m_iMainServer == 0 )
	{
        bResult = false;
        sprintf( szLog, "is_reg_continue, m_iMainCTIRunning = %d, bResult = %d, iClientId = %d",
                 m_MainEntry.m_iMainCTIRunning, bResult, iClientId );
        TraceEx( szLog );

		OnClientClose( iClientId );
        m_MainEntry.CloseSockFunc( iClientId );
	}

	return bResult;
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::is_register_continue() " );
	}
    return true;
}

void CMainDataProc::LoadVDN()
{
	try
	{
	char szVDNNum[32] = {0};
#ifdef _VC6_
	GetPrivateProfileString( "VDN", "VDNNum", "0", szVDNNum, 32, ".\\TsapiServer.ini" );
	m_nVDNNum = atoi( szVDNNum );
	WritePrivateProfileString("VDN","VDNNum",szVDNNum,".\\TsapiServer.ini");
#else
    QSettings *configIni = new QSettings("TsapiServer.ini", QSettings::IniFormat);

    QString strVDNNum = configIni->value("/VDN/VDNNum").toString();
    m_nVDNNum = atoi( (char *)strVDNNum.toStdString().c_str());
    configIni->setValue("/VDN/VDNNum", strVDNNum);


#endif

	char Writed[32] = {0};
	for (int i=0; i<m_nVDNNum; i++) 
	{ 
		char lpszVdn[32] = {0};
		sprintf(Writed,"vdn%d",i); 
#ifdef _VC6_
		GetPrivateProfileString("VDN",Writed,"",lpszVdn,32,".\\TsapiServer.ini"); 
		m_strVDN[i] = lpszVdn;
		WritePrivateProfileString("VDN", Writed,m_strVDN[i], ".\\TsapiServer.ini"); 
#else
        char szSection[32] = {0};
        sprintf( szSection, "/VDN/%s", Writed );
        QString strVDN = configIni->value(szSection).toString();
        m_nVDNNum = atoi( (char *)strVDN.toStdString().c_str());
        configIni->setValue(szSection, strVDN);
#endif
	}

#ifndef _VC6_
    delete configIni;
#endif
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::load_vdn() " );
	}
}

void CMainDataProc::AddQueueList(string QueDevice, string CallID, char *callingDeviceId, char *calledDeviceId)
{
	try
	{
	if( callingDeviceId == NULL || calledDeviceId == NULL )
		return;
    char szLog[256] = {0};
	list<PQUEUEINFO> *pQueList = m_QueueListMap.Find(QueDevice);
	if( pQueList == NULL )
	{
		pQueList = new list<PQUEUEINFO>;
		if( pQueList == NULL )
			return;

		m_QueueListMap.Add( QueDevice, pQueList );
	}

    bool bFind = false;
	list<PQUEUEINFO>::iterator it;
	for( it = pQueList->begin(); it != pQueList->end(); ++it )
	{
		PQUEUEINFO pQueueInfo = *it;
		if( pQueueInfo != NULL && pQueueInfo->CallID == CallID )
		{
            bFind = true;
			break;
		}
	}

	if( !bFind )
	{
		PQUEUEINFO pQue_info = new QUEUEINFO;
		if( pQue_info != NULL )
		{
			pQue_info->CallID = CallID;
			memset( pQue_info->callingDeviceId, 0, 32 );
			memset( pQue_info->calledDeviceId, 0, 32 );
			strcpy( pQue_info->callingDeviceId, callingDeviceId );	//callingDeviceId: IVR分机号
			strcpy( pQue_info->calledDeviceId, calledDeviceId );	//calledDeviceId: VDN号

			pQueList->push_back( pQue_info );

			char lpszTempCallID[64] = {0};

            if( m_MainEntry.m_iNoIVR == 1 )
			{
				strcpy( lpszTempCallID, (char *)CallID.c_str() );
			}
			else
			{
				char *lpszIVRCallId = m_CallID_IVRIDMap.Find( CallID );
				if( lpszIVRCallId != NULL )
				{
					strcpy( lpszTempCallID, lpszIVRCallId );
				}
			}


			//入队列
			int iDestId = m_ExtSockMap.Find( m_szQueueExt );
			if( iDestId > 0 )
			{
				char szInBuff[1024] = {0};
				sprintf(szInBuff,"%s|%s|%s|%s|%s",m_szQueueExt,pQue_info->callingDeviceId,"InHardQueue",pQue_info->calledDeviceId,lpszTempCallID);
				Send_command( iDestId, CMD_EXTENSIONMSG_EVENT, RC_EVENT, szInBuff, strlen(szInBuff) );

                sprintf( szLog, "TsapiServer, Cmd_SendMsg_Proc, szInBuff = %s, strlen(szInBuffer) = %d", szInBuff, strlen(szInBuff) );
                TraceEx( szLog );
			}
		}
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::add_queue_list() " );
	}
}

void CMainDataProc::OutQue(string QueDevice, string CallID)
{
	try
	{
    char szLog[256] = {0};
	int iDestId = m_ExtSockMap.Find( m_szQueueExt );
    if( m_MainEntry.m_iNoIVR == 1 )
	{
		if( iDestId > 0 )
		{
			char szInBuff[1024] = {0};
			sprintf(szInBuff,"%s|%s|%s|%s|%s",m_szQueueExt, QueDevice.c_str(),"OutHardQueue", QueDevice.c_str(),CallID.c_str());
			Send_command( iDestId, CMD_EXTENSIONMSG_EVENT, RC_EVENT, szInBuff, strlen(szInBuff) );

            sprintf( szLog, "TsapiServer, Cmd_SendMsg_Proc, szInBuff = %s, strlen(szInBuffer) = %d", szInBuff, strlen(szInBuff) );
            TraceEx( szLog );
		}
		return;
	}

	list<PQUEUEINFO> *pQueList = m_QueueListMap.Find(QueDevice);
	if( pQueList == NULL )
		return;

	

	list<PQUEUEINFO>::iterator it;
	for( it = pQueList->begin(); it != pQueList->end(); ++it )
	{
		PQUEUEINFO pQueueInfo = *it;
		if( pQueueInfo != NULL && pQueueInfo->CallID == CallID )
		{
			//出队列
			it = pQueList->erase( it );
			
			char lpszTempCallID[64] = {0};

            if( m_MainEntry.m_iNoIVR == 1 )
			{
				strcpy( lpszTempCallID, (char *)CallID.c_str() );
			}
			else
			{
				char *lpszIVRCallId = m_CallID_IVRIDMap.Find( CallID );
				if( lpszIVRCallId != NULL )
				{
					strcpy( lpszTempCallID, lpszIVRCallId );					
				}
			}

			
			if( iDestId > 0 )
			{
				char szInBuff[1024] = {0};
				sprintf(szInBuff,"%s|%s|%s|%s|%s",m_szQueueExt,pQueueInfo->callingDeviceId,"OutHardQueue",pQueueInfo->calledDeviceId,lpszTempCallID);
				Send_command( iDestId, CMD_EXTENSIONMSG_EVENT, RC_EVENT, szInBuff, strlen(szInBuff) );

                sprintf( szLog, "TsapiServer, Cmd_SendMsg_Proc, szInBuff = %s, strlen(szInBuffer) = %d", szInBuff, strlen(szInBuff) );
                TraceEx( szLog );
			}

			delete pQueueInfo;
			pQueueInfo = NULL;
			break;
		}
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::out_que() " );
	}
}

bool CMainDataProc::IsVDNNumber(char *lpszVDN)
{
	try
	{
    bool bResult = false;
	
	for(int i = 0; i < m_nVDNNum; i++)
	{
        if( strcmp( (char *)m_strVDN[i].c_str(), lpszVDN ) == 0 )
		{
            bResult = true;
			break;
		}
	}

	return bResult;
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::is_vdn_number() " );
	}
    return false;
}

void CMainDataProc::AddCallIDInfo(string callID, char *lpszIVRCallId)
{
	try
	{
	if( lpszIVRCallId == NULL )
		return;

    char szLog[256] = {0};
    sprintf( szLog, "add_call_id_info, callID = %s, lpszIVRCallId = %s",
             callID.c_str(), lpszIVRCallId );
    TraceEx( szLog );

	char *ivr_call_id = new char[256];
	if( ivr_call_id != NULL )
	{
		memset( ivr_call_id, 0, 256 );
		strcpy( ivr_call_id, lpszIVRCallId );
		m_CallID_IVRIDMap.Add( callID, ivr_call_id );
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::add_callid_info() " );
	}
}

void CMainDataProc::DelCallIDInfo(string callID)
{
	try
	{
    char szLog[256] = {0};
    sprintf( szLog, "del_call_id_info, callID = %s", callID.c_str() );
    TraceEx( szLog );

	char *ivr_call_id = m_CallID_IVRIDMap.Find( callID );
	if( ivr_call_id != NULL )
	{
		m_CallID_IVRIDMap.Remove( callID );
		delete[] ivr_call_id;
		ivr_call_id = NULL;
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::del_call_id_info() " );
	}
}

void CMainDataProc::AddIVRCallIDInfo(string ivr_num, char *lpszIVRCallId)
{
	try
	{
	if( lpszIVRCallId == NULL )
		return;

    char szLog[256] = {0};
    sprintf( szLog, "add_ivr_callID_info, ivr = %s, lpszIVRCallId = %s",
             ivr_num.c_str(), lpszIVRCallId );
    TraceEx( szLog );

	char *ivr_call_id = NULL;
	ivr_call_id = m_IVR_CALLIDMap.Find( ivr_num );
	if( ivr_call_id != NULL )
	{
		memset( ivr_call_id, 0, 256 );
		strcpy( ivr_call_id, lpszIVRCallId );
	}
	else
	{
		ivr_call_id = new char[256];
		if( ivr_call_id != NULL )
		{
			memset( ivr_call_id, 0, 256 );
			strcpy( ivr_call_id, lpszIVRCallId );
			m_IVR_CALLIDMap.Add( ivr_num, ivr_call_id );
		}
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::add_ivr_callid_info() " );
	}
}

void CMainDataProc::DelIVRCallIDInfo(string ivr_num)
{
	try
	{
    char szLog[256] = {0};
    sprintf( szLog, "del_ivr_callID_info, ivr = %s", ivr_num.c_str() );
    TraceEx( szLog );

	char *ivr_call_id = m_IVR_CALLIDMap.Find( ivr_num );
	if( ivr_call_id != NULL )
	{
		m_IVR_CALLIDMap.Remove( ivr_num );
		delete[] ivr_call_id;
		ivr_call_id = NULL;
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::del_ive_callid_info() " );
	}
}

void CMainDataProc::AddExtAgent(char *lpszExt, char *lpszAgentId)
{
	try
	{
	if( lpszExt == NULL || lpszAgentId == NULL )
		return;

    char szLog[256] = {0};
    sprintf( szLog, "add_ext_agent, ext = %s, agent = %s", lpszExt, lpszAgentId );
    TraceEx( szLog );

	char *agent = m_ExtAgentMap.Find( lpszExt );
	if( agent != NULL )
	{
		delete[] agent;
		agent = NULL;
		m_ExtAgentMap.Remove( lpszExt );
	}

	agent = new char[128];
	if( agent == NULL )
		return;

	memset( agent, 0, 128 );
	strcpy( agent, lpszAgentId );
	m_ExtAgentMap.Add( lpszExt, lpszAgentId );
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::add_ext_agent() " );
	}
}

void CMainDataProc::DelExtAgentMap(char *lpszExt, char *lpszAgentId)
{
	try
	{
	if( lpszExt == NULL || lpszAgentId == NULL )
		return;

	char *agent = m_ExtAgentMap.Find( lpszExt );
	if( agent != NULL )
	{
		delete[] agent;
		agent = NULL;
		m_ExtAgentMap.Remove( lpszExt );
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::del_ext_agent_map() " );
	}
}

void CMainDataProc::SendMsg2Extension(char *SourceNumber, char *DestNumber, char *Param1, char *Param2, char *Param3)
{
	//strSendData.Format( "%s|%s|%s|%s|%s", strExt, "VRecord", "CallerIdComing", m_strCallerId, m_strCalledId );
	try
	{
	char szInBuff[1024*100] = {0};
	int iDestId = 0;
    char szLog[1024*100] = {0};
	
	iDestId = GetClientIdByExt( DestNumber );

	sprintf(szInBuff,"%s|%s|%s|%s|%s",DestNumber,SourceNumber,Param1,Param2,Param3);
	if( iDestId > 0 )
	{
		Send_command( iDestId, CMD_EXTENSIONMSG_EVENT, RC_EVENT, szInBuff, strlen(szInBuff) );
        sprintf( szLog, "TapiServer, Cmd_SendMsg_Proc, szInBuff = %s, strlen(szInBuffer) = %d", szInBuff, strlen(szInBuff) );
        TraceEx( szLog );
	}
	else
	{
        sprintf( szLog, "error, can not find iDestId, szInBuff = %s", szInBuff );
        TraceEx( szLog );
	}
	}
	catch(...)
	{
		TraceEx( "error, exception, CData::send_msg_to_extension() " );
	}
}

void CMainDataProc::ReadRegAgentIniFile()
{
	for(int i = 0; i < m_iMaxClientNum; i++)
	{
		char agent_index[32] = {0}, ext_index[32] = {0};
		char agent_id[32] = {0}, ext_no[32] = {0};

#ifdef _VC6_
		sprintf( agent_index, "agent%d", i );
		GetPrivateProfileString("Setting",agent_index,"",agent_id,32,".\\Agent.ini");
		WritePrivateProfileString("Setting",agent_index,agent_id,".\\Agent.ini");

		sprintf( ext_index, "ext%d", i );
		GetPrivateProfileString("Setting",ext_index,"",ext_no,32,".\\Agent.ini");
		WritePrivateProfileString("Setting",ext_index,ext_no,".\\Agent.ini");
#else
        QSettings *configIni = new QSettings("Agent.ini", QSettings::IniFormat);

        sprintf( agent_index, "/Setting/agent%d", i );
        QString strAgentId = configIni->value(agent_index).toString();
        strcpy( agent_id, (char *)strAgentId.toStdString().c_str() );
        configIni->setValue(agent_index, agent_id);

        sprintf( ext_index, "/Setting/ext%d", i );
        QString strExtNO = configIni->value(ext_index).toString();
        strcpy( ext_no, (char *)strExtNO.toStdString().c_str() );
        configIni->setValue(ext_index, ext_no);

        delete configIni;
#endif

		strcpy( m_AgentExtInfo[i].lpszAgentId, agent_id );
		strcpy( m_AgentExtInfo[i].lpszExtNo, ext_no );
	}
}

void CMainDataProc::WriteAgentExtInfo(char *lpszAgentId, char *lpszExtNo)
{
	if( lpszAgentId == NULL || lpszExtNo == NULL )
		return;

    char szLog[256] = {0};
    int iIndex = -1, i;
    for(i = 0; i < m_iMaxClientNum; i++)
	{
		if( strcmp( m_AgentExtInfo[i].lpszExtNo, lpszExtNo ) == 0 )
		{
			iIndex = i;
			break;
		}
	}

	if( iIndex < 0 )
		return;

	sprintf( m_AgentExtInfo[i].lpszAgentId, "%s", lpszAgentId );

	char agent_index[32] = {0};
#ifdef _VC6_
	sprintf( agent_index, "agent%d", iIndex );
	WritePrivateProfileString("Setting",agent_index, lpszAgentId,".\\Agent.ini");
#else
    QSettings *configIni = new QSettings("Agent.ini", QSettings::IniFormat);
    sprintf( agent_index, "/Setting/agent%d", iIndex );
    configIni->setValue(agent_index, lpszAgentId);

    delete configIni;
#endif

    sprintf( szLog, "write_agent_ext_info, agent_id = %s, ext_no = %s", lpszAgentId, lpszExtNo );
    TraceEx( szLog );
}

void CMainDataProc::GetFileCreateTime()
{
#ifdef _windows_
    QFileInfo info( "encrptinterface.dll" );
#else
    QFileInfo info( "libencrptinterface.so" );
#endif

    QDateTime t = info.created();
    QString strDT = t.toString("yyyy-MM-dd hh:mm:ss");
    char szCreateTime[128] = {0};
    sprintf( szCreateTime, "created time = %s", (char *)strDT.toStdString().c_str());
    TraceEx( szCreateTime );

    char szDateTime[128] = {0};
    strcpy( szDateTime, (char *)strDT.toStdString().c_str() );

    char szInput[32] = {0};
    szInput[0] = szDateTime[5];
    szInput[1] = szDateTime[6];
    szInput[2] = szDateTime[8];
    szInput[3] = szDateTime[9];
    szInput[4] = szDateTime[12];
    szInput[5] = szDateTime[15];
    szInput[6] = szDateTime[17];
    szInput[7] = szDateTime[18];

    memset( m_lpszCardId, 0, 256 );
    strcpy( m_lpszCardId, szInput );
}

void CMainDataProc::InitDataProc()
{
    try
    {
    memset( m_lpUpdateFile, 0, 512 );
    strcpy( m_lpUpdateFile, "Update.ini" );
    m_RegExtInfoList.clear();
    m_TrunkList.clear();
    m_ACDNumList.clear();

    memset( m_AgentExtInfo, 0, sizeof(AGENT_EXT_INFO) );//* MAX_AGENTNUM );
    memset( m_lpszDateTime, 0, 128 );

    m_bSeriCorrect = true;
    memset( m_lpszCardId, 0, 256 );
    memset( m_AgentType, 0, sizeof(AGENTTYPE) * MAX_AGENTNUM );
    memset( m_szQueueExt, 0, 32 );
    m_iCurrentClientNum = 0;
    m_iMaxClientNum = 2;
    m_nMoniNum = 0;
    m_nIvrNum = 0;

//    return;
#ifdef _VC6_
    InitializeCriticalSection( &m_MoniCS );
#endif

 //   return;
#ifdef _VC6_
    SYSTEMTIME ct;
    GetLocalTime(&ct);
    sprintf( m_lpszDateTime, "%4d-%2d-%2d 0:0:0", ct.wYear, ct.wMonth, ct.wDay );
#else

    QDateTime current_dt = QDateTime::currentDateTime();//
    QString strDate = current_dt.toString("yyyy-MM-dd");
    sprintf( m_lpszDateTime, "%s", (char *)strDate.toStdString().c_str() );

    GetFileCreateTime();
    //return;
#endif
    m_bCheckSer = true;
    memset( m_lpszDefaultCard, 0, 32 );
    strcpy( m_lpszDefaultCard, "13579" );
    m_iLimitDay = 1;
    char txtCurPath[256] = {0};

#ifdef _VC6_
    GetSystemDirectory(txtCurPath,sizeof(txtCurPath));
    wsprintf(m_lpszSysFile,"%s\\wintime.ini",txtCurPath);
#endif

#ifdef _linux_
    strcpy(m_lpszSysFile,"wintime.ini");
#endif

    ReadLimitDay();
    TraceEx ("-----init_data_proc end ------");
    }
    catch(...)
    {
        TraceEx( "error, exception, CData::Cdata_proc() " );
    }
}

void CMainDataProc::Cmd_Download_File_Proc(int iClientId, char *buf)
{
    if( buf == NULL || iClientId <= 0 )
        return;

    PACKET_HEAD *pHead = (PACKET_HEAD *)buf;
    if( pHead == NULL )
        return;

    DOWNLOAD_FILE_INFO *pDownloadInfo = (DOWNLOAD_FILE_INFO *)(buf + sizeof(PACKET_HEAD));
    if( pDownloadInfo == NULL )
        return;

    TraceEx( "cmd_download_file_proc, remote_path = %s", pDownloadInfo->lpszRemotePath );

    DOWNLOAD_DATA_INFO download_data_info[16];
    memset( download_data_info, 0, sizeof(DOWNLOAD_DATA_INFO) * 16 );
    strcpy( download_data_info[0].lpszRemoteFileFolder, pDownloadInfo->lpszRemotePath );

    int iTotalDownloadDataNum = 1;

    send_download_event_to_client( iClientId, pHead, iTotalDownloadDataNum, pDownloadInfo, download_data_info );
}

void CMainDataProc::send_download_event_to_client(int iClientId, PACKET_HEAD *pHead, int iTotalDownloadDataNum,
        DOWNLOAD_FILE_INFO *pDownloadInfo, DOWNLOAD_DATA_INFO *pDownload_Data_Info)
{
    if( pHead == NULL || pDownloadInfo == NULL || pDownload_Data_Info == NULL )
        return;

    char lpszSendBuffer[1024*10] = {0};
    pHead->dwSize = sizeof(DOWNLOAD_FILE_INFO) + sizeof(int) + sizeof(DOWNLOAD_DATA_INFO) * iTotalDownloadDataNum;
    memcpy( lpszSendBuffer, pHead, sizeof(PACKET_HEAD) );
    memcpy( lpszSendBuffer + sizeof(PACKET_HEAD), pDownloadInfo, sizeof(DOWNLOAD_FILE_INFO) );
    memcpy( lpszSendBuffer + sizeof(PACKET_HEAD) + sizeof(DOWNLOAD_FILE_INFO), &iTotalDownloadDataNum, sizeof(int) );
    memcpy( lpszSendBuffer + sizeof(PACKET_HEAD) + sizeof(DOWNLOAD_FILE_INFO) + sizeof(int), pDownload_Data_Info,
        sizeof(DOWNLOAD_DATA_INFO) * iTotalDownloadDataNum );

    m_MainEntry.SendToClientFunc( iClientId, lpszSendBuffer, sizeof(PACKET_HEAD) + pHead->dwSize );
    char szLog[1024] =  {0};
    sprintf( szLog, "send_download_event_to_client, iClientId = %d,  sizeof(DOWNLOAD_FILE_INFO) = %d, sizeof(DOWNLOAD_DATA_INFO) = %d,  need_send_len = %d",
             iClientId, sizeof(DOWNLOAD_FILE_INFO), sizeof(DOWNLOAD_DATA_INFO), sizeof(PACKET_HEAD) + pHead->dwSize );
    TraceEx( szLog );
}

void CMainDataProc::Cmd_Request_Download_Folder_Proc(int iClientId, char *lpszBuffer)
{
    if( lpszBuffer == NULL || iClientId <= 0 )
        return;

    PACKET_HEAD *pHead = (PACKET_HEAD *)lpszBuffer;
    if( pHead == NULL )
        return;

    char *buf = new char[sizeof(PACKET_HEAD) + pHead->dwSize + sizeof(int) ];
    if( buf == NULL )
        return;

    memset(buf, 0, sizeof(PACKET_HEAD) + pHead->dwSize + sizeof(int) );
    memcpy(buf, lpszBuffer, sizeof(PACKET_HEAD) + pHead->dwSize);
    memcpy(buf + sizeof(PACKET_HEAD) + pHead->dwSize, &iClientId, sizeof(int) );

    char szLog[1024] = {0};
    sprintf( szLog, "cmd_request_download_folder, iClientId = %d", iClientId );
    TraceEx( szLog );

    pthread_t iThreadId;
    if( pthread_create(&iThreadId, NULL, DownloadFolderThread, buf) != 0 )
    {
        TraceEx("Create pthread error!\n");
    }

}

void CMainDataProc::FindFile(int iClientId, PACKET_HEAD *pHead, DOWNLOAD_FILE_INFO *pDownload_Info,
                         DOWNLOAD_DATA_INFO *pDownload_Data_Info, char *lpszPathName)
{
    if( pHead == NULL || pDownload_Info == NULL || pDownload_Data_Info == NULL || lpszPathName == NULL )
        return;

    char szLog[1024*2] = {0};
    QDir dir(lpszPathName);
    if (!dir.exists())
        return;

    dir.setFilter(QDir::Dirs|QDir::Files);
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList list = dir.entryInfoList();

    int i=0;
    do{
        QFileInfo fileInfo = list.at(i);
        if(fileInfo.fileName()=="."|fileInfo.fileName()=="..")
        {
             i++;
             continue;
        }
        bool bisDir=fileInfo.isDir();
        if(bisDir)
        {
            /*
            std::cout << qPrintable(QString("%1 %2 %3").arg(fileInfo.size(), 10)
                                                 .arg(fileInfo.fileName(),10).arg(fileInfo.path()))<<endl;
            */
            FindFile(iClientId, pHead, pDownload_Info, pDownload_Data_Info, (char *)fileInfo.filePath().toStdString().c_str());

        }
        else
        {
            int iTotalDownloadDataNum = 1;
            memset( pDownload_Data_Info->lpszRemoteFilePathName, 0, MAX_PATH);

            int iLen = strlen( lpszPathName );
#ifdef _windows_
            if( lpszPathName[iLen-1] == '\\' )
#else
            if( lpszPathName[iLen-1] == '/' || lpszPathName[iLen-1] == '//' )
#endif
            {
                sprintf( pDownload_Data_Info->lpszRemoteFilePathName, "%s%s", lpszPathName, (char *)fileInfo.fileName().toStdString().c_str());
            }
            else
            {
#ifdef _windows_
                sprintf( pDownload_Data_Info->lpszRemoteFilePathName, "%s\\%s", lpszPathName, (char *)fileInfo.fileName().toStdString().c_str() );
#else
                sprintf( pDownload_Data_Info->lpszRemoteFilePathName, "%s/%s", lpszPathName, (char *)fileInfo.fileName().toStdString().c_str() );
#endif
            }

            pDownload_Data_Info->lFileSize = fileInfo.size();
            pDownload_Info->iRemoteFileType = FILE_NAME;
            pDownload_Info->iOrientPos++;	//此字段借用来作为文件夹下总的文件数量
            pHead->cmd = CMD_DOWNLOADFILE;

            send_download_event_to_client( iClientId, pHead, iTotalDownloadDataNum, pDownload_Info, pDownload_Data_Info );

            sprintf( szLog, "find_file, iOrientPos = %d, remote_file_path_name = %s, iRemoteFileType = %d",
                     pDownload_Info->iOrientPos, pDownload_Data_Info->lpszRemoteFilePathName, pDownload_Info->iRemoteFileType );
            TraceEx( szLog );

            usleep( 200*1000 );

            /*
            std::cout << qPrintable(QString("%1 %2 %3").arg(fileInfo.size(), 10)
                                                 .arg(fileInfo.fileName(),10).arg(fileInfo.path()))<<endl;
            */
        }
        i++;
    }
    while(i<list.size());

    /*
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    char DirSpec[MAX_PATH + 1];// 指定路径
    DWORD dwError;

    strncpy (DirSpec, lpszPathName, strlen(lpszPathName) + 1);
    strncat (DirSpec, "/*", 3);

    TraceEx( "find_file, lpszPathName = %s, dir_spec = %s", lpszPathName, DirSpec );

    hFind = FindFirstFile(DirSpec, &FindFileData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        TraceEx ("Invalid file handle. Error is %u ", GetLastError());
        return ;
    }
    else
    {
        if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY )
        {
            //printf ("　　%s ", FindFileData.cFileName);   //找到文件

            int iTotalDownloadDataNum = 1;
            memset( pDownload_Data_Info->lpszRemoteFilePathName, 0, MAX_PATH);

            int iLen = strlen( lpszPathName );
            if( lpszPathName[iLen-1] == '\\' )
            {
                sprintf( pDownload_Data_Info->lpszRemoteFilePathName, "%s%s", lpszPathName, FindFileData.cFileName );
            }
            else
            {
                sprintf( pDownload_Data_Info->lpszRemoteFilePathName, "%s\\%s", lpszPathName, FindFileData.cFileName );
            }

//			sprintf( pDownload_Data_Info->lpszRemoteFilePathName, "%s\\%s", lpszPathName, FindFileData.cFileName );
            pDownload_Data_Info->lFileSize = FindFileData.nFileSizeLow;
            pDownload_Info->iRemoteFileType = FILE_NAME;
            pDownload_Info->iOrientPos++;	//此字段借用来作为文件夹下总的文件数量
            pHead->cmd = CMD_DOWNLOADFILE;

            send_download_event_to_client( iClientId, pHead, iTotalDownloadDataNum, pDownload_Info, pDownload_Data_Info );

            TraceEx( "find_file, iOrientPos = %d, remote_file_path_name = %s, iRemoteFileType = %d",
                pDownload_Info->iOrientPos, pDownload_Data_Info->lpszRemoteFilePathName, pDownload_Info->iRemoteFileType );

            Sleep( 200 );
        }
        else if(FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY
            && strcmp(FindFileData.cFileName, ".") != 0
            && strcmp(FindFileData.cFileName, "..") != 0)
        {   //找到目录
            char Dir[MAX_PATH + 1];
            strcpy(Dir, lpszPathName);

            int iLen = strlen( lpszPathName );
            if( lpszPathName[iLen-1] != '\\' )
                strcat( Dir, "\\" );

            //strncat(Dir, "/", 2);
            strcat(Dir, FindFileData.cFileName);

            FindFile(iClientId, pHead, pDownload_Info, pDownload_Data_Info, Dir);
        }

        while (FindNextFile(hFind, &FindFileData) != 0)
        {
            if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
            {   //找到文件
                //printf ("　　%s ", FindFileData.cFileName);

                int iTotalDownloadDataNum = 1;
                memset( pDownload_Data_Info->lpszRemoteFilePathName, 0, MAX_PATH);

                int iLen = strlen( lpszPathName );
                if( lpszPathName[iLen-1] == '\\' )
                {
                    sprintf( pDownload_Data_Info->lpszRemoteFilePathName, "%s%s", lpszPathName, FindFileData.cFileName );
                }
                else
                {
                    sprintf( pDownload_Data_Info->lpszRemoteFilePathName, "%s\\%s", lpszPathName, FindFileData.cFileName );
                }

                pDownload_Data_Info->lFileSize = FindFileData.nFileSizeLow;
                pDownload_Info->iRemoteFileType = FILE_NAME;
                pDownload_Info->iOrientPos++;	//此字段借用来作为文件夹下总的文件数量
                pHead->cmd = CMD_DOWNLOADFILE;

                send_download_event_to_client( iClientId, pHead, iTotalDownloadDataNum, pDownload_Info, pDownload_Data_Info );

                TraceEx( "find_file, iOrientPos = %d, remote_file_path_name = %s, iRemoteFileType = %d",
                    pDownload_Info->iOrientPos, pDownload_Data_Info->lpszRemoteFilePathName, pDownload_Info->iRemoteFileType );

                Sleep( 200 );
            }
            else if(FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY
                && strcmp(FindFileData.cFileName, ".") != 0
                && strcmp(FindFileData.cFileName, "..") != 0)
            { //找到目录
                char Dir[MAX_PATH + 1];
                strcpy(Dir, lpszPathName);

                int iLen = strlen( lpszPathName );
                if( lpszPathName[iLen-1] != '\\' )
                    strcat( Dir, "\\" );

 //               strncat(Dir, "/", 2);
                strcat(Dir, FindFileData.cFileName);
                FindFile(iClientId, pHead, pDownload_Info, pDownload_Data_Info, Dir);
            }
        }

        dwError = GetLastError();
        FindClose(hFind);
        if (dwError != ERROR_NO_MORE_FILES)
        {
            TraceEx ("FindNextFile error. Error is %u ", dwError);
            return;
        }
    }
    */
}

AGENT_STATE *CMainDataProc::GetAgentStateByExt(char *lpszExt)
{
    AGENT_STATE *pAgentState = NULL;
    if( lpszExt == NULL )
        return NULL;

    if( m_AgentStateList.size() <= 0 )
        return NULL;

    list<AGENT_STATE *>::iterator it;
    for( it = m_AgentStateList.begin(); it != m_AgentStateList.end(); ++it)
    {
        AGENT_STATE *pState = *it;
        if( pState != NULL )
        {
            if( strcmp( pState->szDeviceId, lpszExt) == 0 )
            {
                pAgentState = pState;
                break;
            }
        }
    }

    return pAgentState;
}

void CMainDataProc::Cmd_Start_Record_Proc(int iClientId, char *buf, string *strInfo)
{
    //buf = deviceId|lpszRecordFileName
    m_MainEntry.m_CtiCtrl.CstaStartRecord( iClientId, (char *)strInfo[0].c_str(), (char *)strInfo[1].c_str() );

}

void CMainDataProc::Cmd_Stop_Record_Proc(int iClientId, char *buf, string *strInfo)
{
    // buf = deviceId
    m_MainEntry.m_CtiCtrl.CstaStopRecord( iClientId, (char *)strInfo[0].c_str() );
}
