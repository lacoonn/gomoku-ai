// OmDevDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OmDev.h"
#include "OmDevDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void f201701001(int *NewX, int *NewY, int MyColor, int CurTurn);
int Board[MAXXY][MAXXY];

int B(int x, int y, int def = OUTBD)
{
	if (x < 0 || x >= MAXXY) return def;
	if (y < 0 || y >= MAXXY) return def;
	return Board[x][y];
}

/////////////////////////////////////////////////////////////////////////////
// COmDevDlg dialog

COmDevDlg::COmDevDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COmDevDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COmDevDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	szCell = 20;
	MaxTurn = CurTurn = 0;
	NanoSecBlack = NanoSecWhite = NanoSecCur = 0;
}

void COmDevDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COmDevDlg)
	DDX_Control(pDX, IDC_EDIT_INFO, m_edtInfo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COmDevDlg, CDialog)
	//{{AFX_MSG_MAP(COmDevDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BACK, OnButtonBack)
	ON_BN_CLICKED(IDC_BUTTON_FORE, OnButtonFore)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnButtonLoad)
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COmDevDlg message handlers

BOOL COmDevDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);	
	
	return TRUE;
}

void COmDevDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		int szBoard = szCell * MAXXY;

		//	오목판 그리기
		int i;
		CString s;
		dc.FillSolidRect(0,0, szBoard,szBoard, 0x4080f0);
		for (i = 0; i < MAXXY; i++) {
			s.Format("%d", i%10);
			dc.FillSolidRect(szCell/2,szCell/2 + szCell*i, szCell*(MAXXY-1), 1, 0x000000);
			dc.FillSolidRect(szCell/2 + szCell*i,szCell/2, 1, szCell*(MAXXY-1), 0x000000);
			dc.SetBkMode(TRANSPARENT);
			dc.TextOutA(szCell/2 -9,szCell/2 + szCell*i -7, s);
			dc.TextOutA(szCell/2 + szCell*i -4,szCell/2 -17, s);
		}

		//	알그리기
		CBrush BlackBrush(COLORREF(0x000000)), *pBrush;
		pBrush = dc.SelectObject(&BlackBrush);	// 흰부러쉬를 pBrush에 보관
		for (i = CurTurn-1; i >= 0 ; i--) {
			dc.SelectObject(i%2==0 ? &BlackBrush : pBrush);
			CRect r(x[i]*szCell, y[i]*szCell, (x[i]+1)*szCell, (y[i]+1)*szCell);
			dc.Ellipse(&r);
			s.Format("%d", i);
			dc.SetTextColor(i%2==0 ? 0xffffff : 0x000000 );
			if (i == CurTurn-1) dc.SetTextColor(0x0000ff);
			dc.DrawText(s, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		dc.SelectObject(pBrush);

		s.Format("CurTurn : %d\r\nMaxTurn : %d\r\n\r\nExeTime : %dns\r\nBlkTime : %dns\r\nWhtTime : %dns\r\n", CurTurn, MaxTurn, (int)NanoSecCur, (int)NanoSecBlack, (int)NanoSecWhite);
		m_edtInfo.SetWindowTextA(s);
	}
}

HCURSOR COmDevDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void COmDevDlg::OnButtonBack() 
{
	if (CurTurn > 0) CurTurn--;
	Invalidate();
}

void COmDevDlg::OnButtonFore() 
{
	if (CurTurn < MaxTurn) CurTurn++;
	Invalidate();
}

void COmDevDlg::OnButtonSave() 
{
	CFileDialog fd(FALSE);
	fd.DoModal();
	CString fname = fd.GetPathName();

	CFile f;
	if (f.Open(fname, CFile::modeCreate | CFile::modeWrite)) {
		f.Write(&CurTurn, sizeof CurTurn);
		f.Write(&MaxTurn, sizeof MaxTurn);
		f.Write(x, sizeof x);
		f.Write(y, sizeof y);
		f.Close();
	}
}

