#include "GlobalTsapiServer.h"
#include "mainentry.h"

string  m_sServerName = "";
string	m_sLoginName = "";
string	m_sPassword = "";

extern void TraceEx( char *buf, ... );

extern CMainEntry m_MainEntry;

#ifdef _windows_
char lpszCommEngine[64] = {"CommEngine.dll"};
#else
char lpszCommEngine[64] = {"libCommEngine.so"};
#endif

QLibrary commEngineDll(lpszCommEngine);
void CommEngineSocketMsgEvent(void *wParam, void *lParam)
{
    //TraceEx( "----------commengine-msg_event-------------");
    try
    {
    if( wParam == NULL )
    {
        TraceEx( "error: OnNetworkMsg----wParam == NULL" );
        return;
    }

    //TraceEx( "CTsapiServerDlg::OnNetworkMsg begin" );

    PCONNECTINFO pInfo = (PCONNECTINFO)wParam;
    switch( pInfo->iNetworkEvent )
    {
    case ON_NETWORK_RECV://OnRecv
        {
        m_MainEntry.m_DataProc.DataProc( pInfo->iClientId, (char *)lParam );
        m_MainEntry.RecvCompleteFunc( pInfo->iThreadId );
        }
        break;
    case ON_NETWORK_CLOSE://OnClose
        {
        char szLog[256] = {0};
        sprintf( szLog, "OnNetworkMsg----OnClose, iClientId = %d", pInfo->iClientId );
        TraceEx( szLog );
        m_MainEntry.m_DataProc.OnClientClose( pInfo->iClientId );
        }
        break;
    default:
        TraceEx( "warnning: OnNetworkMsg-----OtherMsg");
        break;
    }

//	TraceEx( "CTsapiServerDlg::OnNetworkMsg end" );
    }
    catch(...)
    {
        TraceEx( "error, exception, CTsapiServerDlg::on_network_msg() " );
    }

}

void *CheckSeariNoOnTimer(void *lpVoid)
{
    int sec = 60;

    while( 1 )
    {
        sleep( 60 );

        if( m_MainEntry.m_DataProc.RecordSysRate() )
            m_MainEntry.m_DataProc.CheckSeariNo();
    }
}

void *BeginRunningTimer(void *lpVoid)
{
    TraceEx( "--------------on_timer, nIDEvent = 2------------");
    if( lpVoid == NULL )
        return NULL;

    int *param = (int *)lpVoid;
    int sec = *param;

    while( 1 )
    {
        sleep( sec );

        if( m_MainEntry.Begin2Start(false) )
        {
            if( m_MainEntry.m_MiddleDll.SetKeepAliveFlagFunc != NULL )
                m_MainEntry.m_MiddleDll.SetKeepAliveFlagFunc( 1, "cat" );
            break;
        }
        else
        {
            if( m_MainEntry.m_MiddleDll.SetKeepAliveFlagFunc != NULL )
                m_MainEntry.m_MiddleDll.SetKeepAliveFlagFunc( 0, "cat" );
        }
        TraceEx( "on_timer, idEvent = 2, execute begin2start" );
    }
}

void *ServerShutDownThread(void *lpVoid)
{
    TraceEx( "--------------on_timer, nIDEvent = 3------------");
    if( lpVoid == NULL )
        return NULL;

    int *param = (int *)lpVoid;
    int sec = *param;

    sleep( sec );

    TraceEx( "on_timer, nIDEvent = 3, main_server started and this server shutdown now" );

    exit( 0 );
}

CMainEntry::CMainEntry()
{
    SetPortFunc = NULL;
    m_acsHandle = 123456;
}

CMainEntry::~CMainEntry()
{    
    commEngineDll.unload();
}

