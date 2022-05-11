#include "Layer.h"
#include "GameObject.h"

USING(Engine)

CLayer::CLayer(const wstring& _wstrLayerName)
	:m_wstrLayerName(_wstrLayerName)
{
}

HRESULT CLayer::Add_GameObject(CGameObject * _pGameObject)
{
	if (nullptr == _pGameObject)
	{
		ERROR_BOX(L"_pGameObject is nullptr");
		return E_FAIL;
	}

	auto iter = find(m_GameObjects.begin(), m_GameObjects.end(), _pGameObject);
	if (m_GameObjects.end() == iter)
	{
		m_GameObjects.push_back(_pGameObject);
	}
	else
	{
		Safe_Release(_pGameObject);
	}

	return S_OK;
}

_uint CLayer::Update_Layer(const _float& _fDeltaTime)
{
	_uint iEvent = 0;
	
	auto& iter = m_GameObjects.begin();
	for (; iter != m_GameObjects.end(); )
	{
		iEvent = (*iter)->Update_GameObject(_fDeltaTime);
		if ((_uint)EEvent::Dead == iEvent)
		{
			Safe_Release(*iter);
			iter = m_GameObjects.erase(iter);
		}
		else
			++iter;
	}

	return iEvent;
}

_uint CLayer::LateUpdate_Layer(const _float& _fDeltaTime)
{
	_uint iEvent = 0;

	for (auto& rObj : m_GameObjects)
	{
		if (iEvent = rObj->LateUpdate_GameObject(_fDeltaTime))
			return iEvent;
	}

	return iEvent;
}

CComponent * CLayer::Get_Component(const wstring& _szComponent, _uint _iIndex)
{
	auto iter_begin = m_GameObjects.begin();
	if (m_GameObjects.end() == iter_begin)
	{
		ERROR_BOX(L"Failed To iter_begin")
		return nullptr;
	}

	for (_uint i = 0; i < _iIndex; ++i)
	{
		++iter_begin;
	}

	return (*iter_begin)->Get_Component(_szComponent);
}

CGameObject * CLayer::Get_GameObject(_uint _iIndex)
{
	auto iter_begin = m_GameObjects.begin();
	if (m_GameObjects.end() == iter_begin)
	{
		ERROR_BOX(L"Failed To iter_begin")
		return nullptr;
	}

	for (_uint i = 0; i < _iIndex; ++i)
	{
		++iter_begin;
	}

	return *iter_begin;
}

CGameObject * CLayer::Get_GameObject(EFindGameObject _eType, const wstring & _szName)
{
	auto iter_begin = m_GameObjects.begin();
	if (m_GameObjects.end() == iter_begin)
	{
		ERROR_BOX(L"Failed To iter_begin");
		return nullptr;
	}

	for (auto& iter : m_GameObjects)
	{
		if (EFindGameObject::Proto == _eType)
		{
			if (!lstrcmpi(_szName.c_str(), iter->Get_PrototypeName().c_str()))
			{
				return iter;
			}
		}
		else
		{
			if (!lstrcmpi(_szName.c_str(), iter->Get_TransformDesc().szFileTitle))
			{
				return iter;
			}
		}
		
	}

	return nullptr;
}

CMesh * CLayer::Get_Mesh(const wstring & _szComponent, _uint _iIndex)
{
	auto iter_begin = m_GameObjects.begin();
	if (m_GameObjects.end() == iter_begin)
	{
		ERROR_BOX(L"Failed To iter_begin")
			return nullptr;
	}

	for (_uint i = 0; i < _iIndex; ++i)
	{
		++iter_begin;
	}

	return (*iter_begin)->Get_Mesh(_szComponent);
}

HRESULT CLayer::Delete_Component(_uint _iIndex)
{
	auto iter_begin = m_GameObjects.begin();
	if (m_GameObjects.end() == iter_begin)
	{
		ERROR_BOX(L"Failed To m_pGameObjects In Delete_Component");
		return E_FAIL;
	}

	for (_uint i = 0; i < _iIndex; ++i)
	{
		++iter_begin;
	}

	return (*iter_begin)->Delete_Component();
}

HRESULT CLayer::Delete_GameObject(_uint _iIndex)
{
	auto iter_begin = m_GameObjects.begin();
	if (m_GameObjects.end() == iter_begin)
	{
		ERROR_BOX(L"Failed To m_pGameObjects In Delete_GameObject");
		return E_FAIL;
	}

	for (_uint i = 0; i < _iIndex; ++i)
	{
		++iter_begin;
	}

	for (auto& rObj : m_GameObjects)
	{
		if (*iter_begin == rObj)
		{
			Safe_Release(rObj);
			m_GameObjects.erase(iter_begin);
			break;
		}
	}

	return S_OK;
}

HRESULT CLayer::Set_SceneID(const _uint & _iIdx)
{
	for (auto& rObj : m_GameObjects)
	{
		rObj->Set_SceneIdx(_iIdx);
	}
	return S_OK;
}

list<class CGameObject*>* CLayer::Get_LayerContainer()
{
	return &m_GameObjects;
}

CLayer * CLayer::Create(const wstring& _wstrLayerName)
{
	return new CLayer(_wstrLayerName);
}

void CLayer::Free()
{
	for (auto& rObj : m_GameObjects)
	{
	if(rObj->Get_CheckClone())
		Safe_Release(rObj);
	}

	m_GameObjects.clear();
}