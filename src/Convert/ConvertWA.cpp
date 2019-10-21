#include "StdAfx.h"
#include "ConvertWA.h"
#include <windows.h>

//����ͷת����ͷ
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

//����ͷת����ͷ
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

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//һ��Ҫ��1����Ȼ�����β��  
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

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//һ��Ҫ��1����Ȼ�����β��  
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
*   ���ܣ���һ��ʮ�������ֽڴ�ת���� ASCII ���ʾ��ʮ�����Ƶ��ַ���
*   ���������pHex    -- ʮ���������ֽڴ��׵�ַ
*            pAscii -- ת����� ASCII ���ʾ��ʮ�������ַ������׵�ַ
*            nLen    -- Ҫת����ʮ���������ĳ��ȣ��ֽ�����
*   ���������None
*   ע��  ת����Ľ��ȫ���Ǵ�д ASCII ��ʾ��ʮ��������
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
*   ���ܣ���һ�� ASCII ���ʾ��ʮ�������ַ���ת����ʮ�����Ƶ��ֽڴ�
*   ���������pAscii -- ת����� ASCII ���ʾ��ʮ�������ַ������׵�ַ
*            pHex   -- ʮ���������ֽڴ��׵�ַ
*            nLen   -- Ҫת���� ASCII ���ʾ��ʮ�������ַ����ĳ��ȣ��ֽ�����
*   ���������None
*   ע��  Ҫ������� ASCII ���ʾ��ʮ�����������ַ���������Ϊż����������1 - 9 �� A(a) - F(f) ����û�б���ַ�
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


/***ʮ��������ת���ɶ������ַ���***/
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

/***�������ַ���ת����ʮ��������***/
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
