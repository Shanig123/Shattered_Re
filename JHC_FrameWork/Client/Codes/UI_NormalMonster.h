#pragma once
#ifndef __UI_NORMALMONSTER_H__
#define __UI_NORMALMONSTER_H__

#include "UIBase.h"
#include "JBaseMonster.h"
USING(Engine)

class CUI_NormalMonster : public CUIBase
{
public:
	typedef struct tagMobUIInfo
	{
		tagMobUIInfo() :tUIDesc(), pFromObj(nullptr){ ZeroMemory(&tStatDesc, sizeof(STAT_DESC)); }
		UI_DESC		tUIDesc;
		STAT_DESC	tStatDesc;
		CJBaseMonster* pFromObj;
	}MONSTER_UI_DESC;

protected:
	explicit CUI_NormalMonster(_DEVICE _pDevice);
	explicit CUI_NormalMonster(const CUI_NormalMonster& _rhs);
	virtual ~CUI_NormalMonster();

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
	CJBaseMonster*	m_pOutInfo_Object;
public:
	static CUI_NormalMonster* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free();
};


#endif // !__UI_NORMALMONSTER_H__
