#include "pch.h"
#include "exam_20182653_leedongseok.h"
#include "ClientDlg.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(ClientDlg, CDialogEx)

ClientDlg::ClientDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_CLIENT, pParent)
{
	its_ip = _T("127.0.0.1");
	its_port = 8080;
	timer_count = 0;
}
ClientDlg::ClientDlg(LPCTSTR in_ip, UINT in_port, CWnd* pParent/* = nullptr*/)
	: CDialogEx(IDD_DIALOG_CLIENT, pParent)
{
	its_ip = in_ip;
	its_port = in_port;
	timer_count = 0;
}

ClientDlg::~ClientDlg()
{
}

void ClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ROOM, m_list_room);
	DDX_Control(pDX, IDC_LIST_MSG, m_list_msg);
	DDX_Control(pDX, IDC_LIST_MSG_VS, m_list_msg_vs);
	DDX_Control(pDX, IDC_EDIT_SEND, m_edit_send);
	DDX_Control(pDX, IDC_EDIT_SEND_VS, m_edit_send_vs);
	DDX_Control(pDX, IDC_EDIT_NAME_ROOM, m_edit_name_room);
	DDX_Control(pDX, IDC_BUTTON_OUT_ROOM, m_btn_out_room);
	DDX_Control(pDX, IDC_BUTTON_MAKE_ROOM, m_btn_make_room);
	DDX_Control(pDX, IDC_BUTTON_REFRESH, m_btn_refresh);
	DDX_Control(pDX, IDC_STATIC_TIME, m_static_time);
	DDX_Control(pDX, IDC_STATIC_ONLINE, m_static_online);
}

BEGIN_MESSAGE_MAP(ClientDlg, CDialogEx)
	ON_MESSAGE(WM_CLIENT_RECV, &ClientDlg::OnClientRecv)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_ROOM, &ClientDlg::OnBnClickedButtonMakeRoom)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &ClientDlg::OnBnClickedButtonRefresh)
	ON_LBN_DBLCLK(IDC_LIST_ROOM, &ClientDlg::OnDblclkListRoom)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BUTTON_OUT_ROOM, &ClientDlg::OnBnClickedButtonOutRoom)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL ClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetWindowText(_T("오목 게임 : 메인"));

	m_btn_out_room.ShowWindow(true);
	m_edit_name_room.ShowWindow(true);
	m_list_room.ShowWindow(true);
	m_btn_make_room.ShowWindow(true);
	m_btn_refresh.ShowWindow(true);
	m_list_msg_vs.ResetContent();
	m_edit_send_vs.SetWindowTextW(_T(""));
	m_list_msg_vs.ShowWindow(false);
	m_edit_send_vs.ShowWindow(false);
	m_static_time.ShowWindow(false);

	m_ClientSocket.SetWnd(this->m_hWnd);
	m_ClientSocket.Create();
	if (m_ClientSocket.Connect(its_ip, its_port) == FALSE)
	{
		AfxMessageBox(_T("서버와의 연결에 실패하였습니다."));
		PostQuitMessage(0);
		return FALSE;
	}
	enemy_serial = -1;
	inRoom = false;
	game_started = false;
	game_turned = false;
	timer_count = 15;

	CString str_sender;
	str_sender.Format(_T("OMKWGS"));
	m_ClientSocket.Send(str_sender, str_sender.GetLength() * 2);
	
	return TRUE;
}

void ClientDlg::OnOK()
{
	//CDialogEx::OnOK();
}

