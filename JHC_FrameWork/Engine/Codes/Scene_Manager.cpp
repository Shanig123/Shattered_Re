#include "Scene_Manager.h"
#include "Scene.h"

USING(Engine)

IMPLEMENT_SINGLETON(CScene_Manager)

CScene_Manager::CScene_Manager()
	: m_pCurrentScene(nullptr)
	, m_iCurrentSceneIndex(-1)
{
}

HRESULT CScene_Manager::Setup_CurrentScene(_int _iSceneIndex, CScene * _pCurrentScene)
{
	if (nullptr == _pCurrentScene)
	{
		ERROR_BOX(L"_pCurrentScene is nullptr");
		return E_FAIL;
	}

	if (m_iCurrentSceneIndex != _iSceneIndex)
	{
		Safe_Release(m_pCurrentScene);
		m_pCurrentScene = _pCurrentScene;
		m_iCurrentSceneIndex = _iSceneIndex;
	}
	else
	{
		Safe_Release(_pCurrentScene);
	}

	return S_OK;
}

_uint CScene_Manager::Update_Scene(const _float& _fDeltaTime)
{
	if (nullptr == m_pCurrentScene)
		return 0;

	return m_pCurrentScene->Update_Scene(_fDeltaTime);
}

_uint CScene_Manager::LateUpdate_Scene(const _float& _fDeltaTime)
{
	if (nullptr == m_pCurrentScene)
		return 0;

	return m_pCurrentScene->LateUpdate_Scene(_fDeltaTime);
}

_uint CScene_Manager::Render_Scene()
{
	if (nullptr == m_pCurrentScene)
		return 0;

	return m_pCurrentScene->Render_Scene();
}

void CScene_Manager::Free()
{
	Safe_Release(m_pCurrentScene);
}
