#pragma once



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

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};


