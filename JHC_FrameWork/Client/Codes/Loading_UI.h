#pragma once

#ifndef __LOADING_UI_H__
#define __LOADING_UI_H__

#include "UIBase.h"

USING(Engine)

class CLoading_UI final : public CUIBase
{
protected:
	explicit CLoading_UI(_DEVICE _pDevice);
	explicit CLoading_UI(const CLoading_UI& _rOther);
	virtual ~CLoading_UI() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

public:
	static CLoading_UI* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

};

#endif // !__LOADING_UI_H__