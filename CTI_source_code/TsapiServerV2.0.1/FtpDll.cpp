#include "GlobalTsapiServer.h"
#include "FtpDll.h"

/*
#ifdef _linux_
char  ftp_dll_file[128] = "libDataTrans.so";
#else
char  ftp_dll_file[128] = "DataTrans.dll";
#endif

QLibrary ftpDll(ftp_dll_file);
*/
//extern void TraceEx( char *buf, ...);

CFtpDll::CFtpDll()
{

}

CFtpDll::~CFtpDll()
{
 //   ftpDll.unload();
}

bool CFtpDll::FtpDllInit()
{
    return true;
    /*
    m_startFtpEngineFunc = NULL;
    memset( m_lpszFlag, 0, 32 );
    strcpy( m_lpszFlag, "funk" );

    bool bResult = false;

    if (!ftpDll.load())
    {
        TraceEx( "error, ftpDll.dll load failed!");
        return bResult;
    }

    m_ftpFreeAllDataFunc = NULL;
    m_ftpFreeAllDataFunc = ( FTPFREEALLDATA )ftpDll.resolve( "FreeData" );
    if( m_ftpFreeAllDataFunc == NULL )
    {
        TraceEx( "GetProcAddress - m_ftpFreeAllData failed!" );
        return bResult;
    }

    m_startFtpEngineFunc = NULL;
    m_startFtpEngineFunc = ( STARTFTPENGINE )ftpDll.resolve( "StartFtpEngine" );
    if( m_startFtpEngineFunc == NULL )
    {
        TraceEx( "GetProcAddress - m_ftpFreeAllData failed!" );
        return bResult;
    }

	return m_startFtpEngineFunc(m_lpszFlag);
    */
}
