#pragma once


// AddDeciveDlg 对话框

class AddDeciveDlg : public CDialogEx
{
	DECLARE_DYNAMIC(AddDeciveDlg)

public:
	AddDeciveDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~AddDeciveDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_AddDevice };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 用户输入的密码	// 用户输入的密码
	CString Password;
	// 用户输入端口号	// 用户输入端口号
	CString PortNumber;
	// 用户输入用户名
	CString User;
	// 用户输入IP地址
	DWORD IPAddress;
	// 是否成功添加摄像头
	bool HasAddedNewCamera;

	CString GetIPAddress();
	CString GetUser();
	CString GetPassword();
	afx_msg void OnBnClickedOk();
	bool validateIPAddressAndPortNumber();
	afx_msg void OnBnClickedCancel();
};
