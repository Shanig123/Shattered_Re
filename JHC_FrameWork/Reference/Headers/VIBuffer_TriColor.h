#pragma once

#ifndef __VIBUFFER_TRICOLOR_H__
#define __VIBUFFER_TRICOLOR_H__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_TriColor final : public CVIBuffer
{
private:
	explicit CVIBuffer_TriColor(_DEVICE _pDevice, _COLOR _Color);
	explicit CVIBuffer_TriColor(const CVIBuffer_TriColor& _rOther);
	virtual ~CVIBuffer_TriColor() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component_Clone(void* _pArg = nullptr) override;
	virtual _uint Update_Component(const _float& _fDeltaTime) override;
	virtual HRESULT Render_VIBuffer(_EFFECT* _pEffect = nullptr) override;

public:
	static CVIBuffer_TriColor* Create(_DEVICE _pDevice, _COLOR _Color);
	virtual CComponent* Clone(void* _pArg = nullptr) override;
	virtual void Free() override;

private:
	_COLOR			m_tColor;
};

END

#endif // !__VIBUFFER_TRICOLOR_H__