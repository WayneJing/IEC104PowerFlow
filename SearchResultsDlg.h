///////////////////////// Macros / Includes ///////////////////////////////////

#pragma once

#include "cnominatim.h" //If you get a compilation error about this missing header file, then you need to download my CNominatim class from http://www.naughter.com/nominatim.html

#ifndef _VECTOR_
#pragma message("To avoid this message, please put vector in your pre compiled header (normally stdafx.h)")
#include <vector>
#endif


////////////////////////////////// Classes ////////////////////////////////////

class CSearchResultsDlg : public CDialog
{
public:
//Constructors / Destructors
  CSearchResultsDlg(CWnd* pParent = NULL);
  virtual ~CSearchResultsDlg();

//Member variables
  enum { IDD = IDD_SEARCH_RESULTS };
  CListBox m_ctrlResults;
  CArray<CNominatimSearchPlace, CNominatimSearchPlace&>* m_pResults;
  int m_nResult;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);

  DECLARE_DYNAMIC(CSearchResultsDlg)

  DECLARE_MESSAGE_MAP()

  afx_msg void OnLbnDblclkSearchResults();
};
