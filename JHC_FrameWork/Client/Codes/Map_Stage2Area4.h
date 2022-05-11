#pragma once

#ifndef __MAP_STAGE2AREA4_H__
#define __MAP_STAGE2AREA4_H__

#include "GameObject.h"

USING(Engine)

class CMap_Stage2Area4 final : public CGameObject
{
private:
	enum EMapType
	{
		Area4Fence,
		Area4House,
		End
	};

private:
	explicit CMap_Stage2Area4(_DEVICE _pDevice);
	explicit CMap_Stage2Area4(const CMap_Stage2Area4& _rOther);
	virtual ~CMap_Stage2Area4() = default;

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
	static CMap_Stage2Area4* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr);
	virtual void Free() override;

private:
	class CStaticMesh*			m_pMesh[EMapType::End];
	class CTransform*			m_pTransform;
	class CShader*				m_pShader;

	TRANSFORM_DESC				m_tTransform;

};

#endif // !__MAP_STAGE2AREA4_H__