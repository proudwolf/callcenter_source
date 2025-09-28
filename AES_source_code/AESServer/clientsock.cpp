#include "globaleaesserver.h"
#include "clientsock.h"
#include "dataproc.h"

extern CClientSock m_clientSock;
extern CDataProc   m_DataProc;

extern void TraceEx( char *szMessage, ...);
extern char    m_lpszFSPasswd[64];

#define MAX_READ_BYTE 1024*50

#ifdef _VC6_
HANDLE hRecvThread = NULL;
HANDLE hOnTimerThread = NULL;
#endif

int g_tcpOcxSock = 0;
bool bocxConnected = false;
bool bTerminateRecvThread = false;

pthread_t iocxdllRecvThreadId = 0;
pthread_t iocxdllOnTimerThreadId = 0;

list<char *> m_recvBufList;

QMutex        m_recvBuf_cs;
sem_t         recv_data_signale;

void *HandleRecvBufThread(void *lpVoid)
{
    while(1)
    {
        sem_wait(&recv_data_signale); //WaitForSingleObject --- used in vc

        if( bTerminateRecvThread )
            break;

        char *buf = NULL;
        int iTemp = 1, iSize = 0;
        if( iTemp > 0 )
        {
            QMutexLocker locker(&m_recvBuf_cs);

            if( m_recvBufList.size() == 0 )
                continue;

            buf = m_recvBufList.front();
            m_recvBufList.pop_front();

            iSize = m_recvBufList.size();
        }

        if( iSize > 0 )
            sem_post( &recv_data_signale );

        string strData = "";
        strData = buf;
        string strInfo[1024] = {""};
        int iCount = m_DataProc.DecodeFrame( strInfo, strData, "\n\n", 2);

        for(int i = 0; i < iCount; i++)
        {
            char szLog[1024] = {0};
            sprintf( szLog, "data_proc_event, i = %d, iCount = %d", i, iCount );
            TraceEx( szLog );
            m_DataProc.data_proc_event( (char *)strInfo[i].c_str() );
        }

        delete[] buf;
        buf = NULL;
    }
}

void *recv_ocxdll_thread(void *lpVoid)
{
    fd_set fdread;
    char szLog[256] = {0};
    while(1)
    {
        FD_ZERO(&fdread);
        FD_SET(g_tcpOcxSock,&fdread);

        struct  timeval timeout;
        timeout.tv_sec = 15;
        timeout.tv_usec = 0;
        //select模式
#ifdef _windows_
        int ret=select(0,&fdread,NULL,NULL,&timeout);
#else
        int ret=select(g_tcpOcxSock + 1,&fdread,NULL,NULL,&timeout);
#endif
        if(ret<0)
        {
            iocxdllRecvThreadId = 0;
            bocxConnected = false;
#ifdef _windows_
            sprintf( szLog, "error, select fd_read failed, dwErrorCode = %d", GetLastError() );
#else
            sprintf( szLog, "error, select fd_read failed, dwErrorCode = %s", strerror( errno ) );
#endif
            TraceEx(szLog);
            break;
        }
        else if(ret==0)
        {
            continue;
        }

        if(FD_ISSET(g_tcpOcxSock,&fdread))
        {
            char buf[MAX_READ_BYTE] = {0};
            ret=recv(g_tcpOcxSock,buf,MAX_READ_BYTE,0);
            if( ret <= 0 )
            {
                FD_CLR(g_tcpOcxSock,&fdread);
                iocxdllRecvThreadId = 0;
                bocxConnected = false;
    #ifdef _windows_
                sprintf( szLog, "error, recv failed, dwErrorCode = %d", GetLastError() );
    #else
                sprintf( szLog, "error, recv failed, dwErrorCode = %s", strerror( errno ) );
    #endif
                TraceEx(szLog);
                break;
            }
            else
            {
                m_clientSock.OnRecv( buf, ret );
            }
        }
    }
}

void *on_ocxdll_timer_thread(void *lpVoid)
{
    while( 1 )
    {
        char szLog[512] = {0};
        sprintf( szLog, "on_ocxdll_timer_thread,  g_tcpOcxSock = %d, bocxConnected = %d",  g_tcpOcxSock, bocxConnected );
        TraceEx( szLog );
        if( g_tcpOcxSock > 0 && bocxConnected )
        {
            char buf[128] = {0};
            strcpy( buf, "keep_alive\n\n");
            m_clientSock.SendToClient( buf, strlen( buf ));
        }
        else
        {
            m_clientSock.ReconnectServer();
        }

        sleep( 10 );
    }
    return 0;
}

CClientSock::CClientSock()
{
#ifdef _windows_
    WSADATA WsaData;
    WSAStartup (0x0202, &WsaData);
#endif

    sem_init( &recv_data_signale, 0, 0 );
    m_recvBufList.clear();

    pthread_t iHandleRecvBufThreadId;
    if( pthread_create(&iHandleRecvBufThreadId, NULL, HandleRecvBufThread, NULL) != 0 )
    {
        TraceEx("Create handle_recbuffer_pthread error!\n");
    }
}

