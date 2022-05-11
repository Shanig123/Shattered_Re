#pragma once

#ifndef __UI_SCREEN_H__
#define __UI_SCREEN_H__

#include "UIBase.h"

USING(Engine)

class CUI_Screen final : public CUIBase
{
private:
	explicit CUI_Screen(_DEVICE _pDevice);
	explicit CUI_Screen(const CUI_Screen& _rOther);
	virtual ~CUI_Screen() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual _uint Update_GameObject(const _float& _fDeltaTime) override;
	//virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;
private:
	void DissolveControl(const _float& _fDeltaTime);

private:
	_bool m_bCheckFadeIn;
	_bool m_bCheckInit;
	_bool m_bCheckFadeOut;
public:
	static CUI_Screen* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

private:
};

#endif // !__UI_SCREEN_H__