#pragma once

#ifndef __NAVIMESH_H__
#define __NAVIMESH_H__

#include "Mesh.h"
#include "NaviCell.h"

BEGIN(Engine)

class ENGINE_DLL CNaviMesh final : public CMesh
{
private:
	explicit CNaviMesh(_DEVICE _pDevice);
	explicit CNaviMesh(const CNaviMesh& _rOther);
	virtual ~CNaviMesh() = default;

public:
	virtual HRESULT	Ready_Meshes_Prototype(const wstring& _pFilePath = L"", const wstring& _pFileName = L"") override;
	virtual HRESULT	Ready_Meshes_Clone(void* _pArg = nullptr) override;
	virtual HRESULT Render_Meshes() override;
	virtual HRESULT Render_Meshes(LPD3DXEFFECT& pEffect, COptimization* _pOptimization = nullptr, const _float4x4* _matWolrd = nullptr, _uint _iIndex = 0) override;

private:
	HRESULT Link_NaviCell();

public:
	HRESULT Add_NaviCell(const _float3* _pPointA, const _float3* _pPointB, const _float3* _pPointC);
	HRESULT Add_NaviCell(const _float3* _pNormalPos, 
		_uint _iPointXCount,
		_uint _iPointZCount,
		_float _fInterval = 5.f);
	_float3 Move_OnNaviMesh(const _float3* _pTargetPos, const _float3* _pTargetDir);
	_bool OnNaviMesh(const _float3* _pTargetPos, const _float3* _pTargetDir);

public:
	vector<CNaviCell*>* Get_NaviCell() { return &m_vecNaviCell; }
	void Set_NaviCell(const _ulong& _dwIndex, CNaviCell::POINT _eType, const _float3& _vPos);

public:
	void Set_CellIndex(const _ulong& _dwIndex) { m_dwNaviCellIndex = _dwIndex; }

public:
	static CNaviMesh* Create(_DEVICE _pDevice);
	virtual CMesh* Clone(void * _pArg = nullptr) override;
	virtual void Free() override;

private:
	vector<CNaviCell*>		m_vecNaviCell;
	_ulong					m_dwNaviCellIndex;
};

END

#endif // !__NAVIMESH_H__