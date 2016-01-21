/*
Module : OSMCtrlMapOperations.cpp
Purpose: Implementation for the various COSMCtrl map operations classes
Created: PJN / 10-04-2011
History: PJN / None
                                                    
Copyright (c) 2011 - 2014 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////////  Includes  //////////////////////////////////

#include "stdafx.h"
#include "OSMCtrlMapOperations.h"
#include "OSMCtrl.h"


///////////////////////////////// Implementation //////////////////////////////

UINT COSMCtrlMapOperationsDlg::sm_nMessageAddEvent = RegisterWindowMessage(_T("COSMCtrlMapOperations_AddEvent"));


BEGIN_MESSAGE_MAP(COSMCtrlMapOperationsDlg, CDialog)
  ON_BN_CLICKED(ID_START_OPERATION, &COSMCtrlMapOperationsDlg::OnBnClickedStartOperation)
  ON_BN_CLICKED(ID_STOP_OPERATION, &COSMCtrlMapOperationsDlg::OnBnClickedStopOperation)
  ON_WM_HSCROLL()
  ON_WM_DESTROY()
  ON_REGISTERED_MESSAGE(sm_nMessageAddEvent, &COSMCtrlMapOperationsDlg::OnAddEvent)
END_MESSAGE_MAP()


COSMCtrlMapOperationsDlg::COSMCtrlMapOperationsDlg(CWnd* pParent): CDialog(COSMCtrlMapOperationsDlg::IDD, pParent), 
                                                                   m_nOperation(CacheTilesOnlyThoseNotDownloaded),
                                                                   m_pWorkerThread(NULL)
{
}

COSMCtrlMapOperationsDlg::~COSMCtrlMapOperationsDlg()
{
}

void COSMCtrlMapOperationsDlg::DoDataExchange(CDataExchange* pDX)
{
  //Let the parent class do its thing
  CDialog::DoDataExchange(pDX);

  DDX_Control(pDX, ID_START_OPERATION, m_ctrlStartOperation);
  DDX_Control(pDX, ID_STOP_OPERATION, m_ctrlStopOperation);
  DDX_Control(pDX, IDC_OPERATION, m_ctrlOperation);
  DDX_Control(pDX, IDC_ZOOM_LEVEL, m_ctrlZoomLevel);
  DDX_CBIndex(pDX, IDC_OPERATION, m_nOperation);
  DDX_Control(pDX, IDC_STATUS_TEXT, m_ctrlStatusText);
  DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
}

void COSMCtrlMapOperationsDlg::OnBnClickedStartOperation()
{
  if (UpdateData(TRUE))
  {
    //First thing to do is disable this button and enable the stop operation button
    m_ctrlStartOperation.EnableWindow(FALSE);
    m_ctrlStopOperation.EnableWindow(TRUE);
    
    //Set focus to the Stop button
    m_ctrlStopOperation.SetFocus();

    //Reset the progress control
    m_ctrlProgress.SetPos(0);
    
    //Disable the zoom level control
    m_ctrlZoomLevel.EnableWindow(FALSE);

    //Empty out the array
    m_PendingEvents.clear();

    //Create the worker thread
    CreateWorkerThread();
  }
}

void COSMCtrlMapOperationsDlg::OnBnClickedStopOperation()
{
  //First thing to do is disable this button and enable the start operation button
  m_ctrlStopOperation.EnableWindow(FALSE);
  m_ctrlStartOperation.EnableWindow(TRUE);
  
  //Set focus back to the Start button
  m_ctrlStartOperation.SetFocus();
  
  //Shutdown the worker thread
  DestroyWorkerThread();  
}

BOOL COSMCtrlMapOperationsDlg::OnInitDialog()
{
  //Let the base class do its thing
  CDialog::OnInitDialog();
  
  //Validate our parameters
  ASSERT(m_pOSMCtrl != NULL);
  
  //Setup the various controls
  int nZoom = static_cast<int>(m_pOSMCtrl->GetZoom());
  m_ctrlZoomLevel.SetRange(nZoom, min(COSMCtrl::OSMMaxZoom, nZoom+5), TRUE); //Only allow iteration up to 5 levels of zoom
  m_ctrlZoomLevel.EnableWindow(nZoom != COSMCtrl::OSMMaxZoom);
  m_ctrlStopOperation.EnableWindow(FALSE);  
  UpdateZoom(nZoom);

  return TRUE;
}

void COSMCtrlMapOperationsDlg::UpdateZoom(int nZoomLevel)
{
  //Validate our parameters
  AFXASSUME(m_pOSMCtrl != NULL);
  
  //Display a wait cursor while we do the maths to work out how many tiles we will be working on
  CWaitCursor waitCursor;

  //Reset the tiles array
  m_Tiles.reserve(100000);
  
  //Work out how many tiles this operation will affect
  COSMCtrlPosition topLeft;
  COSMCtrlPosition bottomRight;
  int nZoom = static_cast<int>(m_pOSMCtrl->GetZoom());
  if (m_pOSMCtrl->m_SelectionPolygon.GetBoundingRect(topLeft, bottomRight))
  {
    //Iterate across all the zoom levels we will be operating on
    for (int i=nZoom; i<=nZoomLevel; i++)
    { 
      //Work our the start and end X and Y values for the tiles at this zoom level 
      double fStartX = COSMCtrlHelper::Longitude2TileX(topLeft.m_fLongitude, i);
      int nStartX = static_cast<int>(fStartX);
      double fStartY = COSMCtrlHelper::Latitude2TileY(topLeft.m_fLatitude, i);
      int nStartY = static_cast<int>(fStartY);

      double fEndX = COSMCtrlHelper::Longitude2TileX(bottomRight.m_fLongitude, i);
      int nEndX = static_cast<int>(fEndX);
      double fEndY = COSMCtrlHelper::Latitude2TileY(bottomRight.m_fLatitude, i);
      int nEndY = static_cast<int>(fEndY);
    
      //Add all the tiles for this zoom level to the array
      for (int j=nStartX; j<=nEndX; j++)
      {
        for (int k=nStartY; k<=nEndY; k++)
        {
          COSMCtrlMapOperationsDlgTile tile;
          tile.m_nTileX = j;
          tile.m_nTileY = k;
          tile.m_nZoom = i;
          m_Tiles.push_back(tile);
        }
      }
    }
  }
  
  //Free up any unused memory in the array
  m_Tiles.shrink_to_fit();

  //Set the range on the progress control  
  int nTiles = static_cast<int>(m_Tiles.size());
  m_ctrlProgress.SetPos(0);
  m_ctrlProgress.SetRange32(0, nTiles);

  //Finally update the text on the UI
  CString sStatus;
  CString sZoom1;
  sZoom1.Format(_T("%d"), nZoom);
  CString sTiles;
  sTiles.Format(_T("%d"), nTiles);
  if (nZoom == nZoomLevel)
    AfxFormatString2(sStatus, IDS_OSMCTRL_MAP_OPERATIONS_UPDATE_ZOOM2, sZoom1, sTiles);
  else
  {
    CString sZoom2;
    sZoom2.Format(_T("%d"), nZoomLevel);
    
    LPCTSTR szStrings[3];
    szStrings[0] = sZoom1;
    szStrings[1] = sZoom2;
    szStrings[2] = sTiles;
    AfxFormatStrings(sStatus, IDS_OSMCTRL_MAP_OPERATIONS_UPDATE_ZOOM3, szStrings, 3);
  }
  m_ctrlStatusText.SetWindowText(sStatus);
}

void COSMCtrlMapOperationsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
  //Validate our parameters
  ASSERT(m_pOSMCtrl != NULL);

  //Let the base class do its thing
  CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
  
  //Handle the notification if it is from our control
  AFXASSUME(pScrollBar != NULL);
  if (pScrollBar->GetSafeHwnd() == m_ctrlZoomLevel.GetSafeHwnd())
  {
    switch (nSBCode)
    {
      case TB_BOTTOM:
      {
        UpdateZoom(min(COSMCtrl::OSMMaxZoom, static_cast<int>(m_pOSMCtrl->GetZoom()) + 5));        
        break;
      }
      case TB_TOP:
      {
        UpdateZoom(static_cast<int>(m_pOSMCtrl->GetZoom()));
        break;
      }
      case TB_LINEUP:
      case TB_PAGEUP:
      {
        UpdateZoom(m_ctrlZoomLevel.GetPos());
        break;
      }
      case TB_LINEDOWN:
      case TB_PAGEDOWN:
      {
        UpdateZoom(m_ctrlZoomLevel.GetPos());
        break;
      }
      case TB_THUMBTRACK: //deliberate fallthrough
      case TB_THUMBPOSITION:
      {
        UpdateZoom(nPos);
        break;
      }
      default:
      {
        //Nothing to do
        break;
      }
    }
  }
}

BOOL COSMCtrlMapOperationsDlg::CreateWorkerThread()
{
  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;

  //Destroy the worker thread if currently running
  DestroyWorkerThread();
  
  //Spin off the background thread
  ASSERT(m_pWorkerThread == NULL);
  m_WorkerTerminateEvent.ResetEvent();
  m_pWorkerThread = AfxBeginThread(_WorkerThread, this, THREAD_PRIORITY_IDLE, 0, CREATE_SUSPENDED);
  if (m_pWorkerThread)
  {
    //We're in charge of deletion of the thread
    m_pWorkerThread->m_bAutoDelete = FALSE;

    //Resume the thread now that everything is ready to go
    m_pWorkerThread->ResumeThread();
    
    bSuccess = TRUE;
  }
  
  return bSuccess;
}

void COSMCtrlMapOperationsDlg::DestroyWorkerThread()
{
  //Ensure the download thread is shutdown
  if (m_pWorkerThread)
  {
  #ifdef _DEBUG  
    //If we are running a debug version of the code, display a wait cursor to visually indicate
    //how long the destruction of the worker thread takes
    CWaitCursor wait;
  #endif

    //Signal the worker thread to exit and wait for it to return
    m_WorkerTerminateEvent.SetEvent();
  
    //Close the session handle which is being used by the worker thread.
    WinHttpCloseHandle(m_hSession);
 
    //Wait for the thread to return
    WaitForSingleObject(m_pWorkerThread->m_hThread, INFINITE);
    delete m_pWorkerThread;
    m_pWorkerThread = NULL;
  }
}

void COSMCtrlMapOperationsDlg::OnDestroy()
{
  //kill the worker thread if it is currently running
  DestroyWorkerThread();

  //Let the base class do its thing
  CDialog::OnDestroy();
}

UINT COSMCtrlMapOperationsDlg::_WorkerThread(LPVOID pParam)
{
  //Validate our parameters
  ASSERT(pParam != NULL);
  COSMCtrlMapOperationsDlg* pThis = static_cast<COSMCtrlMapOperationsDlg*>(pParam);
  AFXASSUME(pThis != NULL);

  //Convert from the SDK world to the C++ world
  return pThis->WorkerThread();
}

BOOL COSMCtrlMapOperationsDlg::DeleteCachedTilesHelper()
{
  //What will be the return value from this function (assume the best)
  BOOL bSuccess = TRUE;

  //Accumulate how many tiles we have deleted and not deleted
  int nTilesDeleted = 0;
  int nTilesNotDeleted = 0;

  //Work thro all the tiles
  CString sCacheDirectory(m_pOSMCtrl->GetCacheDirectory());
  for (std::vector<COSMCtrlMapOperationsDlgTile>::size_type i=0; (i<m_Tiles.size()) && bSuccess; i++)
  {
    //Pull out the tile we are working on
    const COSMCtrlMapOperationsDlgTile& tile = m_Tiles[i];

    //Do the deletion of the tile        
    CString sTile(m_pOSMCtrl->GetTileCachePath(sCacheDirectory, tile.m_nZoom, tile.m_nTileX, tile.m_nTileY, FALSE));
    COSMCtrlMapOperationsDlgEvent dlgEvent;
    if (GetFileAttributes(sTile) != INVALID_FILE_ATTRIBUTES) //Don't bother doing anything if the tile does not already exist
    {
      if (DeleteFile(sTile))
      {
        //Update the stats
        ++nTilesDeleted;
        
        dlgEvent.m_bSuccess = true;
      }
      else
      {
        //Update the stats
        ++nTilesNotDeleted;
        
        dlgEvent.m_bSuccess = false;
      }
    }

    //Update the UI          
    dlgEvent.m_Event = COSMCtrlMapOperationsDlgEvent::SimpleStringStatus;
    dlgEvent.m_sString = sTile;
    dlgEvent.m_nItemData = i + 1;
    AddEvent(dlgEvent);
    
    //Check if we have been cancelled before we loop around
    bSuccess = (WaitForSingleObject(m_WorkerTerminateEvent, 0) == WAIT_TIMEOUT);
  }
  
  //Finally add a event about how many items have been deleted
  COSMCtrlMapOperationsDlgEvent dlgEvent;
  dlgEvent.m_Event = COSMCtrlMapOperationsDlgEvent::SimpleStringStatus;
  CString sTilesDeleted;
  sTilesDeleted.Format(_T("%d"), nTilesDeleted);
  CString sTilesNotDeleted;
  sTilesNotDeleted.Format(_T("%d"), nTilesNotDeleted);
  AfxFormatString2(dlgEvent.m_sString, IDS_OSMCTRL_DELETE_FILES_STATS, sTilesDeleted, sTilesNotDeleted);
  AddEvent(dlgEvent);
  
  return bSuccess;
}

BOOL COSMCtrlMapOperationsDlg::DownloadTiles(BOOL bSkipIfTileAlreadyExists)
{
  //Validate our parameters
  AFXASSUME(m_pOSMCtrl != NULL);

  //Pull out the tile provider we will be using
  IOSMCtrlTileProvider* pTileProvider = m_pOSMCtrl->GetTileProvider();
  AFXASSUME(pTileProvider != NULL);

  //Next get the server to connect to
  CStringW sServer(pTileProvider->GetDownloadServer());

  //Accumulate how many tiles we have deleted and not deleted
  int nTilesDownloaded = 0;
  int nTilesNotDownloaded = 0;

  //Next create the WinHTTP session object
  CWinHTTPSession session;
  HRESULT hr = m_pOSMCtrl->CreateSession(session, 0);
  if (SUCCEEDED(hr))
  {
    //Now create the connection object from the session object
    CWinHTTPConnection connection;
    hr = connection.Initialize(session, sServer, pTileProvider->GetDownloadPort());
    if (SUCCEEDED(hr))
    {
      //Iterate across the array of tiles to download
      BOOL bSuccess = TRUE;
      for (std::vector<COSMCtrlMapOperationsDlgTile>::size_type i=0; i<m_Tiles.size() && bSuccess; i++)
      {
        //Pull out the next tile to download
        const COSMCtrlMapOperationsDlgTile& tile = m_Tiles[i];

        //Create the sub directories if we can
        CString sCacheDirectory(m_pOSMCtrl->GetCacheDirectory());
        CString sSubDirectory;
        sSubDirectory.Format(_T("%s\\%d"), sCacheDirectory.operator LPCTSTR(), tile.m_nZoom);
        CreateDirectory(sSubDirectory, NULL);
        sSubDirectory.Format(_T("%s\\%d\\%d"), sCacheDirectory.operator LPCTSTR(), tile.m_nZoom, tile.m_nTileX);
        CreateDirectory(sSubDirectory, NULL);

        //Form the path to the tile we will be downloading to and determine if we should do the download
        CString sFile(COSMCtrl::GetTileCachePath(sCacheDirectory, tile.m_nZoom, tile.m_nTileX, tile.m_nTileY, FALSE));
        BOOL bDownload = TRUE;
        if (bSkipIfTileAlreadyExists)
          bDownload = (GetFileAttributes(sFile) == INVALID_FILE_ATTRIBUTES);

        //Now download the specific tile to the cache if required
        COSMCtrlMapOperationsDlgEvent dlgEvent;
        dlgEvent.m_bSuccess = false;
        if (bDownload)
        {
          //We will accept any mime type
          LPCWSTR pwszAcceptTypes[2];
          pwszAcceptTypes[0] = L"*/*";
          pwszAcceptTypes[1] = NULL;
          CSyncWinHTTPDownloader winHttpRequest;
          winHttpRequest.m_sFileToDownloadInto = sFile;
          CString sObject(pTileProvider->GetDownloadObject(tile.m_nZoom, tile.m_nTileX, tile.m_nTileY));
          hr = winHttpRequest.Initialize(connection, CStringW(sObject), NULL, NULL, NULL, pwszAcceptTypes, WINHTTP_FLAG_REFRESH);
          if (FAILED(hr))
          {
            //report the error
            TRACE(_T("COSMCtrlMapOperationsDlg::DownloadTiles, Failed to create request for tile \"%s\", Error:%08X\n"), sFile.operator LPCTSTR(), hr);
            
            //Ensure any remants of a bad download file are nuked
            DeleteFile(sFile);
            
            //Update the stats
            ++nTilesNotDownloaded;
          }
          else
          {
            hr = winHttpRequest.SendRequestSync();
            if (FAILED(hr))
            {
              //report the error
              TRACE(_T("COSMCtrlMapOperationsDlg::DownloadTiles, Failed to send request for tile \"%s\", Error:%08X\n"), sFile.operator LPCTSTR(), hr);
            
              //Ensure any remants of a bad download file are nuked
              DeleteFile(sFile);
            
              //Update the stats
              ++nTilesNotDownloaded;
            }
            else
            {
              //Update the stats
              ++nTilesDownloaded;
              dlgEvent.m_bSuccess = true;
            }
          }
        }
        else
        {
          //Update the stats
          ++nTilesNotDownloaded;
        }

        //Update the UI          
        dlgEvent.m_Event = COSMCtrlMapOperationsDlgEvent::SimpleStringStatus;
        dlgEvent.m_sString = sFile;
        dlgEvent.m_nItemData = i + 1;
        AddEvent(dlgEvent);

        //Check if we have been cancelled before we loop around
        bSuccess = (WaitForSingleObject(m_WorkerTerminateEvent, 0) == WAIT_TIMEOUT);
      }
    }
  }

  //Finally add a event about how many items have been downloaded
  COSMCtrlMapOperationsDlgEvent dlgEvent;
  dlgEvent.m_Event = COSMCtrlMapOperationsDlgEvent::SimpleStringStatus;
  CString sTilesDownloaded;
  sTilesDownloaded.Format(_T("%d"), nTilesDownloaded);
  CString sTilesNotDownloaded;
  sTilesNotDownloaded.Format(_T("%d"), nTilesNotDownloaded);
  AfxFormatString2(dlgEvent.m_sString, IDS_OSMCTRL_DOWNLOAD_TILES_STATS, sTilesDownloaded, sTilesNotDownloaded);
  AddEvent(dlgEvent);

  return TRUE;
}

