#pragma once

#ifndef __MENU_UI_H__
#define __MENU_UI_H__

#include "UIBase.h"

USING(Engine)

class CMenu_UI final : public CUIBase
{
private:
	explicit CMenu_UI(_DEVICE _pDevice);
	explicit CMenu_UI(const CMenu_UI& _rOther);
	virtual ~CMenu_UI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	//virtual _uint LateUpdate_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;
public:
	_int	Get_IScene() { return m_iTexIdx; }
private:
	void DissolveControl(const _float& _fDeltaTime);

private:
	_int	m_iTexIdx;
	_bool	m_bCheckFadeIn;
	_bool	m_bCheckInit;
public:
	static CMenu_UI* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;




};

#endif // !__MENU_UI_H__