#ifndef URLCONVERT_H
#define URLCONVERT_H


#include <string>

class CUrlConvert
{
public:

    CUrlConvert(void);
    ~CUrlConvert(void);
    // URL编码，编码为GBK
    std::string UrlEncode_GBK(std::string strOrg);
    // URL编码，编码为UTF-8
    std::string UrlEncode_UTF8(std::string strOrg);
    // URL解码，解码为GBK
    std::string UrlDecode_GBK(std::string strOrg);
    // URL解码，解码为UTF-8
    std::string UrlDecode_UTF8(std::string strOrg);
private:
    // URL解码，解码为GBK
    std::string URLDecode(std::string& strOrg);
    // URL编码，编码为GBK
    std::string URLEncode(std::string& strOrg);
    std::string GBKToUTF8(const std::string &strGBK);
    std::string UTF8ToGBK(const std::string &strUTF8);
};

#endif // URLCONVERT_H

