#pragma once
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>

namespace codeutil
{
//
inline int code_convert(char *from_charset,char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
    iconv_t cd;
    //int rc;
    char **pin = &inbuf;
    char **pout = &outbuf;
 
    cd = iconv_open(to_charset,from_charset);
    if (cd==0) return -1;
    memset(outbuf,0,outlen);
    if (-1==(int)iconv(cd,pin,&inlen,pout,&outlen))
	return -1;
    iconv_close(cd);
    return 0;
}
//
inline int u2g(char *inbuf,int inlen,char *outbuf,int outlen)
{
	char _fbuf[32]="utf-8";
	char _tbuf[32]="gb2312";
    return code_convert(_fbuf,_tbuf,inbuf,inlen,outbuf,outlen);
}
//
inline int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
	char _fbuf[32]="gb2312";
	char _tbuf[32]="utf-8";
    return code_convert(_fbuf,_tbuf,inbuf,inlen,outbuf,outlen);
}
 
inline int u2a(char *inbuf,int inlen,char *outbuf,int outlen)
{
	char _fbuf[32]="utf-8";
	char _tbuf[32]="ascii";
    return code_convert(_fbuf,_tbuf,inbuf,inlen,outbuf,outlen);
}
 
inline int a2u(char *inbuf,int inlen,char *outbuf,int outlen)
{
	char _fbuf[32]="ascii";
	char _tbuf[32]="utf-8";
    return code_convert(_fbuf,_tbuf,inbuf,inlen,outbuf,outlen);
}
 
inline int u2k(char *inbuf,int inlen,char *outbuf,int outlen)
{
	char _fbuf[32]="utf-8";
	char _tbuf[32]="gbk";
    return code_convert(_fbuf,_tbuf,inbuf,inlen,outbuf,outlen);
}
 
inline int k2u(char *inbuf,int inlen,char *outbuf,int outlen)
{
	char _fbuf[32]="gbk";
	char _tbuf[32]="utf-8";
    return code_convert(_fbuf,_tbuf,inbuf,inlen,outbuf,outlen);
}
 
//utf-8 to ascii
 
inline std::string UTF_82ASCII(std::string& strUtf8Code)
{
	char lpszBuf[256]={0};
	u2k(const_cast<char*>(strUtf8Code.c_str()),strUtf8Code.size(),lpszBuf,256);
	return std::string(lpszBuf);
}
 
//ascii to Utf8
 
inline std::string ASCII2UTF_8(std::string& strAsciiCode)
{
	char lpszBuf[256]={0};
	k2u(const_cast<char*>(strAsciiCode.c_str()),strAsciiCode.size(),lpszBuf,256);
	return std::string(lpszBuf);
}

unsigned char ToHex(unsigned char x)
{
	return  x > 9 ? x + 55 : x + 48;
}
 
unsigned char FromHex(unsigned char x)
{
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else assert(0);
	return y;
}
 
std::string UrlEncode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ')
			strTemp += "%20";
		else
		{
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] % 16);
		}
	}
	return strTemp;
}
 
std::string UrlDecode(const std::string& str)
{
	std::string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++)
	{
		if (str[i] == '+') strTemp += ' ';
		else if (str[i] == '%')
		{
			assert(i + 2 < length);
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			strTemp += high * 16 + low;
		}
		else strTemp += str[i];
	}
	return strTemp;
}

}