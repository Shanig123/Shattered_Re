#include "GameObject_Manager.h"
#include "GameObject.h"
#include "Layer.h"
#include "Management.h"

USING(Engine)

IMPLEMENT_SINGLETON(CGameObject_Manager)

CGameObject_Manager::CGameObject_Manager()
{
}

HRESULT CGameObject_Manager::Ready_GameObject_Manager(_uint _iSceneCount)
{
	if (0 == _iSceneCount)
	{
		ERROR_BOX(L"Failed To SceneCount Is 0(Zero)");
		return E_FAIL;
	}

	m_pGameObjects = new GAMEOBJECTS[_iSceneCount];
	m_pLayers = new LAYERS[_iSceneCount];
	m_iSceneCount = _iSceneCount;

	return S_OK;
}

HRESULT CGameObject_Manager::Add_GameObject_Prototype(_uint _iSceneIndex, const wstring& _szPrototype, CGameObject * _pPrototype)
{
	if (m_iSceneCount <= _iSceneIndex || nullptr == _pPrototype || nullptr == m_pGameObjects)
	{
		ERROR_BOX(L"Failed To Add_GameObject_Prototype");
		return E_FAIL;
	}

	auto iter_find = m_pGameObjects[_iSceneIndex].find(_szPrototype);
	if (iter_find == m_pGameObjects[_iSceneIndex].end())
	{
		m_pGameObjects[_iSceneIndex].emplace(_szPrototype, _pPrototype);
	}
	else
	{
		Safe_Release(_pPrototype);
	}

	return S_OK;
}

