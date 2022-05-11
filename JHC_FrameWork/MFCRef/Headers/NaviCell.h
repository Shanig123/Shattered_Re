#pragma once

#ifndef __NAVICELL_H__
#define __NAVICELL_H__

#include "Base.h"
#include "NaviLine.h"

BEGIN(Engine)

class ENGINE_DLL CNaviCell final : public CBase
{
public:
	enum POINT { POINT_A , POINT_B, POINT_C, POINT_END };
	enum NEIGHBOR { NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
	enum COMPAREMOVE { MOVE, STOP };

private:
	explicit CNaviCell(_DEVICE _pDevice);
	virtual ~CNaviCell() = default;

public:
	HRESULT Ready_NaviCell(const _ulong& _dwCellIndex, const _float3* _pPointA, const _float3* _pPointB, const _float3* _pPointC);
	void Render_NaviCell();

public:
	_bool Compare_Point(const _float3* _pFirst, const _float3* _pSecond, CNaviCell* _pCell);
	COMPAREMOVE Compare(const _float3* _pEndPos, _ulong* _pNaviCellIndex);

private:
	const _ulong* Get_NaviCellIndex() { return &m_dwNaviCellIndex; }

public:
	CNaviCell* Get_Neighbor(NEIGHBOR _eType) { return m_pNeighbor[_eType]; }
	const _float3* Get_Point(POINT _eType) { return &m_vPoint[_eType]; }
	void Set_Point(POINT _eType, const _float3& _vPos) { m_vPoint[_eType] = _vPos; }

public:
	void Set_Neighbor(NEIGHBOR _eType, CNaviCell* _pNeighbor) { m_pNeighbor[_eType] = _pNeighbor; }

public:
	static CNaviCell* Create(_DEVICE _pDevice, const _ulong& _dwCellIndex, const _float3* _pPointA, const _float3* _pPointB, const _float3* _pPointC);
	virtual void Free() override;

private:
	_DEVICE			m_pDevice;

	_float3			m_vPoint[POINT_END];
	CNaviCell*		m_pNeighbor[NEIGHBOR_END];
	CNaviLine*		m_pNaviLine[LINE_END];

	LPD3DXLINE		m_pD3DXLine;
	_ulong			m_dwNaviCellIndex;
};

END

#endif // !__NAVICELL_H__