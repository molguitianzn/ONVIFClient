#pragma once


// MFC_User_and_Password 对话框

class MFC_User_and_Password : public CDialogEx
{
	DECLARE_DYNAMIC(MFC_User_and_Password)

public:
	MFC_User_and_Password(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~MFC_User_and_Password();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_User_and_password };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString User;
	CString Password;
	// 编辑用户名
	// CEdit EditUser;
	// 边界密码
	// CEdit EditPassword;
	afx_msg void OnBnClickedOk();
	// 密码
	CString Password_;
	// 用户
	CString User_;
	void setInformation(const CString& user, const CString& passwd);
	CString GetUser();
	CString GetPassword();
};