void CMainEntry::MainInit()
{
    try
    {
    m_DataProc.InitDataProc();
    char Writed[20]={0};
#ifdef _VC6_

    char szBuff[1024]={0}, szPBXType[32] = {0}, szPBXAcd[32] = {0}, szQueueExt[32] = {0}, szLogon[32] = {0}, szLogoff[32] = {0};

    GetPrivateProfileString("AcsSettings","ServerName","",szBuff,1024,".\\TsapiServer.ini");
    m_sServerName=szBuff;
    WritePrivateProfileString("AcsSettings","ServerName",m_sServerName,".\\TsapiServer.ini");

    GetPrivateProfileString("AcsSettings","LoginName","",szBuff,1024,".\\TsapiServer.ini");
    m_sLoginName=szBuff;
    WritePrivateProfileString("AcsSettings","LoginName",m_sLoginName,".\\TsapiServer.ini");

    GetPrivateProfileString("AcsSettings","Password","",szBuff,1024,".\\TsapiServer.ini");
    m_sPassword=szBuff;
    WritePrivateProfileString("AcsSettings","Password",m_sPassword,".\\TsapiServer.ini");

    GetPrivateProfileString("AcsSettings","ServerPort","",szBuff,1024,".\\TsapiServer.ini");
    m_strPort = szBuff;
    WritePrivateProfileString("AcsSettings","ServerPort",m_strPort,".\\TsapiServer.ini");

    m_bAutoStart=GetPrivateProfileInt("TServer","AutoStart", 0, ".\\TsapiServer.ini")?TRUE:FALSE;
    sprintf(szBuff,"%d",m_bAutoStart);
    WritePrivateProfileString("TServer","AutoStart",szBuff,".\\TsapiServer.ini");

    m_bLog=GetPrivateProfileInt("TServer","AutoLog", 0, ".\\TsapiServer.ini")?TRUE:FALSE;
    sprintf(szBuff,"%d",m_bLog);
    WritePrivateProfileString("TServer","AutoLog",szBuff,".\\TsapiServer.ini");
    pApp->m_bLog = m_bLog;

    BOOL bGetAgentId=GetPrivateProfileInt("TServer","GetAgentId", 0, ".\\TsapiServer.ini")?TRUE:FALSE;
    sprintf(szBuff,"%d",bGetAgentId);
    WritePrivateProfileString("TServer","GetAgentId",szBuff,".\\TsapiServer.ini");
    pApp->m_bGetAgentId = bGetAgentId;

    GetPrivateProfileString("TServer","QueueExt","0",szQueueExt,32,".\\TsapiServer.ini");
    m_DataProc.SetQueueExt( szQueueExt );
    WritePrivateProfileString("TServer","QueueExt",szQueueExt,".\\TsapiServer.ini");

    //m_lpszSeriNo
    GetPrivateProfileString("System","SeriNo","",m_lpszSeriNo,64,".\\TsapiServer.ini");
    WritePrivateProfileString("System","SeriNo",m_lpszSeriNo,".\\TsapiServer.ini");
    m_DataProc.SeriProc( m_lpszSeriNo );
#else
    //linux - QT
    QSettings *configIni = new QSettings("TsapiServer.ini", QSettings::IniFormat);

    QString strServerName = configIni->value("/AcsSettings/ServerName").toString();
    m_sServerName = strServerName.toStdString();
    configIni->setValue("/AcsSettings/ServerName", strServerName);

    QString strLoginName = configIni->value("/AcsSettings/LoginName").toString();
    m_sLoginName = strLoginName.toStdString();
    configIni->setValue("/AcsSettings/LoginName", strLoginName);

    QString strPassword = configIni->value("/AcsSettings/Password").toString();
    m_sPassword = strPassword.toStdString();
    configIni->setValue("/AcsSettings/Password", strPassword);

    QString strServerPort = configIni->value("/AcsSettings/ServerPort").toString();
    m_strPort = strServerPort.toStdString();
    char szLog[256] = {0};
    sprintf( szLog, "get_server_port, port = %s", (char *)m_strPort.c_str());
    TraceEx(szLog);
    configIni->setValue("/AcsSettings/ServerPort", strServerPort);

    QString strAutoStart = configIni->value("/TServer/AutoStart").toString();
    m_bAutoStart = atoi( (char *)strAutoStart.toStdString().c_str() );
    configIni->setValue("/TServer/AutoStart", strAutoStart);

    QString strGetAgentId = configIni->value("/TServer/GetAgentId").toString();
    m_bGetAgentId = atoi( (char *)strGetAgentId.toStdString().c_str() );
    configIni->setValue("/TServer/GetAgentId", strGetAgentId);

    QString strQueueExt = configIni->value("/TServer/QueueExt").toString();
    m_DataProc.SetQueueExt( (char *)strQueueExt.toStdString().c_str() );
    configIni->setValue("/TServer/QueueExt", strQueueExt);

    QString strLocalQueueExt = configIni->value("/TServer/LocalQueueExt").toString();
    m_DataProc.SetLocalQueueExt( (char *)strLocalQueueExt.toStdString().c_str() );
    configIni->setValue("/TServer/LocalQueueExt", strLocalQueueExt);


    memset( m_lpszSeriNo, 0, 256 );
    QString strSeriNo = configIni->value("/System/SeriNo").toString();
    configIni->setValue("/System/SeriNo", strSeriNo);
    strcpy( m_lpszSeriNo, (char *)strSeriNo.toStdString().c_str() );
    m_DataProc.SeriProc( m_lpszSeriNo );

    QString strCTIType = configIni->value("/TServer/CTIType").toString();
    m_iCTIType = (strCTIType.length() > 0)?atoi( (char *)strCTIType.toStdString().c_str() ):0;
    configIni->setValue("/TServer/CTIType", strCTIType);

    QString strPeerIPAddr = configIni->value("/TServer/PeerIPAddr").toString();
    strPeerIPAddr = (strPeerIPAddr.length() > 0)?strPeerIPAddr:"127.0.0.1";
    memset( m_lpszPeerIPAddr, 0, 32 );
    strcpy( m_lpszPeerIPAddr, (char *)strPeerIPAddr.toStdString().c_str() );
    configIni->setValue("/TServer/PeerIPAddr", strPeerIPAddr);

    QString strPeerPort = configIni->value("/TServer/PeerPort").toString();
    strPeerPort = (strPeerPort.length() > 0)?strPeerPort:"21891";
    m_iPeerPort = atoi( (char *)strPeerPort.toStdString().c_str() );
    configIni->setValue("/TServer/PeerPort", strPeerPort);

    //m_lpszVirtualCTIExt
    QString strVirtualCTIExt = configIni->value("/TServer/VirtualCTIExt").toString();
    strVirtualCTIExt = (strVirtualCTIExt.length() > 0)?strVirtualCTIExt:"VRouteCTI";
    memset( m_lpszVirtualCTIExt, 0, 32 );
    strcpy( m_lpszVirtualCTIExt, (char *)strVirtualCTIExt.toStdString().c_str() );
    configIni->setValue("/TServer/VirtualCTIExt", strVirtualCTIExt);

    sprintf( szLog, "CTIType = %d, peerIPAddr = %s, peerPort = %d, VirtualCTIExt = %s", m_iCTIType, m_lpszPeerIPAddr, m_iPeerPort, m_lpszVirtualCTIExt );
    TraceEx( szLog );

#endif
    if( m_bGetAgentId )
    {
        m_DataProc.ReadRegAgentIniFile();
    }

#ifdef _VC6_
    GetPrivateProfileString("System","PBXType","0",szPBXType,32,".\\TsapiServer.ini");
    m_iPBXType = atoi( szPBXType );
    WritePrivateProfileString("System","PBXType",szPBXType,".\\TsapiServer.ini");

    GetPrivateProfileString("System","PBXAcd","0",szPBXAcd,32,".\\TsapiServer.ini");
    m_iPBXAcd = atoi( szPBXAcd );
    WritePrivateProfileString("System","PBXAcd",szPBXAcd,".\\TsapiServer.ini");

    //m_iNoIVR
    char szNoIVR[32] = {0};
    GetPrivateProfileString( "Setting", "NoIVR", "0", szNoIVR, 32, ".\\TsapiServer.ini" );
    m_iNoIVR = atoi( szNoIVR );
    WritePrivateProfileString("Setting","NoIVR",szNoIVR,".\\TsapiServer.ini");

    m_iKickedOff = 0;
    char szKickedOff[32] = {0};
    GetPrivateProfileString( "Setting", "KickedOff", "0", szKickedOff, 32, ".\\TsapiServer.ini" );
    m_iKickedOff = atoi( szKickedOff );
    WritePrivateProfileString("Setting","KickedOff",szKickedOff,".\\TsapiServer.ini");

    //m_iSetUUID
    char szSetUUID[32] = {0};
    GetPrivateProfileString( "Setting", "SetUUID", "0", szSetUUID, 32, ".\\TsapiServer.ini" );
    m_iSetUUID = atoi( szSetUUID );
    WritePrivateProfileString("Setting","SetUUID",szSetUUID,".\\TsapiServer.ini");

    //int	m_iACDCount;
    char szACDCount[32] = {0};
    GetPrivateProfileString( "Setting", "ACDCount", "1", szACDCount, 32, ".\\TsapiServer.ini" );
    m_iACDCount = atoi( szACDCount );
    WritePrivateProfileString("Setting","ACDCount",szACDCount,".\\TsapiServer.ini");
#else

    QString strPBXType = configIni->value("/System/PBXType").toString();
    m_iPBXType = atoi( (char *)strPBXType.toStdString().c_str() );
    configIni->setValue("/System/PBXType", strPBXType);

    QString strPBXAcd = configIni->value("/System/PBXAcd").toString();
    m_iPBXAcd = atoi( (char *)strPBXAcd.toStdString().c_str() );
    configIni->setValue("/System/PBXAcd", strPBXAcd);

    //m_iNoIVR
    QString strNoIVR = configIni->value("/Setting/NoIVR").toString();
    m_iNoIVR = atoi( (char *)strNoIVR.toStdString().c_str() );
    configIni->setValue("/Setting/NoIVR", strNoIVR);

    m_iKickedOff = 0;
    QString strKickedOff = configIni->value("/Setting/KickedOff").toString();
    m_iKickedOff = atoi( (char *)strKickedOff.toStdString().c_str() );
    configIni->setValue("/Setting/KickedOff", strKickedOff);

    //m_iSetUUID
    QString strSetUUID = configIni->value("/Setting/SetUUID").toString();
    m_iSetUUID = atoi( (char *)strSetUUID.toStdString().c_str() );
    configIni->setValue("/Setting/SetUUID", strSetUUID);

    //int	m_iACDCount;
    QString strACDCount = configIni->value("/Setting/ACDCount").toString();
    m_iACDCount = atoi( (char *)strACDCount.toStdString().c_str() );
    configIni->setValue("/Setting/ACDCount", strACDCount);

#endif
    for (int i=0; i < m_iACDCount; i++)
    {
        //CString stmp;
        sprintf(Writed,"ACD%d",i);

#ifdef _VC6_
        GetPrivateProfileString("Setting",Writed,"1",m_strACDExt[i].GetBuffer(32),32,".\\TsapiServer.ini");
        WritePrivateProfileString("Setting", Writed,m_strACDExt[i], ".\\TsapiServer.ini");
#else
        char szSection[128] = {0};
        sprintf( szSection, "/Setting/%s", Writed );
        QString strACDExt = configIni->value(szSection).toString();
        m_strACDExt[i] = strACDExt.toStdString();
        configIni->setValue(szSection, strACDExt);
#endif
    }

#ifdef _VC6_
    //TrunkGrp
    char szTrunkCount[32] = {0};
    GetPrivateProfileString( "Setting", "TrunkCount", "1", szTrunkCount, 32, ".\\TsapiServer.ini" );
    m_iTrunkCount = atoi( szTrunkCount );
    WritePrivateProfileString("Setting","TrunkCount",szTrunkCount,".\\TsapiServer.ini");
#else
    QString strTrunkCount = configIni->value("/Setting/TrunkCount").toString();
    m_iTrunkCount = atoi( (char *)strTrunkCount.toStdString().c_str() );
    configIni->setValue("/Setting/TrunkCount", strTrunkCount);
#endif
    int i;
    for ( i=0; i<m_iTrunkCount; i++)
    {
        sprintf(Writed,"Trunk%d",i);

#ifdef _VC6_
        GetPrivateProfileString("Setting",Writed,"",m_strTrunkGrp[i].GetBuffer(32),32,".\\TsapiServer.ini");
        WritePrivateProfileString("Setting", Writed,m_strTrunkGrp[i], ".\\TsapiServer.ini");
#else
        char szSectionTrunk[128] = {0};
        sprintf( szSectionTrunk, "/Setting/%s", Writed);
        QString strTrunkGrp = configIni->value(szSectionTrunk).toString();
        m_strTrunkGrp[i] = strTrunkGrp.toStdString();
        configIni->setValue(szSectionTrunk, strTrunkGrp);

#endif
    }

    QString strCtrlSys = configIni->value("/Setting/CtrlSystemFlag").toString();
    m_iCtrlSystemFlag = atoi( (char *)strCtrlSys.toStdString().c_str() );
    configIni->setValue("/Setting/CtrlSystemFlag", strCtrlSys);


#ifndef _VC6_
    delete configIni;
#endif

#ifdef _VC6_
    char szServerFlag[32] = {0};
    GetPrivateProfileString( "ServerType", "ServerFlag", "0", szServerFlag, 32, ".\\Balance.ini" );
    m_iServerFlag = atoi( szServerFlag );
    WritePrivateProfileString("ServerType","ServerFlag",szServerFlag,".\\Balance.ini");

    char szMainServer[32] = {0};
    GetPrivateProfileString( "ServerType", "IsMainServer", "0", szMainServer, 32, ".\\Balance.ini" );
    m_iMainServer = atoi( szMainServer );
    WritePrivateProfileString("ServerType","IsMainServer",szMainServer,".\\Balance.ini");
#else

    QSettings *Balance_configIni = new QSettings(".\\Balance.ini", QSettings::IniFormat);

    QString strServerFlag = Balance_configIni->value("/ServerType/ServerFlag").toString();
    m_iServerFlag = atoi( (char *)strServerFlag.toStdString().c_str() );
    Balance_configIni->setValue("/ServerType/ServerFlag", strServerFlag);

    QString strMainServer = Balance_configIni->value("/ServerType/IsMainServer").toString();
    m_iMainServer = atoi( (char *)strMainServer.toStdString().c_str() );
    Balance_configIni->setValue("/ServerType/IsMainServer", strMainServer);

    delete Balance_configIni;
#endif
    m_DataProc.LoadVDN();

    m_iMainCTIRunning = 0;
    if( m_iServerFlag == 1 && m_iMainServer == 0 )
    {
        //双机热备，并且是备份CTI时，默认情况下主CTI是运行状态
        m_iMainCTIRunning = 1;
    }

    char lpszLog[1024] = {0};
    sprintf( lpszLog, "init, m_iServerFlag = %d, m_iMainServer = %d, m_iMainCTIRunning = %d",
             m_iServerFlag, m_iMainServer, m_iMainCTIRunning );

    TraceEx( lpszLog );

    ReloadMonitorInfo();

#ifdef _VC6_
    UpdateData( FALSE );
#endif

    m_bAutoStart = true;
    if( m_bAutoStart )
        Begin2Start(true);

    if( m_iServerFlag != 0 )
        m_MiddleDll.MiddleDllInit();

    if( m_iCTIType == loadbalance_cti_type )
    {
        if( !m_ocxInterface.Init() )
        {
            TraceEx( "error, ocx_interface init failed!");
            return;
        }

        m_ocxInterface.ConnectToServer();
    }

    if( m_iPBXAcd == 1 )
    {
#ifdef _VC6_
        SetTimer( 4, 1000 * 2, NULL );
        SetTimer( 5, 1000 * 5, NULL );
#else
        //查询ACD和TrunkGroup的信息，暂时不做
#endif
    }

    //m_ftpDll.FtpDllInit();

    TraceEx( "init_end!");
    }
    catch(...)
    {
        TraceEx( "error, exception, CTsapiServerDlg::init() " );
    }

}

