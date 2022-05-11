
// ../Headers/ToolView.h : CToolView Ŭ������ �������̽�
//

#pragma once
class CToolDoc;
class CMainFrame;
class CControlView;
class CObjectView;

class CToolView : public CView
{
protected: // serialization������ ��������ϴ�.
	CToolView();
	DECLARE_DYNCREATE(CToolView)

// Ư���Դϴ�.
public:
	CToolDoc* GetDocument() const;
	CMainFrame*		m_pMain;
	CManagement*	m_pManagement;
	CControlView*	m_pControlView;
	CObjectView*	m_pObjectView;

private:
	_DEVICE			m_pDevice;

// �۾��Դϴ�.
public:
	void Screen_Setting();
	void Device_Seeting();
	HRESULT Ready_StaticResources();
	HRESULT Setup_DefaultSetting();

private:
	void Picking_Object();

// �������Դϴ�.
public:
	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // ../Codes/ToolView.cpp�� ����� ����
inline CToolDoc* CToolView::GetDocument() const
   { return reinterpret_cast<CToolDoc*>(m_pDocument); }
#endif

