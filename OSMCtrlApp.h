#pragma once

#ifndef __AFXWIN_H__
  #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

class COSMCtrlAppApp : public CWinApp
{
public:
  COSMCtrlAppApp();

// Overrides
public:
  virtual BOOL InitInstance();

// Implementation
  afx_msg void OnAppAbout();
  DECLARE_MESSAGE_MAP()
  virtual int ExitInstance();
  
protected:
//Member varibles
  ULONG_PTR m_GDIPlusToken;
};

extern COSMCtrlAppApp theApp;