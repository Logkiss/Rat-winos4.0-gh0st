#pragma once


// CVideoDlg 对话框
#include "include/IOCPServer.h"
#include "BmpToAvi.h"
#include "../Common/VideoCodec.h"
class CVideoDlg : public CDialog
{
	DECLARE_DYNAMIC(CVideoDlg)

public:
	CVideoDlg(CWnd* pParent = NULL, CIOCPServer* pIOCPServer = NULL, ClientContext *pContext = NULL);   // 标准构造函数
	virtual ~CVideoDlg();

	// 对话框数据
	enum { IDD = IDD_VIDEODLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	int	m_nOldWidth; // OnSize时判断是高或宽发生变化，宽优先
	UINT m_nCount;
	HICON m_hIcon;
	CVideoCodec	*m_pVideoCodec;
	DWORD m_fccHandler;
	CString m_aviFile; // 如果文件名不为空就写入

	CBmpToAvi	m_aviStream;       //这个类也是gh0st封装好的类，我们调用其功能就可以了
	HDC			m_hDC;
	HDRAWDIB	m_hDD;

	ClientContext* m_pContext;
	CIOCPServer* m_iocpServer;
	CString m_IPAddress;
	LPVOID m_lpScreenDIB;
	LPBYTE m_lpCompressDIB;
	MINMAXINFO m_MMI;
	LPBITMAPINFO m_lpbmi;
protected:
	void ResetScreen(void);
	void InitMMI(void);
public:
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	void OnReceiveComplete(void);
private:
	void DrawDIB(void);
public:
	virtual BOOL OnInitDialog();
	void InitCodec(DWORD fccHandler);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	void SaveAvi(void);
};
