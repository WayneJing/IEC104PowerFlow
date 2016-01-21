#pragma once

#include "OSMCtrl.h"
#include "GPSCom2Client.h" //If you get a compilation error about this missing header file, then you need to download my GPSCom2 classes from http://www.naughter.com/gpscom2.html
#include "OSMCtrlGPX.h"
#include "OSMCtrlTileProviders.h"
#include "MFCSensor.h" //If you get a compilation error about this missing header file, then you need to download my MFCSensor code from http://www.naughter.com/mfcsensor.html
#include "OSMCtrlAppDoc.h"
#include <iostream> 
#include <string> 
#include <fstream> 
#include "OSMMyStruct.h"
#include <map>
#include <vector>
#include <stdio.h>
#include <sstream>


class COSMCtrlAppDoc;

class CMySensorEvents : public SensorAPI::CSensorEvents
{
public:
  STDMETHOD(OnDataUpdated)(__RPC__in_opt ISensor* pSensor, __RPC__in_opt ISensorDataReport* pNewData);
  STDMETHOD(OnLeave)(__RPC__in_opt REFSENSOR_ID sensorID);
};

class COSMCtrlAppView : public CView, GPSCom2::CClient, IOSMCtrlHandler
{
protected: // create from serialization only
  COSMCtrlAppView();
  DECLARE_DYNCREATE(COSMCtrlAppView)

// Attributes
public:
  COSMCtrlAppDoc* GetDocument() const;
  COSMCtrl& GetCtrl() { return m_ctrlOSM; };
  int FindBusNumByI(double bus_i, std::vector<StationStruct> m_Stations); //Find the father number  of the bus_i(0,1,2...)
  void UpdateStations(int timeNumber); // Fetch stations in doc and show on map

// Operations
public:

// Overrides
public:
  virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnDraw(CDC* pDC);  // overridden to draw this view
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
  virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
  virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
  virtual ~COSMCtrlAppView();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  STDMETHOD(OnDataUpdated)(__RPC__in_opt ISensor* /*pSensor*/, __RPC__in_opt ISensorDataReport* pNewData);
  STDMETHOD(OnLeave)(__RPC__in_opt REFSENSOR_ID sensorID);

protected:
//Enums 
  enum MapPointerMode
  {
    Undefined,
    RefreshTile,
    ViewTileProperties,
    AddressLookup
  };


//Methods
  void OpenGPS(BOOL bReportError);
  virtual void OnRMC(const CString& sSentence, const GPSCom2::CRMCSentence& sentence);
  virtual BOOL SaveTrackLog(const CString& sFileName);
  virtual void LoadTrackLog();
  virtual BOOL GetTrackLogFilename(CString& sFileName);
  virtual BOOL OnOSMCtrlLButtonDown(UINT nFlags, CPoint point);
  void DoAddressLookup(CPoint point);
  void DoRefreshTile(CPoint point);
  void DoTileProperties(CPoint point);
#ifdef COSMCTRL_NOD2D
  HRESULT LoadResourceImage(LPCTSTR pName, LPCTSTR pType, HMODULE hInst, Gdiplus::Image*& pImage);
#endif
  void ReportCOMError(HRESULT hr, const CString& sTemplate);

  COSMCtrlIcon                           m_dummyDefaultIcon;
  COSMCtrl                               m_ctrlOSM;
  int                                    m_nGPSPort;
  int                                    m_nGPSBaudRate;
  GPSCom2::GPS_PARITY                    m_GPSParity;
  int                                    m_nGPSDataBits;
  GPSCom2::GPS_STOP_BITS                 m_GPSStopBits;
  BOOL                                   m_bGPSCenter;
  BOOL                                   m_bChangeBearingOfMap;
  BOOL                                   m_bGPSFix;
  CGPX                                   m_GPX;
  BOOL                                   m_bSensor;
  BOOL                                   m_bGPXTrackLog;
  CString                                m_sLastTrackLogFilename;
  std::vector<COSMCtrlIcon*>::size_type  m_nDefaultMarkerIconIndex;
  MapPointerMode                         m_MapPointerMode;
  SensorAPI::CSensor<CMySensorEvents>    m_Sensor;
  SENSOR_ID                              m_SensorID;
  COSMCtrlMapnikTileProvider             m_MapnikTileProvider;
  COSMCtrlCyclemapTileProvider           m_CyclemapTileProvider;
  COSMCtrlMapquestOSMTileProvider        m_MapquestOSMTileProvider;
  COSMCtrlMapquestOpenAerialTileProvider m_MapquestOpenAerialTileProvider;

