#include "StdAfx.h"
#include "ConvertWA.h"
#include <windows.h>

//多字头转宽字头
std::wstring ConvertWA::ConvertWA_A2W(string strSrc)
{
	std::wstring wstrDst = L"";
	if(strSrc == "")
	{
		return wstrDst;
	}
	const char* chSrc = strSrc.c_str();
	int nStrLen = strSrc.length();
	int nEffStrLen = MultiByteToWideChar( CP_ACP, 0,chSrc, -1, NULL, NULL);;
	wchar_t* wchDst = new wchar_t[nEffStrLen+1];
	::memset(wchDst,0,nEffStrLen*sizeof(wchar_t));
	MultiByteToWideChar( CP_ACP, 0,chSrc, -1, wchDst, nEffStrLen);
	wstrDst = wchDst;
	delete wchDst;
	return wstrDst;
}

//宽字头转多字头
std::string ConvertWA::ConvertWA_W2A(std::wstring wstrSrc) 
{ 
	std::string strDst = "";
	if (wstrSrc == L"") 
	{
		return strDst; 
	}
	int nStrLen = wstrSrc.length();
	const wchar_t* wchSrc = wstrSrc.c_str();
	int nEffStrLen = WideCharToMultiByte(CP_ACP, 0, wchSrc, -1, NULL, NULL, NULL, NULL); 
	char* chDst = new char[nEffStrLen]; 
	::memset(chDst,0,nEffStrLen*sizeof(char));
	WideCharToMultiByte(CP_ACP, 0, wchSrc, -1, chDst, nEffStrLen, NULL, NULL); 
	strDst = chDst; 
	delete chDst; 
	return strDst; 
}
std::string ConvertWA::UTF8_To_string(const std::string & str)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴  
	memset(pwBuf, 0, nwLen * 2 + 2);

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr = pBuf;

	delete[]pBuf;
	delete[]pwBuf;

	pBuf = NULL;
	pwBuf = NULL;

	return retStr;
}
std::string ConvertWA::string_To_UTF8(const std::string & str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴  
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}



/************************************************************************
*   功能：将一个十六进制字节串转换成 ASCII 码表示的十六进制的字符串
*   输入参数：pHex    -- 十六进制数字节串首地址
*            pAscii -- 转换后的 ASCII 码表示的十六进制字符串的首地址
*            nLen    -- 要转换的十六进制数的长度（字节数）
*   输出参数：None
*   注：  转换后的结果全部是大写 ASCII 表示的十六进制数
************************************************************************/
void ConvertWA::HexToAscii(unsigned char * pHex, unsigned char * pAscii, int nLen)
{
	unsigned char Nibble[2];

	for (int i = 0; i < nLen; i++)
	{
		Nibble[0] = (pHex[i] & 0xF0) >> 4;
		Nibble[1] = pHex[i] & 0x0F;
		for (int j = 0; j < 2; j++)
		{
			if (Nibble[j] < 10)
				Nibble[j] += 0x30;
			else
			{
				if (Nibble[j] < 16)
					Nibble[j] = Nibble[j] - 10 + 'A';
			}
			*pAscii++ = Nibble[j];
		}   // for (int j = ...)
	}   // for (int i = ...)
	*pAscii = '\0';
}

/****************************************************************************
*   功能：将一个 ASCII 码表示的十六进制字符串转换成十六进制的字节串
*   输入参数：pAscii -- 转换后的 ASCII 码表示的十六进制字符串的首地址
*            pHex   -- 十六进制数字节串首地址
*            nLen   -- 要转换的 ASCII 码表示的十六进制字符串的长度（字节数）
*   输出参数：None
*   注：  要求输入的 ASCII 码表示的十六进制数的字符个数必须为偶数，除了是1 - 9 和 A(a) - F(f) 以外没有别的字符
****************************************************************************/
void ConvertWA::AsciiToHex(unsigned char * pAscii, unsigned char * pHex, int nLen)
{
	if (nLen % 2)
		return;
	int nHexLen = nLen / 2;

	for (int i = 0; i < nHexLen; i++)
	{
		unsigned char Nibble[2];
		Nibble[0] = *pAscii++;
		Nibble[1] = *pAscii++;
		for (int j = 0; j < 2; j++)
		{
			if (Nibble[j] <= 'F' && Nibble[j] >= 'A')
				Nibble[j] = Nibble[j] - 'A' + 10;
			else if (Nibble[j] <= 'f' && Nibble[j] >= 'a')
				Nibble[j] = Nibble[j] - 'a' + 10;
			else if (Nibble[j] >= '0' && Nibble[j] <= '9')
				Nibble[j] = Nibble[j] - '0';
			else
				return;
		}   // for (int j = ...)
		pHex[i] = Nibble[0] << 4;   // Set the high nibble
		pHex[i] |= Nibble[1];   //Set the low nibble
	}   // for (int i = ...)
}


/***十六进制数转换成二进制字符串***/
void ConvertWA::HexToBinStr(unsigned char* hexStr, unsigned char* binStr, int lenHex)
{
	memset(binStr, '0', lenHex * 8);
	unsigned char hexChar[2];
	for (int i = 0; i < lenHex; i++)
	{
		hexChar[0] = (hexStr[i] & 0xF0) >> 4;
		hexChar[1] = hexStr[i] & 0x0F;
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				if (hexChar[j] & (0x08 >> k))
				{
					binStr[8 * i + 4 * j + k] = '1';
				}
			}
		}
	}
}

/***二进制字符串转换成十六进制数***/
void ConvertWA::BinStrToHex(unsigned char* binStr, unsigned char* hexStr, int lenBin)
{
	int lenHex = lenBin / 8;
	memset(hexStr, '\0', lenHex);
	unsigned char hexChar[2];
	for (int i = 0; i < lenHex; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			hexChar[j] = 0;
			for (int k = 0; k < 4; k++)
			{
				if (binStr[8 * i + 4 * j + k] == '1')
				{
					hexChar[j] |= (0x08 >> k);
				}
			}
		}
		hexStr[i] = ((hexChar[0] & 0x0F) << 4) | (hexChar[1] & 0x0F);
	}
}
