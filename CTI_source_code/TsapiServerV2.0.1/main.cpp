#include <QCoreApplication>
#include "GlobalTsapiServer.h"
#include "Lock.h"
#include "mainentry.h"

void TraceEx( char *buf, ...);

CMainEntry m_MainEntry;

char recPrivateData[SIZE_OF_ALCATEL_PRIVATE_DATA];

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    printf( "tsapiserver begin to start now...\r\n");

    m_MainEntry.MainInit();

    return a.exec();
}

void TraceEx( char *szMessage, ...)
{
    va_list VAList;
    char szMsgBuf[1024*100] = {0};
    va_start(VAList,szMessage);

    sprintf(szMsgBuf,szMessage,VAList);

    char szFile[256],szTmp[256];

    QDateTime current_dt = QDateTime::currentDateTime();

    QString strDate = current_dt.toString("yyyy-MM-dd");
    QString strDT = current_dt.toString("yyyy-MM-dd hh:mm:ss.zzz");

#ifdef _VC6_
    sprintf(szFile,".\\log");
#else
    sprintf(szFile,"log");
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
