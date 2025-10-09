#ifndef CTICLIENTMODLEEVENT_H
#define CTICLIENTMODLEEVENT_H

#include "GlobalTsapiServer.h"

class CCtiClientModleEvent
{
public:
    CCtiClientModleEvent();
    virtual ~CCtiClientModleEvent();
public:
    void OnConnectServerSucEvent();
    void OnConnectServerFailedEvent();
    void OnRegisterExtensionEvent(string *strInfo, PACKET_HEAD *pHead);
    void OnCallMsgEvent(char * szDestExtNo, char * SourceNumber, char * Param1, char * Param2, char * Param3);
};

#endif // CTICLIENTMODLEEVENT_H

