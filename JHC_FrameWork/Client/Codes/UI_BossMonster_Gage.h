#pragma once

#ifndef __UI_BOSSMONSTER_GAGE_H__
#define __UI_BOSSMONSTER_GAGE_H__

#include "UIBase.h"

USING(Engine)

class CUI_BossMonster_Gage : public CUIBase
{
protected:
	explicit CUI_BossMonster_Gage(_DEVICE _pDevice);
	explicit CUI_BossMonster_Gage(const CUI_BossMonster_Gage& _rOther);
	virtual ~CUI_BossMonster_Gage()= default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual _uint LateUpdate_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;
	void	Load_ini();
public:
	static CUI_BossMonster_Gage* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;
	void		Get_Hp(_float	_fHp, _float	_fMaxHp);
	void		Set_Render(_bool _bRender);
private:
	virtual	HRESULT Add_Com() override;
	class CTexture*			m_pHpBarTex;
	_bool					m_bRender;
	_float					m_fMonHp;
	_float					m_fOriginSize;
	_float					m_fOriginPos;
};

#endif // !__UI_BOSSMONSTER_GAGE_H__
