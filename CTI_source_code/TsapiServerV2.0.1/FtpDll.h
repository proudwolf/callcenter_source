#ifndef _FTP_DLL_H_
#define _FTP_DLL_H_

class CFtpDll
{
public:
	CFtpDll();
	virtual ~CFtpDll();
public:
    bool FtpDllInit();
public:
	STARTFTPENGINE	m_startFtpEngineFunc;
	FTPFREEALLDATA	m_ftpFreeAllDataFunc;
private:
	char	m_lpszFlag[32];
};


#endif
