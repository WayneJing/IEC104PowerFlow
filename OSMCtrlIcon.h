/*
Module : OSMCtrlIcon.H
Purpose: Defines the interface for COSMCtrlIcon class
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

#ifndef __OSMCTRLICON_H__
#define __OSMCTRLICON_H__


////////////////////////////////// Classes ////////////////////////////////////

//A class which represents an icon as placed on the main COSMCtrl class instance
class COSMCtrlIcon
{
public:
//constructors / destructors
  COSMCtrlIcon() : m_ptAnchor(0, 0),
                 #ifdef COSMCTRL_NOD2D
                   m_pImage(NULL)
                 #else
                   m_nResourceID(0),
                   m_pImage(NULL)
                 #endif
  {
  }

  COSMCtrlIcon(const COSMCtrlIcon& icon)
  {
    *this = icon;
  }

//Methods
  COSMCtrlIcon& operator=(const COSMCtrlIcon& icon)
  {
    m_ptAnchor = icon.m_ptAnchor;
  #ifdef COSMCTRL_NOD2D
    m_pImage   = icon.m_pImage;
  #else
    m_nResourceID   = icon.m_nResourceID;
    m_sResourceType = icon.m_sResourceType;
    m_sFilename     = icon.m_sFilename;
    m_pImage        = NULL; //We do not transfer across the cached image
  #endif  

    return *this;
  }

//Member variables
  CPoint          m_ptAnchor;          //The anchor position of the icon i.e. the offset in pixels from the top left of the icon which is anchored to the map
#ifdef COSMCTRL_NOD2D
  Gdiplus::Image* m_pImage;            //The actual image
#else
  UINT            m_nResourceID;       //The resource identifier to load the icon from
  CString         m_sResourceType;     //The resource type of the icon
  CString         m_sFilename;         //The filename to load the icon from
  CD2DBitmap*     m_pImage;            //The cached image
#endif
};


#endif //__OSMCTRLICON_H__
