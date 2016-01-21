#pragma once


class CSearchDlg : public CDialog
{
  DECLARE_DYNAMIC(CSearchDlg)

public:
//Constructors / Destructors
  CSearchDlg(CWnd* pParent = NULL);   // standard constructor
  virtual ~CSearchDlg();

//Member variables
  enum { IDD = IDD_SEARCH };
  CString m_sSearch;

protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

  DECLARE_MESSAGE_MAP()
};
