// AddDeciveDlg.cpp: 实现文件
//

#include "pch.h"
#include "MFC_VideoPlayer.h"
#include "AddDeciveDlg.h"
#include "afxdialogex.h"


// AddDeciveDlg 对话框

IMPLEMENT_DYNAMIC(AddDeciveDlg, CDialogEx)

AddDeciveDlg::AddDeciveDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_AddDevice, pParent)
	, Password(_T(""))
	, PortNumber(_T(""))
	, User(_T(""))
	, IPAddress(0)
	, HasAddedNewCamera(false)
{

}

AddDeciveDlg::~AddDeciveDlg()
{
}

void AddDeciveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_Password, Password);
	DDV_MaxChars(pDX, Password, 40);
	DDX_Text(pDX, IDC_EDIT_PortNumber, PortNumber);
	DDV_MaxChars(pDX, PortNumber, 10);
	DDX_Text(pDX, IDC_EDIT_User, User);
	DDV_MaxChars(pDX, User, 50);
	DDX_IPAddress(pDX, IDC_IPADDRESS, IPAddress);
}


BEGIN_MESSAGE_MAP(AddDeciveDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &AddDeciveDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &AddDeciveDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// AddDeciveDlg 消息处理程序

/**
	其实是带有IP地址和端口号的device_service地址
	与设备发现模块所得到的格式相同
*/
CString AddDeciveDlg::GetIPAddress()
{
	// TODO: 在此处添加实现代码.
	CString DeviceServiceAdd;
	unsigned  char* pIP;
	CString  strIP;
	pIP = (unsigned   char*)&IPAddress;
	strIP.Format(L"%u.%u.%u.%u", *(pIP + 3), *(pIP + 2), *(pIP + 1), *pIP);
	if (PortNumber == "")
	{
		DeviceServiceAdd = L"http://" + strIP + PortNumber + L"/onvif/device_service";
	}
	else
	{
		DeviceServiceAdd = L"http://" + strIP + L":" + PortNumber + L"/onvif/device_service";
	}
	return DeviceServiceAdd;
}


CString AddDeciveDlg::GetUser()
{
	// TODO: 在此处添加实现代码.
	return User;
}


CString AddDeciveDlg::GetPassword()
{
	// TODO: 在此处添加实现代码.
	return Password;
}


void AddDeciveDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	this->UpdateData(TRUE); // 控件覆盖变量值
	if (validateIPAddressAndPortNumber())
	{
		CDialogEx::OnOK();
		HasAddedNewCamera = true;
		this->EndDialog(0);
	}
	else
	{
		AfxMessageBox(L"请输入有效IP地址和端口号!");
		IPAddress = 0;
		PortNumber = L"";
		this->UpdateData(FALSE); // 变量覆盖控件值
	}
	return;
}

bool AddDeciveDlg::validateIPAddressAndPortNumber()
{
	bool flag = true;
	int portNumberInt = _ttoi(PortNumber);
	if (portNumberInt > 65535 || portNumberInt < 0)
		flag = false;
	return flag;
}

void AddDeciveDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
	IPAddress = 0;
	PortNumber = L"";
	User = L"";
	Password = L"";
	UpdateData(false);
	this->EndDialog(0);
}