void CMainEntry::AcsGetEventBlock(void *wParam, void *lParam)
{
    m_CtiEvent.CtiEvent( wParam, lParam );
}

void CMainEntry::AutoKeepalive()
{

}

void CMainEntry::Abortstream()
{

}

void CMainEntry::ReloadMonitorInfo()
{
    try
    {
    char	szNum[32] = {0}, szMonitorAll[32] = {0};

    //MonitorAll


    //monitor_agent
#ifdef _VC6_
    m_nMoniExtNum = GetPrivateProfileInt("Monitor_Agent", "TotalNum", 8, ".\\TsapiServer.ini");
    sprintf(szNum,"%d",m_nMoniExtNum);
    WritePrivateProfileString("Monitor_Agent", "TotalNum",szNum, ".\\TsapiServer.ini");
#else
    QSettings *configIni = new QSettings("TsapiServer.ini", QSettings::IniFormat);

    QString strMoniExtNum = configIni->value("/Monitor_Agent/TotalNum").toString();
    m_nMoniExtNum = atoi( (char *)strMoniExtNum.toStdString().c_str() );
    configIni->setValue("/Monitor_Agent/TotalNum", strMoniExtNum);

#endif

    char szLog[256] = {0};
    char szKey[32] = {0};
    for(int i = 0; i < m_nMoniExtNum; i++)
    {
        sprintf(szKey,"ExtNo%d",i);

        m_strMoniExt[i] = "";
        char ext[32] = {0};

#ifdef _VC6_
        GetPrivateProfileString("Monitor_Agent",szKey,"",ext,32,".\\TsapiServer.ini");
        m_strMoniExt[i] = ext;
        WritePrivateProfileString("Monitor_Agent", szKey,ext, ".\\TsapiServer.ini");
#else
        char szSection[128] = {0};
        sprintf( szSection, "/Monitor_Agent/%s", szKey);
        QString strMoniExt = configIni->value(szSection).toString();
        m_strMoniExt[i] = strMoniExt.toStdString();
        configIni->setValue(szSection, strMoniExt);

        sprintf( szLog, "section = %s, monitor_ext = %s", szSection, (char *)m_strMoniExt[i].c_str() );
        TraceEx( szLog );
#endif
    }

#ifndef _VC6_
    delete configIni;
#endif

    }
    catch(...)
    {
        TraceEx( "error, exception, CTsapiServerDlg::reload_monitor_info() " );
    }
}

