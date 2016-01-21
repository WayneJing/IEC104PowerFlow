/*
Module : OSMCtrlCircle.cpp
Purpose: Implementation for the COSMCtrlCircle class
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
#include "OSMCtrlCircle.h"
#include "OSMCtrlHelper.h"


///////////////////////////////// Implementation //////////////////////////////


COSMCtrlCircle::COSMCtrlCircle() : m_fRadius(0),
                                   m_bSelected(FALSE),
                                   m_fLinePenWidth(2),
                                   m_colorSelection(255, 0, 0),
                                   m_DrawingStyle(LineAndInside),
                                   m_colorPen(0, 0, 255),
                                 #ifdef COSMCTRL_NOD2D
                                   m_colorBrush(128, 0, 0, 255),
                                   m_DashStyle(Gdiplus::DashStyleSolid),
                                 #else
                                   m_colorBrush(0, 0, 255, 0.5F),
                                   m_DashStyle(D2D1_DASH_STYLE_SOLID), 
                                 #endif
                                   m_bDraggable(FALSE),
                                   m_bVisible(TRUE),
                                   m_bEditable(FALSE),
                                   m_bHitTest(TRUE),
                                   m_nMinZoomLevel(COSMCtrlHelper::OSMMinZoom),
                                   m_nMaxZoomLevel(COSMCtrlHelper::OSMMaxZoom)
{
}

COSMCtrlCircle::COSMCtrlCircle(const COSMCtrlCircle& circle)
                                                          #ifndef COSMCTRL_NOD2D
                                                            : m_colorPen(D2D1::ColorF::Black),
                                                              m_colorSelection(D2D1::ColorF::Black),
                                                              m_colorBrush(D2D1::ColorF::Black)
                                                          #endif
{
  *this = circle;
}

COSMCtrlCircle& COSMCtrlCircle::operator=(const COSMCtrlCircle& circle)
{
  m_Position       = circle.m_Position;
  m_fRadius        = circle.m_fRadius;
  m_bSelected      = circle.m_bSelected;
  m_fLinePenWidth  = circle.m_fLinePenWidth;
  m_colorPen       = circle.m_colorPen;
  m_colorSelection = circle.m_colorSelection;
  m_colorBrush     = circle.m_colorBrush;
  m_DashStyle      = circle.m_DashStyle;
  m_DrawingStyle   = circle.m_DrawingStyle;
  m_sToolTipText   = circle.m_sToolTipText;
  m_bDraggable     = circle.m_bDraggable;
  m_bVisible       = circle.m_bVisible;
  m_bEditable      = circle.m_bEditable;
  m_bHitTest       = circle.m_bHitTest;
  m_nMinZoomLevel  = circle.m_nMinZoomLevel;
  m_nMaxZoomLevel  = circle.m_nMaxZoomLevel;
  
  return *this;
}

BOOL COSMCtrlCircle::GetBoundingRect(COSMCtrlPosition& topLeft, COSMCtrlPosition& bottomRight) const
{
  double fTopLatitude = -91;
  double fBottomLatitude = 91;
  double fLeftLongitude = 181;
  double fRightLongitude = -181;

  //Look through the four cardinal positions from the center
  double fBearing = 0;
  for (INT_PTR i=0; i<4; i++)
  {
    //Move the specific distance from the center of the circle
    COSMCtrlPosition position(COSMCtrlHelper::GetPosition(m_Position, fBearing, m_fRadius, NULL));

    if (position.m_fLatitude > fTopLatitude)
      fTopLatitude = position.m_fLatitude;
    if (position.m_fLatitude < fBottomLatitude)
      fBottomLatitude = position.m_fLatitude;
    if (position.m_fLongitude < fLeftLongitude)
      fLeftLongitude = position.m_fLongitude;
    if (position.m_fLongitude > fRightLongitude)
      fRightLongitude = position.m_fLongitude;
      
    //prepare for the next time around
    fBearing += 90;
  }

  topLeft.m_fLatitude = fTopLatitude;
  topLeft.m_fLongitude = fLeftLongitude;
  bottomRight.m_fLatitude = fBottomLatitude;
  bottomRight.m_fLongitude = fRightLongitude;  

  return TRUE;
}
