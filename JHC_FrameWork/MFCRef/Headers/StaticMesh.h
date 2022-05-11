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
	// �ٿ�� �ڽ��� ���� ���� �۾�, �븻 ������ ���� �޽��� ������ �븻 ���� �����ϱ� ����
	_MESH		m_pOriMesh;		// ���� �ε��ÿ� ���� �޽� �İ�ü

	_MESH		m_pMesh;			// �븻 ������ ���������� ���Ե� �޽� �� ��ü
	_BUFFER		m_pAdjacency;		// ������ ������ ������ �����ϱ� ���� �ּ�(ù��° �ּ� ����)
	_BUFFER		m_pSubset;

	_MATERIAL	m_pMaterial;		// ���������� �ؽ�ó�� �̸��� �����ϱ� ���� ����ü ������
	_TEXTURE9*	m_ppTextures;		// �������� �ؽ�ó�� �����ϱ� ���� �迭 ������
	_TEXTURE9*	m_ppNormalTextures;		// �������� �ؽ�ó�� �����ϱ� ���� �迭 ������
	_TEXTURE9*	m_ppEmissiveTextures;		// �������� �ؽ�ó�� �����ϱ� ���� �迭 ������
	_TEXTURE9*	m_ppSpecularTextures;		// �������� �ؽ�ó�� �����ϱ� ���� �迭 ������
	_TEXTURE9*	m_ppRoughTextures;		// �������� �ؽ�ó�� �����ϱ� ���� �迭 ������
};

END

#endif // !__STATICMESH_H__