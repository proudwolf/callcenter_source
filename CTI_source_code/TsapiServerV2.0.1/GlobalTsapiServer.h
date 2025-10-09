#ifndef _GLOBAL_TSAPISERVER_H_
#define _GLOBAL_TSAPISERVER_H_

#define _linux_
//#define _windows_

#define _QT_
//#define _VC6_

#define COM_NO_WINDOWS_H

#include <stdio.h>
#ifdef _windows_
#include <io.h>
#else
#include <sys/io.h>
#endif

//#include <wtypes.h>
#include <list>
#include <string>
#include <fstream>

using namespace std;

#include <unistd.h>

#ifdef _QT_
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QLibrary>
#include <QDateTime>
#include <semaphore.h>
#include <qmutex.h>
#include <qdir.h>
#include <QSettings>
#include <QNetworkInterface>
#endif

#define _sys64_


#ifdef _sys32_
typedef unsigned long  DWORD ;
typedef unsigned short WORD ;
#endif

#ifdef _sys64_
typedef unsigned int    DWORD ;
typedef unsigned short  WORD;
#endif



#define __USLC__        //define linux os

#include "acs.h"
#include "csta.h"
#include "Attpdefs.h"
#include "Attpriv.h"

#ifdef  _windows_
#else
//#include <sys/socket.h>
//#include <netinet/in.h>
#endif


#define _USEDOG

#define RC_INVOKEID	2123//1233

#define ON_NETWORK_MSG			WM_USER + 100
#define ON_MSG_START			WM_USER + 101
#define ON_MSG_TAPI				WM_USER + 102
#define ON_UPDATE_MSG			WM_USER + 103

#define WM_ACSEVENT				WM_USER + 500

#define SIZE_OF_ALCATEL_PRIVATE_DATA		3000

#define SETUPTRANS_OK			1
#define DIAL_OK					2

#define ON_NETWORK_RECV		0x01
#define ON_NETWORK_CLOSE	0x02


#define MAIN_CTI_RESTORE			0x16
#define MAIN_CTI_SHUTDOWN			0X17

#define	MAX_AGENTNUM		1024

#define MAX_PATH            260

enum
{
    st_free,		//空闲0
    st_ring,		//振铃1
    st_talking,		//通话2
    st_transfer,	//转接中3，avaya中对于IVR分机表示正在转接中，对坐席分机则表示转接完毕(分机转接完毕后不会触发Hangup或Disconnect事件)
    st_pause,		//小休4
    st_dialing,		//外拨中5
    st_dialfail,	//外拨失败6
    st_recording,	//录音中7
    st_holding,		//通话保持8
    st_transfail,	//转接失败9
    st_picked,		//摘机10
    st_ringback,	//回铃11
    st_logout,		//登出12
    st_backdeal,	//后处理13
    st_dinner,		//就餐14
    st_traning,		//培训15
    st_conference,	//会议16
    st_dialtalk,	//外拨通话17
    st_pending,		//催挂18
    st_playagent,	//报工号19
    st_doing=21,	//主动服务21
};

typedef struct
{
	char	lpszAgentId[32];
	char	lpszExtNo[32];
}AGENT_EXT_INFO, *PAGENT_EXT_INFO;

typedef struct extention_info_t {
    DeviceID_t				szExtNo;
	DeviceID_t				szDynamicID;
    CSTAMonitorCrossRefID_t	nCrossRefID;
	CallingDeviceID_t		CallingDeviceID;
	CalledDeviceID_t		CalledDeviceID;
	SubjectDeviceID_t		SubDeviceID;
	RedirectionDeviceID_t	RedirDeviceID;
	long					nCallID;
	long					nHeldCallID;
    char					szGlobalCallid[128];
	int						iConnect;
}EXTINFO, *PEXTINFO;

typedef struct
{
	int iThreadId;
	int iClientId;
	int iNetworkEvent;
}CONNECTINFO, *PCONNECTINFO;

typedef struct
{
	int iLineId;
    string strDestExt;
}TRANSNODE, *PTRANSNODE;

typedef struct packet_head_t
{
    DWORD       dwFlag;			//协议标识
    WORD        cmd_type;       //命令类型
    WORD        cmd;            //命令编码
    DWORD       dwSize;			//数据长度
}PACKET_HEAD;

typedef struct
{
	int iClientId;
	int	iAgentType;				// 0 - 普通坐席	 1 - 班长坐席  2 - 录音 3 - IVR  4 - 网关  5 - SQL代理
	int	iUsed;
    int iCurrentState;          //当前分机状态
    int iAgentCTISocket;
	char	szDeviceId[32];
}AGENTTYPE, *PAGENTTYPE;

