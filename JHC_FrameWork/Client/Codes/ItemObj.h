#pragma once
#ifndef __ITEMOBJ_H__
#define __ITEMOBJ_H__

#include "GameObject.h"
#include "MapStaticObjectBase.h"

USING(Engine)
class CItemObj : public CMapStaticObjectBase
{
public:
	//typedef struct tagItemObjDesc
	//{
	//	tagItemObjDesc() {
	//		ZeroMemory(this, sizeof(tagItemObjDesc)); eItemCode = eItemCode::Item_End;	}
	//	_float3 vCreatePos;
	//	eItemCode eItemCode;
	//}ITEM_OBJ_DESC;
protected:
	explicit CItemObj(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CItemObj(const CItemObj& _rhs);
	virtual ~CItemObj() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;
	
public:
	const eItemCode& Get_ItemCode() const { return m_eItemCode; }

public:
	void Set_ItemCode(const eItemCode& _eItemCode) { m_eItemCode = _eItemCode; }

private:
	HRESULT ThisAddCom();
	void DeadParticleEffect(const _float & _fDeltaTime);
	void GenerateParticle(const _float& _fDeltaTime);
	void Check_GenerateParticle(const _float& _fDeltaTime);
private:
	class COptimization*        m_pOptimization;

	ITEMTRANSFORM_DESC		m_tItemTransformDesc;

	eItemCode				m_eItemCode;
	const _float4			m_vEffectColor;
	const _float4			m_vMeshColor;
	const _float			m_fScaleUp;

	_float					m_fParticleCreateCoolTime0;
	_float					m_fParticleCreateCoolTime1;

	_float					m_fThisTime;

	//플레이어가 오브젝트로부터 해당 범위 안에 있을 경우 파티클을 생성합니다.
	_float					m_fParticleCreate_Range;

	MyItemData*				m_pData;
	CStaticMesh*			m_pRuneMesh;

	_bool					m_bDeadEffect;


	_float3					m_vStartPos, m_vEndPos, m_vDir;
	_float					m_fJumpTime,m_fPower;
	void					Update_Jump(const _float& _fDeltaTime);
public:
	static CItemObj* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;

};


#endif // !__ITEMOBJ_H__
