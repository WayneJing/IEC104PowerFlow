/*
Module : OSMCtrlPolygon.cpp
Purpose: Implementation for the COSMCtrlPolygon class
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
#include "OSMCtrlPolygon.h"
#include "OSMCtrlHelper.h"


///////////////////////////////// Implementation //////////////////////////////


COSMCtrlPolygon::COSMCtrlPolygon() : m_fLinePenWidth(2),
                                     m_fNodeWidth(4),
                                     m_colorNode(0, 0, 0),
                                     m_fSelectionNodeWidth(4),
                                     m_colorSelectionNode(255, 0, 0),
                                     m_DrawingStyle(LinesOnly),
                                   #ifdef COSMCTRL_NOD2D
                                     m_colorPen(128, 0, 0, 255),
                                     m_colorBrush(128, 0, 0, 255),
                                     m_DashCap(Gdiplus::DashCapFlat),
                                     m_DashStyle(Gdiplus::DashStyleSolid),
                                     m_EndCap(Gdiplus::LineCapRound),
                                     m_StartCap(Gdiplus::LineCapRound),
                                     m_LineJoin(Gdiplus::LineJoinRound),
                                   #else
                                     m_colorPen(0, 0, 255, 0.5F),
                                     m_colorBrush(0, 0, 255, 0.5F),
                                     m_DashCap(D2D1_CAP_STYLE_FLAT),
                                     m_DashStyle(D2D1_DASH_STYLE_SOLID),
                                     m_fMiterLimit(1),
                                     m_EndCap(D2D1_CAP_STYLE_ROUND),
                                     m_StartCap(D2D1_CAP_STYLE_ROUND),
                                     m_LineJoin(D2D1_LINE_JOIN_ROUND),
                                   #endif
                                     m_fDashOffset(0),
                                     m_bDraggable(FALSE),
                                     m_bVisible(TRUE),
                                     m_bHitTest(TRUE),
                                     m_bEditable(FALSE),
                                     m_nMinZoomLevel(COSMCtrlHelper::OSMMinZoom),
                                     m_nMaxZoomLevel(COSMCtrlHelper::OSMMaxZoom)
{
}

COSMCtrlPolygon::COSMCtrlPolygon(const COSMCtrlPolygon& polygon)
                                                              #ifndef COSMCTRL_NOD2D
                                                                : m_colorPen(D2D1::ColorF::Black),
                                                                  m_colorNode(D2D1::ColorF::Black),
                                                                  m_colorSelectionNode(D2D1::ColorF::Black),
                                                                  m_colorBrush(D2D1::ColorF::Black)
                                                              #endif
{
  *this = polygon;
}

COSMCtrlPolygon& COSMCtrlPolygon::operator=(const COSMCtrlPolygon& polygon)
{
  m_Nodes               = polygon.m_Nodes;
  m_fLinePenWidth       = polygon.m_fLinePenWidth;
  m_colorPen            = polygon.m_colorPen;
  m_colorNode           = polygon.m_colorNode;
  m_colorSelectionNode  = polygon.m_colorSelectionNode;
  m_colorBrush          = polygon.m_colorBrush;
  m_DashCap             = polygon.m_DashCap;
  m_DashStyle           = polygon.m_DashStyle;
  m_EndCap              = polygon.m_EndCap;
  m_StartCap            = polygon.m_StartCap;
  m_LineJoin            = polygon.m_LineJoin;
  m_fDashOffset         = polygon.m_fDashOffset;
#ifndef COSMCTRL_NOD2D
  m_fMiterLimit         = polygon.m_fMiterLimit;
#endif
  m_fNodeWidth          = polygon.m_fNodeWidth;
  m_fSelectionNodeWidth = polygon.m_fSelectionNodeWidth;
  m_DrawingStyle        = polygon.m_DrawingStyle;
  m_sToolTipText        = polygon.m_sToolTipText;
  m_bDraggable          = polygon.m_bDraggable;
  m_bVisible            = polygon.m_bVisible;
  m_bEditable           = polygon.m_bEditable;
  m_bHitTest            = polygon.m_bHitTest;
  m_nMinZoomLevel       = polygon.m_nMinZoomLevel;
  m_nMaxZoomLevel       = polygon.m_nMaxZoomLevel;
  
  return *this;
}

BOOL COSMCtrlPolygon::FullySelected() const
{
  //Check all the nodes
  for (std::vector<COSMCtrlNode>::size_type i=0; i<m_Nodes.size(); i++)
  {
    if (!m_Nodes[i].m_bSelected)
      return FALSE;
  }
  
  return TRUE;
}

BOOL COSMCtrlPolygon::AnySelected() const
{
  //Check all the nodes
  for (std::vector<COSMCtrlNode>::size_type i=0; i<m_Nodes.size(); i++)
  {
    if (m_Nodes[i].m_bSelected)
      return TRUE;
  }
  
  return FALSE;
}

void COSMCtrlPolygon::Select()
{
  //Mark all nodes as selected  
  for (std::vector<COSMCtrlNode>::size_type i=0; i<m_Nodes.size(); i++)
    m_Nodes[i].m_bSelected = TRUE;
}

void COSMCtrlPolygon::Deselect()
{
  //Mark all nodes as selected  
  for (std::vector<COSMCtrlNode>::size_type i=0; i<m_Nodes.size(); i++)
    m_Nodes[i].m_bSelected = FALSE;
}

double COSMCtrlPolygon::Length() const
{
  //What will be the return value from this function
  double dDistance = 0;
  
  //Accumulate the distance between all of the nodes in this polyline
  for (std::vector<COSMCtrlNode>::size_type i=1; i<m_Nodes.size(); i++)
    dDistance += COSMCtrlHelper::DistanceBetweenPoints(m_Nodes[i - 1].m_Position, m_Nodes[i].m_Position, NULL, NULL);
  
  return dDistance;
}

BOOL COSMCtrlPolygon::GetBoundingRect(COSMCtrlPosition& topLeft, COSMCtrlPosition& bottomRight) const
{
  double fTopLatitude = -91;
  double fBottomLatitude = 91;
  double fLeftLongitude = 181;
  double fRightLongitude = -181;

  //Look through all the nodes to find the extreme values in latitude and longitude  
  std::vector<COSMCtrlNode>::size_type nNodes = m_Nodes.size();
  for (std::vector<COSMCtrlNode>::size_type i=0; i<nNodes; i++)
  {
    const COSMCtrlNode& node = m_Nodes[i];
    if (node.m_Position.m_fLatitude > fTopLatitude)
      fTopLatitude = node.m_Position.m_fLatitude;
    if (node.m_Position.m_fLatitude < fBottomLatitude)
      fBottomLatitude = node.m_Position.m_fLatitude;
    if (node.m_Position.m_fLongitude < fLeftLongitude)
      fLeftLongitude = node.m_Position.m_fLongitude;
    if (node.m_Position.m_fLongitude > fRightLongitude)
      fRightLongitude = node.m_Position.m_fLongitude;
  }

  topLeft.m_fLatitude = fTopLatitude;
  topLeft.m_fLongitude = fLeftLongitude;
  bottomRight.m_fLatitude = fBottomLatitude;
  bottomRight.m_fLongitude = fRightLongitude;  
  
  return (nNodes != 0); //A polygon without no nodes fails this function
}
