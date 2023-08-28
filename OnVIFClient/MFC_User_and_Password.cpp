// MFC_User_and_Password.cpp: 实现文件
//

#include "pch.h"
#include "MFC_VideoPlayer.h"
#include "MFC_User_and_Password.h"
#include "afxdialogex.h"


// MFC_User_and_Password 对话框

IMPLEMENT_DYNAMIC(MFC_User_and_Password, CDialogEx)

MFC_User_and_Password::MFC_User_and_Password(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_User_and_password, pParent)
	, Password_(_T(""))
	, User_(_T(""))
{
}

MFC_User_and_Password::~MFC_User_and_Password()
{
}

void MFC_User_and_Password::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	// DDX_Control(pDX, IDC_EDIT_User, EditUser);
	// DDX_Control(pDX, IDC_EDIT_Password, EditPassword);
	DDX_Text(pDX, IDC_EDIT_Password, Password_);
	DDV_MaxChars(pDX, Password_, 200);
	DDX_Text(pDX, IDC_EDIT_User, User_);
	DDV_MaxChars(pDX, User_, 200);
}


BEGIN_MESSAGE_MAP(MFC_User_and_Password, CDialogEx)
	ON_BN_CLICKED(IDOK, &MFC_User_and_Password::OnBnClickedOk)
END_MESSAGE_MAP()


// MFC_User_and_Password 消息处理程序
void MFC_User_and_Password::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	// CDialogEx::OnOK();
	this->UpdateData(TRUE);
	this->EndDialog(0);
	//EditUser.GetWindowTextW(User);
	//EditPassword.GetWindowTextW(Password);
}


void MFC_User_and_Password::setInformation(const CString& user, const CString& password)
{
	// TODO: 在此处添加实现代码.
	User_ = user;
	Password_ = password;
}


CString MFC_User_and_Password::GetUser()
{
	// TODO: 在此处添加实现代码.
	return User_;
}

CString MFC_User_and_Password::GetPassword()
{
	return Password_;
}