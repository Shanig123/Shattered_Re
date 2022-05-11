#pragma once

#ifndef __MESH_H__
#define __MESH_H__

#include "Base.h"
#include "Camera.h"
#include "Optimization.h"

BEGIN(Engine)

class ENGINE_DLL CMesh abstract : public CBase
{
public:
	enum class EPickingDir
	{
		X,
		_X,
		Y,
		_Y,
		Z,
		_Z
	};

protected:
	explicit CMesh(_DEVICE _pDevice);
	explicit CMesh(const CMesh& _rOther);
	virtual ~CMesh() = default;

public:
	virtual HRESULT	Ready_Meshes_Prototype(const wstring& _pFilePath = L"", const wstring& _pFileName = L"") PURE;
	virtual HRESULT	Ready_Meshes_Clone(void* _pArg = nullptr) PURE;
	virtual HRESULT Render_Meshes() PURE;
	virtual HRESULT Render_Meshes(LPD3DXEFFECT& _pEffect, _uint _iIndex = 0, MY_FRAME * _pFrame = nullptr, _bool _bHardSkin = false) PURE;
	virtual HRESULT Render_ShadowMeshes(LPD3DXEFFECT& _pEffect, MY_FRAME * _pFrame = nullptr) PURE;
	virtual HRESULT Render_Meshes(LPD3DXEFFECT& _pEffect, COptimization* _pOptimization, const _float4x4* _matWolrd, _uint _iIndex = 0) PURE;

public:
	EMeshType& Get_MeshType();
	_float3* Get_VtxPos() { return m_pVtxPos; }
	_ulong Get_VtxCount() const { return m_dwVtxCount; }
	_ulong Get_Stride() const { return m_dwStride; }
	_long Get_TriCount() const { return m_iTriCount; }
	void* Get_Vertices() const { return m_pVertices; }
	void* Get_Indices() const { return m_pIndices; }
	_uint Get_IndexSize() const { return m_pIndexSize; }
	_float3 Get_ObjectMinPos() const { return m_vObjectMix; }
	_float3 Get_ObjectMaxPos() const { return m_vObjectMax; }
	_float3 Get_ObjectCenterPos() const { return m_vObjectCenter; }
	_float Get_ObjectRadius() const { return m_fObjectRadius; }
	_float3* Get_MinPos() const { return m_vMin; }
	_float3* Get_MaxPos() const { return m_vMax; }
	const _ulong& Get_SubsetCount() const { return m_dwSubsetCount; }
	vector<void*>& Get_VecVertices() { return m_vecVertices; }
	vector<void*>& Get_VecIndtices() { return m_vecIndices; }
	vector<_uint>& Get_VecVerticesCount() { return m_vecVerticesCount; }
	vector<_uint>& Get_VecIndticesCount() { return m_vecIndicesCount; }

	

//★★
public:
	template <typename T, typename I>
	_bool Picking_Mesh(_float3* _pOut, EPickingDir* _ePickingDir, _float* _fDir, HWND _hWnd, _uint _iWinCX, _uint _iWinCY, class CCamera* _pCamera, const _float4x4* _pMatWrold);
private:
	HRESULT Translation_ToLocalSpace(HWND _hWnd, _uint _iWinCX, _uint _iWinCY, class CCamera* _pCamera, const _float4x4* _pMatWorld);
//★★

public:
	template <typename T, typename I>
	_bool Picking_Vertex(_float3* _pOut, EPickingDir* _ePickingDir, _float* _fDir, HWND _hWnd, _uint _iWinCX, _uint _iWinCY, class CCamera* _pCamera, const _float4x4* _pMatWrold);

public:
	virtual CMesh * Clone(void * _pArg = nullptr) PURE;
	virtual void Free() PURE;

protected:
	_DEVICE			m_pDevice;
	_bool			m_bClone;
	EMeshType		m_eMeshType;

	_long			m_iTriCount;
	_VertexBuffer	m_pVB;
	_IndexBuffer	m_pIB;
	void*			m_pVertices;
	void*			m_pIndices;
	vector<void*>	m_vecVertices;
	vector<_uint>	m_vecVerticesCount;
	vector<void*>	m_vecIndices;
	vector<_uint>	m_vecIndicesCount;
	_uint			m_pIndexSize;

	_float3*		m_pVtxPos;		// 정점 정보 중 포지션 값만 얻어오기 위한 포인터
	_ulong			m_dwVtxCount;
	_ulong			m_dwStride;				// 정점의 크기(사이즈) 보관

	_float3			m_vObjectMax;
	_float3			m_vObjectMix;
	_float3			m_vObjectCenter;
	_float			m_fObjectRadius;
	_float3*		m_vMax;
	_float3*		m_vMin;
	_ulong			m_dwSubsetCount;		// 서브셋의 개수 == 텍스처의 개수 == 재질의 개수

private:
	_float3			m_vRayPos;
	_float3			m_vRayDir;
};

END

#endif // !__MESH_H__

