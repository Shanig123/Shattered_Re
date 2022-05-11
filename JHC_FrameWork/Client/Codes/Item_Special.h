#pragma once

#ifndef __ITEM_SPECIAL_H__
#define __ITEM_SPECIAL_H__

#include "JBaseMonster.h"

USING(Engine)

class CItem_Special final : public CJBaseMonster
{
private:
	enum class PATTERN
	{
		Near_1,		// 베기
		Near_2,		// 내려찍었다가 올리기. 올릴때도 피격판정
		Far_1,		// 마법쓰기
		Far_2,		// 돌진 찌르기
		Far_3,		// 돌진 베기
		Walk_F,		// 앞으로 걷기
		Walk_B,		// 뒤로 걷기
		Walk_L,		// 왼쪽으로 걷기
		Walk_R,		// 오른쪽으로 걷기
		Hit,		// 맞음
		Death,		// 죽음
		End
	};

protected:
	explicit CItem_Special(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CItem_Special(const CItem_Special& _rOther);
	virtual ~CItem_Special() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

public:/*Setter*/
	inline void Set_SleepMode() {
		m_eCurTotalPattern = eMonsterPattern::Sleep;
		CJBaseMonster::PatternTotalControl(m_fDeltaTime);
		CJBaseMonster::Do_TotalPattern(m_fDeltaTime);

	};

protected:
	virtual void Do_Idle(const _float & _fDeltaTime) override;
	virtual void Do_Attack(const _float & _fDeltaTime) override;
	virtual void Do_Hit(const _float & _fDeltaTime) override;
	virtual void Do_Walk(const _float & _fDeltaTime) override;
	virtual void Do_Other(const _float & _fDeltaTime) override;
	virtual void Get_Damaged()	override;	//재정의 하십시오.
private:
	HRESULT			UI_Clone();
	HRESULT			Add_Mesh();
private:
	void			Hit_Pattern(const _float & _fDeltaTime);
	void			Death_Pattern(const _float & _fDeltaTime);

	void			Death_Sound();

private:
	CStaticMesh*		m_pRuneMesh;
	CStaticMesh*		m_pStaticMesh;
	CShader*			m_pStaticShader;

	eItemCode				m_eItemCode;

	_bool				m_bAttackMove;
	_bool				m_bDeadEffect;

	_float4				m_vMeshColor;
	_float4				m_vInitilizeColor;
	_float4				m_vEffectColor;

	_float				m_fLifeTime;
	_float				m_fRandomizeColor_fixValue;
	_float				m_fRandomizeColor_MulValue;
	_float				m_fAddLifeTime;
	_float				m_fRotateRandom;
public:
	static CItem_Special* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
protected:
	virtual void Free() override;	




};

#endif // !__ITEM_SPECIAL_H__