bool CMainEntry::Begin2Start(bool bNeedStartOnTimerThread)
{
    bool bResult = true;

    if( m_iCTIType != trans_station_cti_type ) //重定向中转服务器不需要连接CSTA
    {
    if( !m_CtiCtrl.AcsOpenStream(ST_CSTA, false) )
    {
        bResult = false;
#ifdef _VC6_
        SetTimer( 2, 10*1000, NULL );
#else
        if( bNeedStartOnTimerThread )
        {
            int sec = 10;
            pthread_t iThreadId;
            if( pthread_create(&iThreadId, NULL, BeginRunningTimer, (void *)&sec ) != 0 )
            {
                printf("Create pthread error!\n");
                return bResult;
            }
        }
#endif
        TraceEx( "acs_open_stream failed, and restat service in ontimer now" );

        return bResult;
    }
    }

    TraceEx( "begin to dll_init now");
    if( !DllInit() )
        return false;

    if( bResult )
    {
#ifdef _VC6_
        m_BtnStart.EnableWindow( FALSE );
        m_BtnEnd.EnableWindow( TRUE );

        SetTimer( 1, 1000*60, NULL );	//1分钟后检测序列号
#else
        //CheckSeariNoOnTimer
        int sec = 60;
        pthread_t iThreadId;
        if( pthread_create(&iThreadId, NULL, CheckSeariNoOnTimer, (void *)&sec ) != 0 )
        {
            printf("Create pthread error!\n");
            return false;
        }

#endif
    }

    return bResult;
}

