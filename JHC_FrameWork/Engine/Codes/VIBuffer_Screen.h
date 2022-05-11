#pragma once
#ifndef __VIBUFFER_SCREEN_H__
#define __VIBUFFER_SCREEN_H__

#include "VIBuffer.h"
BEGIN(Engine)
class CVIBuffer_Screen : public CVIBuffer
{
protected:
	explicit CVIBuffer_Screen(_DEVICE _pDevice);
	explicit CVIBuffer_Screen(const CVIBuffer_Screen& _rhs);
	virtual ~CVIBuffer_Screen();

protected:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component_Clone(void * _pArg = nullptr) override;
public:
	static CVIBuffer_Screen* Create(_DEVICE _pDevice);
	virtual CComponent * Clone(void * _pArg = nullptr) override;
};
END
#endif // !__VIBUFFER_SCREEN_H__

