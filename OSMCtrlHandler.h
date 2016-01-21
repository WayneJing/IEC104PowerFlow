/*
Module : OSMCtrlHandler.H
Purpose: Defines the interface for IOSMCtrlHandler class
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

#ifndef __OSMCTRLHANDLER_H__
#define __OSMCTRLHANDLER_H__


////////////////////////////////// Classes ////////////////////////////////////


//The event handler class
class IOSMCtrlHandler
{
public:
//Methods
  virtual BOOL OnOSMCtrlLButtonDown(UINT /*nFlags*/, CPoint /*point*/)
  { 
    return FALSE;
  }
  virtual BOOL OnOSMCtrlMouseMove(UINT/*nFlags*/, CPoint /*point*/)
  {
    return FALSE;
  }
  virtual BOOL OnOSMCtrlLButtonUp(UINT /*nFlags*/, CPoint /*point*/)
  {
    return FALSE;
  }
  virtual BOOL OnOSMCtrlKeyDown(UINT /*nChar*/, UINT /*nRepCnt*/, UINT /*nFlags*/)
  {
    return FALSE;
  }
  virtual BOOL OnOSMCtrlLButtonDblClk(UINT /*nFlags*/, CPoint /*point*/)
  {
    return FALSE;
  }
};


#endif //__OSMCTRLHANDLER_H__
