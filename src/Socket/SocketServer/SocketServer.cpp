#include "stdafx.h"
#include "SocketServer.h"
#include <iostream>

#include <sstream>
CSocketServer::CSocketServer()
{
	m_recCB = NULL;
	AfxSocketInit(0);
}


CSocketServer::~CSocketServer()
{
}

std::string StringToHex(const std::string& data)
{
	const std::string hex = "0123456789ABCDEF";
	std::stringstream ss;

	for (std::string::size_type i = 0; i < data.size(); ++i)
		ss << hex[(unsigned char)data[i] >> 4] << hex[(unsigned char)data[i] & 0xf];
	std::cout << ss.str() << std::endl;
	return ss.str();
}

std::string HexToStr(const std::string& str)
{
	std::string result;
	for (size_t i = 0; i < str.length(); i += 2)
	{
		std::string byte = str.substr(i, 2);
		char chr = (char)(int)strtol(byte.c_str(), NULL, 16);
		result.push_back(chr);
	}
	return result;
}

UINT CSocketServer::ServerThreadFunc(LPVOID p)
{
	CSocketServer *ss = (CSocketServer*)p;
	if (ss == NULL)
	{
		return 0;
	}

	WSADATA wsaData;
	WORD wVersion;
	wVersion = MAKEWORD(2, 2);
	WSAStartup(wVersion, &wsaData);

	SOCKADDR_IN local_addr;
	SOCKADDR_IN client_addr;
	int iaddrSize = sizeof(SOCKADDR_IN);
	int res;
	char msg[2048];
	

	local_addr.sin_addr.s_addr = inet_addr(ss->m_serverParams.address);
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(ss->m_serverParams.nPort);
	if ((ss->m_listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)//创建套接字
	{
		//dlg->update(_T(“创建监听失败”));
		if (ss->m_startCB)
		{
			ss->m_startCB(-1);
		}
	}
	if (bind(ss->m_listen_sock, (struct sockaddr*) &local_addr, sizeof(SOCKADDR_IN)))//绑定套接字
	{
		int err = GetLastError();
		//dlg->update(_T(“绑定错误”));
		if (ss->m_startCB)
		{
			ss->m_startCB(-2);
		}
	}
	if (ss->m_startCB)
	{
		ss->m_startCB(0);
	}
	listen(ss->m_listen_sock, 1);
	if ((ss->m_sock = accept(ss->m_listen_sock, (struct sockaddr *)&client_addr, &iaddrSize)) == INVALID_SOCKET)//接收套接字
	{
		//dlg->update(_T(“accept 失败”));
		if (ss->m_startCB)
		{
			ss->m_startCB(-3);
		}
	}
	else
	{
// 		CString port;
// 		port.Format(_T(“%d”), int(ntohs(client_addr.sin_port)));
// 		dlg->update(_T(“已连接客户端：”) + CString(inet_ntoa(client_addr.sin_addr)) + ” 端口：” + port);
		if (ss->m_startCB)
		{
			ss->m_startCB(0);
		}
	}
	////////////接收数据
	while (1)
	{
		memset(msg, 0, 2048);
		if ((res = recv(ss->m_sock, msg, 2048, 0)) == -1)
		{
			if (ss->m_recCB)
			{
				ss->m_recCB(msg,strlen(msg),-1);
			}
		}
		else
		{
			if (ss->m_recCB)
			{
				//string info = msg;
				//string str = HexToStr(info);
				//ss->m_recCB((char*)str.c_str(), strlen(msg), 0);
				ss->m_recCB(msg, strlen(msg), 0);
			}
			
		}
	}
	return 0;
}

int CSocketServer::GetLocalIPAddress(CString &strIPAddr)
{
	strIPAddr = L"";
	char HostName[100];
	gethostname(HostName, sizeof(HostName));// 获得本机主机名.

	struct addrinfo *res;
	struct addrinfo hints;
	int ret;
	struct sockaddr_in *addr;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	ret = getaddrinfo(HostName, NULL, &hints, &res);
	if (ret !=0)
	{
		return ret;
	}
	char ipbuf[16];
	addr = (struct sockaddr_in *)res->ai_addr;
	strIPAddr += inet_ntop(AF_INET, &addr->sin_addr, ipbuf, 16);//将IP地址的二进制转换为十进制
	freeaddrinfo(res);

	return 0;
	
}

int CSocketServer::SetServerParams(SERVERPARAMS *parms)
{
	memcpy_s(&m_serverParams, sizeof(SERVERPARAMS), parms, sizeof(SERVERPARAMS));
	return 0;

}

int CSocketServer::Start(fStartCallBack cb)
{
	m_startCB = cb;
	AfxBeginThread(ServerThreadFunc, this);//创建线程 
	return 0;
}

int CSocketServer::Stop()
{
	return 0;
}

int CSocketServer::SendMsg(const char * msg,int length)
{
	if (send(m_sock, msg, length, 0) == SOCKET_ERROR)
	{
		return -1;
	}

	return 0;
}

int CSocketServer::SetReceiveCallBack(fReceiveMsgCallBack cb)
{
	m_recCB = cb;
	return 0;
}
