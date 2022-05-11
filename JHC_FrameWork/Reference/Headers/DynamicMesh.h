#pragma once

#ifndef __DYNAMICMESH_H__
#define __DYNAMICMESH_H__

#include "Mesh.h"
#include "HierarchyLoader.h"
#include "AniControl.h"

BEGIN(Engine)

class ENGINE_DLL CDynamicMesh final : public CMesh
{
private:
	explicit CDynamicMesh(_DEVICE _pDevice);
	explicit CDynamicMesh(const CDynamicMesh& _rOther);
	virtual ~CDynamicMesh() = default;

public:
	virtual HRESULT	Ready_Meshes_Prototype(const wstring& _pFilePath, const wstring& _pFileName) override;
	virtual HRESULT	Ready_Meshes_Clone(void* _pArg = nullptr) override;
	virtual HRESULT Render_Meshes() override;
	virtual HRESULT Render_Meshes(LPD3DXEFFECT& _pEffect, _uint _iIndex = 0, MY_FRAME * _pFrame = nullptr, _bool _bHardSkin = false) override;
	virtual HRESULT Render_ShadowMeshes(LPD3DXEFFECT& _pEffect, MY_FRAME * _pFrame = nullptr) override;
	virtual HRESULT Render_Meshes(LPD3DXEFFECT& _pEffect, COptimization* _pOptimization, const _float4x4* _matWolrd, _uint _iIndex = 0) override;

public:
	vector<wstring>* Get_BoneName() { return &m_vecBoneName; }
	const MY_FRAME* Get_FrameByName(const char* pFrameName);
	_uint Get_Animationset();
	_bool Get_AnimationFrame(_double _dMaxFrame, _double _dWantFrame);
	void Get_AllBoneMetrix(_float4x4** _OutMatrix, _ulong* _OutBoneCount);
	_bool Is_AnimationSetEnd();

public:
	void Set_Animationset(const _uint& _iIndex);
	void Change_Animationset(const _uint& _iIndex);
	void Play_Animationset(const _float& _fDeltaTime);

public:
	D3DXFRAME& Get_RootBone() { return *m_pRootBone; }

public:
	void Update_FrameMatrices(MY_FRAME* _pFrame, const _float4x4* _pParentMatrix);
	void Setup_FrameMatrixPointer(MY_FRAME* _pFrame);

public:
	static CDynamicMesh* Create(_DEVICE _pDevice, const wstring& _pFilePath, const wstring& _pFileName);
	virtual CMesh* Clone(void* _pArg = nullptr) override;
	virtual void Free() override;

private:
	D3DXFRAME*					m_pRootBone;
	CHierarchyLoader*			m_pLoader;
	CAniControl*				m_pAniControl;
	_float4x4*					m_matBoneMatrix;
	_ulong						m_longBoneCount;
	list<MY_MESHCONTAINER*>		m_listMeshContaier;
	vector<wstring>				m_vecBoneName;
};

END

#endif // !__DYNAMICMESH_H__