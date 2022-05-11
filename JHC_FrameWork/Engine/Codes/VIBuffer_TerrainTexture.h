#pragma once

#ifndef __VIBUFFER_TERRAINTEXTURE_H__
#define	__VIBUFFER_TERRAINTEXTURE_H__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_TerrainTexture final : public CVIBuffer
{
protected:
	explicit CVIBuffer_TerrainTexture(_DEVICE _pDevice, wstring _wstrFilePath, _uint _iVertexCountX, _uint _iVertexCountZ, _float _fVertexInterval = 1.f);
	explicit CVIBuffer_TerrainTexture(const CVIBuffer_TerrainTexture& _rOther);
	virtual ~CVIBuffer_TerrainTexture() = default;

protected:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component_Clone(void* _pArg = nullptr) override;
public:
	_bool IsOnTerrain(_float3* _pTargetPos);

private:
	_uint* Load_HeightMap(wstring _wstrFilePath);

public:
	static CVIBuffer_TerrainTexture* Create(_DEVICE _pDevice, wstring _wstrFilePath, _uint _iVertexCountX, _uint _iVertexCountZ, _float _fVertexInterval = 1.f);
	virtual CComponent* Clone(void* _pArg = nullptr) override;

private:
	wstring		m_wstrFilePath;
	_uint		m_iVertexCountX;
	_uint		m_iVertexCountZ;
	_float		m_fVertexInterval;
};

END

#endif // !__VIBUFFER_TERRAINTEXTURE_H__