typedef struct
{
    int iCurrentState;
    char szDeviceId[32];
}AGENT_STATE, *PAGENT_STATE;

typedef struct
{
	int iClientId;
	string strExtNo;
}SOCKEXT, *PSOCKEXT;

typedef struct{
	char lpszFileName[128];
	long lFileSize;
	char szVersion[64];
	char *lpszData;
}FILEINFO, *PFILEINFO;

typedef struct{
	int	iMsgType;
	int	iClientId;
	FILEINFO FileInfo;
}MSGINFO, *PMSGINFO;

typedef struct{
	int	ibAll;
    string strInfo[64];
}SENDMSGINFO, *PSENDMSGINFO;

typedef struct{
	string	CallID;
	char	callingDeviceId[32];
	char	calledDeviceId[32];
}QUEUEINFO, *PQUEUEINFO;

typedef struct{
    char    lpszIPAddr[32];
    int     iPort;
    char    szTime[32];
    int     iNum;
    int     iClientId;
    string  strExt;
}REGEXTINFOLIST, *PREGEXTINFOLIST;

//命令类型
#define	  RC_REQUEST			0x01//请求数据
#define	  RC_RESPONSE_OK		0x02//响应正确
#define	  RC_RESPONSE_FAILED	0x03//响应错误
#define	  RC_EVENT				0x04//自发事件
#define	  RC_REG_EXT_FAILED		0x05//分机注册失败
#define	  RC_REG_EXT_BEYOND_LISENCE	0x06//分机注册超出lisence限制
//#define	  SNT_MUTI_REQUEST		0x05//多客户端请求数据

#define	  PACKET_FLAG			0x77553311	//协议标识
#define	  HEART_BEAT			0xF22211F1	//心跳包

//命令编码
#define CMD_REGISTERDN			0x01		//注册分机
#define CMD_AGENTLOGON			0x02		//坐席登录
#define CMD_AGENTREADY			0x03		//坐席准备
#define	CMD_SENDMSGTOEXT		0x04		//向分机发送消息
#define	CMD_SENDMSG2ALL			0x05		//向所有分机发送消息
#define CMD_AGENTLOGOFF			0x06		//坐席登出
#define	CMD_AGENTNOTREADY		0x07		//坐席未准备
#define	CMD_HOLDCALL			0x08		//保持
#define	CMD_RETRIEVECALL		0x09		//取回
#define	CMD_PICKUP				0x0A		//摘机
#define	CMD_HANGUP				0x0B		//挂机
#define	CMD_MAKECALL			0x0C		//拨号
#define	CMD_INITTRANS			0x0D		//初始化转接
#define	CMD_COMPLETETRANS		0x0E		//完成转接
#define	CMD_COMPLETECONF		0x0F		//完成会议
#define CMD_LISTEN				0x10		//监听
#define	CMD_INTRUDE				0x11		//强插
#define	CMD_FORCECH				0x12		//强拆
#define CMD_UNREGISTERDN		0x13		//注销分机
#define CMD_SINGLESTEPTRANS		0x14		//单步转接
#define CMD_SETTRANSDATA		0x15		//设置转接数据
#define	CMD_GETTRANSDATA		0x16		//获取转接数据
#define	CMD_HEARTBEAT			0x17		//心跳数据
#define CMD_ALTERCALL			0x18		//电话切换
#define CMD_CALLCOMPLETION		0x19		//电话预占
#define CMD_CLEARCALL			0x1A		//清除话路(可清除未连接的电话,一般用于ivr挂断黑名单电话)
#define CMD_DEFLECTCALL			0x1B		//代接电话
#define CMD_RECONNECTCALL		0x1C		//重连held电话,结束当前通话
#define CMD_SETOTHEREVENT		0x1D		//设置是否响应其他分机事件
#define CMD_SENDFAX				0x1E		//发送传真
#define CMD_SENDOTHERMSG		0x1F		//发送其他媒体消息
#define CMD_REQUERYFILEVER		0x20		//发送请求比较文件版本
#define CMD_DOWNLOADFILE		0x21		//发送请求下载文件
#define CMD_REGISTERVDN			0x22		//注册VDN分机
#define CMD_QUERYDEVINFO		0x23		//询问分机信息（一般用于询问分机登录的工号）
#define CMD_QUERYTRKINFO		0x24		//询问外线组信息
#define	CMD_SEND2DST			0x25		//发送数据到目标分机
#define CMD_REQUEST_DOWNLOAD_FOLDER		0x26		//文件夹请求下载
#define CMD_DOWNLOAD_FILE_FIND_END		0x27		//下载文件搜索结束

