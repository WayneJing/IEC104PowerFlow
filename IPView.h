#pragma once

//

// CIPView 窗体视图

class CIPView : public CFormView
{
	DECLARE_DYNCREATE(CIPView)

protected:
	CIPView();           // 动态创建所使用的受保护的构造函数
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
};


