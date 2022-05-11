#pragma once

#ifndef __STATICMESH_H__
#define __STATICMESH_H__

#include "Mesh.h"

BEGIN(Engine)

class ENGINE_DLL CStaticMesh final : public CMesh
{
private:
	explicit CStaticMesh(_DEVICE _pDevice);
	explicit CStaticMesh(const CStaticMesh& _rOther);
	virtual ~CStaticMesh() = default;

public:
	virtual HRESULT	Ready_Meshes_Prototype(const wstring& _pFilePath, const wstring& _pFileName) override;
	virtual HRESULT	Ready_Meshes_Clone(void* _pArg = nullptr) override;
	virtual HRESULT Render_Meshes() override;
	virtual HRESULT Render_Meshes(LPD3DXEFFECT& _pEffect, _uint _iIndex = 0, MY_FRAME * _pFrame = nullptr, _bool _bHardSkin = false) override;
	virtual HRESULT Render_ShadowMeshes(LPD3DXEFFECT& _pEffect, MY_FRAME * _pFrame = nullptr) override;
	virtual HRESULT Render_Meshes(LPD3DXEFFECT& _pEffect, COptimization* _pOptimization, const _float4x4* _matWolrd, _uint _iIndex = 0) override;

public://Get
	const _MESH Get_MeshCom() const { return m_pMesh; }

public:
	static CStaticMesh* Create(_DEVICE _pDevice, const wstring& _pFilePath, const wstring& _pFileName);
	virtual CMesh* Clone(void* _pArg = nullptr) override;
	virtual void Free() override;

private:
	// 바운딩 박스와 관계 없는 작업, 노말 정보가 없는 메쉬의 정점에 노말 값을 삽입하기 위함
	_MESH		m_pOriMesh;		// 최초 로딩시에 얻어온 메쉬 컴객체

	_MESH		m_pMesh;			// 노말 정보가 최종적으로 삽입된 메쉬 컴 객체
	_BUFFER		m_pAdjacency;		// 인접한 폴리곤 정보를 보관하기 위한 주소(첫번째 주소 보관)
	_BUFFER		m_pSubset;

	_MATERIAL	m_pMaterial;		// 재질저보와 텍스처의 이름을 보관하기 위한 구조체 포인터
	_TEXTURE9*	m_ppTextures;		// 여러장의 텍스처를 보관하기 위한 배열 포인터
	_TEXTURE9*	m_ppNormalTextures;		// 여러장의 텍스처를 보관하기 위한 배열 포인터
	_TEXTURE9*	m_ppEmissiveTextures;		// 여러장의 텍스처를 보관하기 위한 배열 포인터
	_TEXTURE9*	m_ppSpecularTextures;		// 여러장의 텍스처를 보관하기 위한 배열 포인터
	_TEXTURE9*	m_ppRoughTextures;		// 여러장의 텍스처를 보관하기 위한 배열 포인터
};

END

#endif // !__STATICMESH_H__