void CMainEntry::ServerShutDown(int sec)
{
    pthread_t iThreadId;
    if( pthread_create(&iThreadId, NULL, ServerShutDownThread, (void *)&sec) != 0 )
    {
        printf("Create server_shut_down pthread error!\n");
    }
}

bool CMainEntry::DllInit()
{
#ifdef _VC6_
    //CommEngine.dll
        m_hSocketDll = NULL;
        m_hSocketDll = LoadLibrary( "CommEngine.dll" );
        if( m_hSocketDll == NULL )
        {
            AfxMessageBox( "CommEngine.dll cant find" );
            return FALSE;
        }

        RegNetworkHwndFunc = ( REGNETWORKHWND )GetProcAddress( m_hSocketDll, "RegWnd" );
        if( RegNetworkHwndFunc == NULL )
        {
            AfxMessageBox( "GetProcAddress - RegWnd failed!" );
            return FALSE;
        }
        RegNetworkHwndFunc( this->m_hWnd, ON_NETWORK_MSG );

        SetPortFunc = ( SETPORT )GetProcAddress( m_hSocketDll, "SetPort" );
        if( SetPortFunc == NULL )
        {
            AfxMessageBox( "GetProcAddress - SetPort failed!" );
            return FALSE;
        }
        int iPort = atoi( (LPSTR &)m_strPort );
        SetPortFunc( iPort );

        RecvCompleteFunc = ( RECVCOMPLETE )GetProcAddress( m_hSocketDll, "EndRecv" );
        if( RecvCompleteFunc == NULL )
        {
            AfxMessageBox( "GetProcAddress - EndRecv failed!" );
            return FALSE;
        }

        SendToClientFunc = ( SENDTOCLIENT )GetProcAddress( m_hSocketDll, "Send2Client" );
        if( SendToClientFunc == NULL )
        {
            AfxMessageBox( "GetProcAddress - Send2Client failed!" );
            return FALSE;
        }

        CloseSockFunc = ( CLOSESOCK )GetProcAddress( m_hSocketDll, "CloseASock" );
        if( CloseSockFunc == NULL )
        {
            AfxMessageBox( "GetProcAddress - CloseASock failed!" );
            return FALSE;
        }

        NetworkInitFunc = ( NETWORKINIT )GetProcAddress( m_hSocketDll, "StartSocketEngine" );
        if( NetworkInitFunc == NULL )
        {
            AfxMessageBox( "GetProcAddress - StartSocketEngine failed!" );
            return FALSE;
        }
        if( !NetworkInitFunc() )
        {
            AfxMessageBox( "StartSocketEngine failed" );
            return FALSE;
        }

        TerminateDllThreadFunc = ( TERMINATEDLLTHREAD )GetProcAddress( m_hSocketDll, "EndSession" );
        {
            if( TerminateDllThreadFunc == NULL )
            {
                AfxMessageBox( "GetProcAddress EndSession failed" );
                return FALSE;
            }
        }
#else

    if (!commEngineDll.load())
    {
        TraceEx( "error, commEngine.dll load failed!");
        return false;
    }


    SetSocketMsgEventCallbackFunc = (LPSETSOCKETMSGEVENTCALLBACK)commEngineDll.resolve("SetSockMsgCallback");
    if( SetSocketMsgEventCallbackFunc == NULL )
    {
        TraceEx( "error, GetProcAddress - SetSockMsgCallback failed!" );
        return false;
    }
    SetSocketMsgEventCallbackFunc( CommEngineSocketMsgEvent );

    SetPortFunc = (SETPORT)commEngineDll.resolve("SetPort");
    if( SetPortFunc == NULL )
    {
        SetPortFunc = (SETPORT)commEngineDll.resolve("SetServerPort");
        if( SetPortFunc == NULL )
        {
            TraceEx( "error, GetProcAddress - SetPort failed!" );
            return false;
        }
    }
    int iPort = atoi( (char *)m_strPort.c_str() );
    SetPortFunc( iPort );

    RecvCompleteFunc = ( RECVCOMPLETE )commEngineDll.resolve( "EndRecv" );
    if( RecvCompleteFunc == NULL )
    {
        TraceEx( "GetProcAddress - EndRecv failed!" );
        return false;
    }

    SendToClientFunc = ( SENDTOCLIENT )commEngineDll.resolve( "Send2Client" );
    if( SendToClientFunc == NULL )
    {
        TraceEx( "GetProcAddress - Send2Client failed!" );
        return false;
    }

    CloseSockFunc = ( CLOSESOCK )commEngineDll.resolve( "CloseASock" );
    if( CloseSockFunc == NULL )
    {
        TraceEx( "GetProcAddress - CloseASock failed!" );
        return false;
    }

    NetworkInitFunc = ( NETWORKINIT )commEngineDll.resolve( "StartSocketEngine" );
    if( NetworkInitFunc == NULL )
    {
        TraceEx( "GetProcAddress - StartSocketEngine failed!" );
        return false;
    }
    if( !NetworkInitFunc() )
    {
        TraceEx( "StartSocketEngine failed" );
        return false;
    }

    TerminateDllThreadFunc = ( TERMINATEDLLTHREAD )commEngineDll.resolve( "EndSession" );
    {
        if( TerminateDllThreadFunc == NULL )
        {
            TraceEx( "GetProcAddress EndSession failed" );
            return false;
        }
    }

#endif
    return true;
}
