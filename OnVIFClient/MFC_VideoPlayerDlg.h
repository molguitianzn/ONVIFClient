
// MFC_VideoPlayerDlg.h: 头文件
//

#pragma once
#include  "CMyVideoArea.h"
#include  "MFC_User_and_Password.h"
#include  "AddDeciveDlg.h"

#define IPCMAX	20								// 能够存储的摄像头总数

#define BUFFERSIZE 2000							// 显示区域的缓存大小
#define IPADDSIZE IPCMAX						// 可存放的IP地址总数目
#define IPADDCHAR 200							// 每个IP地址的字符数
#define URISIZE IPCMAX							// 可存放的URI地址总数目
#define URICHAR 200								// 每个解析出的URI字符数

std::string WString2String(LPCWSTR pwszSrc);
std::wstring  String2WString(const std::string& s);


// CMFCVideoPlayerDlg 对话框
class CMFCVideoPlayerDlg : public CDialog
{
// 构造
public:
	CMFCVideoPlayerDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CMFCVideoPlayerDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_VIDEOPLAYER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	bool m_bHasBegin;
	bool m_bIsPause;
	bool m_bAlreadyEnded;
	CRect m_cWindowRect;
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedFilebutton();
	// 文件路径，之后改为IP地址，准备用数据库存储
	std::string* IPAddressLists;	// 摄像头地址列表
	std::string* URILists;			// 摄像头对应rtsp地址
	std::string* UserLists;			// 摄像头对应用户名列表
	std::string* PasswdLists;		// 摄像头对应密码列表
	int CameraNum;
	CEdit m_FileUrlEdit;

	CMyVideoArea m_ScreenArea;
	CMyVideoArea m_ScreenArea2;
	CMyVideoArea m_ScreenArea3;
	CMyVideoArea m_ScreenArea4;

	WINDOWPLACEMENT m_stWpOld;
	bool m_bIsFullScreen;
	BOOL CMFCVideoPlayerDlg::PreTranslateMessage(MSG* pMsg); 

	

public:
	// 触发设备发现
	afx_msg void OnBnClickedDiscovery();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnClose();
	afx_msg void OnBnClickedStartbutton();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	
	afx_msg void OnBnClickedPausebutton();

	afx_msg void OnBnClickedFullscreenbutton();

	afx_msg LRESULT OnVideoPlayOver(WPARAM wParam,LPARAM lParam);
	afx_msg void OnAddDeviceButton();
	void renewDeviceList();

	CButton m_StartButton;
	CButton m_PauseButton;
	CButton m_FullScreenButton;
	CButton m_FileButton;
	// 设备发现按钮
	CButton m_DiscoveryButton;
	CListBox DiscoveryList;
	char* LOGBUFFER; // print the information on list box
					 // 选择显示器1显示的摄像头IP地址
	CComboBox ChoseGraph1;
	CComboBox ChoseGraph2;
	CComboBox ChoseGraph3;
	CComboBox ChoseGraph4;
};