  DECLARE_MESSAGE_MAP()
  
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnZoom0();
  afx_msg void OnZoom1();
  afx_msg void OnZoom2();
  afx_msg void OnZoom3();
  afx_msg void OnZoom4();
  afx_msg void OnZoom5();
  afx_msg void OnZoom6();
  afx_msg void OnZoom7();
  afx_msg void OnZoom8();
  afx_msg void OnZoom9();
  afx_msg void OnZoom10();
  afx_msg void OnZoom11();
  afx_msg void OnZoom12();
  afx_msg void OnZoom13();
  afx_msg void OnZoom14();
  afx_msg void OnZoom15();
  afx_msg void OnZoom16();
  afx_msg void OnZoom17();
  afx_msg void OnZoom18();
  afx_msg void OnViewDrawScrollRose();
  afx_msg void OnUpdateViewDrawScrollRose(CCmdUI* pCmdUI);
  afx_msg void OnViewDrawZoomBar();
  afx_msg void OnUpdateViewDrawZoomBar(CCmdUI* pCmdUI);
  afx_msg void OnViewAllowDrag();
  afx_msg void OnUpdateViewAllowDrag(CCmdUI* pCmdUI);
  afx_msg void OnViewAllowKeyboard();
  afx_msg void OnUpdateViewAllowKeyboard(CCmdUI* pCmdUI);
  afx_msg void OnViewAllowMouseZoom();
  afx_msg void OnUpdateViewAllowMouseZoom(CCmdUI* pCmdUI);
  afx_msg void OnViewDrawScaleBar();
  afx_msg void OnUpdateViewDrawScaleBar(CCmdUI* pCmdUI);
  afx_msg void OnUpdateZoom0(CCmdUI* pCmdUI);
  afx_msg void OnUpdateZoom1(CCmdUI* pCmdUI);
  afx_msg void OnUpdateZoom2(CCmdUI* pCmdUI);
  afx_msg void OnUpdateZoom3(CCmdUI* pCmdUI);
  afx_msg void OnUpdateZoom4(CCmdUI* pCmdUI);
  afx_msg void OnUpdateZoom5(CCmdUI* pCmdUI);
  afx_msg void OnUpdateZoom6(CCmdUI* pCmdUI);
  afx_msg void OnUpdateZoom7(CCmdUI* pCmdUI);
  afx_msg void OnUpdateZoom8(CCmdUI* pCmdUI);
  afx_msg void OnUpdateZoom9(CCmdUI* pCmdUI);
  afx_msg void OnUpdateZoom10(CCmdUI* pCmdUI);
  afx_msg void OnUpdateZoom11(CCmdUI* pCmdUI);
  afx_msg void OnUpdateZoom12(CCmdUI* pCmdUI);
  afx_msg void OnUpdateZoom13(CCmdUI* pCmdUI);
  afx_msg void OnUpdateZoom14(CCmdUI* pCmdUI);
  afx_msg void OnUpdateZoom15(CCmdUI* pCmdUI);
  afx_msg void OnUpdateZoom16(CCmdUI* pCmdUI);
  afx_msg void OnUpdateZoom17(CCmdUI* pCmdUI);
  afx_msg void OnUpdateZoom18(CCmdUI* pCmdUI);
  afx_msg void OnSetFocus(CWnd* pOldWnd);
  afx_msg void OnUpdateViewAutoDownloadTiles(CCmdUI* pCmdUI);
  afx_msg void OnViewAutoDownloadTiles();
  afx_msg void OnProvidersMapnik();
  afx_msg void OnUpdateProvidersMapnik(CCmdUI* pCmdUI);
  afx_msg void OnProvidersMapquestOSM();
  afx_msg void OnUpdateProvidersMapquestOSM(CCmdUI* pCmdUI);
  afx_msg void OnProvidersMapquestOpenAerial();
  afx_msg void OnUpdateProvidersMapquestOpenAerial(CCmdUI* pCmdUI);
  afx_msg void OnDestroy();
  afx_msg void OnViewClearCache();
  afx_msg void OnUpdateProvidersCyclemap(CCmdUI*pCmdUI);
  afx_msg void OnProvidersCyclemap();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnViewRefresh();
  afx_msg void OnViewAllowPreviousZoomStretch();
  afx_msg void OnUpdateViewAllowPreviousZoomStretch(CCmdUI* pCmdUI);
  afx_msg void OnViewAllowNextZoomSqueeze();
  afx_msg void OnUpdateViewAllowNextZoomSqueeze(CCmdUI* pCmdUI);
  afx_msg void OnViewDownloadTilesFromCenterOutwards();
  afx_msg void OnUpdateViewDownloadTilesFromCenterOutwards(CCmdUI* pCmdUI);
  afx_msg void OnViewUseTransparencyforZoomBar();
  afx_msg void OnUpdateViewUseTransparencyForZoomBar(CCmdUI* pCmdUI);
  afx_msg void OnViewDrawTileOutlines();
  afx_msg void OnUpdateViewDrawTileOutlines(CCmdUI* pCmdUI);
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void OnViewTileProperties();
  afx_msg void OnViewDrawZoomBarAaSliderControl();
  afx_msg void OnUpdateViewDrawZoomBarAaSliderControl(CCmdUI* pCmdUI);
  afx_msg void OnViewAllowDoubleClickZoom();
  afx_msg void OnUpdateViewAllowDoubleClickZoom(CCmdUI* pCmdUI);
  afx_msg void OnViewRefreshTile();
  afx_msg void OnDrawNormal();
  afx_msg void OnUpdateDrawNormal(CCmdUI* pCmdUI);
  afx_msg void OnDrawRotate();
  afx_msg void OnUpdateDrawRotate(CCmdUI* pCmdUI);
  afx_msg void OnDrawPolygon();
  afx_msg void OnUpdateDrawPolygon(CCmdUI* pCmdUI);
  afx_msg void OnDrawPolyline();
  afx_msg void OnUpdateDrawPolyline(CCmdUI* pCmdUI);
  afx_msg void OnDrawMarker();
  afx_msg void OnUpdateDrawMarker(CCmdUI* pCmdUI);
  afx_msg void OnDrawSelectionRectangle();
  afx_msg void OnUpdateDrawSelectionRectangle(CCmdUI* pCmdUI);
  afx_msg void OnViewMapOperations();
  afx_msg void OnDrawCircle();
  afx_msg void OnUpdateDrawCircle(CCmdUI* pCmdUI);
  afx_msg void OnViewDrawCenterCrosshairs();
  afx_msg void OnUpdateViewDrawCenterCrosshairs(CCmdUI* pCmdUI);
  afx_msg void OnViewGPSSettings();
  afx_msg void OnViewAllowAnimations();
  afx_msg void OnUpdateViewAllowAnimations(CCmdUI* pCmdUI);
  afx_msg void OnFileImportGPXFile();
  afx_msg void OnViewSearch();
  afx_msg void OnViewAddressLookup();
  afx_msg void OnViewGotoCoordinates();

  friend class CMainFrame;
};

#ifndef _DEBUG  // debug version in OSMCtrlAppView.cpp
inline COSMCtrlAppDoc* COSMCtrlAppView::GetDocument() const
   { return reinterpret_cast<COSMCtrlAppDoc*>(m_pDocument); }
#endif

