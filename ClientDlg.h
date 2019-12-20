#pragma once
#include "CClientSocket.h"

/*
#통신 형식 정의 키워드
PTT(Prepare to take) + 수신자 시리얼
 - 방 리스트 클리어 명령
SSR(sign serial) + 수신자 시리얼
 - 고유번호 설정
CHT(chat) + 발신자 시리얼 + 내용
 - 전체 채팅 발송
GRL(get room list) + 수신자 시리얼 + 방장 시리얼 + 대기 여부 + 방 제목
 - 방 리스트 받기
AMR(ask making room) + 발신자 시리얼 + 방 제목
 - 방 만들기
ARL(ask room list) + 발신자 시리얼
 - 방 리스트 요구
ATC(ask to connect) + 발신자 시리얼 + 연결할 방의 방장 시리얼
 - 방 연결 요구
CCF(connect confirm) + 수신자 시리얼
 - 방 연결 완료
DMP(Demand making plate) + 수신자 시리얼
 - 착수 요구
VCT(vs chat) + 발신자 시리얼
 - 1:1 채팅 발송
SIH(stone is here) + 발신자 시리얼 + x좌표 + y좌표
 - 착수 결과 전송
WTG(win this game) + 발신자 시리얼 + x좌표 + y좌표
 - 승리 고지
WTO(want to out) + 수신자 시리얼
 - 연결 끊김 고지
WTG(want get serial) + 수신자 시리얼
 - 시리얼 부여 요청
NNO(notice number of online) + 0000 + (1000 + 현재 접속자)
 - 온라인 수
*/

class ClientDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ClientDlg)
public:
	ClientDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	ClientDlg(LPCTSTR in_ip, UINT in_port, CWnd* pParent = nullptr);
	virtual ~ClientDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_CLIENT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CClientSocket m_ClientSocket;
	CListBox m_list_room;
	CListBox m_list_msg;
	CListBox m_list_msg_vs;
	CEdit m_edit_send_vs;
	CEdit m_edit_send;
	CEdit m_edit_name_room;
	CButton m_btn_out_room;
	CButton m_btn_make_room;
	CButton m_btn_refresh;
	CStatic m_static_time;
	CStatic m_static_online;

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void askRoomList();
	bool chkWin(int x, int y);
	void outRoom();
	void sendServer(CString str);

	LPCTSTR its_ip;
	UINT its_port;
	int enemy_serial;
	bool game_started;
	bool game_turned;
	int plate[16][16] = { 0, };
	bool inRoom;
	int timer_count;
public:
	afx_msg void OnBnClickedButtonMakeRoom();
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnDblclkListRoom();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonOutRoom();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	afx_msg LRESULT OnClientRecv(WPARAM wParam, LPARAM lParam);
};
