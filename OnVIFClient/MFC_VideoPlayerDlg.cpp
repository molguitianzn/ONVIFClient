
// MFC_VideoPlayerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFC_VideoPlayer.h"
#include "MFC_VideoPlayerDlg.h"
extern "C"
{
#include "utils.h"
#include "onvif_comm.h"
}
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMFCVideoPlayerDlg 对话框



CMFCVideoPlayerDlg::CMFCVideoPlayerDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_MFC_VIDEOPLAYER_DIALOG, pParent), m_bIsFullScreen(false),
	m_bIsPause(false), m_bHasBegin(false),m_bAlreadyEnded(false), CameraNum(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	LOGBUFFER = new char[BUFFERSIZE];
	IPAddressLists = new std::string[IPADDSIZE];
	URILists = new std::string[URISIZE];
	UserLists = new std::string[IPCMAX];
	PasswdLists = new std::string[IPCMAX];
	int i = 0;
	while (i < BUFFERSIZE)
	{
		LOGBUFFER[i] = '\0';
		i++;
	}
}

CMFCVideoPlayerDlg::~CMFCVideoPlayerDlg()
{
	delete []IPAddressLists;
	delete[]LOGBUFFER;
}

void CMFCVideoPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_FileUrlEdit);
	DDX_Control(pDX, IDC_STARTBUTTON, m_StartButton);
	DDX_Control(pDX, IDC_PAUSEBUTTON, m_PauseButton);
	DDX_Control(pDX, IDC_FULLSCREENBUTTON, m_FullScreenButton);
	DDX_Control(pDX, IDC_FILEBUTTON, m_FileButton);
	DDX_Control(pDX, IDC_SCREEN, m_ScreenArea);
	DDX_Control(pDX, IDC_SCREEN2, m_ScreenArea2);
	DDX_Control(pDX, IDC_SCREEN3, m_ScreenArea3);
	DDX_Control(pDX, IDC_SCREEN4, m_ScreenArea4);
	DDX_Control(pDX, IDC_Discovery, m_DiscoveryButton);
	DDX_Control(pDX, IDC_LIST_Discovery, DiscoveryList);
	DDX_Control(pDX, IDC_choseGraph1, ChoseGraph1);
	DDX_Control(pDX, IDC_choseGraph2, ChoseGraph2);
	DDX_Control(pDX, IDC_choseGraph3, ChoseGraph3);
	DDX_Control(pDX, IDC_choseGraph4, ChoseGraph4);
}

BEGIN_MESSAGE_MAP(CMFCVideoPlayerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()

	ON_BN_CLICKED(IDC_Discovery, &CMFCVideoPlayerDlg::OnBnClickedDiscovery)

	ON_BN_CLICKED(IDC_FILEBUTTON, &CMFCVideoPlayerDlg::OnBnClickedFilebutton)
	ON_BN_CLICKED(IDC_STARTBUTTON, &CMFCVideoPlayerDlg::OnBnClickedStartbutton)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_PAUSEBUTTON, &CMFCVideoPlayerDlg::OnBnClickedPausebutton)
	
	ON_BN_CLICKED(IDC_FULLSCREENBUTTON, &CMFCVideoPlayerDlg::OnBnClickedFullscreenbutton)

	ON_MESSAGE(WMU_PLAY_OVER, &CMFCVideoPlayerDlg::OnVideoPlayOver)

	ON_BN_CLICKED(IDC_BUTTON_AddDevice, &CMFCVideoPlayerDlg::OnAddDeviceButton)



END_MESSAGE_MAP()


// CMFCVideoPlayerDlg 消息处理程序

BOOL CMFCVideoPlayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetWindowRect(&m_cWindowRect);
	ScreenToClient(&m_cWindowRect);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}


void CMFCVideoPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

	CDialog::OnSysCommand(nID, lParam);

}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCVideoPlayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCVideoPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCVideoPlayerDlg::OnBnClickedDiscovery()
{
	int i = 0, k = 0;
	char IPAddress[IPADDCHAR] = "";
	char LOGINFO[IPADDCHAR] = ""; std::string LOGINFOS;
	ONVIF_DetectDevice(LOGBUFFER, BUFFERSIZE); // cb_discovery
	while ('$' != LOGBUFFER[i])
	{
		if ('\n' != LOGBUFFER[i] && k < IPADDCHAR - 1)
		{
			IPAddress[k] = LOGBUFFER[i];
		}
		else
		{
			IPAddressLists[CameraNum] = IPAddress;
			// DiscoveryList.AddString(CString((IPAddressLists[j]).c_str()));
			CameraNum++;
			k = 0;
		}
		i++; k++;
	}
	i++;
	while ('$' != LOGBUFFER[i])
	{
		LOGINFO[k] = LOGBUFFER[i];
		k++; i++;
	}
	LOGINFOS = LOGINFO;
	DiscoveryList.AddString(CString(LOGINFOS.c_str()));

	/*for debug use*/
	/*j = 0; CameraNum = 10;
	IPAddressLists[0] = "http://admin:admin@192.168.3.11/onvifsnapshot/media_service/snapshot?channel=1&subtype=0";
	IPAddressLists[1] =  "E:\\Mycourse\\Design\\code\\VID_20210331_233602.mp4" ;
	IPAddressLists[2] =  "E:\\Mycourse\\Design\\code\\VID_20210331_233603.mp4" ;
	IPAddressLists[3] =  "E:\\Mycourse\\Design\\code\\VID_20210331_233604.mp4" ;
	IPAddressLists[4] = "E:\\Mycourse\\Design\\code\\VID_20210331_233604.mp4";
	IPAddressLists[5] = "E:\\Mycourse\\Design\\code\\VID_20210331_233604.mp4";
	IPAddressLists[6] = "E:\\Mycourse\\Design\\code\\VID_20210331_233604.mp4";
	IPAddressLists[7] = "E:\\Mycourse\\Design\\code\\VID_20210331_233604.mp4";
	IPAddressLists[8] = "E:\\Mycourse\\Design\\code\\VID_20210331_233604.mp4";
	IPAddressLists[9] = "E:\\Mycourse\\Design\\code\\VID_20210331_233604.mp4";
	while (j < CameraNum)
	{
		DiscoveryList.AddString(CString((IPAddressLists[j]).c_str()));
		j++;
	}*/
	/*for debug use*/

	renewDeviceList();
}

void CMFCVideoPlayerDlg::renewDeviceList()
{	
	while (DiscoveryList.GetCount())
	{
		DiscoveryList.DeleteString(0);
		ChoseGraph1.DeleteString(0);
		ChoseGraph2.DeleteString(0);
		ChoseGraph3.DeleteString(0);
		ChoseGraph4.DeleteString(0);
	}
	int i = 0;
	while (i < CameraNum)
	{
		DiscoveryList.AddString(CString((IPAddressLists[i]).c_str()));
		ChoseGraph1.AddString(CString(IPAddressLists[i].c_str()));
		ChoseGraph2.AddString(CString(IPAddressLists[i].c_str()));
		ChoseGraph3.AddString(CString(IPAddressLists[i].c_str()));
		ChoseGraph4.AddString(CString(IPAddressLists[i].c_str()));
		i++;
	}
}