BOOL COSMCtrlMapOperationsDlg::ForceMapnikRerenderHelper()
{
  //Validate our parameters
  AFXASSUME(m_pOSMCtrl != NULL);

  //Next get the server to connect to
  COSMCtrlMapnikTileProvider MapnikTileProvider;
  CStringW sServer(MapnikTileProvider.GetDownloadServer());

  //Accumulate how many tiles we have request to rerender and which ones indicated a failure to rerender
  int nTilesRerendered = 0;
  int nTilesNotRerendered = 0;

  //Next create the WinHTTP session object
  CWinHTTPSession session;
  HRESULT hr = m_pOSMCtrl->CreateSession(session, 0);
  if (SUCCEEDED(hr))
  {
    //Now create the connection object from the session object
    CWinHTTPConnection connection;
    hr = connection.Initialize(session, sServer, MapnikTileProvider.GetDownloadPort());
    if (SUCCEEDED(hr))
    {
      //Iterate across the array of tiles to download
      BOOL bSuccess = TRUE;
      for (std::vector<COSMCtrlMapOperationsDlgTile>::size_type i=0; i<m_Tiles.size() && bSuccess; i++)
      {
        //Pull out the next tile to download
        const COSMCtrlMapOperationsDlgTile& tile = m_Tiles[i];

        //Now issue the request to rerender
        COSMCtrlMapOperationsDlgEvent dlgEvent;
        dlgEvent.m_bSuccess = false;
        LPCWSTR pwszAcceptTypes[2];
        pwszAcceptTypes[0] = L"*/*";
        pwszAcceptTypes[1] = NULL;
        CSyncWinHTTPDownloader winHttpRequest;
        //winHttpRequest.m_sFileToDownloadInto = sFile;
        CString sObject(MapnikTileProvider.GetDownloadObject(tile.m_nZoom, tile.m_nTileX, tile.m_nTileY) + _T("/dirty"));
        hr = winHttpRequest.Initialize(connection, CStringW(sObject), NULL, NULL, NULL, pwszAcceptTypes, WINHTTP_FLAG_REFRESH);
        if (FAILED(hr))
        {
          //report the error
          TRACE(_T("COSMCtrlMapOperationsDlg::ForceMapnikRerenderHelper, Failed to create request for object \"%s\", Error:%08X\n"), sObject.operator LPCTSTR(), hr);
            
          //Update the stats
          ++nTilesNotRerendered;
        }
        else
        {
          hr = winHttpRequest.SendRequestSync();
          if (FAILED(hr))
          {
            //report the error
            TRACE(_T("COSMCtrlMapOperationsDlg::ForceMapnikRerenderHelper, Failed to send request for object \"%s\", Error:%08X\n"), sObject.operator LPCTSTR(), hr);
            
            //Update the stats
            ++nTilesNotRerendered;
          }
          else
          {
            CStringA sResponse;
            sResponse.Append(reinterpret_cast<LPCSTR>(winHttpRequest.m_Response.GetData()), static_cast<int>(winHttpRequest.m_Response.GetSize()));
            if (sResponse.Find("Tile submitted for rendering") != -1)
            {
              //Update the stats
              ++nTilesRerendered;
              dlgEvent.m_bSuccess = true;
            }
            else
            {
              //Update the stats
              ++nTilesNotRerendered;
            }  
          }
        }

        //Update the UI          
        dlgEvent.m_Event = COSMCtrlMapOperationsDlgEvent::SimpleStringStatus;
        dlgEvent.m_sString = sObject;
        dlgEvent.m_nItemData = i + 1;
        AddEvent(dlgEvent);

        //Check if we have been cancelled before we loop around
        bSuccess = (WaitForSingleObject(m_WorkerTerminateEvent, 0) == WAIT_TIMEOUT);
      }
    }
  }

  //Finally add a event about how many items have been downloaded
  COSMCtrlMapOperationsDlgEvent dlgEvent;
  dlgEvent.m_Event = COSMCtrlMapOperationsDlgEvent::SimpleStringStatus;
  CString sTilesRerendered;
  sTilesRerendered.Format(_T("%d"), nTilesRerendered);
  CString sTilesNotRerendered;
  sTilesNotRerendered.Format(_T("%d"), nTilesNotRerendered);
  AfxFormatString2(dlgEvent.m_sString, IDS_OSMCTRL_RERENDER_TILES_STATS, sTilesRerendered, sTilesNotRerendered);
  AddEvent(dlgEvent);

  return TRUE;
}

