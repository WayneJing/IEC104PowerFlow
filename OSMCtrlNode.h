/*
Module : OSMCtrlNode.H
Purpose: Defines the interface for COSMCtrlNode class
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

#ifndef __OSMCTRLNODE_H__
#define __OSMCTRLNODE_H__

#include "OSMCtrlPosition.h"


////////////////////////////////// Classes ////////////////////////////////////


//A class which represents a node on the map (a combination of COSMCtrlPosition and a selected state)
class COSMCtrlNode
{
public:
//Constructors / Destructors
  COSMCtrlNode() : m_bSelected(FALSE)
  {
  }

  COSMCtrlNode(double fLongitude, double fLatitude, double fBearing = 0, double fSpeed = 0, BOOL bSelected = FALSE, BOOL bBearingValid = FALSE, BOOL bSpeedValid = FALSE, double fHDOP = 0) : m_Position(fLongitude, fLatitude, fBearing, fSpeed, bBearingValid, bSpeedValid, fHDOP),
                                                                                                                                                                                              m_bSelected(bSelected)
  {
  }

  COSMCtrlNode(const COSMCtrlNode& node)
  {
    *this = node;
  }
  
//Methods
  COSMCtrlNode& operator=(const COSMCtrlNode& node)
  {
    m_Position = node.m_Position;
    m_bSelected = node.m_bSelected;

    return *this;
  }

//Member variables  
  COSMCtrlPosition m_Position;
  BOOL             m_bSelected;
};


#endif //__OSMCTRLNODE_H__
