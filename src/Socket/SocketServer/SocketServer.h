#pragma once
#include <afxsock.h> 

#include <string>
using namespace std;
typedef void(*fReceiveMsgCallBack)(char * info,int length,int nError);
typedef void(*fStartCallBack)(int nError);
typedef struct SERVERPARAMS_TAG
{
	char address[50];
	int  nPort;
	SERVERPARAMS_TAG()
	{
		memset(address, 0, 50);
		nPort = 0;
	}

}SERVERPARAMS;

class CSocketServer
{
public:
	CSocketServer();
	~CSocketServer();

	
		//获取本机IP
	int GetLocalIPAddress(CString &ipStr);

	int SetServerParams(SERVERPARAMS *parms);
	int Start(fStartCallBack cb);
	int Stop();

	int SendMsg(const char * msg, int length);
	int SetReceiveCallBack(fReceiveMsgCallBack);

protected:
	SOCKET m_listen_sock;
	SOCKET m_sock;
	SERVERPARAMS m_serverParams;
	fReceiveMsgCallBack m_recCB;
	fStartCallBack m_startCB;
protected:
	static UINT ServerThreadFunc(LPVOID p);
};

