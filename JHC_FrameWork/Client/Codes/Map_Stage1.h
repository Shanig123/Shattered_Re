#pragma once

#ifndef __MAP_STAGE1_H__
#define __MAP_STAGE1_H__

#include "GameObject.h"

USING(Engine)

class CMap_Stage1 final : public CGameObject
{
private:
	enum EMapType
	{
		Glow,
		Tutorial,
		Door,
		Boss,
		End
	};

private:
	explicit CMap_Stage1(_DEVICE _pDevice);
	explicit CMap_Stage1(const CMap_Stage1& _rOther);
	virtual ~CMap_Stage1() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr);
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	//virtual _uint LateUpdate_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_EFFECT _pEffect);
	HRESULT Ready_MapCollider();

public:
	static CMap_Stage1* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr);
	virtual void Free() override;

private:
	class CStaticMesh*			m_pMesh[EMapType::End];
	class CTransform*			m_pTransform;
	class CShader*				m_pShader;
	//class COptimization*        m_pOptimization;

	TRANSFORM_DESC				m_tTransform;

};

#endif // !__MAP_STAGE1_H__