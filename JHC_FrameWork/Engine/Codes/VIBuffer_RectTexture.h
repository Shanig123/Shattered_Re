#pragma once

#ifndef __VIBUFFER_RECTTEXTURE_H__
#define	__VIBUFFER_RECTTEXTURE_H__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_RectTexture final : public CVIBuffer
{
protected:
	explicit CVIBuffer_RectTexture(_DEVICE _pDevice);
	explicit CVIBuffer_RectTexture(const CVIBuffer_RectTexture& _rOther);
	virtual ~CVIBuffer_RectTexture() = default;

protected:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component_Clone(void* _pArg = nullptr) override;

public:
	static CVIBuffer_RectTexture* Create(_DEVICE _pDevice);
	virtual CComponent* Clone(void* _pArg = nullptr) override;


};

END

#endif // !__VIBUFFER_RECTTEXTURE_H__