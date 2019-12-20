#pragma once

#define WM_CLIENT_RECV WM_USER+1
// CClientSocket 명령 대상

class CClientSocket : public CSocket
{
public:
	CClientSocket();
	virtual ~CClientSocket();

	int serial = -1;
	HWND m_hwnd;
	void SetWnd(HWND hwnd);
	virtual void OnReceive(int nErrorCode);
	virtual void OnClose(int nErrorCode);
};