#define CMD_RECONNECT			0x41		//重连接
#define CMD_MONITOR_INFO		0x42		//返回监控信息
#define CMD_CONNECT_SERVER_SUCCESS  0x43    //connect server successfull flag
#define CMD_CONNECT_SERVER_FAILED   0x44

#define	CMD_EXTPICKUP_EVENT			0x90		//分机摘机
#define	CMD_EXTHANGUP_EVENT			0x91		//分机挂机
#define	CMD_CALLESTABLISHED_EVENT	0x92		//通话建立
#define	CMD_CALLDISCONNECT_EVENT	0x93		//通话结束
#define	CMD_CALLERIDCOMING_EVENT	0x94		//来电	   
#define	CMD_EXTENSIONRINGBACK_EVENT	0x95		//回铃	   
#define	CMD_RETTRANSDATA_EVENT		0x96		//得到传输数据
#define	CMD_EXTENSIONMSG_EVENT		0x97		//分机消息事件
#define	CMD_EXTONHOLD_EVENT			0x98		//分机保持事件
#define	CMD_EXTONRETRIEVE_EVENT		0x99		//分机取回事件
#define	CMD_EXTDIALING_EVENT		0x9A		//分机拨号事件
#define	CMD_EXTTRANSFERED_EVENT		0x9B		//分机转接事件
#define	CMD_FAILEDCONNECTION_EVENT	0x9C		//连接失败事件
#define	CMD_EXTDIVERTCALL_EVENT		0x9D		//分机代接成功事件
#define	CMD_EXTCONFERENCED_EVENT	0x9E		//会议建立成功事件

//媒体命令
#define		CMD_START_PLAY			0x50		//开始放音
#define		CMD_STOP_PLAY			0x51		//停止放音
#define		CMD_START_RECORD		0x52		//开始录音
#define		CMD_STOP_RECORD			0x53		//停止录音


enum
{
    // 0 - 单机（或主备）模式, 1 - loadbalance_cti_type-负载均衡CTI 2 - trans_station_cti_type-中转CTI
    normal_cti_type,
    loadbalance_cti_type,
    trans_station_cti_type,
};

enum
{
	//0 - 普通坐席	 1 - 网关  2 - 录音 3 - IVR  4 - 班长坐席  5 - SQL代理 6 - 多客户端类型(虚拟注册服务器)
    //7 - loadbalance_cti_server_type负载均衡CTI类型， 8 - 中转CTI类型, 9 - WS(websocket)类型
	agent_type,
	msg_engine_type,
	record_type,
	ivr_type,
	agent_monitor_type,
	sql_type,
	mutl_agent_type,
    loadbalance_cti_server_type,
    trans_station_cti_server_type,
    ws_type,
};

enum
{
	col_no,
	col_ip,
	col_port,
	col_extno,
	col_time,
	col_num,
	col_sock,
};

enum{
	OnAllEvent,
	OnPickupEvent,
	OnHangupEvent, 
	OnEstablishedEvent,
	OnDisconnectEvent,
	OnCallComingEvent,
	OnRingbackEvent,
	OnTransDataEvent,
	OnMsgEvent,
	OnHoldEvent,
	OnRetrieveEvent,
	OnDialingEvent,
	OnTranferedEvent,
	OnDestBusyEvent,
	OnDivertEvent,
	OnConfEvent,
};

enum
{
    FILE_FOLDER,
    FILE_NAME
};

typedef struct
{
    int  iTaskBackupId;
    int	 iBackupRuse;
    int	 iOrientDataFileType;				//FILE_NAME, FILE_FOLDER
    int	 iCompleteBackupFlag;
#ifdef _windows_
    long lFileSize;
#else
    int lFileSize;
#endif
    char lpszExcTime[32];
    char lpszRemoteFilePathName[MAX_PATH];
    char lpszRemoteFileFolder[MAX_PATH];
}DOWNLOAD_DATA_INFO, *LDOWNLOAD_DATA_INFO;

