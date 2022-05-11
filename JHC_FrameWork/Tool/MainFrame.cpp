
// ../Codes/MainFrame.cpp : CMainFrame 클래스의 구현
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
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
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
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.


	cs.cx = WINCX + (550);				// MFC 윈도우창 X크기 조절
	cs.cy = WINCY;						// MFC 윈도우창 Y크기 조절
	cs.style &= ~WS_THICKFRAME;			// MFC 윈도우창 크기 조절 기능 비활성화, ~ : 버튼 보임
	cs.style &= ~WS_MAXIMIZEBOX;		// MFC 윈도우창 최대화 기능 비활성화, ~ : 버튼 보임

	return TRUE;
}

// CMainFrame 진단

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


// CMainFrame 메시지 처리기

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

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
