// ControlView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Tool.h"
#include "ControlView.h"

#include "MapView.h"
#include "ObjectView.h"

#include "FileInfo.h"
#include "StaticMesh_Object.h"
#include "DynamicMesh_Object.h"


// CControlView

IMPLEMENT_DYNCREATE(CControlView, CFormView)

CControlView::CControlView()
	: CFormView(IDD_CONTROLVIEW)
{

}

CControlView::~CControlView()
{
	Safe_Delete(m_pMapView);
	Safe_Delete(m_pObjectView);
}

void CControlView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_TabCtrl);
}

BEGIN_MESSAGE_MAP(CControlView, CFormView)
	ON_WM_DROPFILES()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CControlView::OnTcnSelchangeTab)
END_MESSAGE_MAP()


// CControlView 진단입니다.

#ifdef _DEBUG
void CControlView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CControlView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CControlView 메시지 처리기입니다.


void CControlView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	SetTimer(1000, 500, nullptr);

	m_pManagement = GET_MANAGEMENT;
	m_pDevice = m_pManagement->Get_Device();
	Safe_AddRef(m_pDevice);

	m_TabCtrl.InsertItem(0, _T("Map"));
	m_TabCtrl.InsertItem(1, _T("Object"));
	m_TabCtrl.SetCurSel(0);

	CRect rc;
	m_TabCtrl.GetWindowRect(&rc);

	m_pMapView = new CMapView;
	m_pMapView->Create(IDD_MAPVIEW, &m_TabCtrl);
	m_pMapView->MoveWindow(0, 25, rc.Width(), rc.Height());
	m_pMapView->ShowWindow(SW_SHOW);

	m_pObjectView = new CObjectView;
	m_pObjectView->Create(IDD_OBJECTVIEW, &m_TabCtrl);
	m_pObjectView->MoveWindow(0, 25, rc.Width(), rc.Height());
	m_pObjectView->ShowWindow(SW_HIDE);
}

void CControlView::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_uint iIndex = m_TabCtrl.GetCurSel();

	switch (iIndex)
	{
	case 0:
		m_pMapView->ShowWindow(SW_SHOW);
		m_pObjectView->ShowWindow(SW_HIDE);
		break;
	case 1:
		m_pMapView->ShowWindow(SW_HIDE);
		m_pObjectView->ShowWindow(SW_SHOW);
		break;
	}

	*pResult = 0;
}
