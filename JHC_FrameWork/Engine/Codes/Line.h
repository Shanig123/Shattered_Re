#pragma once

#ifndef __LINE_H__
#define __LINE_H__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CLine final : public CComponent
{
private:
	explicit CLine(_DEVICE _pDevice, _float3 _vDist, _COLOR _Color);
	explicit CLine(const CLine& _rOther);
	virtual ~CLine() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component_Clone(void* _pArg = nullptr) override;
	virtual _uint Update_Component(const _float& _fDeltaTime) override;
	HRESULT Render_Line(_EFFECT & _pEffect, _float4x4* _matWorld);
	HRESULT Render_Line(_float4x4* _matWorld);
public:
	static CLine* Create(_DEVICE _pDevice, _float3 _vDist, _COLOR _Color);
	virtual CComponent* Clone(void* _pArg = nullptr) override;
	virtual void Free() override;

private:
	_TEXTURE9			m_pTexture;
	_VertexBuffer		m_pVB;
	_uint				m_iVertexSize;
	_uint				m_iVertexCount;
	_uint				m_iFVF;

	_float3				m_vDist;
	_COLOR				m_Color;
};

END

#endif // !__LINE_H__