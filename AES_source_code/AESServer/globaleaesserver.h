#ifndef GLOBALEAESSERVER_H
#define GLOBALEAESSERVER_H

//#define _windows_
#define _linux_


#define _QT_
//#define _VC6_

#include <QtCore/qglobal.h>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QLibrary>
#include <QDateTime>
#include <semaphore.h>
#include <qmutex.h>
#include <qdir.h>
#include <QSettings>
#include <QNetworkInterface>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextCodec>


#include <stdio.h>

#ifdef _windows_
#include<io.h>
#include <winsock2.h>
#else
#include <sys/io.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>
#endif

//#include <wtypes.h>
#include <list>
#include <string>
#include <fstream>

#define _sys64_

#ifdef _linux_

#ifdef _sys32_
typedef unsigned long  DWORD ;
typedef unsigned short WORD ;
#endif

#ifdef _sys64_
typedef unsigned int    DWORD ;
typedef unsigned short  WORD;
#endif

#endif

#include <unistd.h>

using namespace std;

//命令类型
#define	  SNT_REQUEST			0x01//请求数据
#define	  SNT_RESPONSE_OK		0x02//响应正确
#define	  SNT_RESPONSE_FAILED	0x03//响应错误
#define	  SNT_EVENT				0x04//自发事件

#define	  PACKET_FLAG			0x77553311	//协议标识
#define	  HEART_BEAT			0xF22211F1	//心跳包

#define ON_NETWORK_RECV		0x01
#define ON_NETWORK_CLOSE	0x02

//CTI控制命令
#define		CMD_REGISTERDN			0x01		//注册分机
#define     CMD_AGENTLOGON			0x02		//坐席登录
#define     CMD_AGENTREADY			0x03		//坐席准备
#define		CMD_SENDMSGTOEXT		0x04		//向分机发送消息
#define     CMD_SENDMSG2ALL			0x05		//向所有分机发送消息
#define     CMD_AGENTLOGOFF			0x06		//坐席登出
#define     CMD_AGENTNOTREADY		0x07		//坐席未准备
#define		CMD_HOLDCALL			0x08		//保持
#define		CMD_RETRIEVECALL		0x09		//取回
#define		CMD_PICKUP				0x0A		//摘机
#define		CMD_HANGUP				0x0B		//挂机
#define		CMD_MAKECALL			0x0C		//拨号
#define		CMD_INITTRANS			0x0D		//初始化转接
#define		CMD_COMPLETETRANS		0x0E		//完成转接
#define		CMD_COMPLETECONF		0x0F		//完成会议
#define		CMD_LISTEN				0x10		//监听
#define		CMD_INTRUDE				0x11		//强插
#define		CMD_FORCECH				0x12		//强拆
#define		CMD_UNREGISTERDN		0x13		//注销分机
#define		CMD_SINGLESTEPTRANS		0x14		//单步转接

#define		CMD_HEARTBEAT			0x17		//心跳数据
#define		CMD_MONITOR				0x18		//分机监控

#define		CMD_OPEN_STREAM			0x60		//打开流
#define		CMD_CLOSE_STREAM		0x61		//关闭流
#define		CMD_ABORT_STREAM		0x62		//退出流


