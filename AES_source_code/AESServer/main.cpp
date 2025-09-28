#include <QCoreApplication>
#include "globaleaesserver.h"
#include "clientsock.h"
#include "commonenginedll.h"
#include "dataproc.h"
#include "ctictrl.h"

void TraceEx( char *szMessage, ...);
void MainParamInit();
bool CheckSeriNo();
void GetFileCreateTime();

CClientSock m_clientSock;
CCommonEngineDll m_CommonEngineDll;
CDataProc   m_DataProc;
CCtiCtrl    m_CtiCtrl;

char    m_lpszFSIPAddr[32];
int     m_iFSPort;
char    m_lpszFSPasswd[64];
char    m_lpszCardId[256];
bool    m_bSeriCorrect;
int     m_iLisenceNumber;
int     m_iLimitDay;
int     m_record_all_log = 0;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MainParamInit();
    m_DataProc.InitParam();

    bool bResult = CheckSeriNo();

    m_CommonEngineDll.ParamInit();

    if( !bResult )
    {
        TraceEx( "序列号验证错误，请与软件开发商联系!" );
        return 1;
    }

    if( m_CommonEngineDll.DllInit() )
    {
        m_clientSock.SetDualIP( m_lpszFSIPAddr, m_lpszFSIPAddr ); //"121.40.143.232");
        m_clientSock.StartConnectServerThread( m_iFSPort );//4521 );
    }
    else
    {
        TraceEx( "error, commone_engine_dll init failed!");
    }

    return a.exec();
}

void MainParamInit()
{
    QSettings *configIni = new QSettings("AESServer.ini", QSettings::IniFormat);

    QString strFSIPAddr = configIni->value("/FSSet/FSIPAddr").toString();

    strFSIPAddr = (strFSIPAddr.length() > 0)?strFSIPAddr:"192.168.1.18";
    memset( m_lpszFSIPAddr, 0, 32 );
    strcpy( m_lpszFSIPAddr, (char *)strFSIPAddr.toStdString().c_str() );
    configIni->setValue("/FSSet/FSIPAddr", strFSIPAddr);


    QString strFSPort = configIni->value("/FSSet/FSPort").toString();

    strFSPort = (strFSPort.length() > 0)?strFSPort:"8021";
    m_iFSPort = atoi( (char *)strFSPort.toStdString().c_str() );
    configIni->setValue("/FSSet/FSPort", strFSPort);


    QString strFSPasswd = configIni->value("/FSSet/FSPasswd").toString();

    strFSPasswd = (strFSPasswd.length() > 0)?strFSPasswd:"rain";
    memset( m_lpszFSPasswd, 0, 64 );
    strcpy( m_lpszFSPasswd, (char *)strFSPasswd.toStdString().c_str() );
    configIni->setValue("/FSSet/FSPasswd", strFSPasswd);


    QString strRecordAllLog = configIni->value("/FSSet/RecordAllLog").toString();

    strRecordAllLog = (strRecordAllLog.length() > 0)?strRecordAllLog:"0";
    m_record_all_log = atoi( (char *)strRecordAllLog.toStdString().c_str() );
    configIni->setValue("/FSSet/RecordAllLog", strRecordAllLog);

    delete configIni;
}

void GetFileCreateTime(char *lpszDateTime)
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

void getAdapterInfoWithQt(char *lpszHardWareAddr, char *lpszIPAddr)
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
                    m_DataProc.DecodeFrame( strInfo, strData, ":", 1 );
                    sprintf( lpszHardWareAddr, "%s%s%s", (char *)strInfo[1].c_str(), (char *)strInfo[3].c_str(), (char *)strInfo[5].c_str());


                    string strIPInfo[16] = {""};
                    strData = addressEntryItem.ip().toString().toStdString();
                    m_DataProc.DecodeFrame( strIPInfo, strData, ".", 1 );
                    sprintf( lpszIPAddr, "%s.%s", (char *)strIPInfo[2].c_str(), (char *)strIPInfo[3].c_str() );

                    return;
                }
            }
        }
    }
}


bool CheckSeriNo()
{
    m_bSeriCorrect = false;

    char lpszHardWareAddr[64] = {0},lpszIPAddr[32] = {0};
    getAdapterInfoWithQt(lpszHardWareAddr, lpszIPAddr);

    char lpszCreateDateTime[64] = {0};
    GetFileCreateTime( lpszCreateDateTime );

    QSettings *configIni = new QSettings("AESServer.ini", QSettings::IniFormat);

    char lpszSeriNo[256] = {0}, lpszOutput[256] = {0};
    QString strSeriNo = configIni->value("/System/SeriNo").toString();
    configIni->setValue("/System/SeriNo", strSeriNo);
    strcpy( lpszSeriNo, (char *)strSeriNo.toStdString().c_str() );

    delete configIni;

    //decode --- created_time
    encrpt_decode( lpszSeriNo, strlen(lpszSeriNo), lpszOutput );

    string strInfo[16] = {""}, strData = "";
    strData = lpszOutput;
    m_DataProc.DecodeFrame( strInfo, strData, ",", 1 );

    char lpszHardWareAddr_decoder[64] = {0}, lpszIPAdddr_decoder[64] = {0}, lpszCreateDateTime_decoder[64] = {0};
    strcpy( lpszHardWareAddr_decoder, (char *)strInfo[0].c_str() );
    strcpy( lpszIPAdddr_decoder, (char *)strInfo[1].c_str() );
    strcpy( lpszCreateDateTime_decoder, (char *)strInfo[2].c_str() );

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
        m_iLisenceNumber = atoi( (char *)strInfo[3].c_str() );
        m_iLimitDay = atoi( (char *)strInfo[4].c_str() );
    }

    return m_bSeriCorrect;
}


void TraceEx( char *szMessage, ...)
{
    va_list VAList;
    char szMsgBuf[1024*200] = {0};
    va_start(VAList,szMessage);

    sprintf(szMsgBuf,szMessage,VAList);

    char szFile[256],szTmp[256];

    QDateTime current_dt = QDateTime::currentDateTime();

    QString strDate = current_dt.toString("yyyy-MM-dd");
    QString strDT = current_dt.toString("yyyy-MM-dd hh:mm:ss.zzz");

#ifdef _VC6_
    sprintf(szFile,".\\aes_log");
#else
    sprintf(szFile,"aes_log");
#endif
    if(access(szFile, 0) == -1)
    {
        QDir *temp = new QDir;
        if( temp == NULL )
            return;

        if( !temp->mkdir(szFile) )
        {
#ifdef _VC6_
            strcpy(szFile ,  (".\\temp"));
#else
            strcpy(szFile ,  ("temp"));
#endif
        }

        delete temp;
        temp = NULL;
    }

    FILE *pFile;
#ifdef _VC6_
    sprintf(szTmp,"\\%s.log",strDate.toStdString().c_str() );
#else
    sprintf(szTmp,"/%s.log",strDate.toStdString().c_str() );
#endif
    strcat(szFile,szTmp);
    pFile=fopen(szFile,"a+");
    if( pFile != NULL )
    {
        fprintf(pFile,"[%s]%s\r\n", strDT.toStdString().c_str() ,szMsgBuf);

#ifndef _VC6_
        printf( "[%s]%s\r\n", strDT.toStdString().c_str() ,szMsgBuf);
#endif
        fclose(pFile);
    }
}
