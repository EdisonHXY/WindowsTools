#include "stdafx.h"
#include "CurlManager.h"

CCurlManager::CCurlManager()
{
	m_pCurl = NULL;
	m_nPort = 80;
	m_hFile = INVALID_HANDLE_VALUE;
	m_pCallback = NULL;
	m_pCallbackParam = NULL;
	m_curlCode = CURLE_OK;
	m_lfFlag = Lf_None;
	m_curlList = NULL;

	m_pCurl = curl_easy_init();
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, this);

	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, 0L);//忽略证书检查
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, 0L);
}


CCurlManager::~CCurlManager()
{
	ClearHeaderList();
	if (m_pCurl)
	{
		curl_easy_cleanup(m_pCurl);
	}

	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
	}
}

bool CCurlManager::SetPort(LONG port)
{
	if (port == m_nPort)
		return true;
	m_nPort = port;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_PORT, m_nPort);
	return CURLE_OK == m_curlCode;
}

bool CCurlManager::SetTimeout(int nSecond)
{
	if (nSecond < 0)
		return false;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, nSecond);
	return CURLE_OK == m_curlCode;
}

bool CCurlManager::SetConnectTimeout(int nSecond)
{
	if (nSecond < 0)
		return false;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, nSecond);
	return CURLE_OK == m_curlCode;
}

bool CCurlManager::SetUserAgent(LPCSTR lpAgent)
{
	if (NULL == lpAgent)
		return false;
	int nLen = strlen(lpAgent);
	if (nLen == 0)
		return false;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_USERAGENT, lpAgent);
	return CURLE_OK == m_curlCode;
}

bool CCurlManager::SetResumeFrom(LONG lPos)
{
	if (lPos < 0)
		return false;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_RESUME_FROM, lPos);
	return CURLE_OK == m_curlCode;
}

bool CCurlManager::SetResumeFromLarge(LONGLONG llPos)
{
	if (llPos < 0)
		return false;
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_RESUME_FROM_LARGE, llPos);
	return CURLE_OK == m_curlCode;
}

bool CCurlManager::AddHeader(LPCSTR lpKey, LPCSTR lpValue)
{
	if (lpKey == NULL || lpValue == NULL)
	{
		return false;
	}
	
	int nLen1 = strlen(lpKey), nLen2 = strlen(lpValue);
	if (nLen1 <=0 || nLen2 <=0)
	{
		return false;
	}
	string strHeader(lpKey);
	strHeader.append(": ");
	strHeader.append(lpValue);
	m_curlList = curl_slist_append(m_curlList, strHeader.c_str());
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, m_curlList);
	return CURLE_OK == m_curlCode;
	
}

void CCurlManager::ClearHeaderList()
{
	if (m_curlList)
	{
		curl_slist_free_all(m_curlList);
		m_curlList = NULL;
	}
}

bool CCurlManager::SetCookie(LPCSTR lpCookie)
{
	if (lpCookie == NULL)
	{
		return false;
	}
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_COOKIE, lpCookie);
	return CURLE_OK == m_curlCode;
}

bool CCurlManager::SetCookieFile(LPCSTR lpFilePath)
{
	if (lpFilePath == NULL)
	{
		return false;
	}
	m_curlCode = curl_easy_setopt(m_pCurl, CURLOPT_COOKIEFILE, lpFilePath);
	return CURLE_OK == m_curlCode;
}

const char* CCurlManager::GetError() const
{
	return curl_easy_strerror(m_curlCode);
}

void CCurlManager::SetCallback(fProgressCallBack pCallback, void* lpParam)
{
	m_pCallbackParam = lpParam;
	m_pCallback = pCallback;
}

