/*
Module : OSMCtrlTimerEventHandler.H
Purpose: Defines the interface for COSMCtrlTimerEventHandler class
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

#ifndef __OSMCTRLTIMEREVENTHANDLER_H__
#define __OSMCTRLTIMEREVENTHANDLER_H__


////////////////////////////////// Classes ////////////////////////////////////


//Forward declaration
class COSMCtrl;

#ifndef COSMCTRL_NOANIMATION
//The timer event handler class for Windows Animation support
class ATL_NO_VTABLE COSMCtrlTimerEventHandler : public ATL::CComObjectRootEx<ATL::CComSingleThreadModel>,
                                                public IUIAnimationTimerEventHandler
{
public:
//Constructors / Destructors
  COSMCtrlTimerEventHandler();

//Methods
  static HRESULT CreateInstance(ATL::CComObjectNoLock<COSMCtrlTimerEventHandler>** ppTimerEventHandler);

  BEGIN_COM_MAP(COSMCtrlTimerEventHandler)
    COM_INTERFACE_ENTRY(IUIAnimationTimerEventHandler)
  END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()
  DECLARE_NOT_AGGREGATABLE(COSMCtrlTimerEventHandler)

//IUIAnimationTimerEventHandler support
  virtual HRESULT STDMETHODCALLTYPE OnPreUpdate();
  virtual HRESULT STDMETHODCALLTYPE OnPostUpdate();
  virtual HRESULT STDMETHODCALLTYPE OnRenderingTooSlow(UINT32 framesPerSecond);

//Member variables
  COSMCtrl* m_pOSMCtrl;
};
#endif


#endif //__OSMCTRLTIMEREVENTHANDLER_H__
