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
	SOCKET_CODE_TYPE_TEXT, //文本（字符流）
	SOCKET_CODE_TYPE_HEX //16进制 (字节流)
};


class CMySocket :
	public CAsyncSocket
{
public:
	CMySocket();
	~CMySocket();
	
	//连接 可通过回调确定是否连接成功
	void ConnectServer(LPCTSTR lpszHostAddress, UINT nHostPort, fResultStatueCallBack cb = NULL);
	
	//断开
	void Close();

	//是否在连接中
	bool SocketIsConntectting();

	//设置消息回调
	void SetReceiveCallBack(fReceiveMsgCallBack cb, void *lparam = NULL);

	//设置关闭的回调
	void SetCloseCallBack(fResultStatueCallBack cb);

	//发送消息
	bool SendMessageInfo(const char *info,unsigned int length);

	//设置编码方式
	void SetStringCode(SOCKET_CODE_TYPE codeType);

private:
	unsigned int m_bufferSize;
	char m_szBuffer[8192];  //消息缓冲区
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

