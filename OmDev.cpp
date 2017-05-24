// OmDev.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "OmDev.h"
#include "OmDevDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// COmDevApp

BEGIN_MESSAGE_MAP(COmDevApp, CWinApp)
	//{{AFX_MSG_MAP(COmDevApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COmDevApp construction

COmDevApp::COmDevApp()
{
}

COmDevApp theApp;

/////////////////////////////////////////////////////////////////////////////
// COmDevApp initialization

BOOL COmDevApp::InitInstance()
{
	AfxEnableControlContainer();


#ifdef _AFXDLL
	Enable3dControls();
#else
	Enable3dControlsStatic();
#endif

	COmDevDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	return FALSE;
}