typedef struct
{
    int  iTaskBackupId;
    int	 iBackupRuse;
    int	 iOrientPos;
    int	 iRequestType;							//LOCAL_CLIENT, REMOTE_CLIENT
    int	 iDownloadType;							//REPLACE_OLD_FILE, REPLACE_EXSITE_FILE, JUMP_EXSITE_FILE
#ifdef _windows_
    long lBeginTime;
    long lTime;
#else
    int lBeginTime;
    int lTime;
#endif
    char lpszLastWriteTime[32];
    char lpszTaskGroupName[32];
    char lpszTaskName[32];
    char lpszDestClientName[32];
    char lpszFtpServer[32];
    int	 iRemoteFileType;
    char lpszSrcDBName[64];
    char lpszDestDBName[64];
    char lpszDestDBUserName[32];
    char lpszDestDBPasswd[32];
    char lpszLocalPath[MAX_PATH];
    char lpszRemotePath[MAX_PATH];
    char lpszRemoteFileName[MAX_PATH];
}DOWNLOAD_FILE_INFO, *LPDOWNLOAD_FILE_INFO;

typedef void(*SENDTODEST)(char *buf, char *lpszIP, int iPort);

#ifdef _VC6_
typedef void(*REGNETWORKHWND)(HWND hWnd, UINT nMsgId);
#endif

typedef bool(*NETWORKINIT)();
typedef void(*SETPORT)(int iPort);
typedef void(*RECVCOMPLETE)(int iIndex);
typedef void(*SENDTOCLIENT)(int iClientId, char *lpszBuffer, int iDataLen);
typedef void(*CLOSESOCK)(int iClientId);
typedef void(*HEARTBEAT)(int iClientId, char *buf);
typedef void(*TERMINATEDLLTHREAD)();
typedef void(*LPSOCKMSGEVENT)(void *wParam, void *lParam);
typedef void(*LPSETSOCKETMSGEVENTCALLBACK)(LPSOCKMSGEVENT);

#ifdef _VC6_
typedef void(*REGUPDATEEVENTHWND)(HWND hWnd, UINT nMsgId);
#endif

typedef int(*QUERYVERSION)(char *lpszFileName, PMSGINFO pMsgInfo, int iClientId);
typedef void(*DOWNLOADSTART)(char *lpszFileName, int iClientId);

//ftp
typedef int(*STARTFTPENGINE)(char *lpszFlag);
typedef int(*FTPFREEALLDATA)(char *lpszFlag);


#ifdef _VC6_
#define	DLLEXPORT	_declspec(dllexport)
extern "C" 
{
	DLLEXPORT int	encrpt_decode(char *lpszInput, int iDataLen, char *lpszOutput);
}
#endif

#ifdef _QT_
extern "C"
{
    Q_DECL_EXPORT int	encrpt_decode(char *lpszInput, int iDataLen, char *lpszOutput);
}
#endif

#ifdef _VC6_
typedef void(*LPRESTOREEVENT)(WPARAM wParam, LPARAM lParam);
#endif

#ifdef _QT_
typedef void(*LPRESTOREEVENT)(void *wParam, void *lParam);
#endif

typedef void(*LPSETCALLBACK)(LPRESTOREEVENT);
typedef void(*BEGINCHECKALIVE)(char *szFlag);
typedef bool(*MIDDLEDLLINIT)(char *szFlag);
typedef void(*MIDDLEDLLSETPORT)(int iPort, char *szFlag);
typedef void(*SETKEEPALIVEFLAG)(int iKeepAliveFlag, char *szFlag);

typedef void(*LPACSNOTIFYEVENT)(void *wParam, void *lParam);
typedef void(*LPSETNOTIFYEVENTCALLBACK)(LPACSNOTIFYEVENT);


//ocx_interface
typedef void(*SETAGENTTYPE)(int iAgentType);
typedef void(*SETDUALIP)(char *lpszIP1, char *lpszIP2);
typedef bool(*CONNECTTOSERVER)(int iPort);
typedef void(*REGISTEREXTENSION)(char *extention);
typedef int(*SENDCMD)(int cmd, char *lpszBuffer, int iDataLen);
typedef void(*LPOCXINTERFACEEVENT)(void *wParam, void *lParam);
typedef void(*LPSETOCXINTERFACEEVENTCALLBACK)(LPOCXINTERFACEEVENT);
typedef void(*SENDMSG2EXTEINTION)(char *srcDN, char *destDN, char *param1, char *param2, char *param3);
typedef void(*SENDMSG2ALL)(char *SourceNumber, char *Param1, char *Param2, char *Param3);
typedef void(*SETAGENTPARAM)(char *szQueueExt, char *szRecordExt, int iQueueGrpNo, int iRecordGrpNo, int iAgentType);

#endif
