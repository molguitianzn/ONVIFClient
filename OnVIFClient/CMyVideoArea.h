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

// ͼ��Ԫ���ݷ�װ��һ���Ĳ�
struct ImageMetaData
{
	uint8_t* st_pMetaData;
	int st_nDelay;
	int st_nWidth;
	int st_nHeight;
	int st_nLenth;
	// ����ͼƬ�Ĺ��캯��
	ImageMetaData(uint8_t* pFrameData = nullptr, int nDelay = 0, int nWidth = 0, int nHeight = 0, int nLenth = 0)
		:st_nDelay(nDelay),st_nWidth(nWidth),st_nHeight(nHeight),st_nLenth(nLenth)
	{
		st_pMetaData = new uint8_t [nLenth]();
		memcpy_s(st_pMetaData, nLenth, pFrameData, nLenth);
	}
	// ����ͬ��Ĺ��캯��
	ImageMetaData(ImageMetaData& srcObj)
	{
		st_nDelay = srcObj.st_nDelay;
		st_nWidth = srcObj.st_nWidth;
		st_nHeight = srcObj.st_nHeight;
		st_nLenth = srcObj.st_nLenth;
		st_pMetaData = new uint8_t  [st_nLenth];
		memcpy_s(st_pMetaData, st_nLenth, srcObj.st_pMetaData, st_nLenth);
	}
	// ����ͬ��ָ��Ĺ��캯��
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
	// =���������
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

// ��װ��һ��hBitmap�����һ��st_nDelay����
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


// ��װ��Ƶ�������ݣ���Ƶ���������룬��ͣ���̵߳�
class CMyVideoArea :
	public CStatic
{
	static UINT ThreadPlay(LPVOID lpParam);
	static UINT ThreadRender(LPVOID lpParam);
private:
	//�����߳̿��Ʊ���
	bool m_bThreadExit;
	bool m_bThreadBegin;
	bool m_bThreadPause;
	bool m_bEndNormal;
	//���ƿ���
	CMutex *m_pPaintMutex;
	//�����Ѽ�
	CString m_cExceptionStr;

	//���ƾ���
	CRect m_ScreenRect;
	CRect m_DrawRect;
	CRect m_ImageRect;
	CRgn m_Rgn;
	//ͼ�����
	CImage m_cImage;
	//�ļ�·��
	CStringA m_cFileName;
	//��ͼ���
	int m_nImageWidth;
	int m_nImageHeight;
	//�߳̿���
	CWinThread* m_pThreadPlay;
	CWinThread* m_pThreadRender;
	//ȫ������
	bool m_bIsFullScreen;

	//�������


private:
	
	CMyBlockQueue<CoverImage*> m_ImageQueue;

	// ���ų���ʱ��һ������
	LRESULT ErrorWarning(WPARAM,LPARAM);

	//����Ϣ�洢ΪԴ���ݣ��Ѷ�+1
	void RestoreMetaData(AVFrame* pFrame, int nWidth, int nHeight, int nBpp,int Delay ,int *padding);
	//�������������Ѷ�+2
	void AdjustDrawRect(bool model = true);
	//�����̹߳����������Ѷ�
	int FFmpegplayer();
	//���ƿ����߳�
	void RenderArea();
	//���������߳�
	void LetAllThreadEnd();
	//���������߳�
	void LetRenderThreadEnd();
	//������Դ
	void CleanUp();
	//HBITMAP SourceMadeBitmap();//��Ԫ����ת��Ϊbitmap
	//�������ݼ�飬����Debugʹ��
	int SaveYUVFrameToFile(AVFrame* frame, int width, int height, int flag);

protected:
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()

public:
	CMyVideoArea();
	~CMyVideoArea();
	//�����ļ���
	void SetFile(CStringA str);
	//��ͣ�����
	void PlayPause();
	//��ʼ����
	void PlayBegin();
	//ֹͣ����
	void PlayStop();
	//ȫ��
	void SetFullScreen(bool bCurModel);
};