// 基于此函数做一个用IP来选择相机的对话框，对2*2监控画面分别进行选择。每选择完成一张图片后就打开该监控，否则在画面中显示
// “请选择摄像头”字样。监控对话框显示的是摄像头的控制ID，为实现界面友好，不对用户直接展示IP地址
// 用户选择完成后，在该函数中找到对应摄像头的ID号码，改变一个4元素字符串数组类型的成员变量，里面存储了之前用户选择的四个
// 画面对应的摄像机的IP地址。在显示对话框时，也能够利用该字符串数组，显示出之前选择的摄像机。
void CMFCVideoPlayerDlg::OnBnClickedFilebutton()
{
	// CString FilePathName;
	// CString Filter = CString("视频文件|*.mp4;*.mpeg;*.avi;*.mkv;*.rmvb;*.wmv;*.flv||");

	// CFileDialog dlg(TRUE, L".", L"", OFN_HIDEREADONLY | OFN_READONLY, Filter, NULL);
	// if (dlg.DoModal() == IDOK)
	// {
		// FilePathName = dlg.GetPathName();
		// m_FileUrlEdit.SetWindowText(FilePathName); 
		// 在设置了4个窗口后应该时没办法在总窗口栏显示所播放的文件了。但是可以在小窗口上写一个标签？

	CStringA FilePath;
	char URIs[URICHAR] = "";
	// CStringA FilePath2 = "http://admin:Admin123321@111.38.131.29/onvifsnapshot/media_service/snapshot?channel=1&subtype=0";
	// CStringA FilePath3 = "rtsp://192.168.3.11:554/cam/realmonitor?channel=1&subtype=0&unicast=true&proto=Onvif"; ==================
	//						"rtsp://admin:adminadmin123@192.168.3.168:554/onvif1" ====================================================
	// CStringA FilePath4 = "E:\\Mycourse\\Design\\code\\VID_20210331_233604.mp4";
		// GetWindowTextA(m_FileUrlEdit.m_hWnd, (LPSTR)(LPCSTR)FilePath, 200);
	int IPAddIndex = ChoseGraph1.GetCurSel();
	if (-1 != IPAddIndex)
	{
		IPAddIndex = CameraNum - IPAddIndex - 1; //  GetCurSel()顺序是反的，取补数
		// UserLists[IPAddIndex] = "admin";
		// PasswdLists[IPAddIndex] = "Admin123321";
		if (("" == UserLists[IPAddIndex]) || ("" == PasswdLists[IPAddIndex]))
		{
			MFC_User_and_Password mfc_user_and_password;
			mfc_user_and_password.User_ = UserLists[IPAddIndex].c_str();
			mfc_user_and_password.Password_ = UserLists[IPAddIndex].c_str();
			mfc_user_and_password.DoModal();
			UserLists[IPAddIndex] = WString2String(mfc_user_and_password.GetUser());
			PasswdLists[IPAddIndex] = WString2String(mfc_user_and_password.GetPassword());
		}
		getURI(URIs, IPAddressLists[IPAddIndex].c_str(),URICHAR, UserLists[IPAddIndex].c_str(), PasswdLists[IPAddIndex].c_str());
		// getURI(URIs, "http://111.38.131.29:37779/onvif/device_service", URICHAR, UserLists[IPAddIndex].c_str(), PasswdLists[IPAddIndex].c_str());
		URILists[IPAddIndex] = URIs;
		FilePath = CStringA(URILists[IPAddIndex].c_str());
	}
	else
	{
		// FilePath = CString("rtsp://admin:adminadmin123@192.168.3.168:554/onvif1");
		AfxMessageBox(L"未选择摄像头");
	}
		// GetWindowTextA(ChoseGraph1.m_hWnd, (LPSTR)(LPCSTR)FilePath, 200);
		// m_FileUrlEdit.SetWindowTextW(NULL);
		// =================================
	m_ScreenArea.SetFile(FilePath);
	m_StartButton.EnableWindow(TRUE);

	// }
	// delete dlg;
}

