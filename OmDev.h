// OmDev.h : main header file for the OMDEV application
//

#if !defined(AFX_OMDEV_H__7BFA1D5E_E0B8_4C2D_8224_6F6AB9C46DE4__INCLUDED_)
#define AFX_OMDEV_H__7BFA1D5E_E0B8_4C2D_8224_6F6AB9C46DE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// COmDevApp:
// See OmDev.cpp for the implementation of this class
//

class COmDevApp : public CWinApp
{
public:
	COmDevApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COmDevApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(COmDevApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OMDEV_H__7BFA1D5E_E0B8_4C2D_8224_6F6AB9C46DE4__INCLUDED_)
