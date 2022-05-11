#include "Component_Manager.h"

USING(Engine)

IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{
}


HRESULT CComponent_Manager::Ready_Component_Manager(_uint _iSceneCount)
{
	if (0 == _iSceneCount)
	{
		ERROR_BOX(L"SceneCount is 0");
		return E_FAIL;
	}

	m_pComponents = new COMPONENTS[_iSceneCount];
	m_pTextures = new map<wstring, CTexture*>[_iSceneCount];
	m_iSceneCount = _iSceneCount;

	return S_OK;
}

HRESULT CComponent_Manager::Add_Component_Prototype(_uint _iSceneIndex, const wstring & _szPrototype, CComponent * _pPrototype)
{
	if (m_iSceneCount <= _iSceneIndex || nullptr == _pPrototype, nullptr == m_pComponents)
	{
		ERROR_BOX(L"Failed to Add_Component_Prototype");
		return E_FAIL;
	}

	auto iter_find = m_pComponents[_iSceneIndex].find(_szPrototype);
	if (iter_find == m_pComponents[_iSceneIndex].end())
	{
		m_pComponents[_iSceneIndex].emplace(_szPrototype, _pPrototype);
	}
	else
	{
		Safe_Release(_pPrototype);
	}

	return S_OK;
}

HRESULT CComponent_Manager::Add_Texture_Prototype(_uint _iSceneIndex, const wstring & _szPrototype, CTexture * _pPrototype)
{
	if (m_iSceneCount <= _iSceneIndex || nullptr == _pPrototype, nullptr == m_pTextures)
	{
		ERROR_BOX(L"Failed to Add_Component_Prototype");
		return E_FAIL;
	}

	auto iter_find = m_pTextures[_iSceneIndex].find(_szPrototype);
	if (iter_find == m_pTextures[_iSceneIndex].end())
	{
		m_pTextures[_iSceneIndex].emplace(_szPrototype, _pPrototype);
	}
	else
	{
		Safe_Release(_pPrototype);
	}

	return S_OK;
}


CComponent * CComponent_Manager::Get_Component(_uint _iSceneIndex, const wstring & _szPrototype)
{
	if (m_iSceneCount <= _iSceneIndex)
	{
		ERROR_BOX(L"Failed to Get_Component");
		return nullptr;
	}

	auto iter_find = m_pComponents[_iSceneIndex].find(_szPrototype);
	if (iter_find == m_pComponents[_iSceneIndex].end())
	{
		return nullptr;
	}

	return iter_find->second;
}

CComponent * CComponent_Manager::Clone_Component(_uint _iSceneIndex, const wstring & _szPrototype, void * _pArg)
{
	if (m_iSceneCount <= _iSceneIndex || nullptr == m_pComponents)
	{
		ERROR_BOX(L"Failed to Clone_Component");
		return nullptr;
	}

	auto iter_find = m_pComponents[_iSceneIndex].find(_szPrototype);
	if (m_pComponents[_iSceneIndex].end() == iter_find)
	{
		ERROR_BOX(L"m_pComponents is end");
		return nullptr;
	}

	return iter_find->second->Clone(_pArg);
}

void CComponent_Manager::Clear_ForScene(_uint _iSceneIndex)
{
	if (m_iSceneCount <= _iSceneIndex)
	{
		ERROR_BOX(L"_iSceneIndex is over");
		return;
	}

	for_each(m_pComponents[_iSceneIndex].begin(), m_pComponents[_iSceneIndex].end(), CDeleteMap());
	m_pComponents[_iSceneIndex].clear();
	for_each(m_pTextures[_iSceneIndex].begin(), m_pTextures[_iSceneIndex].end(), CDeleteMap());
	m_pTextures[_iSceneIndex].clear();
}

HRESULT CComponent_Manager::Set_Texture(const _uint& _iSceneIndex, const wstring & _wstrPrototypeTexture, _EFFECT & _pEffect, const char * _pConstantTable, const _uint & _iIndex)
{
	CTexture* pFind = nullptr;
	pFind =	m_pTextures[_iSceneIndex][_wstrPrototypeTexture];
	NULL_CHECK_RETURN(pFind, E_FAIL);
	HRESULT hr = (pFind)->Set_Texture(_pEffect, _pConstantTable, _iIndex);
	_pEffect->CommitChanges();
	return hr;
}

void CComponent_Manager::Free()
{
	for (_uint i = 0; i < m_iSceneCount; ++i)
	{
		for (auto& rPair : m_pComponents[i])
		{
			_uint iRefCount = 0;
			if (!rPair.second->Get_CheckClone())
			{
				if (iRefCount = Safe_Release(rPair.second))
				{
					__debugbreak();
				}
			}
		}
		for (auto& rPair : m_pTextures[i])
		{
			_uint iRefCount = 0;
			if (!rPair.second->Get_CheckClone())
			{
				if (iRefCount = Safe_Release(rPair.second))
				{
					__debugbreak();
				}
			}
		}

	}

	Safe_Delete_Array(m_pComponents);
	Safe_Delete_Array(m_pTextures);
}
