#pragma once

#ifndef __MAP_SHRINE_H__
#define __MAP_SHRINE_H__

#include "GameObject.h"

USING(Engine)

class CMap_Shrine final : public CGameObject
{
private:
	enum EMapType
	{
		Shrine,
		BrokenDoor,
		End
	};

private:
	explicit CMap_Shrine(_DEVICE _pDevice);
	explicit CMap_Shrine(const CMap_Shrine& _rOther);
	virtual ~CMap_Shrine() = default;

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
	static CMap_Shrine* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr);
	virtual void Free() override;

private:
	class CStaticMesh*			m_pMesh[EMapType::End];
	class CTransform*			m_pTransform;
	class CShader*				m_pShader;

	TRANSFORM_DESC				m_tTransform;

};

#endif // !__MAP_SHRINE_H__