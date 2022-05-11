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
		Near_1,		// ����
		Near_2,		// ��������ٰ� �ø���. �ø����� �ǰ�����
		Far_1,		// ��������
		Far_2,		// ���� ���
		Far_3,		// ���� ����
		Walk_F,		// ������ �ȱ�
		Walk_B,		// �ڷ� �ȱ�
		Walk_L,		// �������� �ȱ�
		Walk_R,		// ���������� �ȱ�
		Hit,		// ����
		Death,		// ����
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
	virtual void Get_Damaged()	override;	//������ �Ͻʽÿ�.
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