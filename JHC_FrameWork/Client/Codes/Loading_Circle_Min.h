#pragma once

#ifndef __LOADING_CIRCLE_MIN_H__
#define __LOADING_CIRCLE_MIN_H__

#include "UIBase.h"

USING(Engine)

class CLoading_Circle_Min final : public CUIBase
{
protected:
	explicit CLoading_Circle_Min(_DEVICE _pDevice);
	explicit CLoading_Circle_Min(const CLoading_Circle_Min& _rOther);
	virtual ~CLoading_Circle_Min() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	//virtual _uint LateUpdate_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;
private:
	void DissolveControl(const _float& _fDeltaTime);
private:
	_bool	m_bCheckFadeIn;
	_bool	m_bCheckInit;
public:
	static CLoading_Circle_Min* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

};

#endif // !__LOADING_CIRCLE_MIN_H__