CClientSock::~CClientSock()
{
    CloseSock();

    bTerminateRecvThread = true;
    sem_post( &recv_data_signale );
    usleep( 200*1000 );
    ClearAllRecvBufList();

    sem_destroy( &recv_data_signale );

#ifdef _windows_
    WSACleanup();
#endif
}

void CClientSock::SetIPAddr(char *lpszAddr)
{
    memset( m_szIPAddr, 0, 32 );
    strcpy( m_szIPAddr, lpszAddr );
}

void CClientSock::CloseSock()
{
    shutdown( g_tcpOcxSock, 2 );
#ifdef _windows_
    closesocket( g_tcpOcxSock );
#else
    close( g_tcpOcxSock );
#endif

    g_tcpOcxSock = 0;
}

bool CClientSock::ocxdll_ConnectToServer(short nPort)
{
    TraceEx( "--------------------in aes-----------------------");
    char szLog[256] = {0};
    sprintf( szLog, "aes begin to connect server socket = %d, m_lpszIPAddr = %s, port = %d", g_tcpOcxSock, m_szIPAddr, nPort );
    TraceEx( szLog );

    if( g_tcpOcxSock > 0 )
        CloseSock();

    if( !CreateOCXSocket() )
        return false;

    sockaddr_in addr;
    memset( &addr, 0, sizeof( sockaddr_in ) );
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(m_szIPAddr);
    addr.sin_port = htons(nPort);


    sprintf( szLog, "aes  connect server, sock_clinet = %d", g_tcpOcxSock );
    TraceEx( szLog );
    if( connect( g_tcpOcxSock, (struct sockaddr *)&addr, sizeof( sockaddr ) ) < 0 )
    {
#ifdef _windows_
        sprintf( szLog, "aes connect to server failed, dwErrorCode = %d", GetLastError() );
        closesocket( g_tcpOcxSock );
#else
        sprintf( szLog, "aes connect to server failed, dwErrorCode = %s", strerror( errno) );
        close( g_tcpOcxSock );
#endif
        TraceEx( szLog );

        return false;
    }

    bocxConnected = true;

    if( iocxdllRecvThreadId == 0 )
    {
        if( pthread_create(&iocxdllRecvThreadId, NULL, recv_ocxdll_thread, NULL) != 0 )
        {
            TraceEx("Create recv_pthread error!\n");
            return false;
        }
    }

    return true;
}

int CClientSock::SendToClient(char *buf, int data_len)
{
    if( buf == NULL || data_len <= 0 )
        return -1;

    return BeginToSendData(buf, data_len);
}

void CClientSock::AddToRecvBufList(char *buf, int recvLen)
{
    if( buf == NULL || recvLen <= 0 )
        return;

    char *data = new char[recvLen + 1];
    if( data == NULL )
    {
        TraceEx( "error, add_to_recv_buflist, malloc failed!");
        return;
    }
    else
    {
        memset( data, 0, recvLen + 1 );
        memcpy( data, buf, recvLen );

        QMutexLocker locker(&m_recvBuf_cs);
        m_recvBufList.push_back( data );
    }

    sem_post( &recv_data_signale ); //set event and notify comm_data reached
}

void CClientSock::OnRecv(char *data, int len)
{
    if( data == NULL || len <= 0 )
        return;

    char szLog[1024*20] = {0};
    sprintf( szLog, "recv_data, len = %d", len);
    TraceEx( szLog );

    AddToRecvBufList( data, len );

    /*
    string strData = "";
    strData = data;
    string strInfo[1024] = {""};
    int iCount = m_DataProc.DecodeFrame( strInfo, strData, "\n\n", 2);

    for(int i = 0; i < iCount; i++)
    {
        m_DataProc.data_proc_event( (char *)strInfo[i].c_str() );
    }
    */

}


bool CClientSock::CreateOCXSocket()
{
    bool bResult =	true;
    g_tcpOcxSock = socket(AF_INET, SOCK_STREAM, 0);
    if( g_tcpOcxSock <= 0 )
    {
        char szLog[256] = {0};
#ifdef _windows_
        sprintf( szLog, "error, create_sock failed, dwErrorCode = %d", GetLastError() );

#else
        sprintf( szLog, "error, create_sock failed, dwErrorCode = %s", strerror( errno) );
#endif
        TraceEx( szLog );
        bResult = false;
    }

    char szLog[256] = {0};
    sprintf( szLog, "create_sock, sock_client = %d, bResult = %d", g_tcpOcxSock, bResult );
    TraceEx( szLog );
    return bResult;
}