UINT COSMCtrlMapOperationsDlg::WorkerThread()
{
  //Validate our parameters
  AFXASSUME(m_pOSMCtrl != NULL);

  //What will be the return value from this function (assume the worst)
  BOOL bSuccess = FALSE;
  
  //Switch on the operation type we have been asked to work on
  switch (m_nOperation)
  {
    case DeleteCachedTiles:
    {
      bSuccess = DeleteCachedTilesHelper();
      break;
    }
    case CacheTilesIncludingThoseDownloaded:
    {
      bSuccess = DownloadTiles(FALSE);
      break;
    }
    case CacheTilesOnlyThoseNotDownloaded:
    {
      bSuccess = DownloadTiles(TRUE);
      break;
    }
    case ForceMapnikRerender:
    {
      bSuccess = ForceMapnikRerenderHelper();
      break;
    }
    default:
    {
      ASSERT(FALSE);
      break;
    }
  }

  //Inform the UI that the work is completed
  COSMCtrlMapOperationsDlgEvent dlgEvent;
  dlgEvent.m_Event = COSMCtrlMapOperationsDlgEvent::ThreadCompleted;
  dlgEvent.m_bSuccess = bSuccess ? true : false;
  AddEvent(dlgEvent);

  return 0;
}

void COSMCtrlMapOperationsDlg::AddEvent(COSMCtrlMapOperationsDlgEvent& dlgEvent)
{
  //Serialize access to the array
  CSingleLock sl(&m_csPendingEvents, TRUE);
  m_PendingEvents.push_back(dlgEvent);
  
  //Inform the GUI thread that there has been a new event added to the array
  PostMessage(sm_nMessageAddEvent);
}

