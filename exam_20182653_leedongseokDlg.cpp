#include "pch.h"
#include "framework.h"
#include "exam_20182653_leedongseok.h"
#include "exam_20182653_leedongseokDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

Cexam20182653leedongseokDlg::Cexam20182653leedongseokDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EXAM_20182653_LEEDONGSEOK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cexam20182653leedongseokDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_IP, m_edit_ip);
	DDX_Control(pDX, IDC_EDIT_PORT, m_edit_port);
}

BEGIN_MESSAGE_MAP(Cexam20182653leedongseokDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &Cexam20182653leedongseokDlg::OnBnClickedButtonConnect)
END_MESSAGE_MAP()

BOOL Cexam20182653leedongseokDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	SetWindowText(_T("오목 게임 : IP 접속"));
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	m_ClientSocket.SetWnd(this->m_hWnd);
	m_ClientSocket.Create();

	return TRUE;
}

void Cexam20182653leedongseokDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void Cexam20182653leedongseokDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR Cexam20182653leedongseokDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL Cexam20182653leedongseokDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && (GetFocus() == GetDlgItem(IDC_EDIT_IP) || GetFocus() == GetDlgItem(IDC_EDIT_PORT)))
	{
		chkServer();
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void Cexam20182653leedongseokDlg::chkServer()
{
	CString ip;
	m_edit_ip.GetWindowTextW(ip);
	CString str_port;
	m_edit_port.GetWindowTextW(str_port);
	LPCTSTR con_ip = ip;
	UINT con_port = _ttoi(str_port);
	
	if (m_ClientSocket.Connect(con_ip, con_port) == FALSE)
	{
		AfxMessageBox(_T("서버와의 연결에 실패하였습니다."));
		return;
	}
	else
	{
		AfxMessageBox(_T("서버와의 연결에 성공하였습니다."));
		m_ClientSocket.Close();
		ClientDlg dlg(con_ip,con_port, this);
		dlg.DoModal();
		PostQuitMessage(0);
	}
}

void Cexam20182653leedongseokDlg::OnOK()
{
}

void Cexam20182653leedongseokDlg::OnBnClickedButtonConnect()
{
	chkServer();
}
