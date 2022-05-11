
// ../Codes/ToolView.cpp : CToolView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "Tool.h"
#endif

#include "ToolDoc.h"
#include "ToolView.h"
#include "MainFrame.h"
#include "ControlView.h"
#include "ObjectView.h"

#include "ToolScene.h"
#include "DynamicCamera.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CToolView

IMPLEMENT_DYNCREATE(CToolView, CView)

BEGIN_MESSAGE_MAP(CToolView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CToolView ����/�Ҹ�
HWND g_hWnd;

CToolView::CToolView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CToolView::~CToolView()
{
}

BOOL CToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CToolView �׸���

void CToolView::Screen_Setting()
{
	g_hWnd = m_hWnd;

	m_pMain = dynamic_cast<CMainFrame*>(AfxGetApp()->GetMainWnd());
	m_pControlView = dynamic_cast<CControlView*>(m_pMain->m_MainSplitter.GetPane(0, 1));
	m_pObjectView = dynamic_cast<CObjectView*>(m_pControlView->m_pObjectView);

	RECT rcMain = {};
	// ������â ũ�� �޾ƿ���
	m_pMain->GetWindowRect(&rcMain);
	SetRect(&rcMain, 0, 0, rcMain.right - rcMain.left, rcMain.bottom - rcMain.top);

	// MainViewâ ũ�� �޾ƿ���
	RECT rcMainView = {};
	GetClientRect(&rcMainView);

	int iWidth = rcMain.right - rcMainView.right;		//�¿� ����
	int iHeight = rcMain.bottom - rcMainView.bottom;	//���� ����

														//������â ũ�� �缳��. 800 + �¿� ���� + ��ũ��(18), 600 + ���� ����(��ũ�� �ڵ����õ�)
	m_pMain->SetWindowPos(nullptr, 0, 0, WINCX + iWidth + 18, WINCY + iHeight, SWP_NOZORDER);
}

void CToolView::Device_Seeting()
{
	m_pManagement = GET_MANAGEMENT;

	if (FAILED(m_pManagement->Ready_Engine(g_hInst, g_hWnd, WINCX, WINCY, EDisplayMode::Win, EBuildSetting::Tool, (_uint)ESceneType::EndID)))
	{
		ERROR_BOX(L"Failed To Ready_Engine");
		return;
	}

	m_pDevice = m_pManagement->Get_Device();
	if (nullptr == m_pDevice)
	{
		ERROR_BOX(L"Failed To Get_Device");
		return;
	}

	Safe_AddRef(m_pDevice);

	if (FAILED(Ready_StaticResources()))
	{
		ERROR_BOX(L"Failed to Ready_StaticResources");
		return;
	}

	if (FAILED(m_pManagement->Setup_CurrentScene((_int)ESceneType::Logo, CToolScene::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed To Setup_CurrentScene");
		return;
	}

	if (FAILED(Setup_DefaultSetting()))
	{
		ERROR_BOX(L"Failed To Setup_DefaultSetting");
		return;
	}

	if (FAILED(m_pManagement->Ready_Font(m_pDevice, L"Font_Default", L"���", 15, 15, FW_HEAVY)))
	{
		ERROR_BOX(L"Failed To Ready_Font");
		return ;
	}
}

HRESULT CToolView::Ready_StaticResources()
{
	CManagement* pManagement = GET_MANAGEMENT;

	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

#pragma region Component
	/* For.Component_Transform */
	if (FAILED(m_pManagement->Add_Component_Prototype((_uint)ESceneType::Static, L"Component_Transform", CTransform::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed to Add_Component_Prototype");
		return E_FAIL;
	}

	/* For.Component_Collider_Cube */
	if (FAILED(m_pManagement->Add_Component_Prototype((_uint)ESceneType::Static, L"Component_Collider_Cube", CCube_Collider::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed to Add_Component_Prototype");
		return E_FAIL;
	}

	/* For.Component_Collider_Sphere */
	if (FAILED(m_pManagement->Add_Component_Prototype((_uint)ESceneType::Static, L"Component_Sphere_Collider", CSphere_Collider::Create(m_pDevice, 1.f, 10, 10))))
	{
		ERROR_BOX(L"Failed to Add_Component_Prototype");
		return E_FAIL;
	}

	/* For.Component_Calculator */
	if (FAILED(m_pManagement->Add_Component_Prototype((_uint)ESceneType::Static, L"Component_Calculator", CCalculator::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed to Add_Component_Prototype");
		return E_FAIL;
	}

	/* For.Component_VIBuffer_xyzLineColor */
	if (FAILED(m_pManagement->Add_Component_Prototype((_uint)ESceneType::Static, L"Component_XYZLine", CXYZLine::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed to Add_Component_Prototype");
		return E_FAIL;
	}
#pragma endregion

#pragma region GameObject
	/* For.GameObject_DynamicCamera */
	if (FAILED(pManagement->Add_GameObject_Prototype((_uint)ESceneType::Static, L"GameObject_DynamicCamera", CDynamicCamera::Create(m_pDevice))))
	{
		ERROR_BOX(L"Failed to Add_GameObject_Prototype");
		return E_FAIL;
	}
#pragma endregion

	return S_OK;
}

HRESULT CToolView::Setup_DefaultSetting()
{
	m_pDevice->SetRenderState(D3DRS_LIGHTING, false);

	if (FAILED(m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE)))
	{
		ERROR_BOX(L"Failed To SetRenderState");
		return E_FAIL;
	}

	return S_OK;
}

void CToolView::Picking_Object()
{
	/*vector<CGameObject*> vecGameObject = m_pObjectView->Get_GameObjects();
	_uint iSize = (_uint)vecGameObject.size();

	for (_uint i = 0; i < iSize; ++i)
	{
		dynamic_cast<CSphere_Collider*>(vecGameObject[i]->Get_Component(L"ss"));
	}
*/


	//_float fDir = 999999.f;
	//_float3 vPos;
	//_float3 vMaxPos;
	//CMesh::EPickingDir ePickingDir;

	//CDynamicCamera* pCamera = dynamic_cast<CDynamicCamera*>(m_pManagement->Get_GameObject((_uint)ESceneType::Logo, L"Camera_Layer"));

	//for (_uint i = 0; i < iSize; ++i)
	//{
	//	CMesh* pMesh = dynamic_cast<CMesh*>(vecGameObject[i]->Get_Mesh(L"Com_Mesh"));
	//	CTransform* pTransform = dynamic_cast<CTransform*>(vecGameObject[i]->Get_Component(L"Com_Transform"));
	//	_float4x4 matTerrainWolrd = pTransform->Get_TransformDesc().matWorld;
	//	_float3 vOutPos;
	//	_float fOutDir;

	//	_bool bPicking = pMesh->Picking_Mesh<_float3, INDEX16>
	//		(
	//			&vOutPos,
	//			&ePickingDir,
	//			&fOutDir,
	//			g_hWnd,
	//			WINCX,
	//			WINCY,
	//			pCamera,
	//			&matTerrainWolrd
	//		);

	//	if (bPicking)
	//	{
	//		if (fDir > fOutDir)
	//		{
	//			fDir = fOutDir;
	//			memcpy(&vPos, &matTerrainWolrd.m[3][0], sizeof(_float3));
	//			vMaxPos = *pMesh->Get_VtxPos();
	//			/*iSelect = i;
	//			bPickingAdd = true;
	//			
	//			memcpy(&vNearGoalScale, &vScale, sizeof(_float3));*/
	//		}
	//	}
	//}

	//if (fDir != 999999.f)
	//{
	//	wstring wstr = L"X : " + to_wstring(vMaxPos.x) + L"Y : " + to_wstring(vMaxPos.y) + L"Z : " + to_wstring(vMaxPos.z);
	//	Print_Debug_Str(wstr);

	//	switch (ePickingDir)
	//	{
	//	case CMesh::EPickingDir::X:
	//		Print_Debug_Str(L"X");
	//		//vNearGoalPos.x += ((vNearGoalScale.x + m_pMapTool->m_vScale.x) / 2.f);
	//		break;
	//	case CMesh::EPickingDir::_X:
	//		Print_Debug_Str(L"-X");
	//		//vNearGoalPos.x -= ((vNearGoalScale.x + m_pMapTool->m_vScale.x) / 2.f);
	//		break;
	//	case CMesh::EPickingDir::Y:
	//		Print_Debug_Str(L"Y");
	//		//vNearGoalPos.y += ((vNearGoalScale.y + m_pMapTool->m_vScale.y) / 2.f);
	//		break;
	//	case CMesh::EPickingDir::_Y:
	//		Print_Debug_Str(L"-Y");
	//		//vNearGoalPos.y -= ((vNearGoalScale.y + m_pMapTool->m_vScale.y) / 2.f);
	//		break;
	//	case CMesh::EPickingDir::Z:
	//		Print_Debug_Str(L"Z");
	//		//vNearGoalPos.z += ((vNearGoalScale.z + m_pMapTool->m_vScale.z) / 2.f);
	//		break;
	//	case CMesh::EPickingDir::_Z:
	//		Print_Debug_Str(L"-Z");
	//		//vNearGoalPos.z -= ((vNearGoalScale.z + m_pMapTool->m_vScale.z) / 2.f);
	//		break;
	//	}
	//}
}

void CToolView::OnDraw(CDC* /*pDC*/)
{
	CToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CToolView �μ�

BOOL CToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CToolView ����

#ifdef _DEBUG
void CToolView::AssertValid() const
{
	CView::AssertValid();
}

void CToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CToolDoc* CToolView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToolDoc)));
	return (CToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CToolView �޽��� ó����


void CToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	Screen_Setting();
	Device_Seeting();

	

	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
}


void CToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	Picking_Object();

	CView::OnLButtonDown(nFlags, point);
}
