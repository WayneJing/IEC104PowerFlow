#pragma once
//
#define WM_SHOWIECDATA WM_USER + 413


#include "OSMCtrlAppView.h"
#include "IPView.h"
#include "PowerDataView.h"
#include "IEC104Extention.h"
#include "IECShowView.h"
#include <vector>


class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
  CMainFrame();
  DECLARE_DYNCREATE(CMainFrame)

public:
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

// Implementation
public:
  virtual ~CMainFrame();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

public:
	CIPView* pIPView;
	CPowerDataView* pPowerDView;
	COSMCtrlAppView* pOSMVIew;
	CIECShowView* pIECSView;
	iec104ex_class ie;

protected:  // control bar embedded members
  CStatusBar  m_wndStatusBar;
  CToolBar    m_wndToolBar;
  CSplitterWnd m_splitter;
  CSplitterWnd m_splitter1;
  CSplitterWnd m_splitter2;
// Generated message map functions
protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnUpdatePosition(CCmdUI* pCmdUI);
  afx_msg void OnUpdateLength(CCmdUI* pCmdUI);
  DECLARE_MESSAGE_MAP()
  afx_msg LRESULT OnInfonotify(WPARAM wParam, LPARAM lParam);
 // void CMainFrame::ReceiveIEC(UINT_PTR nIDEvent);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	std::vector<CString> v;
	std::vector<std::vector<CString>>v_powerflow;
	int n_pq = 0;
	int n_station = 0;
};


