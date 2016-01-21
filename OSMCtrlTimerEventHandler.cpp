/*
Module : OSMCtrlMarker.cpp
Purpose: Implementation for the COSMCtrlTimerEventHandler class
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
#include "OSMCtrlTimerEventHandler.h"
#include "OSMCtrl.h"


///////////////////////////////// Implementation //////////////////////////////


#ifndef COSMCTRL_NOANIMATION
COSMCtrlTimerEventHandler::COSMCtrlTimerEventHandler() : m_pOSMCtrl(NULL)
{
}

HRESULT COSMCtrlTimerEventHandler::OnPreUpdate()
{
  return S_OK;
}

HRESULT COSMCtrlTimerEventHandler::OnPostUpdate()
{
  //Validate our parameters
  AFXASSUME(m_pOSMCtrl);

  //Delegate the work to the control
  m_pOSMCtrl->HandleAnimationTimerEvent();

  return S_OK;
}

HRESULT COSMCtrlTimerEventHandler::OnRenderingTooSlow(UINT32 /*framesPerSecond*/)
{
  return S_OK;
}

HRESULT COSMCtrlTimerEventHandler::CreateInstance(ATL::CComObjectNoLock<COSMCtrlTimerEventHandler>** ppTimerEventHandler)
{
  ATLASSERT(ppTimerEventHandler != NULL);
  if (ppTimerEventHandler == NULL)
    return E_POINTER;
  *ppTimerEventHandler = NULL;

  HRESULT hRes = E_OUTOFMEMORY;
  CComObjectNoLock<COSMCtrlTimerEventHandler>* p = NULL;
  ATLTRY(p = new CComObjectNoLock<COSMCtrlTimerEventHandler>())
  if (p != NULL)
  {
    p->SetVoid(NULL);
    p->InternalFinalConstructAddRef();
    hRes = p->_AtlInitialConstruct();
    if (SUCCEEDED(hRes))
      hRes = p->FinalConstruct();
    if (SUCCEEDED(hRes))
      hRes = p->_AtlFinalConstruct();
    p->InternalFinalConstructRelease();
    if (hRes != S_OK)
    {
      delete p;
      p = NULL;
    }
  }
  *ppTimerEventHandler = p;	
  return hRes;
}
#endif //#ifndef COSMCTRL_NOANIMATION
