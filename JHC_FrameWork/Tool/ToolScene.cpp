#include "stdafx.h"
#include "ToolScene.h"

_float3 g_vPickingPos;

CToolScene::CToolScene(_DEVICE _pDevice)
	: CScene(_pDevice)
{
}

HRESULT CToolScene::Ready_Scene()
{
	if (FAILED(CScene::Ready_Scene()))
	{
		ERROR_BOX(L"Failed to Ready_Scene");
		return E_FAIL;
	}

	if (FAILED(Add_Camera_Layer(L"Camera_Layer")))
	{
		ERROR_BOX(L"Failed to Add_Camera_Layer");
		return E_FAIL;
	}

	if (FAILED(Add_LightInfo()))
	{
		ERROR_BOX(L"Failed to Add_LightInfo");
		return E_FAIL;
	}

	g_vPickingPos = _float3(0.f, 0.f, 0.f);

	return S_OK;
}

_uint CToolScene::Update_Scene(const _float& _fDeltaTime)
{
	CScene::Update_Scene(_fDeltaTime);

	return (_uint)EEvent::None;
}

_uint CToolScene::LateUpdate_Scene(const _float& _fDeltaTime)
{
	CScene::LateUpdate_Scene(_fDeltaTime);

	CManagement* pManagement = GET_MANAGEMENT;

	if (FAILED(pManagement->Add_RenderSceneList(this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return 0;
	}

	return (_uint)EEvent::None;
}

_uint CToolScene::Render_Scene()
{

	CManagement* pManagement = GET_MANAGEMENT;
	TCHAR szPickingPos[64] = L"";


	swprintf_s(szPickingPos, L"PickingPos: %f, %f, %f", g_vPickingPos.x, g_vPickingPos.y, g_vPickingPos.z);

	D3DCOLOR Color = D3DCOLOR_ARGB(255, 255, 0, 0);
	pManagement->Render_Font(L"Font_Default", szPickingPos, &_float2(20, 20), Color);

	return _uint();
}

HRESULT CToolScene::Add_Camera_Layer(const wstring & _TerrainTag)
{
	auto pManagement = GET_MANAGEMENT;

	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return E_FAIL;
	}

	CAMERA_DESC tCameraDesc;
	tCameraDesc.vEye = _float3(0.f, 10.f, -5.f);
	tCameraDesc.vAt = _float3(0.f, 0.f, 1.f);
	tCameraDesc.vUp = _float3(0.f, 1.f, 0.f);
	tCameraDesc.fFovY = D3DXToRadian(60.f);
	tCameraDesc.fAspect = (_float)WINCX / WINCY;
	tCameraDesc.fNear = 0.1f;
	tCameraDesc.fFar = 1000.f;

	if (FAILED(pManagement->Clone_GameObject_ToLayer((_uint)ESceneType::Static, L"GameObject_DynamicCamera", (_uint)ESceneType::Logo, _TerrainTag, &tCameraDesc)))
	{
		ERROR_BOX(L"Failed to Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CToolScene::Add_LightInfo()
{
	D3DLIGHT9 tLightInfo;
	ZeroMemory(&tLightInfo, sizeof(D3DLIGHT9));

	tLightInfo.Type = D3DLIGHT_DIRECTIONAL;
	tLightInfo.Diffuse = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Specular = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
	tLightInfo.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	tLightInfo.Direction = _float3(1.f, -1.f, 1.f);

	auto pManagement = GET_MANAGEMENT;

	if (FAILED(pManagement->Ready_Light(m_pDevice, &tLightInfo, 0)))
	{
		ERROR_BOX(L"Failed to Ready_Light");
		return E_FAIL;
	}

	return S_OK;
}

CToolScene * CToolScene::Create(_DEVICE _pDevice)
{
	CToolScene* pInstance = new CToolScene(_pDevice);

	if (FAILED(pInstance->Ready_Scene()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToolScene::Free()
{
	CScene::Free();
}