LRESULT COSMCtrlMapOperationsDlg::OnAddEvent(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
  //Validate our parameters
  AFXASSUME(m_pOSMCtrl != NULL);

  //Serialize access to the array
  CSingleLock sl(&m_csPendingEvents, TRUE);

  //Pull as many items of the queue as we can
  while (m_PendingEvents.size())
  {
    COSMCtrlMapOperationsDlgEvent dlgEvent(m_PendingEvents[0]);
    m_PendingEvents.erase(m_PendingEvents.cbegin());
    switch (dlgEvent.m_Event)
    {
      case COSMCtrlMapOperationsDlgEvent::ThreadCompleted:
      {
        //Disable the stop button and reenable the start button
        m_ctrlStopOperation.EnableWindow(FALSE);
        m_ctrlStartOperation.EnableWindow(TRUE);
        
        //Reset the progress control if necessary
        if (!dlgEvent.m_bSuccess)
          m_ctrlProgress.SetPos(0);
          
        //Also reset the zoom level control
        m_ctrlZoomLevel.EnableWindow(TRUE);
        
        //Set focus back to the Start button
        m_ctrlStartOperation.SetFocus();

        //Force a refresh of map as some of the map operations may have invalidate the tiles
        m_pOSMCtrl->Refresh();
        
        break;
      }
      case COSMCtrlMapOperationsDlgEvent::SimpleStringStatus:
      {
        //Simple update the status static
        m_ctrlStatusText.SetWindowText(dlgEvent.m_sString);
        
        //Update the progress control if necessary
        if (dlgEvent.m_nItemData)
          m_ctrlProgress.SetPos(static_cast<int>(dlgEvent.m_nItemData));
        break;
      }
      default:
      {
        ASSERT(FALSE);
        break;
      }
    }
  }

  return 0L;
}
