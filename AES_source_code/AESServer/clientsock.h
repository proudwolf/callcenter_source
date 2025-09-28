#ifndef CLIENTSOCK_H
#define CLIENTSOCK_H

class CClientSock
{
public:
    CClientSock();
    virtual ~CClientSock();
public:
    bool BeginToConnectServer(short nPort);
    bool StartConnectServerThread(short nPort);
    void ReconnectServer();
    void SetDualIP(char *IP1, char *IP2);
    void OnRecv(char *data, int len);
    void SetIPAddr(char *lpszAddr);
    void CloseSock();
    bool ocxdll_ConnectToServer(short nPort);
    int SendToClient(char *buf, int data_len);

public:
    int m_iPort;
private:
    void ClearAllRecvBufList();
    void AddToRecvBufList(char *buf, int recvLen);
    char *GetReconnectAddr();
    bool CreateOCXSocket();
    int SendNextData(char *buf, int iDataLen);
    unsigned long BeginToSendData(char *lpszSendBuf, int iDataLen);
private:
    char m_szIPAddr[32];
    char m_lpszIP1[32];
    char m_lpszIP2[32];
};

#endif // CLIENTSOCK_H

