#pragma once

#ifndef __VIBUFFER_CUBETEXTURE_H__
#define	__VIBUFFER_CUBETEXTURE_H__

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_CubeTexture final : public CVIBuffer
{
protected:
	explicit CVIBuffer_CubeTexture(_DEVICE _pDevice);
	explicit CVIBuffer_CubeTexture(const CVIBuffer_CubeTexture& _rOther);
	virtual ~CVIBuffer_CubeTexture() = default;

protected:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component_Clone(void* _pArg = nullptr) override;

public:
	static CVIBuffer_CubeTexture* Create(_DEVICE _pDevice);
	virtual CComponent* Clone(void* _pArg = nullptr) override;

};

END

#endif // !__VIBUFFER_CUBETEXTURE_H__