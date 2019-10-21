// ConvertWA.h: interface for the ConvertWA class.
//
// ConvertWA Debug 1.0
// Copyright (C) 2016-2026 Beijing Mysher Technology CO., LTD All rights reserved
// Use in commercial applications requires written permission
// This software is provided "as is", with no warranty.

#pragma once
#include <string>
using namespace std;
class ConvertWA
{
public:
	//����ͷת����ͷ
	static std::wstring ConvertWA_A2W(string strSrc);

	//����ͷת����ͷ
	static std::string ConvertWA_W2A(std::wstring wstrSrc);

	static std::string UTF8_To_string(const std::string & str);
	static std::string string_To_UTF8(const std::string & str);


	//16����ת�ַ�
	static void HexToAscii(unsigned char * pHex, unsigned char * pAscii, int nLen);
	static void AsciiToHex(unsigned char * pAscii, unsigned char * pHex, int nLen);
	//ʮ��������ת���ɶ������ַ���
	static void HexToBinStr(unsigned char* hexStr, unsigned char* binStr, int lenHex);
	static void BinStrToHex(unsigned char* binStr, unsigned char* hexStr, int lenBin);



	/* ����ch�ַ���sign�����е���� */
	static int getIndexOfSigns(char ch)
	{
		if (ch >= '0' && ch <= '9')
		{
			return ch - '0';
		}
		if (ch >= 'A' && ch <= 'F')
		{
			return ch - 'A' + 10;
		}
		if (ch >= 'a' && ch <= 'f')
		{
			return ch - 'a' + 10;
		}
		return -1;
	}
	/* ʮ��������ת��Ϊʮ������ */
	static long hexToDec(const char *source)
	{
		long sum = 0;
		long t = 1;
		int i, len;

		len = strlen(source);
		for (i = len - 1; i >= 0; i--)
		{
			sum += t * getIndexOfSigns(*(source + i));
			t *= 16;
		}

		return sum;
	}



};