/*
Module : OSMCtrl.H
Purpose: Defines the interface for an MFC GUI control which implements display of OpenStreetMaps tiles
Created: PJN / 28-11-2009

Copyright (c) 2009 - 2014 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

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

#ifndef __OSMCTRL_H__
#define __OSMCTRL_H__

#include "OSMCtrlIcon.h"
#include "OSMCtrlMarker.h"
#include "OSMCtrlPolyline.h"
#include "OSMCtrlPolygon.h"
#include "OSMCtrlCircle.h"
#include "OSMCtrlMapOperations.h"
#include "OSMCtrlTileProviders.h"
#include "OSMCtrlHandler.h"
#include "OSMCtrlCachedTile.h"

#ifdef COSMCTRL_NOD2D
#ifndef _GDIPLUS_H
#pragma message("To avoid this message, please put gdiplus.h in your pre compiled header (normally stdafx.h)")
#include <gdiplus.h>
#endif
#else
#include <afxrendertarget.h> //If you get a compilation error about this missing header file, then you need to compile with VC 2010 + SP1
#endif

#ifndef COSMCTRL_NOANIMATION
  #include <UIAnimation.h>  //If you get a compilation error about this missing header file, then you need to download and install the Windows 7 SDK
#endif

#include "SortedArray.h" //If you get a compilation error about this missing header file, then you need to download my CSortedArray class from http://www.naughter.com/sortedarray.html
#include "WinHttpWrappers.h" //If you get a compilation error about this missing header file, then you need to download my WinHttpWrappers classes from http://www.naughter.com/winhttpwrappers.html


////////////////////////////////// Classes ////////////////////////////////////

//A simple UDT which contains the tiles to download
class COSMCtrlDownloadTileItem
{
public:
//Constructors / Destructors
  COSMCtrlDownloadTileItem() : m_nTileX(-1),
                               m_nTileY(-1),
                               m_fDistanceFromCenterPosition(0)
  {
  }
#ifdef COSMCTRL_NOD2D
  COSMCtrlDownloadTileItem(int nTileX, int nTileY, const Gdiplus::RectF& rTile, double fDistanceFromCenterPosition, const CString& sFile) : m_nTileX(nTileX),
                                                                                                                                            m_nTileY(nTileY),
                                                                                                                                            m_rTile(rTile),
                                                                                                                                            m_fDistanceFromCenterPosition(fDistanceFromCenterPosition),
                                                                                                                                            m_sFile(sFile)
  {
  }
#else
  COSMCtrlDownloadTileItem(int nTileX, int nTileY, const CD2DRectF& rTile, double fDistanceFromCenterPosition, const CString& sFile) : m_nTileX(nTileX),
                                                                                                                                       m_nTileY(nTileY),
                                                                                                                                       m_rTile(rTile),
                                                                                                                                       m_fDistanceFromCenterPosition(fDistanceFromCenterPosition),
                                                                                                                                       m_sFile(sFile)
  {
  }
#endif
  
//Methods
  COSMCtrlDownloadTileItem& operator=(const COSMCtrlDownloadTileItem& item)
  {
    m_nTileX = item.m_nTileX;
    m_nTileY = item.m_nTileY;
    m_rTile = item.m_rTile;
    m_fDistanceFromCenterPosition = item.m_fDistanceFromCenterPosition;
    m_sFile = item.m_sFile;
    return *this;
  } 

//Member variables
  int            m_nTileX;                      //The X tile value
  int            m_nTileY;                      //The Y tile value
#ifdef COSMCTRL_NOD2D
  Gdiplus::RectF m_rTile;                       //The client coordinates of the tile
#else
  CD2DRectF      m_rTile;                       //The client coordinates of the tile
#endif
  double         m_fDistanceFromCenterPosition; //The distance of the center of this tile from the center position of the map
  CString        m_sFile;                       //The local cached filename
};


//A functor for comparing COSMCtrlDownloadTileItem's based on their distance from the center of the map
class CompareCOSMCtrlDownloadTileItem
{
public:
  int operator()(const COSMCtrlDownloadTileItem& item1, const COSMCtrlDownloadTileItem& item2) const
  {
    if (item1.m_fDistanceFromCenterPosition > item2.m_fDistanceFromCenterPosition)
      return 1;
    else if (item1.m_fDistanceFromCenterPosition < item2.m_fDistanceFromCenterPosition)
      return -1;
    else
      return 0;
  }
};



//The helper class to do async tile downloads via WinHTTP
class COSMCtrlWinHTTPRequest : public CAsyncWinHTTPDownloader
{
public:
//Constructors / Destructors
  COSMCtrlWinHTTPRequest();

//Methods
  void SetOSMCtrl(COSMCtrl* pOSMCtrl);

protected:
//Methods
  virtual HRESULT OnCallbackComplete(_In_ HRESULT hr, _In_ HINTERNET hInternet, _In_ DWORD dwInternetStatus, _In_opt_ LPVOID lpvStatusInformation, _In_ DWORD dwStatusInformationLength);

//Member variables
  COSMCtrl* m_pOSMCtrl;
};


//The main control class
class COSMCtrl : public CStatic
{
public:
//Enums
  enum DownloadOrder
  {
    ClosestToCenterFirst,
    YOuterXInnerLeftToRight
  };
  enum ControlAnchorPosition
  {
    Undefined = -1,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
  };
  enum Misc
  {
    OSMTileWidth = 256,
    OSMTileHeight = 256,
    OSMHalfTileWidth = 128,
    OSMHalfTileHeight = 128,
    OSMMaxZoom = 18,
    OSMMinZoom = 0,
    SLIDER_ID = 1,
    COPYRIGHT_ID = 2
  };
  enum MapItem
  {
    None,
    Map,
    ScrollerUp,
    ScrollerDown,
    ScrollerRight,
    ScrollerLeft,
    ZoomIn,
    ZoomOut,
    ZoomBar,
    Marker,
    Polyline,
    Polygon,
    PolylineNode,
    PolygonNode,
    Circle,
    CircleCircumference,
  };
  enum MapMode
  {
    Normal,
    Selection,
    PolylineCreation,
    PolygonCreation,
    MarkerCreation,
    CircleCreation,
    Rotate,
  };
  enum ScaleBarUnits
  {
    UseOSDefault,
    Metric,
    Imperial
  };

//Constructors / Destructors
  COSMCtrl();

//static methods
  static CString GetTileCachePath(const CString& sCacheDirectory, int nZoom, int nTileX, int nTileY, BOOL bOld);
#ifdef COSMCTRL_NOD2D
  static BOOL    PositionToClient(const COSMCtrlPosition& position, const COSMCtrlPosition& centerPosition, double fZoom, const CRect& rClient, Gdiplus::PointF& point);
  static BOOL    ClientToPosition(const Gdiplus::PointF& point, const COSMCtrlPosition& centerPosition, double fZoom, const CRect& rClient, COSMCtrlPosition& position);
#else
  static BOOL    PositionToClient(const COSMCtrlPosition& position, const COSMCtrlPosition& centerPosition, double fZoom, double fBearingOfTopOfMap, const CRect& rClient, CD2DPointF& point);
  static BOOL    ClientToPosition(const CD2DPointF& point, const COSMCtrlPosition& centerPosition, double fZoom, double fBearingOfTopOfMap, const CRect& rClient, COSMCtrlPosition& position);
#endif

//Methods
  virtual BOOL Create(LPCTSTR lpszText, DWORD dwStyle,	const RECT& rect, CWnd* pParentWnd, UINT nID = 0xFFFF);
#ifdef COSMCTRL_NOD2D
  virtual BOOL Draw(CDC* pDC, const CRect& rSource, const CRect* prDest = NULL, BOOL bDrawScrollRose = TRUE, BOOL bDrawZoomBar = TRUE, BOOL bDrawScaleBar = TRUE, BOOL bDrawMarkers = TRUE, BOOL bDrawPolylines = TRUE, BOOL bDrawPolygons = TRUE, BOOL bDrawCircles = TRUE, BOOL bDrawGPSTracks = TRUE);
  virtual BOOL Draw(Gdiplus::Graphics& graphics, const CRect& rClient, const CRect* prClip = NULL, BOOL bDrawScrollRose = TRUE, BOOL bDrawZoomBar = TRUE, BOOL bDrawScaleBar = TRUE, BOOL bDrawMarkers = TRUE, BOOL bDrawPolylines = TRUE, BOOL bDrawPolygons = TRUE, BOOL bDrawCircles = TRUE, BOOL bDrawGPSTracks = TRUE);
#else
  HRESULT CreateImageFromWICBitmap(IWICBitmapSource* WICBitmapSource, ATL::CImage& image);
  virtual HRESULT Draw(CDC* pDC, const CRect& rSource, const CRect* prDest = NULL, BOOL bDrawScrollRose = TRUE, BOOL bDrawZoomBar = TRUE, BOOL bDrawScaleBar = TRUE, BOOL bDrawMarkers = TRUE, BOOL bDrawPolylines = TRUE, BOOL bDrawPolygons = TRUE, BOOL bDrawCircles = TRUE, BOOL bDrawGPSTracks = TRUE);
  virtual HRESULT Draw(CRenderTarget* pRenderTarget, const CRect& rClient, const CRect* prClip = NULL, BOOL bDrawScrollRose = TRUE, BOOL bDrawZoomBar = TRUE, BOOL bDrawScaleBar = TRUE, BOOL bDrawMarkers = TRUE, BOOL bDrawPolylines = TRUE, BOOL bDrawPolygons = TRUE, BOOL bDrawCircles = TRUE, BOOL bDrawGPSTracks = TRUE, BOOL bResetDPI = TRUE);
  double GetBearingOfTopOfMap() const { return m_fBearingOfTopOfMap; };
  BOOL   SetBearingOfTopOfMap(double fBearingOfTopOfMap, double fAnimationDuration);
#endif
  double GetZoom() const { return m_fZoom; };
  BOOL SetZoom(double fZoom, double fAnimationDuration);
  BOOL SetCenter(const COSMCtrlPosition& position, double fAnimationDuration);
  BOOL SetCenterAndZoom(const COSMCtrlPosition& position, double fZoom, double fAnimationDuration);
  BOOL SetCenterAndZoomAndBearing(const COSMCtrlPosition& position, double fZoom, double fBearingOfTopOfMap, double fAnimationDuration);
  COSMCtrlPosition GetCenter() const { return m_CenterPosition; };
  BOOL ZoomToBounds(const COSMCtrlPosition& position1, const COSMCtrlPosition& position2, double fAnimationDuration);
  CString GetCacheDirectory() const { return m_sCacheDirectory; };
  void SetCacheDirectory(const CString& sCacheDirectory, BOOL bInvalidateCache = FALSE);
  BOOL GetDownloadTiles() const { return m_bDownloadTiles; };
  void SetDownloadTiles(BOOL bDownloadTiles);
  void Refresh();
  void SetDrawCenterCrossHairs(BOOL bDrawCenterCrossHairs);
  BOOL GetDrawCenterCrossHairs() const { return m_bDrawCenterCrossHairs; };
  void SetMapMode(MapMode mode);
  MapMode GetMapMode() const { return m_MapMode; };
  BOOL GetDrawScrollRose() const { return m_bDrawScrollRose; };
  BOOL SetDrawScrollRose(BOOL bDrawScrollRose);
  BOOL GetScrollRoseAnchorPosition() const { return m_ScrollRoseAnchorPosition; };
  BOOL SetScrollRoseAnchorPosition(ControlAnchorPosition controlAnchorPosition);
  CPoint GetScrollRoseAnchorPositionOffset() const { return m_ptOffsetScrollRose; };
  BOOL SetScrollRoseAnchorPositionOffset(CPoint ptOffset);
  BOOL GetDrawZoomBar() const { return m_bDrawZoomBar; };
  BOOL SetDrawZoomBar(BOOL bDrawZoomBar);
  BOOL GetZoomBarAnchorPosition() const { return m_ZoomBarAnchorPosition; };
  BOOL SetZoomBarAnchorPosition(ControlAnchorPosition controlAnchorPosition);
  CPoint GetZoomBarAnchorPositionOffset() const { return m_ptOffsetZoomBar; };
  BOOL SetZoomBarAnchorPositionOffset(CPoint ptOffset);
  BOOL GetUseTransparencyForZoomBar() const { return m_bTransparencyForZoomBar; };
  void SetUseTransparencyForZoomBar(BOOL bUseTransparencyForZoomBar);
  BOOL GetDrawZoomBarAsSlider() const { return m_bDrawZoomBarAsSlider; };
  BOOL SetDrawZoomBarAsSlider(BOOL bDrawZoomBarAsSlider);
  BOOL GetDrawScaleBar() const { return m_bDrawScaleBar; };
  void SetDrawScaleBar(BOOL bDrawScaleBar);
  BOOL GetScaleBarAnchorPosition() const { return m_ScaleBarAnchorPosition; };
  void SetScaleBarAnchorPosition(ControlAnchorPosition controlAnchorPosition);
  CPoint GetScaleBarAnchorPositionOffset() const { return m_ptOffsetScaleBar; };
  void SetScaleBarAnchorPositionOffset(CPoint ptOffset);
  void SetScaleBarUnits(ScaleBarUnits units) { m_ScaleBarUnits = units; };
  ScaleBarUnits GetScaleBarUnits() const { return m_ScaleBarUnits; };
  BOOL GetDrawCopyright() const { return m_bDrawCopyright; };
  BOOL SetDrawCopyright(BOOL bDrawCopyright);
  BOOL GetCopyrightAnchorPosition() const { return m_CopyrightAnchorPosition; };
  BOOL SetCopyrightAnchorPosition(ControlAnchorPosition controlAnchorPosition);
  CPoint GetCopyrightAnchorPositionOffset() const { return m_ptOffsetCopyright; };
  BOOL SetCopyrightAnchorPositionOffset(CPoint ptOffset);
  BOOL GetAllowDrag() const { return m_bAllowDrag; };
  void SetAllowDrag(BOOL bAllowDrag) { m_bAllowDrag = bAllowDrag; };
  BOOL GetAllowKeyboard() const { return m_bAllowKeyboard; };
  void SetAllowKeyboard(BOOL bAllowKeyboard) { m_bAllowKeyboard = bAllowKeyboard; };
  BOOL GetAllowMouseZoom() const { return m_bAllowMouseZoom; };
  void SetAllowMouseZoom(BOOL bAllowMouseZoom) { m_bAllowMouseZoom = bAllowMouseZoom; };
  BOOL GetAllowDoubleClickZoom() const { return m_bAllowDoubleClickZoom; };
  void SetAllowDoubleClickZoom(BOOL bAllowDoubleClickZoom) { m_bAllowDoubleClickZoom = bAllowDoubleClickZoom; };
  BOOL GetAllowUsePreviousZoomStretch() const { return m_bAllowPreviousZoomStretch; };
  void SetAllowUsePreviousZoomStretch(BOOL bAllowPreviousZoomStretch) { m_bAllowPreviousZoomStretch = bAllowPreviousZoomStretch; };
  BOOL GetAllowUseNextZoomSqueeze() const { return m_bAllowNextZoomSqueeze; };
  void SetAllowUseNextZoomSqueeze(BOOL bAllowNextZoomSqueeze) { m_bAllowNextZoomSqueeze = bAllowNextZoomSqueeze; };
  CString GetUserAgent() const { return m_sUserAgent; };
  void SetUserAgent(const CString& sUserAgent) { m_sUserAgent = sUserAgent; };
#ifndef COSMCTRL_NOANIMATION
  BOOL GetDoAnimations() const { return m_bAnimations; };
  void SetDoAnimations(BOOL bAnimations) { m_bAnimations = bAnimations; };
#endif
  DownloadOrder GetDownloadOrder() const { return m_DownloadOrder; };
  void SetDownloadOrder(DownloadOrder downloadOrder) { m_DownloadOrder = downloadOrder; };
  BOOL GetDrawTileOutlines() const { return m_bDrawTileOutlines; };
  void SetDrawTileOutlines(BOOL bDrawTileOutlines);
  MapItem HitTest(const CPoint& point, INT_PTR& nItem, INT_PTR& nSubItem, const CRect& rClient) const;
  BOOL GetBoundingRect(const COSMCtrlMarker& marker, CRect& rBounds, const CRect& rClient) const;
  BOOL GetBoundingNodeRect(const COSMCtrlPolyline& polyline, INT_PTR nIndex, CRect& rBounds, const CRect& rClient) const;
  BOOL GetBoundingNodeRect(const COSMCtrlPolygon& polygon, INT_PTR nIndex, CRect& rBounds, const CRect& rClient) const;
#ifdef COSMCTRL_NOD2D
  BOOL ClientToPosition(const Gdiplus::PointF& point, const CRect& rClient, COSMCtrlPosition& position) const;
  BOOL PositionToClient(const COSMCtrlPosition& position, const CRect& rClient, Gdiplus::PointF& point) const;
#else
  BOOL ClientToPosition(const CD2DPointF& point, const CRect& rClient, COSMCtrlPosition& position) const;
  BOOL PositionToClient(const COSMCtrlPosition& position, const CRect& rClient, CD2DPointF& point) const;
#endif
  int GetDownlodTilesEdgeCount() const { return m_nDownlodTilesEdgeCount; };
  void SetDownloadTilesEdgeCount(int nDownloadTilesEdgeCount) { m_nDownlodTilesEdgeCount = nDownloadTilesEdgeCount; };
  virtual void ScrollTileUp();
  virtual void ScrollTileDown();
  virtual void ScrollTileLeft();
  virtual void ScrollTileRight();
  virtual BOOL ScrollUp(int nPixels);
  virtual BOOL ScrollDown(int nPixels);
  virtual BOOL ScrollLeft(int nPixels);
  virtual BOOL ScrollRight(int nPixels);
  void DeselectAllItems();
  void SelectAllItems();
  void SelectMarker(INT_PTR nIndex);
  void SelectPolyline(INT_PTR nIndex);
  void SelectPolylineNode(INT_PTR nIndex, INT_PTR nSubItemIndex);
  void SelectPolygon(INT_PTR nIndex);
  void SelectPolygonNode(INT_PTR nIndex, INT_PTR nSubItemIndex);
  void SelectCircle(INT_PTR nIndex);
  int  DeleteSelectedItems();
  int GetFirstSelectedPolyline() const;
  int GetFirstSelectedPolygon() const;
  int GetFirstSelectedMarker() const;
  int GetFirstSelectedCircle() const;
  INT_PTR ShowOperationsDialog(COSMCtrlMapOperationsDlg& dialog);
  BOOL DecimatePolyline(COSMCtrlPolyline& polyline, int nNewNodesPerWay, const CRect& rClient);
  BOOL DecimatePolygon(COSMCtrlPolygon& polyline, int nNewNodesPerWay, const CRect& rClient);
#ifdef COSMCTRL_NOD2D
  void DeleteInMemoryCache();
  void SetMaxTilesInMemoryCache(INT_PTR nMaxTiles);
  INT_PTR GetMaxTilesInMemoryCache() const { return m_nMaxTilesInMemoryCache; };
#endif
  void SetScrollPixels(int nPixels) { m_nScrollPixels = nPixels; };
  int GetScrollPixels() const { return m_nScrollPixels; };
  void SetMaxGPSTracks(int nGPSTracks);
  std::vector<COSMCtrlNode>::size_type GetMaxGPSTracks() const { return m_nMaxGPSTracks; };
  virtual void AddGPSTrack(BOOL bGPSFix, double fLongitude, double fLatitude, double fBearing, double fSpeed, BOOL bBearingValid, BOOL bSpeedValid, double fHDOP);
  IOSMCtrlHandler* GetEventHandler() const { return m_pEventHandler; };
  void SetEventHandler(IOSMCtrlHandler* pHandler) { m_pEventHandler = pHandler; };
  IOSMCtrlTileProvider* GetTileProvider() const { return m_pTileProvider; };
  void SetTileProvider(IOSMCtrlTileProvider* pTileProvider);
  void SetDefaultAnimationDuration(double fDuration) { m_fDefaultAnimationDuration = fDuration; }
  double GetDefaultAnimationDuration() const { return m_fDefaultAnimationDuration; }

//Member variables  
  std::vector<COSMCtrlMarker>   m_Markers;                       //The marker array associated with this map
  std::vector<COSMCtrlPolygon>  m_Polygons;                      //The polygons array associated with this map
  std::vector<COSMCtrlPolyline> m_Polylines;                     //The polylines array associated with this map
  std::vector<COSMCtrlCircle>   m_Circles;                       //The circles array associated with this map
  std::vector<COSMCtrlIcon*>    m_Icons;                         //The icon array associated with this map
  COSMCtrlPolygon               m_SelectionPolygon;              //The selection polygon
  COSMCtrlPolyline              m_GPSTrack;                      //The GPS track on the map
#ifdef COSMCTRL_NOD2D
  Gdiplus::Color                m_colorGPSTrackPointer;          //The color to use for the GPS track pointer
  Gdiplus::Color                m_colorGPSTrackNoBearingPointer; //The color to use for the GPS track pointer when we do not have a bearing
#else
#endif
  BOOL                          m_bGPSFix;                       //Do we currently have a GPS fix

protected:
//Message handlers
  afx_msg void OnPaint();
  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnDestroy();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg void OnNMClickCopyright(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg LRESULT OnCreateChildControls(WPARAM wParam, LPARAM lParam);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg LRESULT OnDownloadComplete(WPARAM wParam, LPARAM lParam);
#ifndef COSMCTRL_NOD2D
  afx_msg LRESULT OnDraw2D(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnRecreatedResources(WPARAM wParam, LPARAM lParam);
#endif
  
  DECLARE_MESSAGE_MAP()

//Methods
  virtual INT_PTR  OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
  virtual CPoint   GetControlPosition(ControlAnchorPosition anchorPosition, const CPoint& ptOffset, const CRect& rClient);
  virtual HRESULT  CreateSession(CWinHTTPSession& session, DWORD dwFlags);
  virtual HRESULT  CreateConnection(const CString& sServer, INTERNET_PORT nPort);
  virtual HRESULT  CreateRequest(const CString& sObject, const CString& sFile, BOOL bForcedRefresh);
  virtual BOOL     CreateZoomBarSlider();
  virtual BOOL     CreateCopyrightLinkCtrl();
  virtual CString  GetDeltaFile(const CString& sFile);
  virtual BOOL     HitTest(const CPoint& point, const COSMCtrlPolyline& polyline, const CRect& rClient) const;
  virtual BOOL     HitTest(const CPoint& point, const COSMCtrlPolygon& polygon, const CRect& rClient) const;
  virtual MapItem  HitTest(const CPoint& point, const COSMCtrlCircle& circle, const CRect& rClient) const;
#ifdef COSMCTRL_NOD2D
  virtual BOOL     DrawTileNotAvailable(Gdiplus::Graphics& graphics, const Gdiplus::RectF& rTile);
  virtual BOOL     DrawTile(Gdiplus::Graphics& graphics, const Gdiplus::RectF& rTile, int nTileX, int nTileY);
  virtual BOOL     DrawTileOutline(Gdiplus::Graphics& graphics, const Gdiplus::RectF& rTile);
  virtual BOOL     DrawScrollRose(Gdiplus::Graphics& graphics, const CRect& rClip, const CRect& rClient);
  virtual BOOL     DrawZoomBar(Gdiplus::Graphics& graphics, const CRect& rClip, const CRect& rClient);
  virtual BOOL     DrawScaleBar(Gdiplus::Graphics& graphics, const CRect& rClip, const CRect& rClient);
  virtual BOOL     DrawCenterCrossHairs(Gdiplus::Graphics& graphics, const CRect& rClip, const CRect& rClient);
  virtual int      DrawPolylines(Gdiplus::Graphics& graphics, const CRect& rClip, const CRect& rClient) const;
  virtual BOOL     DrawPolyline(Gdiplus::Graphics& path, const COSMCtrlPolyline& polyline, const CRect& rClient) const;
  virtual int      DrawPolygons(Gdiplus::Graphics& graphics, const CRect& rClip, const CRect& rClient) const;
  virtual BOOL     DrawPolygon(Gdiplus::Graphics& graphics, const COSMCtrlPolygon& polygon, const CRect& rClient) const;
  virtual BOOL     DrawPolygonOutline(Gdiplus::GraphicsPath& path, const COSMCtrlPolygon& polygon, const CRect& rClient) const;
  virtual BOOL     DrawPolygonInternal(Gdiplus::GraphicsPath& path, const COSMCtrlPolygon& polygon, const CRect& rClient) const;
  virtual BOOL     DrawCircle(Gdiplus::Graphics& graphics, const COSMCtrlCircle& circle, const CRect& rClient) const;
  virtual BOOL     DrawCircle(Gdiplus::GraphicsPath& path, const COSMCtrlCircle& circle, BOOL bCircumference, const CRect& rClient) const;
  virtual int      DrawCircles(Gdiplus::Graphics& graphics, const CRect& rClip, const CRect& rClient) const;
  virtual int      DrawMarkers(Gdiplus::Graphics& graphics, const CRect& rClip, const CRect& rClient) const;
  virtual BOOL     DrawMarker(Gdiplus::Graphics& graphics, const COSMCtrlMarker& marker, const CRect& rClient) const;
  virtual BOOL     DrawGPSTrack(Gdiplus::Graphics& graphics, const CRect& rClient);
  virtual BOOL     DrawGPSLocation(Gdiplus::Graphics& graphics, const CRect& rClient);
  BOOL             Draw3dRect(Gdiplus::Graphics& graphics, const Gdiplus::RectF& r, const Gdiplus::Color& clrTopLeft, const Gdiplus::Color& clrBottomRight);
  virtual Gdiplus::CachedBitmap* GetCachedBitmap(Gdiplus::Graphics& graphics, const CStringW& sFile, int nTileX, int nTileY, int nZoom, Gdiplus::Bitmap*& pBitmap);
#else
  virtual BOOL     DrawTileNotAvailable(CRenderTarget* pRenderTarget, const CD2DRectF& rTile);
  virtual BOOL     DrawTile(CRenderTarget* pRenderTarget, const CD2DRectF& rTile, int nTileX, int nTileY);
  virtual BOOL     DrawTileOutline(CRenderTarget* pRenderTarget, const CD2DRectF& rTile);
  virtual BOOL     DrawScrollRose(CRenderTarget* pRenderTarget, const CRect& rClip, const CRect& rClient);
  virtual BOOL     DrawZoomBar(CRenderTarget* pRenderTarget, const CRect& rClip, const CRect& rClient);
  virtual BOOL     DrawScaleBar(CRenderTarget* pRenderTarget, const CRect& rClip, const CRect& rClient);
  virtual BOOL     DrawCenterCrossHairs(CRenderTarget* pRenderTarget, const CRect& rClip, const CRect& rClient);
  virtual int      DrawPolylines(CRenderTarget* pRenderTarget, const CRect& rClip, const CRect& rClient) const;
  virtual BOOL     DrawPolyline(CRenderTarget* pRenderTarget, const COSMCtrlPolyline& polyline, const CRect& rClient) const;
  virtual int      DrawPolygons(CRenderTarget* pRenderTarget, const CRect& rClip, const CRect& rClient) const;
  virtual BOOL     DrawPolygon(CRenderTarget* pRenderTarget, const COSMCtrlPolygon& polygon, const CRect& rClient) const;
  virtual BOOL     DrawCircle(CRenderTarget* pRenderTarget, const COSMCtrlCircle& circle, const CRect& rClient) const;
  virtual int      DrawCircles(CRenderTarget* pRenderTarget, const CRect& rClip, const CRect& rClient) const;
  virtual int      DrawMarkers(CRenderTarget* pRenderTarget, const CRect& rClip, const CRect& rClient, BOOL bUseInMemoryCache);
  virtual BOOL     DrawMarkerInternal(CRenderTarget* pRenderTarget, const COSMCtrlMarker& marker, CD2DBitmap* pBitmap, CD2DPointF& ptMarker);
  virtual BOOL     DrawMarkerWithoutCache(CRenderTarget* pRenderTarget, const COSMCtrlMarker& marker, const CRect& rClient);
  virtual BOOL     DrawMarkerFromCache(CRenderTarget* pRenderTarget, const COSMCtrlMarker& marker, const CRect& rClient);
  virtual BOOL     DrawGPSTrack(CRenderTarget* pRenderTarget, const CRect& rClient);
  virtual BOOL     DrawGPSHDOP(CRenderTarget* pRenderTarget, const CRect& rClient);
  virtual BOOL     DrawGPSLocation(CRenderTarget* pRenderTarget, const CRect& rClient);
  BOOL             Draw3dRect(CRenderTarget* pRenderTarget, const CD2DRectF& r, const D2D1::ColorF& clrTopLeft, const D2D1::ColorF& clrBottomRight);
#endif
  virtual CString  FormScaleBarText(double fScaleDistance, BOOL bMetric);
  virtual CString  FormCopyrightText();
  virtual void     PerformInMemoryGPSTrackMaintenance(std::vector<COSMCtrlNode>::size_type nMaxTracks);
  virtual void     EnsureBoundingRectForGPSTrackTriangle(const COSMCtrlPosition& position, CRect& rBounds, const CRect& rClient) const;
  virtual BOOL     UseMetric();
  virtual void     StartDrag(const CPoint& point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     CalculateScaleBar(double& fDistance, double& fScaleDistance, BOOL& bMetric, int& nScaleLength);
  virtual void     CalculateStartTilePositions(const CRect& rClient, double& fClientX, double& fClientY, double& fEndClientX, double& fEndClientY, int& nStartX, int& nStartY, double& fOSMTileWidth, double& fOSMTileHeight);
  BOOL             ContainsPosition(const COSMCtrlPosition& topLeft, const COSMCtrlPosition& bottomRight, const COSMCtrlPosition& point);
#ifdef COSMCTRL_NOD2D
  virtual CStringW GetFileForMemoryCache(const CStringW& sFile);
#endif
#ifdef COSMCTRL_NOD2D
  virtual void     PerformInMemoryCacheMaintenance(INT_PTR nMaxTiles);
#endif
  virtual void     InitializeNewPolyline(COSMCtrlPolyline& polyline);
  virtual void     InitializeNewPolygon(COSMCtrlPolygon& polygon);
  virtual void     InitializeNewSelectionPolygon(COSMCtrlPolygon& polygon);
  virtual void     InitializeNewMarker(COSMCtrlMarker& marker);
  virtual void     InitializeNewCircle(COSMCtrlCircle& circle);
  virtual void     HandleLButtonDownStandard(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     HandleLButtonDownPolylineCreation(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     HandleLButtonDownPolygonCreation(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     HandleLButtonDownPolylineCreationWithNode(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     HandleLButtonDownPolygonCreationWithNode(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     HandleLButtonDownMarkerCreation(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     HandleLButtonDownCircleCreation(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     HandleLButtonDownCircleCircumference(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     HandleLButtonDownSelection(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     HandleLButtonDownPolygonNode(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     HandleLButtonDownPolygon(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     HandleLButtonDownPolylineNode(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     HandleLButtonDownPolyline(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     HandleLButtonDownMarker(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     HandleLButtonDownCircle(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     HandleLButtonDownMap(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
#ifndef COSMCTRL_NOD2D
  virtual void     HandleLButtonDownRotate(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
#endif
  virtual void     HandleMouseMoveSelection(UINT nFlags, CPoint point);
  virtual void     HandleMouseMoveCircleCreation(UINT nFlags, CPoint point);
  virtual void     HandleMouseMovePolylineCreation(UINT nFlags, CPoint point);
  virtual void     HandleMouseMovePolygonCreation(UINT nFlags, CPoint point);
  virtual void     HandleMouseMovePolygonCreationWithNode(UINT nFlags, CPoint point);
  virtual void     HandleMouseMoveMap(UINT nFlags, CPoint point);
  virtual void     HandleMouseMoveMarker(UINT nFlags, CPoint point);
  virtual void     HandleMouseMoveCircle(UINT nFlags, CPoint point);
  virtual void     HandleMouseMovePolylineNode(UINT nFlags, CPoint point);
  virtual void     HandleMouseMovePolyline(UINT nFlags, CPoint point);
  virtual void     HandleMouseMovePolygonNode(UINT nFlags, CPoint point);
  virtual void     HandleMouseMovePolygon(UINT nFlags, CPoint point);
  virtual void     HandleLButtonDblClickPolyline(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     HandleLButtonDblClickPolygon(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     HandleLButtonDblClickMap(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     HandleLButtonDblClickMarker(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
  virtual void     HandleLButtonDblClickCircle(UINT nFlags, CPoint point, MapItem item, INT_PTR nItem, INT_PTR nSubItem);
#ifndef COSMCTRL_NOD2D
  virtual void     HandleMouseMoveRotate(UINT nFlags, CPoint point);
#endif
#ifndef COSMCTRL_NOANIMATION
  virtual HRESULT  CreateTransition(double fOldZoom, double fNewZoom, double fOldLongitude, double fNewLongitude, double fOldLatitude, double fNewLatitude, double fOldBearingAtTopOfMap, double fNewBearingAtTopOfMap, double fAnimationDuration);
  virtual void     HandleAnimationTimerEvent();
#endif
  virtual HRESULT       StartAsyncDownload(BOOL bBuildArray, BOOL bForcedRefresh);
  virtual HRESULT       StopAsyncDownload();
  static ID2D1Factory*  GetD2DFactory();
  virtual void          BuildDownloadArray(CSortedArrayEx<COSMCtrlDownloadTileItem, CompareCOSMCtrlDownloadTileItem, const COSMCtrlDownloadTileItem&>& tilesToDownload, BOOL bForceRefresh);
  
//Member variables
  double                                      m_fZoom;                                                                                 //The current zoom level
  double                                      m_fFinalZoom;                                                                            //The final zoom level (used for animations)
  double                                      m_fDefaultAnimationDuration;                                                             //The default animation duration in seconds
  COSMCtrlPosition                            m_CenterPosition;                                                                        //The center point of the map
  COSMCtrlPosition                            m_FinalCenterPosition;                                                                   //The final center position (used for animations)
  CString                                     m_sCacheDirectory;                                                                       //The directory where downloaded OSM tiles will be cached
  IOSMCtrlTileProvider*                       m_pTileProvider;                                                                         //The tileprovider for this map
  COSMCtrlMapnikTileProvider                  m_DefaultTileProvider;                                                                   //The default tile provider (Mapnik)
  BOOL                                        m_bDownloadTiles;                                                                        //Should we download tiles
  BOOL                                        m_bMouseCapturedForDrag;                                                                 //Is the mouse currently captured for dragging the map
  BOOL                                        m_bMouseCapturedForZoom;                                                                 //Is the mouse currently captured for zooming the map
  CPoint                                      m_pointMouseCapture;                                                                     //the client coordinates where we started dragging from
  BOOL                                        m_bDrawTileOutlines;                                                                     //Should we draw outlines around all the tiles
  BOOL                                        m_bDrawScrollRose;                                                                       //Should we draw the scroll rose
  ControlAnchorPosition                       m_ScrollRoseAnchorPosition;                                                              //The anchor position for the scroll rose
  CPoint                                      m_ptOffsetScrollRose;                                                                    //the offset position for the scroll rose
  BOOL                                        m_bDrawZoomBar;                                                                          //Should we draw the zoom bar
  ControlAnchorPosition                       m_ZoomBarAnchorPosition;                                                                 //The anchor position for the Zoom bar
  CPoint                                      m_ptOffsetZoomBar;                                                                       //the offset position for the Zoom bar
  BOOL                                        m_bTransparencyForZoomBar;                                                               //Should we use transparency to draw the zoom bar
  BOOL                                        m_bDrawScaleBar;                                                                         //Should we draw the scale bar
  ControlAnchorPosition                       m_ScaleBarAnchorPosition;                                                                //The anchor position for the Scale bar
  CPoint                                      m_ptOffsetScaleBar;                                                                      //the offset position for the Scale bar
  BOOL                                        m_bAllowDrag;                                                                            //Is the map allowed to be dragged
  BOOL                                        m_bAllowKeyboard;                                                                        //Is the map allowed to be scrolled/zoomed via the keyboard
  BOOL                                        m_bDeltaMode;                                                                            //Is "Delta" mode enabled
  BOOL                                        m_bAllowMouseZoom;                                                                       //Is the map allowed to be zoomed via the mouse wheel
  BOOL                                        m_bAllowDoubleClickZoom;                                                                 //Is the map allowed to be zoomed via a double click
  BOOL                                        m_bAllowPreviousZoomStretch;                                                             //Do we allow the previous zoom level image to be stretched
  BOOL                                        m_bAllowNextZoomSqueeze;                                                                 //Do we allow the next zoom level image to be squeezed
  CString                                     m_sUserAgent;                                                                            //The HTTP_USER_AGENT header to use
  DownloadOrder                               m_DownloadOrder;                                                                         //The order in which we will download tiles
  int                                         m_nDownlodTilesEdgeCount;                                                                //The number of rows or columns of tiles to cache which are outside of the visible client area
  CRect                                       m_rNorthScrollRose;                                                                      //Used for hittesting
  CRect                                       m_rSouthScrollRose;                                                                      //Used for hittesting
  CRect                                       m_rEastScrollRose;                                                                       //Used for hittesting
  CRect                                       m_rWestScrollRose;                                                                       //Used for hittesting
  CRect                                       m_rZoomInZoomBar;                                                                        //Used for hittesting
  CRect                                       m_rZoomOutZoomBar;                                                                       //Used for hittesting
  CRect                                       m_rZoomBar;                                                                              //Used for hittesting
  CSliderCtrl                                 m_ctrlZoomBar;                                                                           //The Zoom bar if we are using a native slider control
  BOOL                                        m_bDrawZoomBarAsSlider;                                                                  //Should we use a slider control for the zoom bar
  TCHAR                                       m_szCurrentToolTipText[1024];                                                            //The current tooltip text
  MapItem                                     m_MouseDragItem;                                                                         //What we are dragging
  MapItem                                     m_ToolHitItem;                                                                           //What we hit for tooltips
  INT_PTR                                     m_nMouseDragItemIndex;                                                                   //The index value associated with what we are dragging
  INT_PTR                                     m_nMouseDragSubItemIndex;                                                                //The node index value associated with what we are dragging
  CSize                                       m_DragOffset;                                                                            //Used to implement smooth / non jumpy dragging of markers, polylines and polygons
  std::vector<COSMCtrlNode>                   m_OriginalDragNodes;                                                                     //The nodes we are dragging from (used for dragging of polylines and polygons)
  BOOL                                        m_bModifyingCircleRadius;                                                                //Ised to implement circle editing
  MapMode                                     m_MapMode;                                                                               //The mapping mode currently active
  BOOL                                        m_bDrawCenterCrossHairs;                                                                 //Should cross hairs be shown on the map
  BOOL                                        m_bDrawCopyright;                                                                        //Should we draw the copyright on the map
  ControlAnchorPosition                       m_CopyrightAnchorPosition;                                                               //The anchor position for the copyright message
  CPoint                                      m_ptOffsetCopyright;                                                                     //the offset position for the copyright message
  ScaleBarUnits                               m_ScaleBarUnits;                                                                         //What units should we use for drawing in the scale bar
  CLinkCtrl                                   m_ctrlCopyright;                                                                         //The copyright hyperlink control
#ifdef COSMCTRL_NOD2D
  CSortedArrayEx<COSMCtrlCachedTile, CompareCOSMCtrlCachedTile, COSMCtrlCachedTile&> m_CachedTiles;                                    //The cache of tiles we have
  INT_PTR                                     m_nMaxTilesInMemoryCache;                                                                //The maximum number of tiles which are allowed in "m_CachedTiles"
  int                                         m_nInMemoryCachedTilesInsertionCounter;                                                  //The insertion counter value for the next item to be added to "m_CachedTiles"
#endif
  int                                         m_nScrollPixels;                                                                         //The amount to scroll in pixels 
  std::vector<COSMCtrlNode>::size_type        m_nMaxGPSTracks;                                                                         //The maximum number of GPS tracks to display on the map
  IOSMCtrlHandler*                            m_pEventHandler;                                                                         //The event handler instance
  double                                      m_fBearingOfTopOfMap;                                                                    //The compass bearing of the top of the map (i.e. at 12 o clock)
  double                                      m_fBearingAtTopOfMapMouseCapture;                                                        //The bearing at the top of the map when the mouse was captured
  double                                      m_fFinalBearingAtTopOfMap;                                                               //The final bearing (used for animations)
#ifndef COSMCTRL_NOANIMATION
  ATL::CComPtr<IUIAnimationManager>           m_pAnimationManager;                                                                     //The Windows Animation manager
  ATL::CComPtr<IUIAnimationTimer>             m_pAnimationTimer;                                                                       //The Windows Animation timer     
  ATL::CComPtr<IUIAnimationTransitionLibrary> m_pTransitionLibrary;                                                                    //The Windows Transaction Library
  ATL::CComPtr<IUIAnimationVariable>          m_pZoomLevelAnimationVariable;                                                           //The animation variable for zooming
  ATL::CComPtr<IUIAnimationVariable>          m_pLatitudeAnimationVariable;                                                            //The animation variable for latitude
  ATL::CComPtr<IUIAnimationVariable>          m_pLongitudeAnimationVariable;                                                           //The animation variable for zooming
  ATL::CComPtr<IUIAnimationVariable>          m_pBearingAnimationVariable;                                                             //The animation variable for bearing
  BOOL                                        m_bAnimations;                                                                           //Should we do animations
#endif
  CWinHTTPSession                             m_WinHTTPSession;                                                                        //The WinHttp session object
  CWinHTTPConnection                          m_WinHTTPConnection;                                                                     //The WinHttp connection object
  CSortedArrayEx<COSMCtrlDownloadTileItem, CompareCOSMCtrlDownloadTileItem, const COSMCtrlDownloadTileItem&> m_tilesToDownload;        //The tiles to download
  CSortedArrayEx<COSMCtrlDownloadTileItem, CompareCOSMCtrlDownloadTileItem, const COSMCtrlDownloadTileItem&> m_tilesPendingToDownload; //The pending tiles to download
  BOOL                                        m_bDownloading;                                                                          //Are we currently downloading
  COSMCtrlWinHTTPRequest*                     m_pWinHTTPRequest;                                                                       //The pointer to this controls WinHTTP request object in global scope

  friend class COSMCtrlMapOperationsDlg;
  friend class COSMCtrlTimerEventHandler;
};


#endif //__OSMCTRL_H__