bool CCurlManager::DownloadToFile(LPCSTR lpUrl, LPCSTR lpFile)
{
	if (lpUrl == NULL || lpFile == NULL)
	{
		return false;
	}
	CURLcode code = curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);
	DeleteFileA(lpFile);
	m_hFile = CreateFileA(lpFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == m_hFile)
	{
		return FALSE;
	}
	curl_easy_setopt(m_pCurl, CURLOPT_NOPROGRESS, 0);
	curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSFUNCTION, ProgressCallback);
	curl_easy_setopt(m_pCurl, CURLOPT_PROGRESSDATA, this);
	m_lfFlag = Lf_Download;
	//开始执行请求
	m_curlCode = curl_easy_perform(m_pCurl);
	return CURLE_OK == m_curlCode;
}

bool CCurlManager::Post(LPCSTR lpUrl, LPCSTR lpData)
{
	if (lpUrl == NULL || lpData == NULL)
	{
		return false;
	}

	curl_easy_setopt(m_pCurl, CURLOPT_POST, 1);
	curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, lpData);
	//curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, lpData);
	curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);
	m_lfFlag = Lf_Post;
	m_strRespons.clear();
	m_curlCode = curl_easy_perform(m_pCurl);
	return CURLE_OK == m_curlCode;

}

bool CCurlManager::Post(LPCSTR lpUrl, unsigned char* lpData, unsigned int nSize)
{
	if (lpData == NULL || lpUrl == NULL || nSize <=0)
	{
		return false;
	}

	curl_easy_setopt(m_pCurl, CURLOPT_POST, 1);
	curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, lpData);
	curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, nSize);
	curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);
	m_lfFlag = Lf_Post;
	m_strRespons.clear();
	m_curlCode = curl_easy_perform(m_pCurl);
	return CURLE_OK == m_curlCode;
	
}

bool CCurlManager::Get(LPCSTR lpUrl)
{
	if (lpUrl == NULL)
	{
		return false;
	}
	curl_easy_setopt(m_pCurl, CURLOPT_HTTPGET, 1);
	curl_easy_setopt(m_pCurl, CURLOPT_URL, lpUrl);
	curl_easy_setopt(m_pCurl, CURLOPT_FOLLOWLOCATION, 1);//支持重定向
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(m_pCurl, CURLOPT_SSL_VERIFYHOST, 0L);
	m_lfFlag = Lf_Get;
	m_strRespons.clear();
	m_curlCode = curl_easy_perform(m_pCurl);
	return CURLE_OK == m_curlCode;

}

const string& CCurlManager::GetRespons() const
{
	return m_strRespons;
}

const char* CCurlManager::GetResponsPtr() const
{
	return m_strRespons.c_str();
}

size_t CCurlManager::WriteCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
	//把下载到的数据以追加的方式写入文件(一定要有a，否则前面写入的内容就会被覆盖了)
	CCurlManager* pThis = (CCurlManager*)pParam;
	if (pThis == NULL)
	{
		return 0;
	}
	DWORD dwWritten = 0;
	switch (pThis->m_lfFlag)
	{
	case Lf_Download://下载
	{
						 if (pThis->m_hFile == INVALID_HANDLE_VALUE)
							 return 0;
						 if (!WriteFile(pThis->m_hFile, pBuffer, nSize*nMemByte, &dwWritten, NULL))
							 return 0;
	}
		break;
	case Lf_Post://Post数据
	case Lf_Get://Get数据
	{
					pThis->m_strRespons.append((const char*)pBuffer, nSize*nMemByte);
					dwWritten = nSize*nMemByte;
	}
		break;
	case Lf_None://未定义
		break;
	}
	return dwWritten;
}

size_t CCurlManager::HeaderCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
	CCurlManager* pThis = (CCurlManager*)pParam;
	return 0;
}

int CCurlManager::ProgressCallback(void *pParam, double dltotal, double dlnow, double ultotal, double ulnow)
{
	CCurlManager* pThis = (CCurlManager*)pParam;
	if (pThis == NULL)
	{
		return -1;
	}
	if (pThis->m_pCallback)
	{
		pThis->m_pCallback(pThis->m_pCallbackParam, dltotal, dlnow);
	}
	return 0;

}
