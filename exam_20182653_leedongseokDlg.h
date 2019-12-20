
//exam_20182653_leedongseokDlg.h: 헤더 파일

/*
IDD_EXAM_20182653_LEEDONGSEOK에서 IP,PORT 체크 후 IDD_DIALOG_CLIENT로 넘어감
*/

//

#pragma once

#include "CClientSocket.h"
#include "ClientDlg.h"


// Cexam20182653leedongseokDlg 대화 상자
class Cexam20182653leedongseokDlg : public CDialogEx
{
// 생성입니다.
public:
	Cexam20182653leedongseokDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXAM_20182653_LEEDONGSEOK_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;
	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CClientSocket m_ClientSocket;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CEdit m_edit_ip;
	CEdit m_edit_port;
	void chkServer();
	virtual void OnOK();
	afx_msg void OnBnClickedButtonConnect();
};
