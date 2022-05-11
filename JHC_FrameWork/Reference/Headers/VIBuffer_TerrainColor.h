#pragma once

#ifndef __VIBUFFER_TERRAINCOLOR_H__
#define	__VIBUFFER_TERRAINCOLOR_H__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_TerrainColor final : public CVIBuffer
{
private:
	explicit CVIBuffer_TerrainColor(_DEVICE _pDevice, _COLOR _Color, _uint _iVertexCountX, _uint _iVertexCountZ, _float _fVertexInterval = 1.f);
	explicit CVIBuffer_TerrainColor(const CVIBuffer_TerrainColor& _rOther);
	virtual ~CVIBuffer_TerrainColor() = default;

protected:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component_Clone(void* _pArg = nullptr) override;

public:
	HRESULT Reset_VertexCount(_uint _iVertexCountX, _uint _iVertexCountZ);
	_uint Get_VertexCountX() { return m_iVertexCountX; }
	_uint Get_VertexCountZ() { return m_iVertexCountZ; }
	const _float3* Get_VertexPos() const { return m_pPos; }

public:
	static CVIBuffer_TerrainColor* Create(_DEVICE _pDevice, _COLOR _Color, _uint _iVertexCountX, _uint _iVertexCountZ, _float _fVertexInterval = 1.f);
	virtual CComponent* Clone(void* _pArg = nullptr) override;
	virtual void Free() override;

private:
	_COLOR		m_tColor;
	_uint		m_iVertexCountX = 0;
	_uint		m_iVertexCountZ = 0;
	_float		m_fVertexInterval = 1.f;
	_float3*	m_pPos;
};

END

#endif // !__VIBUFFER_TERRAINCOLOR_H__