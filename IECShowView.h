#pragma once

//

// CIECShowView ������ͼ

class CIECShowView : public CFormView
{
	DECLARE_DYNCREATE(CIECShowView)

protected:
	CIECShowView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CIECShowView();

public:
	enum { IDD = IDD_IECSHOWVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const; 
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
public:
	//CEdit* pBox = (CEdit*)GetDlgItem(IDC_EDIT1);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	
protected:
	afx_msg LRESULT OnShowiecdata(WPARAM wParam, LPARAM lParam);
public:
	std::vector<int> v_box;
};