BOOL ClientDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && GetFocus() == GetDlgItem(IDC_EDIT_SEND))
	{
		CString str, str_sender;
		UpdateData(TRUE);
		m_edit_send.GetWindowTextW(str);
		if (!str.CompareNoCase(_T(""))) return true;
		//공백의 경우 리턴
		str_sender.Format(_T("OMKCHT%d%s"), m_ClientSocket.serial, str);
		m_ClientSocket.Send((LPVOID)(LPCTSTR)str_sender, str_sender.GetLength() * 2);
		m_edit_send.SetWindowTextW(_T(""));
		UpdateData(FALSE);
		return true;
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && GetFocus() == GetDlgItem(IDC_EDIT_SEND_VS))
	{
		CString str, str_sender;
		UpdateData(TRUE);
		m_edit_send_vs.GetWindowTextW(str);
		if (!str.CompareNoCase(_T(""))) return true;
		if (!str.CompareNoCase(_T("교수님 사랑해요")))
		{
			timer_count = 15;
			game_turned = false;
			KillTimer(1);
			m_static_time.SetWindowTextW(_T(""));
			str_sender.Format(_T("OMKWTG%d9999"), m_ClientSocket.serial);
			m_ClientSocket.Send((LPVOID)(LPCTSTR)str_sender, str_sender.GetLength() * 2);
			m_edit_send_vs.SetWindowTextW(_T(""));
		}
		else
		{
			str_sender.Format(_T("OMKVCT%d%s"), m_ClientSocket.serial, str);
			m_ClientSocket.Send((LPVOID)(LPCTSTR)str_sender, str_sender.GetLength() * 2);
			m_edit_send_vs.SetWindowTextW(_T(""));
		}
		UpdateData(FALSE);
		return true;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void ClientDlg::askRoomList()
{
	UpdateData(TRUE);
	CString str_sender;
	str_sender.Format(_T("OMKARL%d"), m_ClientSocket.serial);
	m_ClientSocket.Send((LPVOID)(LPCTSTR)str_sender, str_sender.GetLength() * 2);
	UpdateData(FALSE);
}

bool ClientDlg::chkWin(int x, int y)
{
	int chk_horizon = 1;
	int chk_vertical = 1;
	int chk_cross = 1;

	for (int i = 1; i < 5; i++)
	{
		if (x - i < 1 || plate[x - i][y] != 1) break;
		if (plate[x - i][y] == 1) chk_horizon++;
	}
	for (int i = 1; i < 5; i++)
	{
		if (x + i > 15 || plate[x + i][y] != 1) break;
		if (plate[x + i][y] == 1) chk_horizon++;
	}
	if (chk_horizon >= 5) return true;

	for (int i = 1; i < 5; i++)
	{
		if (y - i < 1 || plate[x][y - i] != 1) break;
		if (plate[x][y - i] == 1) chk_vertical++;
	}
	for (int i = 1; i < 5; i++)
	{
		if (y + i > 15 || plate[x][y + i] != 1) break;
		if (plate[x][y + i] == 1) chk_vertical++;
	}
	if (chk_vertical >= 5) return true;

	for (int i = 1; i < 5; i++)
	{
		if ((x - i < 1) || (y - i < 1) || (plate[x - i][y - i] != 1)) break;
		if (plate[x - i][y - i] == 1) chk_cross++;
	}
	for (int i = 1; i < 5; i++)
	{
		if ((x + i > 15) || (y + i > 15) || (plate[x + i][y + i] != 1)) break;
		if (plate[x + i][y + i] == 1) chk_cross++;
	}
	if (chk_cross >= 5) return true;

	chk_cross = 1;
	for (int i = 1; i < 5; i++)
	{
		if ((x - i < 1) || (y + i > 15) || (plate[x - i][y + i] != 1)) break;
		if (plate[x - i][y + i] == 1) chk_cross++;
	}
	for (int i = 1; i < 5; i++)
	{
		if ((x + i > 15) || (y - i < 1) || (plate[x + i][y - i] != 1)) break;
		if (plate[x + i][y - i] == 1) chk_cross++;
	}
	if (chk_cross >= 5) return true;

	return false;
}

void ClientDlg::outRoom()
{
	inRoom = false;
	enemy_serial = -1;
	game_started = false;
	game_turned = false;
	for (int a = 0; a < 16; a++)
	{
		for (int b = 0; b < 16; b++)
		{
			plate[a][b] = 0;
		}
	}
	Invalidate();
	m_btn_out_room.ShowWindow(true);
	m_edit_name_room.ShowWindow(true);
	m_list_room.ShowWindow(true);
	m_btn_make_room.ShowWindow(true);
	m_btn_refresh.ShowWindow(true);
	m_list_msg_vs.ResetContent();
	m_edit_send_vs.SetWindowTextW(_T(""));
	m_list_msg_vs.ShowWindow(false);
	m_edit_send_vs.ShowWindow(false);
	m_static_time.ShowWindow(false);
	askRoomList();
}

void ClientDlg::sendServer(CString str)
{
	TCHAR temp[65536];
	if (str.GetLength() * 2 >= sizeof(temp))
	{
		MessageBox(_T("전송량이 너무 큽니다\n65536byte 제한"), _T("경고"));
		return;
	}
	m_ClientSocket.Send(str, str.GetLength() * 2);
}

afx_msg LRESULT ClientDlg::OnClientRecv(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR lpszStr = (LPCTSTR)lParam;
	CString str_in = lpszStr;

	CString given_rooms_list;
	given_rooms_list.Format(_T("OMKGRL%d"), m_ClientSocket.serial);
	CString pre_given_rooms_list;
	pre_given_rooms_list.Format(_T("OMKPTT%d"), m_ClientSocket.serial);
	CString connect_confirm;
	connect_confirm.Format(_T("OMKCCF%d"), m_ClientSocket.serial);

	CString on_game_enemy;
	CString on_game_me;
	CString chat_game_enemy;
	CString chat_game_me;
	CString stone_game_enemy;
	CString stone_game_me;
	CString win_game_enemy;
	CString win_game_me;
	CString outer;
	if (enemy_serial != -1)
	{
		on_game_enemy.Format(_T("OMKDMP%d"), enemy_serial);
		on_game_me.Format(_T("OMKDMP%d"), m_ClientSocket.serial);
		chat_game_enemy.Format(_T("OMKVCT%d"), enemy_serial);
		chat_game_me.Format(_T("OMKVCT%d"), m_ClientSocket.serial);
		stone_game_enemy.Format(_T("OMKSIH%d"), enemy_serial);
		stone_game_me.Format(_T("OMKSIH%d"), m_ClientSocket.serial);
		win_game_enemy.Format(_T("OMKWTG%d"), enemy_serial);
		win_game_me.Format(_T("OMKWTG%d"), m_ClientSocket.serial);
		outer.Format(_T("OMKOUT%d"), enemy_serial);
	}
	// 고유번호 부여
	if (m_ClientSocket.serial == -1 && !str_in.Left(6).CompareNoCase(_T("OMKSSR")))
	{
		m_ClientSocket.serial = _ttoi(str_in.Mid(6, 4));
		askRoomList();
	}


	//////////////////////판단식 시작
	//전체 채팅
	if (!str_in.Left(6).CompareNoCase(_T("OMKCHT")))
	{
		CString str_notice;
		str_notice = str_in.Mid(6, 4) + _T(" : ") + str_in.Mid(10);
		m_list_msg.InsertString(-1, str_notice);
		m_list_msg.SetCurSel(m_list_msg.GetCount() - 1);
	}
	//접속자 수 변경
	else if (!str_in.Left(10).CompareNoCase(_T("OMKNNO0000")))
	{
		int numOnline = _ttoi(str_in.Mid(10, 4)) - 1000;
		CString str_notice;
		str_notice.Format(_T("접속자 수 : %d"), numOnline);
		m_static_online.SetWindowTextW(str_notice);
	}
	//유저 접속
	else if (!str_in.Left(6).CompareNoCase(_T("OMKSSR")))
	{
		CString str_notice;
		str_notice = str_in.Mid(6, 4) + _T("님이 입장하셨습니다.");
		m_list_msg.InsertString(-1, str_notice);
		m_list_msg.SetCurSel(m_list_msg.GetCount() - 1);
	}
	//방 리스트 클리어
	else if (!str_in.Left(10).CompareNoCase(pre_given_rooms_list))
	{
		m_list_room.ResetContent();
	}
	//방 리스트
	else if (!str_in.Left(10).CompareNoCase(given_rooms_list))
	{
		CString room_title;
		if (str_in.Mid(14, 1) == _T("1"))
		{
			room_title.Format(_T("%d [게임중] : %s"), _ttoi(str_in.Mid(10, 4)), str_in.Mid(15));
		}
		else
		{
			room_title.Format(_T("%d [대기중] : %s"), _ttoi(str_in.Mid(10, 4)), str_in.Mid(15));
		}
		m_list_room.InsertString(-1, room_title);
	}
	//게임 연결 완료
	else if (!str_in.Left(10).CompareNoCase(connect_confirm))
	{
		game_started = true;
		enemy_serial = _ttoi(str_in.Mid(10, 4));
		m_btn_out_room.ShowWindow(false);
		m_edit_name_room.ShowWindow(false);
		m_list_room.ShowWindow(false);
		m_btn_make_room.ShowWindow(false);
		m_btn_refresh.ShowWindow(false);
		m_list_msg_vs.ResetContent();
		m_edit_send_vs.SetWindowTextW(_T(""));
		m_list_msg_vs.ShowWindow(true);
		m_edit_send_vs.ShowWindow(true);
		m_static_time.ShowWindow(true);

		m_list_msg_vs.InsertString(-1, _T("SYS : 당신은 백입니다."));
		m_list_msg_vs.SetCurSel(m_list_msg_vs.GetCount() - 1);
	}
	//착수 요구
	else if (!str_in.Left(10).CompareNoCase(on_game_enemy))
	{
		m_list_msg_vs.InsertString(-1, _T("SYS : 상대의 차례입니다."));
		m_list_msg_vs.SetCurSel(m_list_msg_vs.GetCount() - 1);
	}
	else if (!str_in.Left(10).CompareNoCase(on_game_me))
	{
		m_list_msg_vs.InsertString(-1, _T("SYS : 당신의 차례입니다. 정해진 시간 안에 착수하세요."));
		m_list_msg_vs.SetCurSel(m_list_msg_vs.GetCount() - 1);
		game_turned = true;
		timer_count = 15;
		SetTimer(1, 1000, NULL);
	}
	// 1:1 채팅
	else if (!str_in.Left(10).CompareNoCase(chat_game_enemy))
	{
		CString str_notice;
		str_notice.Format(_T("상대(%d) : %s"), _ttoi(str_in.Mid(6, 4)), str_in.Mid(10));
		m_list_msg_vs.InsertString(-1, str_notice);
		m_list_msg_vs.SetCurSel(m_list_msg_vs.GetCount() - 1);
	}
	else if (!str_in.Left(10).CompareNoCase(chat_game_me))
	{
		CString str_notice;
		str_notice.Format(_T("당신(%d) : %s"), m_ClientSocket.serial, str_in.Mid(10));
		m_list_msg_vs.InsertString(-1, str_notice);
		m_list_msg_vs.SetCurSel(m_list_msg_vs.GetCount() - 1);
	}
	//착수
	else if (!str_in.Left(10).CompareNoCase(stone_game_enemy))
	{
		CString str_notice;
		int x, y;
		x = _ttoi(str_in.Mid(10, 2)) - 10;
		y = _ttoi(str_in.Mid(12, 2)) - 10;

		if (x == 89 && y == 89)
		{
			str_notice.Format(_T("SYS : 상대의 착수 시간 초과"));
			m_list_msg_vs.InsertString(-1, str_notice);
			m_list_msg_vs.SetCurSel(m_list_msg_vs.GetCount() - 1);
		}
		else
		{
			plate[x][y] = 2;
			str_notice.Format(_T("SYS : 상대의 착수(%d, %d)"), x, y);
			m_list_msg_vs.InsertString(-1, str_notice);
			m_list_msg_vs.SetCurSel(m_list_msg_vs.GetCount() - 1);
			CClientDC dc(this);
			CBrush black_brush(RGB(0, 0, 0));
			dc.SelectObject(&black_brush);
			x *= 30;
			y *= 30;
			dc.Ellipse(x - 15, y - 15, x + 15, y + 15);
		}
	}
	else if (!str_in.Left(10).CompareNoCase(stone_game_me))
	{
		int x, y;
		x = _ttoi(str_in.Mid(10, 2)) - 10;
		y = _ttoi(str_in.Mid(12, 2)) - 10;

		if (x == 89 && y == 89);
		else
		{
			plate[x][y] = 1;
		}
	}
	//승리 고지
	else if (!str_in.Left(10).CompareNoCase(win_game_enemy))
	{
		KillTimer(1);
		CString str_notice;
		int x, y;
		x = _ttoi(str_in.Mid(10, 2)) - 10;
		y = _ttoi(str_in.Mid(12, 2)) - 10;
		if (x == 89 && y == 89);
		else
		{
			plate[x][y] = 2;
			str_notice.Format(_T("SYS : 상대의 착수(%d, %d)"), x, y);
			m_list_msg_vs.InsertString(-1, str_notice);
			m_list_msg_vs.SetCurSel(m_list_msg_vs.GetCount() - 1);

			CClientDC dc(this);
			CBrush black_brush(RGB(0, 0, 0));
			dc.SelectObject(&black_brush);
			x *= 30;
			y *= 30;
			dc.Ellipse(x - 15, y - 15, x + 15, y + 15);
		}

		MessageBox(_T("패배했습니다."), _T("저런"));

		outRoom();
	}
	else if (!str_in.Left(10).CompareNoCase(win_game_me))
	{
		KillTimer(1);
		int x, y;
		x = _ttoi(str_in.Mid(10, 2)) - 10;
		y = _ttoi(str_in.Mid(12, 2)) - 10;
		if (x == 89 && y == 89);
		else
		{
			plate[x][y] = 1;
		}
		
		MessageBox(_T("승리했습니다."), _T("우와"));

		outRoom();
	}
	else if (!str_in.Left(10).CompareNoCase(outer))
	{
		KillTimer(1);
		MessageBox(_T("적이 도주했습니다."), _T("맙소사"));
		outRoom();
	}
	else if (str_in.Left(3).CompareNoCase(_T("OMK")))
	{
	}
	return 0;
}

void ClientDlg::OnBnClickedButtonMakeRoom()
{
	CString str,str_sender;
	UpdateData(TRUE);
	m_edit_name_room.GetWindowTextW(str);
	if (!str.CompareNoCase(_T("")) || !str.CompareNoCase(_T("###BLANK###"))) return;
	str_sender.Format(_T("OMKAMR%d%s"), m_ClientSocket.serial, str);
	m_ClientSocket.Send((LPVOID)(LPCTSTR)str_sender, str_sender.GetLength() * 2);
	m_edit_name_room.SetWindowTextW(_T(""));
	inRoom = true;
	UpdateData(FALSE);
}

void ClientDlg::OnBnClickedButtonRefresh()
{
	askRoomList();
}

void ClientDlg::OnDblclkListRoom()
{
	CString str_serial;
	m_list_room.GetText(m_list_room.GetCurSel(), str_serial);
	str_serial = str_serial.Left(4);
	if (_ttoi(str_serial) == m_ClientSocket.serial)
	{
		MessageBox(_T("자신의 방에는 참여할 수 없습니다."), _T("경고"));
		return;
	}
	else
	{
		CString str_sender;
		str_sender.Format(_T("OMKATC%d%d"), m_ClientSocket.serial, _ttoi(str_serial));
		m_ClientSocket.Send((LPVOID)(LPCTSTR)str_sender, str_sender.GetLength() * 2);
	}
}


void ClientDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (game_started == true && game_turned == true && point.x > 15 && point.x < 465 && point.y>15 && point.y < 465)
	{
		CString str_notice, str_sender;
		CClientDC dc(this);
		int x = (point.x + 15) / 30, y = (point.y + 15) / 30;
		int point_x = x * 30, point_y = y * 30;
		if (plate[x][y] != 0) return;
		str_notice.Format(_T("SYS : 당신의 착수(%d, %d)"), x, y);
		m_list_msg_vs.InsertString(-1, str_notice);
		m_list_msg_vs.SetCurSel(m_list_msg_vs.GetCount() - 1);
		dc.Ellipse(point_x - 15, point_y - 15, point_x + 15, point_y + 15);
		game_turned = false;
		if (chkWin(x, y) == true)
		{
			str_sender.Format(_T("OMKWTG%d%d%d"), m_ClientSocket.serial, x + 10, y + 10);
			m_ClientSocket.Send((LPVOID)(LPCTSTR)str_sender, str_sender.GetLength() * 2);
		}
		else
		{
			str_sender.Format(_T("OMKSIH%d%d%d"), m_ClientSocket.serial, x + 10, y + 10);
			m_ClientSocket.Send((LPVOID)(LPCTSTR)str_sender, str_sender.GetLength() * 2);
		}
		m_static_time.SetWindowTextW(_T(""));
		KillTimer(1);
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}

void ClientDlg::OnBnClickedButtonOutRoom()
{
	inRoom = false;
	CString str_sender;
	str_sender.Format(_T("OMKWTO%d"), m_ClientSocket.serial);
	m_ClientSocket.Send((LPVOID)(LPCTSTR)str_sender, str_sender.GetLength() * 2);
	Sleep(100);
	askRoomList();
}

void ClientDlg::OnPaint()
{
	CPaintDC dc(this);
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 15; j++)
		{
			dc.Rectangle(j * 30 + 15, i * 30 + 15, j * 30 + 31 + 15, i * 30 + 31 + 15);
		}
	}
}

void ClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// case 1 : 착수
	switch (nIDEvent)
	{
	case 1:
		if (timer_count == 0)
		{
			CString str_notice, str_sender;
			timer_count = 15;
			UpdateData();
			str_sender.Format(_T("OMKSIH%d9999"), m_ClientSocket.serial);
			m_ClientSocket.Send((LPVOID)(LPCTSTR)str_sender, str_sender.GetLength() * 2);
			game_turned = false;
			m_static_time.SetWindowTextW(_T(""));
			KillTimer(1);
			str_notice.Format(_T("SYS : 시간 초과로 착수를 포기합니다."));
			m_list_msg_vs.InsertString(-1, str_notice);
			m_list_msg_vs.SetCurSel(m_list_msg_vs.GetCount() - 1);
			UpdateData(false);
		}
		else
		{
			UpdateData();
			timer_count--;
			CString str_notice;
			str_notice.Format(_T("남은 초 : %d"), timer_count);
			m_static_time.SetWindowTextW(str_notice);
			UpdateData(false);
		}
		break;
	default:
		break;
	}

	CDialogEx::OnTimer(nIDEvent);
}