//CTI事件返回命令
#define		CMD_EXTPICKUP_EVENT			0x70		//分机摘机 112
#define		CMD_EXTHANGUP_EVENT			0x71		//分机挂机 113
#define		CMD_CALLESTABLISHED_EVENT	0x72		//通话建立 114
#define		CMD_CALLDISCONNECT_EVENT	0x73		//通话结束 115
#define		CMD_CALLERIDCOMING_EVENT	0x74		//来电	   116
#define		CMD_EXTENSIONRINGBACK_EVENT	0x75		//回铃	   117
#define		CMD_RETTRANSDATA_EVENT		0x76		//得到传输数据
#define		CMD_EXTENSIONMSG_EVENT		0x77		//分机消息事件
#define		CMD_EXTONHOLD_EVENT			0x78		//分机保持事件 120
#define		CMD_EXTONRETRIEVE_EVENT		0x79		//分机取回事件
#define		CMD_EXTDIALING_EVENT		0x7A		//分机拨号事件
#define		CMD_EXTTRANSFERED_EVENT		0x7B		//分机转接事件
#define		CMD_FAILEDCONNECTION_EVENT	0x7C		//连接失败事件
#define		CMD_EXTDIVERTCALL_EVENT		0x7D		//分机代接成功事件
#define		CMD_EXTCONFERENCED_EVENT	0x7E		//会议建立成功事件
#define		CMD_MONITOR_EVENT			0x7F		//分机监控事件
#define		CMD_QUEUE_IN				0x80		//入队列
#define		CMD_QUEUE_OUT				0x81		//出队列



//媒体命令
#define		CMD_START_PLAY			0x50		//开始放音
#define		CMD_STOP_PLAY			0x51		//停止放音
#define		CMD_START_RECORD		0x52		//开始录音
#define		CMD_STOP_RECORD			0x53		//停止录音
#define		CMD_MEDIAREGISTER		0x54		//media register
#define		CMD_START_RECV_FAX		0x55		//开始接收传真
#define		CMD_FAX_CHECK_RESULT	0x56		//传真检查结果
#define		CMD_FAX_START_TONE_AN	0x57		//开始传真tone音分析
#define		CMD_MEDIA_GET_TONE_RESULT	0x58	//获取TONE结果
#define		CMD_FAX_START_SEND		0x59		//开始发送传真
#define		CMD_TALK_WITH			0x5A		//开始通道连接
#define		CMD_SEARCH_IDLE_TRK		0x5B		//搜索空闲通道



//媒体事件返回命令
#define		CMD_END_PLAY			0x60		//放音结束
#define		CMD_RECV_DTMF			0x61		//DTMF事件
#define		CMD_END_RECORD			0x62		//录音结束

#define     CMD_ASR_RESPONSE_EVENT  0x30        //asr event

enum
{
    device_type,
    multi_media_type,
};

typedef struct packet_head_t
{
    DWORD       dwFlag;			//协议标识
    WORD        cmd_type;       //命令类型
    WORD        cmd;            //命令编码
    DWORD       dwSize;			//数据长度
}PACKET_HEAD;

typedef struct
{
    int iThreadId;
    int iClientId;
    int iNetworkEvent;
}CONNECTINFO, *PCONNECTINFO;

typedef struct
{
    int     iClientId;
    int     iClientType; // 0 - ext or ivr type, 1 - multi_media_typ
    char    szDevice[128];
}DEVICE_SOCK_INFO, *PDEVICE_SOCK_INFO;

typedef struct
{
    char    szDeviceId[64];
    char    szDestDeviceId[64]; //destinaton device for transfer or conference
    char    szActiveCallID[64];
    char    szHeldCallID[64];
    int     iStartTransferFlag;
    int     iHoldFlag;
    int     iRetrieveFlag;
    int     iConferenceFlag;
    int     iConferenceCount;   //conference total number
    int     iGradeFlag; // 0 - 无满意度调查， 1 -  满意度调查标志
    int     iRingbackFlag; // 0 - 无回铃， 1 - 已产生回铃
    char    szCalleeId[64];
}CALL_INFO, *PCALL_INFO;

typedef struct
{
    char    szUUID[64];
    char    szCalleeId[32];
}CALLEEID_INFO, *PCALLEEID_INFO;

typedef struct
{
    char    szIVRExt[64];
    char    szUUID[64];
    char    szCaller[64];
    int     iRunningFlag;
    int     iChannelId;
    int     iGradeFlag;
}IVR_INFO, *PIVR_INFO;

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


#endif // GLOBALEAESSERVER_H

