#pragma once

#ifndef __EndingCredit_H__
#define __EndingCredit_H__

#include "UIBase.h"

USING(Engine)

class CEndingCredit final : public CUIBase
{
public:
	explicit CEndingCredit(_DEVICE _pDevice);
	explicit CEndingCredit(const CEndingCredit& _rOther);
	virtual ~CEndingCredit() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;
private:
	void DissolveControl(const _float& _fDeltaTime);
private:
	_bool	m_bCheckFadeIn;
	_bool	m_bCheckInit;

	_float	m_fEndTime;
public:
	static CEndingCredit* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

};

#endif // !__EndingCredit_H__