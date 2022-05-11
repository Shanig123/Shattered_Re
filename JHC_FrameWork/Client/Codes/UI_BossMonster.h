#pragma once

#ifndef __UI_BOSSMONSTER_H__
#define __UI_BOSSMONSTER_H__

#include "UIBase.h"

USING(Engine)

class CUI_BossMonster : public CUIBase
{
public:
	enum class eBossName
	{
		Preto,
		Malkhel,
		End
	};
protected:
	explicit CUI_BossMonster(_DEVICE _pDevice);
	explicit CUI_BossMonster(const CUI_BossMonster& _rOther);
	virtual ~CUI_BossMonster()= default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	//virtual _uint LateUpdate_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

public:/*Setter*/
	//inline void		Set_RenderOnOff(_bool _bRender) { m_bRender = _bRender; }
	void Set_ParentClass(const LPPARENTOBJECT& _pParent) { NULL_CHECK_RETURN(_pParent, ); m_pParentObject = _pParent;/* Safe_AddRef(_pParent);*/ }
	void Set_BossName(eBossName _eBossName) { m_eBossName = _eBossName; }
public:/*Getter*/
	
private:
	HRESULT Render_Hp(_EFFECT& _pEffect);
	HRESULT Render_Name(_EFFECT& _pEffect,const _float3& _vPos);
	/*void		Set_Hp(_uint iCheck);
	virtual	HRESULT Add_Com() override;*/
private:
	LPPARENTOBJECT			m_pParentObject;

	eBossName				m_eBossName;

	_float					m_fMonHp;
	_float					m_fOriginSize;
	_float					m_fOriginPos;
	_float					m_fAccTime;

public:
	static CUI_BossMonster* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;


};

#endif // !__UI_BOSSMONSTER_H__