// 本函数可以改为放到选择IP的那个函数里面，选好了就触发播放。然后在关闭界面的那个回调函数触发停止播放
// 当没有选择摄像头IP或选择空IP时，显示一张提示用户选择IP的图片
void CMFCVideoPlayerDlg::OnBnClickedStartbutton()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!m_bHasBegin)
	{
		//if (m_FileUrlEdit.GetWindowTextLengthW() < 1)
		//{
		//	AfxMessageBox(L"请输入文件名！");
		//	return;
		//}
		/*if (ChoseGraph1.GetWindowTextLengthW() < 1)
		{
			AfxMessageBox(L"请选择有效摄像头!");
			return;
		}*/
		m_bHasBegin = true;
		m_ScreenArea.PlayBegin();
		// m_ScreenArea2.PlayBegin();
		// m_ScreenArea3.PlayBegin();
		// m_ScreenArea4.PlayBegin();


		m_PauseButton.ShowWindow(SW_SHOW);
		m_FullScreenButton.ShowWindow(SW_SHOW);
		m_bIsPause = false;
		m_bAlreadyEnded = false;
		
		m_PauseButton.SetWindowTextW(L"暂停");
		m_StartButton.SetWindowTextW(L"停止播放");
		m_FileButton.EnableWindow(FALSE);
		m_FileUrlEdit.EnableWindow(FALSE);
	}
	else
	{
	    m_ScreenArea.PlayStop();//通过消息来更新界面，让资源完整释放后再响应用户操作
		// m_ScreenArea2.PlayStop();
		// m_ScreenArea3.PlayStop();
		// m_ScreenArea4.PlayStop();
		m_bHasBegin = false;
		m_PauseButton.ShowWindow(SW_HIDE);
		m_FullScreenButton.ShowWindow(SW_HIDE);
		m_StartButton.SetWindowTextW(L"开始播放");
		m_FileButton.EnableWindow(TRUE);
		m_FileUrlEdit.EnableWindow(TRUE);
	}

}

void CMFCVideoPlayerDlg::OnSize(UINT nType, int cx, int cy)
{
	Invalidate();
	CDialog::OnSize(nType, cx, cy);
	// TODO: 在此处添加消息处理程序代码
}

void CMFCVideoPlayerDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	lpMMI->ptMinTrackSize.x = m_cWindowRect.Width();
	lpMMI->ptMinTrackSize.y = m_cWindowRect.Height();
	CDialog::OnGetMinMaxInfo(lpMMI);
}

// 该函数暂时没有用
void CMFCVideoPlayerDlg::OnBnClickedPausebutton()
{
	// TODO: 在此添加控件通知处理程序代码

	if (!m_bIsPause)
	{
		m_PauseButton.SetWindowTextW(L"继续");
	}
	else
	{
		m_PauseButton.SetWindowTextW(L"暂停");
	}
	m_bIsPause = !m_bIsPause;
	m_ScreenArea.PlayPause();
	// m_ScreenArea2.PlayPause();
	// m_ScreenArea3.PlayPause();
	// m_ScreenArea4.PlayPause();

}

