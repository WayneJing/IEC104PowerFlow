/*
Module : OSMCtrlMarker.H
Purpose: Defines the interface for COSMCtrlMarker class
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

#ifndef __OSMCTRLMARKER_H__
#define __OSMCTRLMARKER_H__

#include "OSMCtrlPosition.h"
#include "OSMCtrlHelper.h"


////////////////////////////////// Classes ////////////////////////////////////


//A class which represents a marker which is a icon at a specific position on the map
class COSMCtrlMarker
{
public:
//Constructors / Destructors
  COSMCtrlMarker() : m_nIconIndex(-1),
                     m_bSelected(FALSE),
                     m_bDraggable(FALSE),
                     m_bVisible(TRUE),
                     m_bHitTest(TRUE),
                     m_nMinZoomLevel(COSMCtrlHelper::OSMMinZoom),
                     m_nMaxZoomLevel(COSMCtrlHelper::OSMMaxZoom),
                   #ifdef COSMCTRL_NOD2D
                     m_colorSelection(255, 255, 0, 0),
                   #else   
                     m_colorSelection(255, 0, 0, 1.0F),
                   #endif
                     m_fSelectionPenWidth(2)
  {
  }

  COSMCtrlMarker(const COSMCtrlMarker& marker)
                                             #ifndef COSMCTRL_NOD2D
                                               : m_colorSelection(D2D1::ColorF::Black)
                                             #endif
  {
    *this = marker;
  }
  
//Methods
  COSMCtrlMarker& operator=(const COSMCtrlMarker& marker)
  {
    m_nIconIndex         = marker.m_nIconIndex;
    m_Position           = marker.m_Position;
    m_bSelected          = marker.m_bSelected;
    m_sToolTipText       = marker.m_sToolTipText;
    m_bDraggable         = marker.m_bDraggable;
    m_bVisible           = marker.m_bVisible;
    m_bHitTest           = marker.m_bHitTest;
    m_nMinZoomLevel      = marker.m_nMinZoomLevel;
    m_nMaxZoomLevel      = marker.m_nMaxZoomLevel;
    m_colorSelection     = marker.m_colorSelection;
    m_fSelectionPenWidth = marker.m_fSelectionPenWidth;
  
    return *this;
  }
  
//Member variables
  INT_PTR          m_nIconIndex;         //The index of the icon for this marker (Using this level of indirection means that we keep down memory usage so that multiple markers can reuse the same icon resource
  COSMCtrlPosition m_Position;           //The position of this marker
  BOOL             m_bSelected;          //Is this marker selected
  CString          m_sToolTipText;       //The tooltip text for this marker
  BOOL             m_bDraggable;         //Is this marker draggable
  BOOL             m_bVisible;           //Is this marker currently visible
  BOOL             m_bHitTest;           //Is this marker to be hit-tested
  int              m_nMinZoomLevel;      //The minimum zoom level at which this marker appears
  int              m_nMaxZoomLevel;      //The max zoom level at which this marker appears
#ifdef COSMCTRL_NOD2D
  Gdiplus::Color   m_colorSelection;     //The color of the brush to use for selection
#else
  D2D1::ColorF     m_colorSelection;     //The color of the brush to use for selection
#endif
  float            m_fSelectionPenWidth; //The width of the selection rectangle to draw
};


#endif //__OSMCTRLMARKER_H__