void COmDevDlg::OnButtonLoad() 
{
	CFileDialog fd(TRUE);
	fd.DoModal();
	CString fname = fd.GetPathName();

	CFile f;
	if (f.Open(fname, CFile::modeRead)) {
		f.Read(&CurTurn, sizeof CurTurn);
		f.Read(&MaxTurn, sizeof MaxTurn);
		f.Read(x, sizeof x);
		f.Read(y, sizeof y);
		f.Close();
	}
	Invalidate();
}

void COmDevDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	szCell = cy / MAXXY;
	Invalidate();
	
}

void COmDevDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	int nx = point.x / szCell;
	int ny = point.y / szCell;

	if (CurTurn < MAXTURN && nx >= 0 && nx < MAXXY && ny >= 0 && ny < MAXXY) {
		MakeBoard();
		Progress(nx,ny);
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void COmDevDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	int nx, ny;
	int Turn = CurTurn%2==0 ? BLACK : WHITE;
	MakeBoard();

	CStopwatch Timer;
	f201701001(&nx, &ny, Turn, CurTurn);
	NanoSecCur = Timer.Now();

	if (Turn == BLACK)	NanoSecBlack += NanoSecCur;
	else				NanoSecWhite += NanoSecCur;

	Progress(nx, ny);

	CDialog::OnRButtonDown(nFlags, point);
}

void COmDevDlg::MakeBoard()
{
	memset(Board, -1, sizeof Board);
	for (int i = CurTurn-1; i >= 0 ; i--)
		Board[x[i]] [y[i]] = 
			i%2==0 ? BLACK : WHITE;
}

void COmDevDlg::Progress(int nx, int ny)
{
	if (!IsGameOver()) {
		x[CurTurn] = nx % MAXXY;	//	오목판 바깥에 두었을 경우, 내부로 수정 적용
		y[CurTurn] = ny % MAXXY;
		CurTurn++;
		MaxTurn = CurTurn;
		Invalidate();
	}
}

BOOL COmDevDlg::IsGameOver()
{
	int iy, ix, n, d;
	int dx[] = {1,1,0,-1};
	int dy[] = {0,1,1,1};
	for (iy = 0; iy < MAXXY; iy++) {
		for (ix = 0; ix < MAXXY; ix++) {
			for (d = 0; d < 4; d++) 
				if (B(ix,iy) != EMPTY) {
					for (n = 1; n < 5; n++)
						if (B(ix,iy) != B(ix+n*dx[d],iy+n*dy[d]))
							break;
					if (n == 5) return TRUE;
				}
		}
	}

	return FALSE;
}
void COmDevDlg::OnButtonRecv() 
{
	CWnd *pWnd = FindWindow("ShowOmok",NULL);
	if (pWnd == NULL) return;
	if (pWnd->SendMessage(WM_USER+1111, -1) == TRUE) {
		unsigned char Order[0x100];

		HANDLE hBuff;
		LPSTR pBuff=NULL;
		if (!OpenClipboard())	return; 
		hBuff = GetClipboardData(CF_TEXT);
		pBuff=(LPSTR)GlobalLock(hBuff);
		memcpy(&CurTurn, pBuff, sizeof CurTurn);
		memcpy(Order, pBuff+sizeof CurTurn, sizeof Order);
		GlobalUnlock(hBuff);
		CloseClipboard();

		for (int i = 0; i < MAXTURN; i++) {
			x[i] = Order[i]&0xf;
			y[i] = Order[i]>>4;
		}
		Invalidate();
	}
}

//[보내기]버튼 코드 - OmDev에서 마지막 둔 알을 ShowOmok에 둠
void COmDevDlg::OnButtonSend() 
{
	CWnd *pWnd = FindWindow("ShowOmok",NULL);
	if (pWnd == NULL || CurTurn == 0) return;
	int xy = (y[CurTurn-1]<<16) | x[CurTurn-1];
	if (pWnd->SendMessage(WM_USER+1111, xy) == TRUE) {
		Sleep(1000);
		OnButtonRecv();
	}
}