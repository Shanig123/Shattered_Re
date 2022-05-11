#pragma once

#ifndef __MESH_MANAGER_H__
#define __MESH_MANAGER_H__

#include "Base.h"
#include "StaticMesh.h"
#include "DynamicMesh.h"

BEGIN(Engine)

class ENGINE_DLL CMesh_Manager final : public CBase
{
	DECLARE_SINGLETON(CMesh_Manager)

private:
	explicit CMesh_Manager();
	virtual ~CMesh_Manager() = default;
		
public:
	HRESULT Ready_Mesh_Manager(_uint _iSceneCount);
	HRESULT Add_Mesh_Prototype(_uint _iSceneIndex, const wstring& _szPrototype, class CMesh* _pPrototype);
	_bool Find_Mesh_Prototype(_uint _iSceneIndex, const wstring& _szPrototype);
	_TEXTURE9 Find_MeshTexture(const wstring& _szTexturePath, const wstring& _szTextureName);
	CMesh* Clone_Mesh(_uint _iSceneIndex, const wstring& _szPrototype, void* _pArg = nullptr);
	void Clear_ForScene(_uint _iSceneIndex);

public:
	const EMeshType Get_MeshType(_uint _iSceneIndex, const wstring& _szPrototype);

public:
	virtual void Free() override;

private:
	typedef map<wstring, class CMesh*>	MESHS;
	typedef map<wstring , _TEXTURE9>	MESH_TEX;
	MESHS*		m_pMeshs;
	MESH_TEX	m_mapMeshTexture;

	_uint		m_iSceneCount;
};

END

#endif // !__MESH_MANAGER_H__