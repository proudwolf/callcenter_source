#ifndef OCXDLL_H
#define OCXDLL_H

#include "GlobalTsapiServer.h"

class COCXInterface
{
public:
    COCXInterface();
    virtual ~COCXInterface();
public:
    bool Init();
    bool ConnectToServer();
    void SendMsg2Extension(char *srcDN, char *destDN, char *param1, char *param2, char *param3);
    void SetTransferedData(char *szExtNo, char *TansferedData);
public:
    bool m_bReg;
    SETAGENTTYPE                    SetAgentTypeFunc;
    SETDUALIP                       SetDualIPFunc;
    CONNECTTOSERVER                 ConnectToServerFunc;
    REGISTEREXTENSION               RegisterExtentionFunc;
    SENDCMD                         SendCmdFunc;
    SENDMSG2EXTEINTION              SendMsg2ExtFunc;
    SENDMSG2ALL                     SendMsg2AllFunc;
    LPSETOCXINTERFACEEVENTCALLBACK  lpSetOCXEventCallbalck;
    LPOCXINTERFACEEVENT             lpfnOCXInterfaceEvent;
    SETAGENTPARAM                   SetAgentParamFunc;
};

#endif // OCXDLL_H

