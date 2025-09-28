#ifndef COMMONENGINEDLL_H
#define COMMONENGINEDLL_H
#include "globaleaesserver.h"

class CCommonEngineDll
{
public:
    CCommonEngineDll();
    virtual ~CCommonEngineDll();
public:
    bool DllInit();
    void ParamInit();
public:
    NETWORKINIT NetworkInitFunc;
    SETPORT SetPortFunc;
    RECVCOMPLETE RecvCompleteFunc;
    SENDTOCLIENT SendToClientFunc;
    CLOSESOCK CloseSockFunc;
    TERMINATEDLLTHREAD	TerminateDllThreadFunc;
    LPSETSOCKETMSGEVENTCALLBACK SetSocketMsgEventCallbackFunc;
private:
    int m_iPort;
};

#endif // COMMONENGINEDLL_H

