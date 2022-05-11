#include "Mesh_Manager.h"
#include "Management.h"

USING(Engine)

IMPLEMENT_SINGLETON(CMesh_Manager)

CMesh_Manager::CMesh_Manager()
{
}

HRESULT CMesh_Manager::Ready_Mesh_Manager(_uint _iSceneCount)
{
	if (0 == _iSceneCount)
	{
		ERROR_BOX(L"_iSceneCount is 0");
		return E_FAIL;
	}

	m_pMeshs = new MESHS[_iSceneCount];
	m_iSceneCount = _iSceneCount;

	return S_OK;
}

HRESULT CMesh_Manager::Add_Mesh_Prototype(_uint _iSceneIndex, const wstring& _szPrototype, CMesh * _pPrototype)
{
	if (m_iSceneCount <= _iSceneIndex || nullptr == _pPrototype || nullptr == m_pMeshs)
	{
		ERROR_BOX(L"Failed to Add_Mesh_Prototype");
		return E_FAIL;
	}

	auto iter_find = m_pMeshs[_iSceneIndex].find(_szPrototype);
	if (iter_find == m_pMeshs[_iSceneIndex].end())
	{
		m_pMeshs[_iSceneIndex].emplace(_szPrototype, _pPrototype);
	}
	else
	{
		Safe_Release(_pPrototype);
	}

	return S_OK;
}

_bool CMesh_Manager::Find_Mesh_Prototype(_uint _iSceneIndex, const wstring & _szPrototype)
{
	if (m_iSceneCount <= _iSceneIndex || nullptr == m_pMeshs)
	{
		ERROR_BOX(L"Failed to Add_Mesh_Prototype");
		return false;
	}

	auto iter_find = m_pMeshs[_iSceneIndex].find(_szPrototype);
	if (iter_find == m_pMeshs[_iSceneIndex].end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

_TEXTURE9 CMesh_Manager::Find_MeshTexture(const wstring& _szTexturePath, const wstring& _szTextureName)
{
	auto iter_find = m_mapMeshTexture.find(_szTextureName);
	if (iter_find == m_mapMeshTexture.end())
	{
		_TEXTURE9 pTexture = nullptr;

		if (FAILED(D3DXCreateTextureFromFile(Get_Manager()->Get_Device(), _szTexturePath.c_str(), &pTexture)))
		{
			/*TCHAR szBuff[MAX_PATH] = L"";
			swprintf_s(szBuff, L"Failed to %s\n", _szTexturePath.c_str());
			ERROR_BOX(szBuff);*/
			return nullptr;
		}

		m_mapMeshTexture.emplace(_szTextureName, pTexture);

		return pTexture;
	}

	return iter_find->second;
}

CMesh * CMesh_Manager::Clone_Mesh(_uint _iSceneIndex, const wstring& _szPrototype, void * _pArg)
{
	if (m_iSceneCount <= _iSceneIndex || nullptr == m_pMeshs)
	{
		ERROR_BOX(L"Failed to Clone_Mesh");
		return nullptr;
	}

	auto iter_find = m_pMeshs[_iSceneIndex].find(_szPrototype);
	if (m_pMeshs[_iSceneIndex].end() == iter_find)
	{
		ERROR_BOX(L"m_pMeshs is end");
		return nullptr;
	}

	return iter_find->second->Clone(_pArg);
}

void CMesh_Manager::Clear_ForScene(_uint _iSceneIndex)
{
	if (m_iSceneCount <= _iSceneIndex)
	{
		ERROR_BOX(L"_iSceneIndex is over");
		return;
	}

	for (auto& rPair : m_pMeshs[_iSceneIndex])
	{
		 
		Safe_Release(rPair.second);
	}
}

const EMeshType CMesh_Manager::Get_MeshType(_uint _iSceneIndex, const wstring& _szPrototype)
{
	if (m_iSceneCount <= _iSceneIndex || nullptr == m_pMeshs)
	{
		ERROR_BOX(L"Failed to Get_MeshType");
		return EMeshType::End;
	}

	auto iter_find = m_pMeshs[_iSceneIndex].find(_szPrototype);
	if (m_pMeshs[_iSceneIndex].end() == iter_find)
	{
		ERROR_BOX(L"m_pMeshs is end");
		return EMeshType::End;
	}

	return iter_find->second->Get_MeshType();
}

void CMesh_Manager::Free()
{
	for (_uint i = 0; i < m_iSceneCount; ++i)
	{
		for (auto& rPair : m_pMeshs[i])
		{
			Safe_Release(rPair.second);
		}
	}

	for (auto& rPair : m_mapMeshTexture)
	{
		Safe_Release(rPair.second);
	}
	Safe_Delete_Array(m_pMeshs);
}
