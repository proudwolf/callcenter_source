#ifndef _MAIN_ENTRY_
#define _MAIN_ENTRY_

#include "CtiCtrl.h"
#include "DataProc.h"
#include "CtiEvent.h"
#include "MiddleDll.h"
#include "FtpDll.h"
#include "cticlientmodleevent.h"
#include "ocxdll.h"

class CMainEntry
{
public:
    CMainEntry();
    virtual ~CMainEntry();
public:
    bool Begin2Start(bool bNeedStartOnTimerThread);
    void ServerShutDown(int sec);
    void Abortstream();
    void AutoKeepalive();
    void MainInit();
    void AcsGetEventBlock(void *wParam, void *lParam);
public:
    CMainDataProc   m_DataProc;
    CCtiCtrl    m_CtiCtrl;
    CCtiEvent   m_CtiEvent;
    CMiddleDll  m_MiddleDll;
    CCtiClientModleEvent m_CtiClientModleEvent;
    COCXInterface   m_ocxInterface;
    //CFtpDll     m_ftpDll;

#ifdef _VC6_
    REGNETWORKHWND RegNetworkHwndFunc;
#endif

    ACSHandle_t m_acsHandle;

    NETWORKINIT NetworkInitFunc;
    SETPORT SetPortFunc;
    RECVCOMPLETE RecvCompleteFunc;
    SENDTOCLIENT SendToClientFunc;
    CLOSESOCK CloseSockFunc;
    TERMINATEDLLTHREAD	TerminateDllThreadFunc;
    LPSETSOCKETMSGEVENTCALLBACK SetSocketMsgEventCallbackFunc;

    bool    	m_bAutoStart;
    bool        m_bGetAgentId;
    int         m_iPBXType;
    int         m_iNoIVR;

    int         m_iCTIType; // 0 - 单机（或主备）模式, 1 - AgentCTI， 2 - RouteCTI
    char        m_lpszPeerIPAddr[32];
    int         m_iPeerPort;
    char        m_lpszVirtualCTIExt[32];


    int         m_iCtrlSystemFlag; // 1 - 调度系统， 0 - 非调度系统
    int         m_iSetUUID;		// 0 - 不设置UUID（无随路数据）, 1 - 设置UUID（有随路数据）
    int         m_iMainCTIRunning;
    int         m_iServerFlag;	// 0 - 单机方案， 1 - 双机热备， 2 - 负载均衡
    int         m_iMainServer;	// 0 - 备机， 1 - 主机 (仅当m_iServerFlag = 1时才生效)

    int         m_iPBXAcd;
    bool        m_bAutoKeepAlive;
    bool        m_bOpenedStream;
    bool        m_bAutoReconnect;

    int         m_iKickedOff;

    int         m_nMoniExtNum;
    string      m_strMoniExt[1024];

    string      m_sServerName;
    string      m_sLoginName;
    string      m_sPassword;
    string      m_strPort;

    char        m_lpszSeriNo[256];

    int         m_iACDCount;
    string  	m_strACDExt[32];

    int         m_iTrunkCount;
    string      m_strTrunkGrp[32];
private:
    bool DllInit();
    void ReloadMonitorInfo();
};

#endif // MAINENTRY

