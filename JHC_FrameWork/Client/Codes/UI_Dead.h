#pragma once

#ifndef __UI_DEAD_H__
#define __UI_DEAD_H__

#include "UIBase.h"

USING(Engine)

class CUI_Dead final : public CUIBase
{
private:
	explicit CUI_Dead(_DEVICE _pDevice);
	explicit CUI_Dead(const CUI_Dead& _rOther);
	virtual ~CUI_Dead() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual _uint Update_GameObject(const _float& _fDeltaTime) override;
	//virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;
private:
	void DissolveControl(const _float& _fDeltaTime);
	HRESULT Render_Normal(_EFFECT& _pEffect);
private:
	_bool m_bFadeIn;
	_bool m_bFadeOut;
	_bool m_bFadeNormal;
	_bool m_bCheckFadeOut;
public:
	static CUI_Dead* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

private:
};

#endif // !__UI_DEAD_H__