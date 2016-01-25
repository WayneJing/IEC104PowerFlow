/*
Module : OSMCtrlPolyline.H
Purpose: Defines the interface for COSMCtrlPolyline class
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

#ifndef __OSMCTRLPOLYLINE_H__
#define __OSMCTRLPOLYLINE_H__

#include "OSMCtrlNode.h"

#ifndef _VECTOR_
#pragma message("To avoid this message, please put vector in your pre compiled header (normally stdafx.h)")
#include <vector>
#endif


////////////////////////////////// Classes ////////////////////////////////////


//A class which represents a polyline (i.e. a sequence of nodes) on the map
class COSMCtrlPolyline
{
public:
//Constructors / Destructors
  COSMCtrlPolyline();
  COSMCtrlPolyline(const COSMCtrlPolyline& polyline);

//Enums
  enum DrawingStyle
  {
    LinesOnly,
    LinesAndNodes,
    NodesOnly
  };
  
//Methods
  COSMCtrlPolyline& operator=(const COSMCtrlPolyline& polyline);
  BOOL   FullySelected() const;
  BOOL   AnySelected() const;
  void   Select();
  void   Deselect();
  double Length() const; //returns the length of this polyline in kilometers
  double LastBearing() const; //Returns the start bearing between the second last and last node in this polyline
  BOOL   GetBoundingRect(COSMCtrlPosition& topLeft, COSMCtrlPosition& bottomRight) const;
  
//Member variables
  std::vector<COSMCtrlNode>           m_Nodes;                //The array of position for this polyline
  float                               m_fLinePenWidth;        //The width of the line to draw
  int						  relatedBranch;
#ifdef COSMCTRL_NOD2D
  Gdiplus::Color                      m_colorPen;             //The color of the pen to use
  Gdiplus::Color                      m_colorNode;            //The color of the node
  Gdiplus::Color                      m_colorSelectionNode;   //The color of the brush to use for selection nodes
  Gdiplus::DashCap                    m_DashCap;              //The dash cap to use
  Gdiplus::DashStyle                  m_DashStyle;            //The dash style to use
  Gdiplus::LineCap                    m_EndCap;               //The end cap to use
  Gdiplus::LineCap                    m_StartCap;             //The start cap to use
  Gdiplus::LineJoin                   m_LineJoin;             //The line join style to use
  float                               m_fDashOffset;          //The dash offset to use
#else
  D2D1::ColorF                        m_colorPen;             //The color of the pen to use
  D2D1::ColorF                        m_colorNode;            //The color of the node
  D2D1::ColorF                        m_colorSelectionNode;   //The color of the brush to use for selection nodes
  D2D1_CAP_STYLE                      m_DashCap;              //The dash cap to use
  D2D1_DASH_STYLE                     m_DashStyle;            //The dash style to use
  D2D1_CAP_STYLE                      m_EndCap;               //The end cap to use
  D2D1_CAP_STYLE                      m_StartCap;             //The start cap to use
  D2D1_LINE_JOIN                      m_LineJoin;             //The line join style to use
  FLOAT                               m_fDashOffset;          //The dash offset to use
  FLOAT                               m_fMiterLimit;          //The limit of the thickness of the join on a mitered corner
#endif
  float                               m_fNodeWidth;           //The width of the node to draw
  float                               m_fSelectionNodeWidth;  //The width of the node to draw if selected
  DrawingStyle                        m_DrawingStyle;         //The drawing style to use for this polyline
  CString                             m_sToolTipText;         //The tooltip text for this polyline
  BOOL                                m_bDraggable;           //Is this polyline draggable
  BOOL                                m_bVisible;             //Is this polyline currently visible
  BOOL                                m_bEditable;            //Is this polyline editable
  BOOL                                m_bHitTest;             //Is this polyline to be hit-tested
  int                                 m_nMinZoomLevel;        //The minimum zoom level at which this polyline appears
  int                                 m_nMaxZoomLevel;        //The max zoom level at which this polyline appears

  float fLon;
  float fLat;
  float tLon;
  float tLat;
  float voltage;



};


#endif //__OSMCTRLPOLYLINE_H__
