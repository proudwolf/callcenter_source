//UrlConvert.cpp
#include "globaleaesserver.h"
#include "urlconvert.h"

unsigned char toHex(const unsigned char &x)
{

    return x > 9 ? x -10 + 'A': x + '0';
}

unsigned char fromHex(const unsigned char &x)
{
    return isdigit(x) ? x-'0' : x-'A'+10;
}

CUrlConvert::CUrlConvert(void)
{
}

CUrlConvert::~CUrlConvert(void)
{
}

// URL解码，解码为GBK
std::string CUrlConvert::URLDecode(std::string& strOrg)
{

    std::string sOut;

    for( size_t ix = 0; ix < strOrg.size(); ix++ )
    {
        unsigned char ch = 0;
        if(strOrg[ix]=='%')
        {
            ch = (fromHex(strOrg[ix+1])<<4);
            ch |= fromHex(strOrg[ix+2]);
            ix += 2;
        }
        else if(strOrg[ix] == '+')
        {
            ch = ' ';
        }
        else
        {
            ch = strOrg[ix];

        }
        sOut += (char)ch;

    }

    return sOut;
}

// URL编码，编码为GBK
std::string CUrlConvert::URLEncode(std::string& strOrg)
{
    std::string sOut;

    for( size_t ix = 0; ix < strOrg.size(); ix++ )
    {
        unsigned char buf[4];
        memset( buf, 0, 4 );
        if( isalnum( (unsigned char)strOrg[ix] ) )
        {
            buf[0] = strOrg[ix];
        }
        //else if ( isspace( (byte)strOrg[ix] ) ) //貌似把空格编码成%20或者+都可以
        //{
        //    buf[0] = '+';
        //}
        else
        {
            buf[0] = '%';
            buf[1] = toHex( (unsigned char)strOrg[ix] >> 4 );
            buf[2] = toHex( (unsigned char)strOrg[ix] % 16);
        }
        sOut += (char *)buf;
    }
    return sOut;
}

// URL解码，解码为GBK
std::string CUrlConvert::UrlDecode_GBK(std::string strOrg)
{
    return URLDecode(strOrg);
}








