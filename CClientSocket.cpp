// CClientSocket.cpp: 구현 파일
//

#include "pch.h"
#include "exam_20182653_leedongseok.h"
#include "CClientSocket.h"

CClientSocket::CClientSocket()
{
}

CClientSocket::~CClientSocket()
{
}

void CClientSocket::SetWnd(HWND hwnd)
{
	m_hwnd = hwnd;
}
void CClientSocket::OnReceive(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	TCHAR szBuffer[65536];
	ZeroMemory(szBuffer, sizeof(szBuffer));

	//CString temp_serial;
	if (Receive(szBuffer, sizeof(szBuffer)) > 0)
	{
		SendMessage(m_hwnd, WM_CLIENT_RECV, 0, (LPARAM)szBuffer);
	}

	CSocket::OnReceive(nErrorCode);
}

void CClientSocket::OnClose(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	ShutDown();
	Close();

	CSocket::OnClose(nErrorCode);

	AfxMessageBox(_T("ERROR:Disconnected from server!"));
	PostQuitMessage(0);
}
// CClientSocket 멤버 함수
