/*
Module : OSMCtrlPolygon.H
Purpose: Defines the interface for COSMCtrlPolygon class
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

#ifndef __OSMCTRLPOLYGON_H__
#define __OSMCTRLPOLYGON_H__

#include "OSMCtrlPosition.h"
#include "OSMCtrlNode.h"

#ifndef _VECTOR_
#pragma message("To avoid this message, please put vector in your pre compiled header (normally stdafx.h)")
#include <vector>
#endif


////////////////////////////////// Classes ////////////////////////////////////


//A class which represents a polygon (i.e. a closed sequence of nodes) on the map
class COSMCtrlPolygon
{
public:
//Constructors / Destructors
  COSMCtrlPolygon();
  COSMCtrlPolygon(const COSMCtrlPolygon& polygon);

//Enums
  enum DrawingStyle
  {
    LinesOnly,
    LinesAndNodes,
  };
  
//Methods
  COSMCtrlPolygon& operator=(const COSMCtrlPolygon& polygon);
  BOOL   FullySelected() const;
  BOOL   AnySelected() const;
  void   Select();
  void   Deselect();
  double Length() const;
  BOOL   GetBoundingRect(COSMCtrlPosition& topLeft, COSMCtrlPosition& bottomRight) const;
  
//Member variables
  std::vector<COSMCtrlNode>           m_Nodes;               //The array of position for this polyline
  float                               m_fLinePenWidth;       //The width of the line to draw
#ifdef COSMCTRL_NOD2D
  Gdiplus::Color                      m_colorPen;            //The color of the pen to use
  Gdiplus::Color                      m_colorNode;           //The color of the node
  Gdiplus::Color                      m_colorSelectionNode;  //The color of the brush to use for selection nodes
  Gdiplus::Color                      m_colorBrush;          //The color of the brush to use
  Gdiplus::DashCap                    m_DashCap;             //The dash cap to use
  Gdiplus::DashStyle                  m_DashStyle;           //The dash style to use
  Gdiplus::LineCap                    m_EndCap;              //The end cap to use
  Gdiplus::LineCap                    m_StartCap;            //The start cap to use
  Gdiplus::LineJoin                   m_LineJoin;            //The line join style to use
  float                               m_fDashOffset;         //The dash offset to use
#else
  D2D1::ColorF                        m_colorPen;            //The color of the pen to use
  D2D1::ColorF                        m_colorNode;           //The color of the node
  D2D1::ColorF                        m_colorSelectionNode;  //The color of the brush to use for selection nodes
  D2D1::ColorF                        m_colorBrush;          //The color of the brush to use
  D2D1_CAP_STYLE                      m_DashCap;             //The dash cap to use
  D2D1_DASH_STYLE                     m_DashStyle;           //The dash style to use
  D2D1_CAP_STYLE                      m_EndCap;              //The end cap to use
  D2D1_CAP_STYLE                      m_StartCap;            //The start cap to use
  D2D1_LINE_JOIN                      m_LineJoin;            //The line join style to use
  FLOAT                               m_fDashOffset;         //The dash offset to use
  FLOAT                               m_fMiterLimit;         //The limit of the thickness of the join on a mitered corner
#endif
  float                               m_fNodeWidth;          //The width of the node to draw
  float                               m_fSelectionNodeWidth; //The width of the node to draw if selected
  DrawingStyle                        m_DrawingStyle;        //The drawing style to use for this polyline
  CString                             m_sToolTipText;        //The tooltip text for this polygon
  BOOL                                m_bDraggable;          //Is this polygon draggable
  BOOL                                m_bVisible;            //Is this polygon currently visible
  BOOL                                m_bEditable;           //Is this polygon editable
  BOOL                                m_bHitTest;            //Is this polygon to be hit-tested
  int                                 m_nMinZoomLevel;       //The minimum zoom level at which this polyline appears
  int                                 m_nMaxZoomLevel;       //The max zoom level at which this polyline appears
};


#endif //__OSMCTRLPOLYGON_H__
