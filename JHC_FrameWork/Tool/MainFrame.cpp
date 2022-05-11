
// ../Codes/MainFrame.cpp : CMainFrame Ŭ������ ����
//

#include "stdafx.h"
#include "Tool.h"

#include "MainFrame.h"
#include "ToolView.h"
#include "ControlView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ���� �� ǥ�ñ�
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/�Ҹ�

CMainFrame::CMainFrame()
{
	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.
}

CMainFrame::~CMainFrame()
{
	CManagement::Release_Engine();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.


	cs.cx = WINCX + (550);				// MFC ������â Xũ�� ����
	cs.cy = WINCY;						// MFC ������â Yũ�� ����
	cs.style &= ~WS_THICKFRAME;			// MFC ������â ũ�� ���� ��� ��Ȱ��ȭ, ~ : ��ư ����
	cs.style &= ~WS_MAXIMIZEBOX;		// MFC ������â �ִ�ȭ ��� ��Ȱ��ȭ, ~ : ��ư ����

	return TRUE;
}

// CMainFrame ����

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame �޽��� ó����

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	//return CFrameWnd::OnCreateClient(lpcs, pContext);
	if (!m_MainSplitter.CreateStatic(this, 1, 2))
	{
		return FALSE;
	}

	if (!m_MainSplitter.CreateView(0, 0, RUNTIME_CLASS(CToolView), CSize(0, 0), pContext))
	{
		return FALSE;
	}


	if (!m_MainSplitter.CreateView(0, 1, RUNTIME_CLASS(CControlView), CSize(0, 0), pContext))
	{
		return FALSE;
	}

	m_MainSplitter.SetColumnInfo(0, WINCX, WINCX);

	return TRUE;
}
