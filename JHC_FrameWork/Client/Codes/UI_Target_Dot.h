#pragma once
#ifndef __UI_TARGET_DOT_H__
#define __UI_TARGET_DOT_H__

#include "UIBase.h"
#include "JBaseMonster.h"
USING(Engine)

class CUI_Target_Dot : public CUIBase
{
protected:
	explicit CUI_Target_Dot(_DEVICE _pDevice);
	explicit CUI_Target_Dot(const CUI_Target_Dot& _rhs);
	virtual ~CUI_Target_Dot();

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void* _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float& _fDeltaTime) override;
	//virtual _uint LateUpdate_GameObject(const _float& _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;
protected:
	//virtual HRESULT Add_Com();
private:
	//자전*위치 행렬곱을 반환합니다.
	_float4x4 IsBillBoarding();
private:
	STAT_DESC		m_tStatusDesc;
public:
	static CUI_Target_Dot* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free();
};


#endif // !__UI_TARGET_DOT_H__
