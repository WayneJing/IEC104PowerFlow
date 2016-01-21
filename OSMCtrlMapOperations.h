/*
Module : OSMCtrlMapOperations.H
Purpose: Defines the interface for the various COSMCtrl map operations classes
Created: PJN / 10-04-2011

Copyright (c) 2011 - 2014 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


///////////////////////// Macros / Includes ///////////////////////////////////

#pragma once

#ifndef __OSMCTRLMAPOPERATIONS_H__
#define __OSMCTRLMAPOPERATIONS_H__

#ifndef __AFXMT_H__
#pragma message("To avoid this message, please put afxmt.h in your pre compiled header (normally stdafx.h)")
#include <afxmt.h>
#endif

#ifndef _WINHTTPX_
#pragma message("To avoid this message, please put WinHttp.h in your pre compiled header (normally stdafx.h)")
#include <WinHttp.h>
#endif

#ifndef _VECTOR_
#pragma message("To avoid this message, please put vector in your pre compiled header (normally stdafx.h)")
#include <vector>
#endif

#include "resource.h"


////////////////////////////////// Classes ////////////////////////////////////


//A simple class used for status operations in "COSMCtrlMapOperationsDlg"
class COSMCtrlMapOperationsDlgEvent
{
public:
//Constructors / Destructors
  COSMCtrlMapOperationsDlgEvent() : m_Event(Undefined),
                                    m_nItemData(0),
                                    m_bSuccess(false)
  {
  }
  COSMCtrlMapOperationsDlgEvent(const COSMCtrlMapOperationsDlgEvent& dlgEvent)
  {
    *this = dlgEvent;
  }

//Methods
  COSMCtrlMapOperationsDlgEvent& operator=(const COSMCtrlMapOperationsDlgEvent& dlgEvent)
  {
    m_Event = dlgEvent.m_Event;
    m_nItemData = dlgEvent.m_nItemData;
    m_sString = dlgEvent.m_sString;
    m_bSuccess = dlgEvent.m_bSuccess;
  
    return *this;
  }

//Enums
  enum Event
  {
    Undefined = -1,
    SimpleStringStatus,
    ThreadCompleted
  };
  
//Member variables:
  Event   m_Event;
  INT_PTR m_nItemData;
  CString m_sString;
  bool    m_bSuccess;
};


//A simple class used to represent a tile in "COSMCtrlMapOperationsDlg"
class COSMCtrlMapOperationsDlgTile
{
public:
//Constructors / Destructors
  COSMCtrlMapOperationsDlgTile() : m_nTileX(-1),
                                   m_nTileY(-1),
                                   m_nZoom(-1)
  {
  }
  COSMCtrlMapOperationsDlgTile(const COSMCtrlMapOperationsDlgTile& tile)
  {
    *this = tile;
  }

//Methods
  COSMCtrlMapOperationsDlgTile& operator=(const COSMCtrlMapOperationsDlgTile& tile)
  {
    m_nTileX = tile.m_nTileX;
    m_nTileY = tile.m_nTileY;
    m_nZoom = tile.m_nZoom;
  
    return *this;
  }

//Member variables:
  int m_nTileX;
  int m_nTileY;
  int m_nZoom;
};


//forward declaration
class COSMCtrl;


//The dialog which allows you to perform map operations
class COSMCtrlMapOperationsDlg : public CDialog
{
public:
//Constructors / Destructors
  COSMCtrlMapOperationsDlg(CWnd* pParent = NULL);
  virtual ~COSMCtrlMapOperationsDlg();

//Member variables
  enum { IDD = IDD_OSMCTRL_OPERATIONS };
  CButton m_ctrlStartOperation;
  CButton m_ctrlStopOperation;
  CComboBox m_ctrlOperation;
  CSliderCtrl m_ctrlZoomLevel;
  CStatic m_ctrlStatusText;
  int m_nOperation;
  CProgressCtrl m_ctrlProgress;

protected:
//Enums
  enum Operation
  {
    CacheTilesOnlyThoseNotDownloaded,
    CacheTilesIncludingThoseDownloaded, 
    ForceMapnikRerender,
    DeleteCachedTiles
  };

//Member variables
  COSMCtrl*                                  m_pOSMCtrl;             //The pointer to the map instance
  CWinThread*                                m_pWorkerThread;        //The pointer for the worker thread
  CEvent                                     m_WorkerTerminateEvent; //Event using to terminate the thread
  HINTERNET                                  m_hSession;             //The Wininet session handle we are using for downloads
  std::vector<COSMCtrlMapOperationsDlgEvent> m_PendingEvents;        //The pending events coming from the worker thread
  CCriticalSection                           m_csPendingEvents;      //critical section used to serialize access to "m_PendingEvents"
  std::vector<COSMCtrlMapOperationsDlgTile>  m_Tiles;                //The tiles we will be working on
  static UINT                                sm_nMessageAddEvent;    //The ID of the message posted between the worker thread and the GUI thread

//Methods
  virtual void DoDataExchange(CDataExchange* pDX);
  virtual BOOL OnInitDialog();
  void UpdateZoom(int nZoomLevel);
  BOOL CreateWorkerThread();
  void DestroyWorkerThread();
  static UINT _WorkerThread(LPVOID pParam);
  UINT WorkerThread();  
  void AddEvent(COSMCtrlMapOperationsDlgEvent& dlgEvent);
  BOOL DeleteCachedTilesHelper();
  BOOL DownloadTiles(BOOL bSkipIfTileAlreadyExists);
  BOOL ForceMapnikRerenderHelper();

  afx_msg void OnBnClickedStartOperation();
  afx_msg void OnBnClickedStopOperation();
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnDestroy();
  afx_msg LRESULT OnAddEvent(WPARAM wParam, LPARAM lParam);

  DECLARE_MESSAGE_MAP()
  
  friend class COSMCtrl;
};


#endif //__OSMCTRLMAPOPERATIONS_H__
