#pragma once

#ifndef __MAP_STAGE2AREA5_H__
#define __MAP_STAGE2AREA5_H__

#include "GameObject.h"

USING(Engine)

class CMap_Stage2Area5 final : public CGameObject
{
private:
	enum EMapType
	{
		Area5Fence,
		Area5House,
		Area5SmallHouse,
		Area5BigHouse,
		Area5Meteor,
		Area5Crate,
		End
	};

private:
	explicit CMap_Stage2Area5(_DEVICE _pDevice);
	explicit CMap_Stage2Area5(const CMap_Stage2Area5& _rOther);
	virtual ~CMap_Stage2Area5() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr);
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_EFFECT _pEffect);
	HRESULT Ready_MapCollider();

public:
	void Add_RenderList();

public:
	_uint Get_StageMeshCount() { return EMapType::End; }
	CStaticMesh* Get_StageMesh(_uint _iIndex) { return m_pMesh[_iIndex]; }

public:
	static CMap_Stage2Area5* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr);
	virtual void Free() override;

private:
	class CStaticMesh*			m_pMesh[EMapType::End];
	class CTransform*			m_pTransform;
	class CShader*				m_pShader;

	TRANSFORM_DESC				m_tTransform;

};

#endif // !__MAP_STAGE2AREA5_H__