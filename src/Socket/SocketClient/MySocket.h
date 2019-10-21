#pragma once
#include <afxsock.h>

typedef void(*fReceiveMsgCallBack)(char *recInfo, unsigned int length, int nErrorCode,void *lparam);
typedef void(*fResultStatueCallBack)(int nErrorCode);

typedef struct ReceiveBufferStruct_Tag
{
	unsigned char header[2][20];
	unsigned char reserve[2][20];
	unsigned char number[2][20];
	unsigned char jsLength[3][20];
	unsigned char buffer[4096];

}ReceiveBufferStruct;

typedef enum SOCKET_CODE_TYPE
{
	SOCKET_CODE_TYPE_TEXT, //�ı����ַ�����
	SOCKET_CODE_TYPE_HEX //16���� (�ֽ���)
};


class CMySocket :
	public CAsyncSocket
{
public:
	CMySocket();
	~CMySocket();
	
	//���� ��ͨ���ص�ȷ���Ƿ����ӳɹ�
	void ConnectServer(LPCTSTR lpszHostAddress, UINT nHostPort, fResultStatueCallBack cb = NULL);
	
	//�Ͽ�
	void Close();

	//�Ƿ���������
	bool SocketIsConntectting();

	//������Ϣ�ص�
	void SetReceiveCallBack(fReceiveMsgCallBack cb, void *lparam = NULL);

	//���ùرյĻص�
	void SetCloseCallBack(fResultStatueCallBack cb);

	//������Ϣ
	bool SendMessageInfo(const char *info,unsigned int length);

	//���ñ��뷽ʽ
	void SetStringCode(SOCKET_CODE_TYPE codeType);

private:
	unsigned int m_bufferSize;
	char m_szBuffer[8192];  //��Ϣ������
	bool m_isConnnect;
	fReceiveMsgCallBack m_receiveCB;
	void *m_receiveCB_param;
	SOCKET_CODE_TYPE m_codeType;
	fResultStatueCallBack m_connectCB;
	fResultStatueCallBack m_closeCB;

protected:
	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnOutOfBandData(int nErrorCode);
	virtual void OnAccept(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);

private:
	void HexToAscii(unsigned char * pHex, unsigned char * pAscii, int nLen);
	void AsciiToHex(unsigned char * pAscii, unsigned char * pHex, int nLen);
};

