#pragma once

#ifndef __VIBUFFER_XYZLINECOLOR_H__
#define __VIBUFFER_XYZLINECOLOR_H__

#include "VIBuffer.h"
#include "Sphere_Collider.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_xyzLineColor final : public CVIBuffer
{
private:
	explicit CVIBuffer_xyzLineColor(_DEVICE _pDevice);
	explicit CVIBuffer_xyzLineColor(const CVIBuffer_xyzLineColor& _rOther);
	virtual ~CVIBuffer_xyzLineColor() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component_Clone(void* _pArg = nullptr) override;
	virtual _uint Update_Component(_float& _fDeltaTime) override;
	virtual HRESULT Render_VIBuffer(_EFFECT* _pEffect = nullptr) override;

public:
	static CVIBuffer_xyzLineColor* Create(_DEVICE _pDevice);
	virtual CComponent* Clone(void* _pArg = nullptr) override;
	virtual void Free() override;

private:
	CSphere_Collider*			m_pSphere_Collider[3];
};

END

#endif // !__VIBUFFER_XYZLINECOLOR_H__