HRESULT CGameObject_Manager::Clone_GameObject_ToLayer(_uint _iFromSceneIndex,
	const wstring& _szPrototype, _uint _iToSceneIndex, const wstring& _szLayer, void * _pArg)
{
	if (m_iSceneCount <= _iFromSceneIndex || m_iSceneCount <= _iToSceneIndex || nullptr == m_pGameObjects || nullptr == m_pLayers)
	{
		ERROR_BOX(L"Failed To Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	auto iter_Prototype = m_pGameObjects[_iFromSceneIndex].find(_szPrototype);

	if (iter_Prototype == m_pGameObjects[_iFromSceneIndex].end())
	{
		TCHAR _szLogMsg[] = L"Not Found %s Prototype In Clone_GameObject_ToLayer";
		TCHAR _szLog[MAX_PATH] = L"";
		swprintf_s(_szLog, _szLogMsg, _szPrototype);
		ERROR_BOX(_szLog);
		return E_FAIL;
	}

	auto pClone = iter_Prototype->second->Clone(_iToSceneIndex,_pArg);
	if (nullptr == pClone)
	{
		TCHAR _szLogMsg[] = L"Failed To %s Clone In Clone_GameObject_ToLayer";
		TCHAR _szLog[MAX_PATH] = L"";
		swprintf_s(_szLog, _szLogMsg, _szPrototype);
		ERROR_BOX(_szLog);
		return E_FAIL;
	}

	auto iter_Layer = m_pLayers[_iToSceneIndex].find(_szLayer);
	if (iter_Layer == m_pLayers[_iToSceneIndex].end())
	{
		m_pLayers[_iToSceneIndex].emplace(_szLayer, CLayer::Create(_szLayer));
	}

	if (FAILED(m_pLayers[_iToSceneIndex][_szLayer]->Add_GameObject(pClone)))
	{
		ERROR_BOX(L"Failed To Add_GameObject");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGameObject_Manager::Clone_GameObject_ToLayer(CGameObject * _pGameCloneObject, _uint _iToSceneIndex, const wstring & _szLayer)
{
	if (nullptr == _pGameCloneObject || m_iSceneCount <= _iToSceneIndex || nullptr == m_pGameObjects || nullptr == m_pLayers)
	{
		ERROR_BOX(L"Failed To Clone_GameObject_ToLayer");
		return E_FAIL;
	}

	auto iter_Layer = m_pLayers[_iToSceneIndex].find(_szLayer);
	if (iter_Layer == m_pLayers[_iToSceneIndex].end())
	{
		m_pLayers[_iToSceneIndex].emplace(_szLayer, CLayer::Create(_szLayer));
	}

	if (FAILED(m_pLayers[_iToSceneIndex][_szLayer]->Add_GameObject(_pGameCloneObject)))
	{
		ERROR_BOX(L"Failed To Add_GameObject");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CGameObject_Manager::Clone_GameObject(_uint _iFromSceneIndex, const wstring & _szPrototype, CGameObject ** _pOutObject, void * _pArg,_uint _iSceneIndex)
{
	if ((m_iSceneCount <= _iFromSceneIndex) ||
		( (*_pOutObject) != nullptr) || 
		(nullptr == m_pGameObjects))
	{
		ERROR_BOX(L"Failed To Clone_GameObject");
		return E_FAIL;
	}

	auto iter_Prototype = m_pGameObjects[_iFromSceneIndex].find(_szPrototype);
	
	if (iter_Prototype == m_pGameObjects[_iFromSceneIndex].end())
	{
		TCHAR _szLogMsg[] = L"Not Found %s Prototype In Clone_GameObject";
		TCHAR _szLog[MAX_PATH] = L"";
		swprintf_s(_szLog, _szLogMsg, _szPrototype);
		ERROR_BOX(_szLog);
		return E_FAIL;
	}

	CGameObject* pClone = iter_Prototype->second->Clone(_iSceneIndex, _pArg);
	if (!pClone)
	{
		TCHAR _szLogMsg[] = L"Failed To %s Clone In Clone_GameObject";
		TCHAR _szLog[MAX_PATH] = L"";
		swprintf_s(_szLog, _szLogMsg, _szPrototype);
		ERROR_BOX(_szLog);
		return E_FAIL;
	}
	*_pOutObject = pClone;
	//Safe_AddRef(*_pOutObject);
	return S_OK;
}

HRESULT CGameObject_Manager::Delete_Component(_uint _iSceneIndex, const wstring & _wstrLayer, _uint _iIndex)
{
	if (m_iSceneCount <= _iSceneIndex || nullptr == m_pLayers)
	{
		ERROR_BOX(L"Failed To Delete_Component");
		return E_FAIL;
	}

	auto iter_find = m_pLayers[_iSceneIndex].find(_wstrLayer);
	if (m_pLayers[_iSceneIndex].end() == iter_find)
	{
		ERROR_BOX(L"Failed To iter_find In Delete_Component");
		return E_FAIL;
	}

	return iter_find->second->Delete_Component(_iIndex);
}

HRESULT CGameObject_Manager::Delete_GameObject(_uint _iSceneIndex, const wstring & _wstrLayer, _uint _iIndex)
{
	if (m_iSceneCount <= _iSceneIndex || nullptr == m_pLayers)
	{
		ERROR_BOX(L"Failed To Delete_GameObject");
		return E_FAIL;
	}

	auto iter_find = m_pLayers[_iSceneIndex].find(_wstrLayer);
	if (m_pLayers[_iSceneIndex].end() == iter_find)
	{
		ERROR_BOX(L"Failed To iter_find In Delete_GameObject");
		return E_FAIL;
	}

	return iter_find->second->Delete_GameObject(_iIndex);
}

_uint CGameObject_Manager::Update_GameObject(const _float& _fDeltaTime)
{
	_uint iEvnet = 0;
	for (_uint i = 0; i < m_iSceneCount; ++i)
	{
		if (i == 0)
			continue;
		for (auto& rPair : m_pLayers[i])
		{
		
			if (!lstrcmpi(rPair.second->Get_LayerKey().c_str(), L"Layer_Monster_Appear1"))
				continue;

			if (iEvnet = rPair.second->Update_Layer(_fDeltaTime))
				return iEvnet;
		}
	}

	return iEvnet;
}

_uint CGameObject_Manager::LateUpdate_GameObject(const _float& _fDeltaTime)
{
	_uint iEvnet = 0;
	for (_uint i = 0; i < m_iSceneCount; ++i)
	{
		if (i == 0)
			continue;
		for (auto& rPair : m_pLayers[i])
		{
			if (iEvnet = rPair.second->LateUpdate_Layer(_fDeltaTime))
				return iEvnet;
		}
	}
	return iEvnet;
}

void CGameObject_Manager::Clear_ForScene(_uint _iSceneIndex)
{
	if (m_iSceneCount <= _iSceneIndex)
	{
		ERROR_BOX(L"_iSceneIndex Is Over");
		return;
	}

	for_each(m_pLayers[_iSceneIndex].begin(), m_pLayers[_iSceneIndex].end(), CDeleteMap());
	m_pLayers[_iSceneIndex].clear();

	for_each(m_pGameObjects[_iSceneIndex].begin(), m_pGameObjects[_iSceneIndex].end(), CDeleteMap());
	m_pGameObjects[_iSceneIndex].clear();
}

void CGameObject_Manager::Clear_Layer(_uint _iSceneIndex, const wstring & _wstrLayer)
{
	if (m_iSceneCount <= _iSceneIndex || nullptr == m_pLayers)
	{
		ERROR_BOX(L"Failed To Get_Component");
		return ;
	}

	auto iter_find = m_pLayers[_iSceneIndex].find(_wstrLayer);
	if (m_pLayers[_iSceneIndex].end() == iter_find)
	{
		ERROR_BOX(L"Failed To iter_find");
		return ;
	}
	Safe_Release(iter_find->second);
	m_pLayers[_iSceneIndex].erase(iter_find);
}

CComponent * CGameObject_Manager::Get_Component(_uint _iSceneIndex, const wstring& _szLayer, const wstring& _szComponent, _uint _iIndex)
{
	if (m_iSceneCount <= _iSceneIndex || nullptr == m_pLayers)
	{
		ERROR_BOX(L"Failed To Get_Component");
		return nullptr;
	}

	auto iter_find = m_pLayers[_iSceneIndex].find(_szLayer);
	if (m_pLayers[_iSceneIndex].end() == iter_find)
	{
		ERROR_BOX(L"Failed To iter_find");
		return nullptr;
	}

	return iter_find->second->Get_Component(_szComponent, _iIndex);
}

CGameObject * CGameObject_Manager::Get_GameObject(_uint _iSceneIndex, const wstring& _szLayer, _uint _iIndex)
{
	if (m_iSceneCount <= _iSceneIndex || nullptr == m_pLayers)
	{
		ERROR_BOX(L"Failed To Get_GameObject");
		return nullptr;
	}

	auto iter_find = m_pLayers[_iSceneIndex].find(_szLayer);
	if (m_pLayers[_iSceneIndex].end() == iter_find)
	{
		ERROR_BOX(L"Failed To iter_find");
		return nullptr;
	}

	return iter_find->second->Get_GameObject(_iIndex);
}

CGameObject * CGameObject_Manager::Get_GameObject(_uint _iSceneIndex, const wstring & _szLayer, EFindGameObject _eType, const wstring & _szName)
{
	if (m_iSceneCount <= _iSceneIndex || nullptr == m_pLayers)
	{
		ERROR_BOX(L"Failed To Get_GameObject");
		return nullptr;
	}

	auto iter_find = m_pLayers[_iSceneIndex].find(_szLayer);
	if (m_pLayers[_iSceneIndex].end() == iter_find)
	{
		ERROR_BOX(L"Failed To iter_find");
		return nullptr;
	}

	return iter_find->second->Get_GameObject(_eType, _szName);
}

CMesh * CGameObject_Manager::Get_Mesh(_uint _iSceneIndex, const wstring & _szLayer, const wstring & _szComponent, _uint _iIndex)
{
	if (m_iSceneCount <= _iSceneIndex || nullptr == m_pLayers)
	{
		ERROR_BOX(L"Failed To Get_Component");
		return nullptr;
	}

	auto iter_find = m_pLayers[_iSceneIndex].find(_szLayer);
	if (m_pLayers[_iSceneIndex].end() == iter_find)
	{
		ERROR_BOX(L"Failed To iter_find");
		return nullptr;
	}

	return iter_find->second->Get_Mesh(_szComponent, _iIndex);
}

list<class CGameObject*>* CGameObject_Manager::Get_LayerContainer(_uint _iSceneIndex, const wstring& _szLayer)
{
	if (m_iSceneCount <= _iSceneIndex || nullptr == m_pLayers)
	{
		//ERROR_BOX(L"Failed To Get_LayerContainer");
		return nullptr;
	}

	auto iter_find = m_pLayers[_iSceneIndex].find(_szLayer);
	if (m_pLayers[_iSceneIndex].end() == iter_find)
	{
		//ERROR_BOX(L"Failed To iter_find");
		return nullptr;
	}

	return iter_find->second->Get_LayerContainer();
}

HRESULT CGameObject_Manager::Set_StaticObjToScene(const _uint & _iToSceneIdx)
{
	/*
	스태틱 레이어에서 문자열 복사->레이어 생성 -> 오브젝트 이동 ->스테틱 레이어 삭제->반복
	*/
	for (auto& rPair : m_pLayers[0])
	{
		const wstring& wstrLayerName = rPair.second->Get_LayerKey();
		auto iter_Layer = m_pLayers[_iToSceneIdx].find(wstrLayerName);
		if (iter_Layer == m_pLayers[_iToSceneIdx].end())
		{
			m_pLayers[_iToSceneIdx].emplace(wstrLayerName, nullptr);
		}
		m_pLayers[_iToSceneIdx][wstrLayerName] = m_pLayers[0][wstrLayerName];
		m_pLayers[_iToSceneIdx][wstrLayerName]->Set_SceneID(_iToSceneIdx);

	}
	m_pLayers[0].clear();
	return S_OK;
}

HRESULT CGameObject_Manager::Set_SceneToStaticObj(const _uint & _iToSceneIdx)
{
	auto& iter = m_pLayers[_iToSceneIdx].begin();

	for (; iter != m_pLayers[_iToSceneIdx].end();)
	{
		const wstring& wstrLayerName = iter->second->Get_LayerKey();
		if (wstrLayerName.find(L"Player") != wstring::npos)
		{
			auto iter_Layer = m_pLayers[0].find(wstrLayerName);
			if (iter_Layer == m_pLayers[0].end())
			{
				m_pLayers[0].emplace(wstrLayerName, m_pLayers[_iToSceneIdx][wstrLayerName]);
				m_pLayers[0][wstrLayerName]->Set_SceneID(0);
				iter = m_pLayers[_iToSceneIdx].erase(iter);
			}
			else
				++iter;
		}
		else
			++iter;
	}


	return S_OK;
}

HRESULT CGameObject_Manager::Add_Layer(const _uint & _iToSceneIndex, const wstring & _szLayer)
{
	auto iter_Layer = m_pLayers[_iToSceneIndex].find(_szLayer);
	if (iter_Layer == m_pLayers[_iToSceneIndex].end())
	{
		m_pLayers[_iToSceneIndex].emplace(_szLayer, CLayer::Create(_szLayer));
	}
	return S_OK;
}

void CGameObject_Manager::Free()
{
	for (_uint i = 0; i < m_iSceneCount; ++i)
	{
		for (auto& rPair : m_pLayers[i])
		{
			Safe_Release(rPair.second);
		}
		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	for (_uint i = 0; i < m_iSceneCount; ++i)
	{
		for (auto& rPair : m_pGameObjects[i])
		{
			Safe_Release(rPair.second);
		}
		m_pGameObjects[i].clear();
	}

	Safe_Delete_Array(m_pGameObjects);
}
