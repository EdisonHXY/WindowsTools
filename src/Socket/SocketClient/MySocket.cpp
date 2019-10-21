#include "stdafx.h"
#include "MySocket.h"

CMySocket::CMySocket()
{
	m_receiveCB = NULL;
	m_receiveCB_param = NULL;
	m_isConnnect = false;
	m_bufferSize = 8192;
	memset(m_szBuffer, 0, m_bufferSize);
	
	bool bRet = AfxSocketInit();
	CAsyncSocket::Create(0, SOCK_STREAM, FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE, NULL);

	m_codeType = SOCKET_CODE_TYPE_TEXT;

}


CMySocket::~CMySocket()
{
	if (m_isConnnect)
	{
		Close();
	}
	
}

void CMySocket::ConnectServer(LPCTSTR lpszHostAddress, UINT nHostPort, fResultStatueCallBack cb)
{
	if (m_hSocket == INVALID_SOCKET)
	{
		CAsyncSocket::Create(0, SOCK_STREAM, FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE, NULL);
	}
	m_connectCB = cb;
	CAsyncSocket::Connect(lpszHostAddress, nHostPort);
}

void CMySocket::Close()
{
	CAsyncSocket::Close();
}

bool CMySocket::SocketIsConntectting()
{
	return m_isConnnect;
}

void CMySocket::SetReceiveCallBack(fReceiveMsgCallBack cb, void *lparam)
{
	m_receiveCB = cb;
	m_receiveCB_param = lparam;
}

void CMySocket::SetCloseCallBack(fResultStatueCallBack cb)
{
	m_closeCB = cb;
}


bool CMySocket::SendMessageInfo(const char *info, unsigned int length)
{

	if (m_codeType == SOCKET_CODE_TYPE_TEXT)
	{

		return CAsyncSocket::Send(info, length);
	}
	else
	{

		char *asStr = new char[length];
		memset(asStr, 0, length);
		AsciiToHex((unsigned char*)info, (unsigned char*)asStr, length );

		bool bRet = CAsyncSocket::Send(asStr, length);

		delete asStr;
		asStr = NULL;
		return bRet;
	}

	
}


void CMySocket::SetStringCode(SOCKET_CODE_TYPE codeType)
{
	m_codeType = codeType;
}

void CMySocket::OnReceive(int nErrorCode)
{

	int length = 0;
	if (nErrorCode == 0)
	{
		length = Receive(m_szBuffer, m_bufferSize, 0);
	}
	else
	{
		memset(m_szBuffer, 0, m_bufferSize);
	}

	if (m_codeType == SOCKET_CODE_TYPE_TEXT)
	{
		if (m_receiveCB)
		{
			m_receiveCB(m_szBuffer, length, nErrorCode, m_receiveCB_param);
		}
	}
	else
	{
		char *asciiStr = new  char[2 * length + 1];
		HexToAscii((unsigned char*)m_szBuffer, (unsigned char*)asciiStr, length);

		if (m_receiveCB)
		{
			m_receiveCB((char*)asciiStr, length, nErrorCode, m_receiveCB_param);
		}

		if (asciiStr)
		{
			delete[]asciiStr;
		}
	}



	memset(m_szBuffer, 0, m_bufferSize);

	CAsyncSocket::OnReceive(nErrorCode);
}

void CMySocket::OnSend(int nErrorCode)
{
	CAsyncSocket::OnSend(nErrorCode);
}

void CMySocket::OnOutOfBandData(int nErrorCode)
{
	CAsyncSocket::OnOutOfBandData(nErrorCode);
}

void CMySocket::OnAccept(int nErrorCode)
{
	CAsyncSocket::OnAccept(nErrorCode);

}

void CMySocket::OnConnect(int nErrorCode)
{
	if (nErrorCode == 0)
	{
		m_isConnnect = true;
	}
	if (m_connectCB)
	{
		m_connectCB(nErrorCode);
	}
	CAsyncSocket::OnConnect(nErrorCode);

}

void CMySocket::OnClose(int nErrorCode)
{
	if (nErrorCode == 0)
	{
		m_isConnnect = false;
	}
	if (m_closeCB)
	{
		m_closeCB(nErrorCode);
	}
	CAsyncSocket::OnClose(nErrorCode);

	Close();
}




/************************************************************************
*   ���ܣ���һ��ʮ�������ֽڴ�ת���� ASCII ���ʾ��ʮ�����Ƶ��ַ���
*   ���������pHex    -- ʮ���������ֽڴ��׵�ַ
*            pAscii -- ת����� ASCII ���ʾ��ʮ�������ַ������׵�ַ
*            nLen    -- Ҫת����ʮ���������ĳ��ȣ��ֽ�����
*   ���������None
*   ע��  ת����Ľ��ȫ���Ǵ�д ASCII ��ʾ��ʮ��������
************************************************************************/
void CMySocket::HexToAscii(unsigned char * pHex, unsigned char * pAscii, int nLen)
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
void CMySocket::AsciiToHex(unsigned char * pAscii, unsigned char * pHex, int nLen)
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