int CClientSock::SendNextData(char *buf, int iDataLen)
{
    if( buf == NULL || iDataLen < 0 )
        return -1;


    int iLen = -1;

    if( g_tcpOcxSock > 0 && bocxConnected )
    {
        if( ( iLen = send( g_tcpOcxSock, buf, iDataLen, 0 ) ) < 0 )
        {
            char szLog[1024*100] = {0};
#ifdef _windows_
            DWORD dwErrorCode = GetLastError();
            if( dwErrorCode == 10035 )
                return 0;
            sprintf( szLog, "SendNextData errcode=%d!!!!",dwErrorCode);
#else
            sprintf( szLog, "SendNextData buf = %s, len = %d, errcode=%s!!!!", buf, iDataLen, strerror( errno ));
#endif
            TraceEx( szLog );
            CloseSock();
            return -1;
        }
    }

    return iLen;
}

unsigned long CClientSock::BeginToSendData(char *lpszSendBuf, int iDataLen)
{
    int iBytesSent = 0;
    int iBytesThisTime;
    char *lpszBuf = lpszSendBuf;
    do{
        iBytesThisTime = SendNextData( lpszBuf, iDataLen - iBytesSent );
        if( iBytesThisTime < 0 )
            return 0;
        iBytesSent += iBytesThisTime;
        lpszBuf += iBytesThisTime;
    }while( iBytesSent < iDataLen );

    return iDataLen;
}

void CClientSock::SetDualIP(char *IP1, char *IP2)
{
    if( IP1 == NULL || IP2 == NULL )
        return;

    memset( m_lpszIP1, 0, 32 );
    memset( m_lpszIP2, 0, 32 );
    memset( m_szIPAddr, 0, 32 );

    strcpy( m_lpszIP1, IP1 );
    strcpy( m_lpszIP2, IP2 );
    strcpy( m_szIPAddr, IP2 );
}

void CClientSock::ReconnectServer()
{
    GetReconnectAddr();

    char szLog[512] = {0};
    sprintf( szLog, "aes, reconnect server, m_szIPAddr = %s,  m_iPort = %d", m_szIPAddr, m_iPort );
    TraceEx( szLog );

    BeginToConnectServer( m_iPort );
}

char *CClientSock::GetReconnectAddr()
{
    if( strcmp( m_lpszIP1, m_szIPAddr ) == 0 )
    {
        memset( m_szIPAddr, 0, 32 );
        strcpy( m_szIPAddr, m_lpszIP2 );
    }
    else
    {
        memset( m_szIPAddr, 0, 32 );
        strcpy( m_szIPAddr, m_lpszIP1 );
    }

    return m_szIPAddr;
}

bool CClientSock::StartConnectServerThread(short nPort)
{
    iocxdllOnTimerThreadId = 0;
    bocxConnected = false;
    m_iPort = nPort;
    char szLog[512] = {0};
    sprintf(szLog, "aes,  start_connect_server_thread, iocxdllOnTimerThreadId = %d, bocxConnected = %d,  port = %d", iocxdllOnTimerThreadId, bocxConnected,  m_iPort);
    TraceEx(szLog);
    if( iocxdllOnTimerThreadId == 0 )
    {

        if( pthread_create(&iocxdllOnTimerThreadId, NULL, on_ocxdll_timer_thread, NULL) != 0 )
        {
            TraceEx("Create on_timer_pthread error!\n");
            return false;
        }

    }


    return true;
}

bool CClientSock::BeginToConnectServer(short nPort)
{
    char szLog[512] = {0};
    sprintf( szLog, "in aes,  call connect_to_server,  port = %d", nPort);
    TraceEx( szLog );

    bool bResult = ocxdll_ConnectToServer( nPort );
    if( bResult )
    {
        usleep( 500*1000 );
        char lpszSendBuffer[256] = {0};
        sprintf( lpszSendBuffer, "auth %s\n\n", m_lpszFSPasswd );

 //       strcpy(  lpszSendBuffer, "auth rain\n\n"); //"auth wzw\n\n");

        SendToClient( lpszSendBuffer, strlen( lpszSendBuffer ));

        sleep( 1 );
        memset( lpszSendBuffer, 0, 256 );
        strcpy( lpszSendBuffer, "event plain RECORD_STOP DTMF CHANNEL_APPLICATION CHANNEL_CREATE CHANNEL_ANSWER BACKGROUND_JOB CHANNEL_HOLD CHANNEL_UNHOLD CHANNEL_PARK CHANNEL_HANGUP PLAYBACK_STOP CUSTOM asr sofia::register sofia::unregister sofia::expire callcenter::info\n\n");
        //strcpy( lpszSendBuffer, "event plain all\n\n");
        SendToClient( lpszSendBuffer, strlen( lpszSendBuffer ));
    }

    return bResult;
}

void CClientSock::ClearAllRecvBufList()
{
    if( m_recvBufList.size() == 0 )
        return;

    list<char *>::iterator it;
    for( it = m_recvBufList.begin(); it != m_recvBufList.end(); ++it )
    {
        char *buf = *it;
        if( buf != NULL )
        {
            delete[] buf;
            buf = NULL;
        }
    }

    m_recvBufList.clear();
}

