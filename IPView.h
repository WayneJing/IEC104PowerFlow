#pragma once

//

// CIPView ������ͼ

class CIPView : public CFormView
{
	DECLARE_DYNCREATE(CIPView)

protected:
	CIPView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CIPView();

public:
	enum { IDD = IDD_IPVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
};


