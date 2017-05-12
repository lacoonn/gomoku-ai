// OmDevDlg.h : header file
//

#if !defined(AFX_OMDEVDLG_H__85B250C0_9BD5_4CC1_AC60_D4F85882A8E4__INCLUDED_)
#define AFX_OMDEVDLG_H__85B250C0_9BD5_4CC1_AC60_D4F85882A8E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAXXY 16
#define MAXTURN (MAXXY*MAXXY)
#define BLACK 1
#define WHITE 0
#define EMPTY -1
#define OUTBD -2

/////////////////////////////////////////////////////////////////////////////
// COmDevDlg dialog

class COmDevDlg : public CDialog
{
// Construction
public:
	COmDevDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(COmDevDlg)
	enum { IDD = IDD_OMDEV_DIALOG };
	CEdit	m_edtInfo;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(COmDevDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	int szCell;
	int CurTurn, MaxTurn;
	int x[MAXTURN], y[MAXTURN];

	void MakeBoard();
	void Progress(int nx, int ny);
	BOOL IsGameOver();

	//	 게임시간 측정
	__int64 NanoSecBlack, NanoSecWhite, NanoSecCur;
	class CStopwatch {
		LARGE_INTEGER m_liPerfFreq, m_liPerfStart;    // 초당 계수, 시작 계수
	public:
		CStopwatch() { QueryPerformanceFrequency(&m_liPerfFreq); QueryPerformanceCounter(&m_liPerfStart);}
		__int64 Now() const {			// 클래스 생성된 후 경과한 나노초를 리턴
			LARGE_INTEGER liPerfNow;	// 초당 계수, 시작 계수, 임시 변수
			QueryPerformanceCounter(&liPerfNow);
			return(((liPerfNow.QuadPart - m_liPerfStart.QuadPart) * 1000000) / m_liPerfFreq.QuadPart);
		}
	 };

	//{{AFX_MSG(COmDevDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonBack();
	afx_msg void OnButtonFore();
	afx_msg void OnButtonSave();
	afx_msg void OnButtonLoad();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_OMDEVDLG_H__85B250C0_9BD5_4CC1_AC60_D4F85882A8E4__INCLUDED_)
