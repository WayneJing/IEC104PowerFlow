/*
Module : OSMCtrlCircle.H
Purpose: Defines the interface for COSMCtrlCircle class
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

#ifndef __OSMCTRLCIRCLE_H__
#define __OSMCTRLCIRCLE_H__

#include "OSMCtrlPosition.h"


////////////////////////////////// Classes ////////////////////////////////////


//A class which represents a circle (i.e. a closed sequence of nodes with a specific radius) on the map
class COSMCtrlCircle
{
public:
//Constructors / Destructors
  COSMCtrlCircle();
  COSMCtrlCircle(const COSMCtrlCircle& polygon);

//Enums
  enum DrawingStyle
  {
    LineOnly,
    LineAndInside,
  };

//Methods
  COSMCtrlCircle& operator=(const COSMCtrlCircle& polygon);
  BOOL GetBoundingRect(COSMCtrlPosition& topLeft, COSMCtrlPosition& bottomRight) const;
  
//Member variables
  COSMCtrlPosition                    m_Position;            //The position of this circle
  double                              m_fRadius;             //The radius distance in meters
  int								  relatedBus;
  BOOL                                m_bSelected;           //Is this circle selected
  float                               m_fLinePenWidth;       //The width of the line to draw
#ifdef COSMCTRL_NOD2D
  Gdiplus::Color                      m_colorPen;            //The color of the pen to use
  Gdiplus::Color                      m_colorSelection;      //The color of the brush to use for selection
  Gdiplus::Color                      m_colorBrush;          //The color of the brush to use
  Gdiplus::DashStyle                  m_DashStyle;           //The dash style to use
#else
  D2D1::ColorF                        m_colorPen;            //The color of the pen to use
  D2D1::ColorF                        m_colorSelection;      //The color of the brush to use for selection
  D2D1::ColorF                        m_colorBrush;          //The color of the brush to use
  D2D1_DASH_STYLE                     m_DashStyle;           //The dash style to use
#endif
  DrawingStyle                        m_DrawingStyle;        //The drawing style to use for this circle
  CString                             m_sToolTipText;        //The tooltip text for this circle
  BOOL                                m_bDraggable;          //Is this circle draggable
  BOOL                                m_bVisible;            //Is this circle currently visible
  BOOL                                m_bEditable;           //Is this circle editable
  BOOL                                m_bHitTest;            //Is this circle to be hit-tested
  int                                 m_nMinZoomLevel;       //The minimum zoom level at which this circle appears
  int                                 m_nMaxZoomLevel;       //The max zoom level at which this circle appears
};


#endif //__OSMCTRLCIRCLE_H__