// 该函数好像也暂时没有用，如果要放大的话就需要四张图一起放大了看的，不如把窗口做大一点。
void CMFCVideoPlayerDlg::OnBnClickedFullscreenbutton()
{
	m_bIsFullScreen = true;
	int cxScreen, cyScreen;
	cxScreen = GetSystemMetrics(SM_CXSCREEN);
	cyScreen = GetSystemMetrics(SM_CYSCREEN);

	WINDOWPLACEMENT wpNew;
	CRect WindowRect;
	::GetWindowRect(::GetDesktopWindow(), &WindowRect);
	GetWindowPlacement(&m_stWpOld);
	CRect ClientRect;
	RepositionBars(0, 0xffff, AFX_IDW_PANE_FIRST, reposQuery, &ClientRect);
	ClientToScreen(&ClientRect);
	wpNew.length = sizeof(WINDOWPLACEMENT);
	wpNew.showCmd = SW_MAXIMIZE;
	wpNew.rcNormalPosition = WindowRect;
	SetWindowPos(&wndTopMost, 0, 0, cxScreen, cyScreen, SWP_SHOWWINDOW);

	LONG IStyle = ::GetWindowLong(this->m_hWnd, GWL_STYLE);

	::SetWindowLong(this->m_hWnd, GWL_STYLE, IStyle & ~WS_CAPTION);
	::SetWindowPos(this->m_hWnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
	SetWindowPlacement(&wpNew);

	m_ScreenArea.MoveWindow(0, 0, WindowRect.Width(), WindowRect.Height());
	// m_ScreenArea2.MoveWindow(0, 0, WindowRect.Width(), WindowRect.Height());
	// m_ScreenArea3.MoveWindow(0, 0, WindowRect.Width(), WindowRect.Height());
	// m_ScreenArea4.MoveWindow(0, 0, WindowRect.Width(), WindowRect.Height());

	m_FileButton.ShowWindow(SW_HIDE);
	m_FullScreenButton.ShowWindow(SW_HIDE);
	m_FileUrlEdit.ShowWindow(SW_HIDE);
	m_PauseButton.ShowWindow(SW_HIDE);
	m_StartButton.ShowWindow(SW_HIDE);
}

// 撤销全屏的函数，暂时没有用
BOOL CMFCVideoPlayerDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		if (m_bIsFullScreen)
		{
			m_bIsFullScreen = false;
			m_ScreenArea.SetFullScreen(m_bIsFullScreen);
			// m_ScreenArea2.SetFullScreen(m_bIsFullScreen);
			// m_ScreenArea3.SetFullScreen(m_bIsFullScreen);
			// m_ScreenArea4.SetFullScreen(m_bIsFullScreen);

			LONG IStyle = ::GetWindowLong(this->m_hWnd, GWL_STYLE);
			::SetWindowLong(this->m_hWnd, GWL_STYLE, IStyle | WS_CAPTION);
			::SetWindowPos(this->m_hWnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER
				| SWP_NOACTIVATE | SWP_FRAMECHANGED);
			SetWindowPlacement(&m_stWpOld);
			if (m_bHasBegin)
			{
				m_FullScreenButton.ShowWindow(SW_SHOW);
				m_PauseButton.ShowWindow(SW_SHOW);
			}
			m_FileButton.ShowWindow(SW_SHOW);
			m_FileUrlEdit.ShowWindow(SW_SHOW);
			m_StartButton.ShowWindow(SW_SHOW);
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

// 这个可以
void CMFCVideoPlayerDlg::OnCancel()
{
	if (IDOK == AfxMessageBox(L"确定退出?", MB_OKCANCEL))
	{
		m_ScreenArea.PlayStop();
		// m_ScreenArea2.PlayStop();
		// m_ScreenArea3.PlayStop();
		// m_ScreenArea4.PlayStop();
        EndDialog(IDCANCEL);
	}
}
// 这个可以
void CMFCVideoPlayerDlg::OnOK()
{
	EndDialog(IDOK);
	return;
}

// 这个必须
void CMFCVideoPlayerDlg::OnClose()
{
	OnCancel();
}

// 把这个改为断网或者信号不好时的函数，并在该窗口提示该摄像头信号不好
LRESULT CMFCVideoPlayerDlg::OnVideoPlayOver(WPARAM wParam, LPARAM lParam)
{
	if(m_bHasBegin)
        ::PostMessage(m_hWnd, WM_COMMAND, MAKEWPARAM(IDC_STARTBUTTON, BN_CLICKED), NULL);
	return 0UL;
}

void CMFCVideoPlayerDlg::OnAddDeviceButton()
{
	AddDeciveDlg addDevicedlg;
	addDevicedlg.DoModal();
	if (addDevicedlg.HasAddedNewCamera)
	{
		IPAddressLists[CameraNum] = WString2String(addDevicedlg.GetIPAddress());
		UserLists[CameraNum] = WString2String(addDevicedlg.GetUser());
		PasswdLists[CameraNum] = WString2String(addDevicedlg.GetPassword());
		CameraNum += 1;
		renewDeviceList();
	}
}



//wstring转换成string
std::string WString2String(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);

	if (nLen <= 0) return std::string("");

	char* pszDst = new char[nLen];
	if (NULL == pszDst) return std::string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;

	std::string strTemp(pszDst);
	delete[] pszDst;

	return strTemp;
}

//string转换车wstring
std::wstring  String2WString(const std::string& s)
{
	std::wstring wszStr;

	int nLength = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, NULL, NULL);
	wszStr.resize(nLength);
	LPWSTR lpwszStr = new wchar_t[nLength];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, lpwszStr, nLength);
	wszStr = lpwszStr;
	delete[] lpwszStr;
	return wszStr;
}

