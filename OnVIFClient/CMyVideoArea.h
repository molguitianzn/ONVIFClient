#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <atlimage.h>
#include <afxmt.h>
#include "CMyBlockQueue.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/avutil.h"
#include "libavutil/imgutils.h"
#include "SDL.h"
}


#define WMU_PLAY_OVER (WM_USER+3)
#define WMU_PLAY_ERROR (WM_USER+9)

constexpr int EXIT_EXCPTION = -1;
constexpr int EXIT_NORMAL = 0;

// 图像元数据封装，一针四参
struct ImageMetaData
{
	uint8_t* st_pMetaData;
	int st_nDelay;
	int st_nWidth;
	int st_nHeight;
	int st_nLenth;
	// 利用图片的构造函数
	ImageMetaData(uint8_t* pFrameData = nullptr, int nDelay = 0, int nWidth = 0, int nHeight = 0, int nLenth = 0)
		:st_nDelay(nDelay),st_nWidth(nWidth),st_nHeight(nHeight),st_nLenth(nLenth)
	{
		st_pMetaData = new uint8_t [nLenth]();
		memcpy_s(st_pMetaData, nLenth, pFrameData, nLenth);
	}
	// 利用同类的构造函数
	ImageMetaData(ImageMetaData& srcObj)
	{
		st_nDelay = srcObj.st_nDelay;
		st_nWidth = srcObj.st_nWidth;
		st_nHeight = srcObj.st_nHeight;
		st_nLenth = srcObj.st_nLenth;
		st_pMetaData = new uint8_t  [st_nLenth];
		memcpy_s(st_pMetaData, st_nLenth, srcObj.st_pMetaData, st_nLenth);
	}
	// 利用同类指针的构造函数
	ImageMetaData(ImageMetaData* srcObj)
	{
		st_nDelay = srcObj->st_nDelay;
		st_nWidth = srcObj->st_nWidth;
		st_nHeight = srcObj->st_nHeight;
		st_nLenth = srcObj->st_nLenth;
		st_pMetaData = new uint8_t[st_nLenth];
		memcpy_s(st_pMetaData, st_nLenth, srcObj->st_pMetaData, st_nLenth);
	}
	~ImageMetaData()
	{
		delete[] st_pMetaData;
	}
	// =运算符重载
	bool operator = (ImageMetaData& srcObj)
	{
		
		delete[] st_pMetaData;

		st_nDelay = srcObj.st_nDelay;
		st_nWidth = srcObj.st_nWidth;
		st_nHeight = srcObj.st_nHeight;
		st_nLenth = srcObj.st_nLenth;
		st_pMetaData = new uint8_t[st_nLenth];
		memcpy_s(st_pMetaData, st_nLenth, srcObj.st_pMetaData, st_nLenth);
	}
	operator uint8_t* ()
	{
		return st_pMetaData;
	}
};

// 封装了一个hBitmap句柄和一个st_nDelay参数
struct CoverImage
{
private:
	HBITMAP st_hBitmap;
	
public:
	int st_nDelay;
	CoverImage(HBITMAP curBitmap=nullptr,int Delay = 0)
	{
		st_hBitmap = curBitmap;
		st_nDelay = Delay;
	}
	~CoverImage()
	{
		if(st_hBitmap!=nullptr)
		   DeleteObject(st_hBitmap);
	}
	bool operator= (CoverImage& srcObj)
	{
		st_hBitmap = srcObj.Detach();
		return true;
	}
	HBITMAP Detach()
	{
		HBITMAP temp = st_hBitmap;
		st_hBitmap = nullptr;
		return temp;
	}
	operator HBITMAP()
	{
		return st_hBitmap;
	}
};


// 封装视频播放内容，视频参数，解码，启停，线程等
class CMyVideoArea :
	public CStatic
{
	static UINT ThreadPlay(LPVOID lpParam);
	static UINT ThreadRender(LPVOID lpParam);
private:
	//工作线程控制变量
	bool m_bThreadExit;
	bool m_bThreadBegin;
	bool m_bThreadPause;
	bool m_bEndNormal;
	//绘制控制
	CMutex *m_pPaintMutex;
	//错误搜集
	CString m_cExceptionStr;

	//绘制矩形
	CRect m_ScreenRect;
	CRect m_DrawRect;
	CRect m_ImageRect;
	CRgn m_Rgn;
	//图形相关
	CImage m_cImage;
	//文件路径
	CStringA m_cFileName;
	//画图宽高
	int m_nImageWidth;
	int m_nImageHeight;
	//线程控制
	CWinThread* m_pThreadPlay;
	CWinThread* m_pThreadRender;
	//全屏控制
	bool m_bIsFullScreen;

	//解码相关


private:
	
	CMyBlockQueue<CoverImage*> m_ImageQueue;

	// 播放出错时给一个弹窗
	LRESULT ErrorWarning(WPARAM,LPARAM);

	//将信息存储为源数据，难懂+1
	void RestoreMetaData(AVFrame* pFrame, int nWidth, int nHeight, int nBpp,int Delay ,int *padding);
	//调整绘制区域，难懂+2
	void AdjustDrawRect(bool model = true);
	//解码线程工作函数，难懂
	int FFmpegplayer();
	//绘制控制线程
	void RenderArea();
	//结束所有线程
	void LetAllThreadEnd();
	//结束绘制线程
	void LetRenderThreadEnd();
	//清理资源
	void CleanUp();
	//HBITMAP SourceMadeBitmap();//将元数据转变为bitmap
	//解码数据检查，仅供Debug使用
	int SaveYUVFrameToFile(AVFrame* frame, int width, int height, int flag);

protected:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

public:
	CMyVideoArea();
	~CMyVideoArea();
	//设置文件名
	void SetFile(CStringA str);
	//暂停与继续
	void PlayPause();
	//开始播放
	void PlayBegin();
	//停止播放
	void PlayStop();
	//全屏
	void SetFullScreen(bool bCurModel);
};

