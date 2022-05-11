#include "Scene.h"

USING(Engine)

CScene::CScene(_DEVICE _pDevice)
	: m_pDevice(_pDevice)
{
	Safe_AddRef(m_pDevice);
}

HRESULT CScene::Ready_Scene()
{
	return S_OK;
}

_uint CScene::Update_Scene(const _float& _fDeltaTime)
{
	

	return 0;
}

_uint CScene::LateUpdate_Scene(const _float& _fDeltaTime)
{
	return 0;
}

void CScene::Free()
{
	Safe_Release(m_pDevice);
}
