#pragma once

#ifndef __MAP_STAGE2_H__
#define __MAP_STAGE2_H__

#include "GameObject.h"
#include "Map_Stage2Area1.h"
#include "Map_Stage2Area2.h"
#include "Map_Stage2Area3.h"
#include "Map_Stage2Area4.h"
#include "Map_Stage2Area5.h"
#include "Map_Stage2Area6.h"
#include "Map_Stage2Area7.h"
#include "Map_Stage2Area8.h"
#include "Map_Stage2BossRoom.h"
#include "Map_Stage2Chess.h"
#include "Map_Stage2Trap.h"

USING(Engine)

class CMap_Stage2 final : public CGameObject
{
private:
	enum EMapType
	{
		Stage2Start,
		Stage2Floor,
		Stage2Wall,
		End
	};

	enum EMapArea
	{
		Area1,
		Area2,
		Area3,
		Area4,
		Area5,
		Area6,
		Area7,
		Area8,
		BossRoom,
		Chess,
		Trap,
		EndArea
	};

protected:
	explicit CMap_Stage2(_DEVICE _pDevice);
	explicit CMap_Stage2(const CMap_Stage2& _rOther);
	virtual ~CMap_Stage2() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr);
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_EFFECT _pEffect);
	HRESULT Ready_MapCollider();
	HRESULT Add_Area1();
	HRESULT Add_Area2();
	HRESULT Add_Area3();

public:	
	HRESULT Add_Area4();
	HRESULT Add_Area5();
	HRESULT Add_Area6();
	HRESULT Add_Area7();
	HRESULT Add_Area8();
	HRESULT Add_BossRoom();
	HRESULT Add_Chess();
	HRESULT Add_Trap();

private:
	void Render_Area1();
	void Render_Area2();
	void Render_Area3();
	void Render_Area4();
	void Render_Area5();
	void Render_Area6();
	void Render_Area7();
	void Render_Area8();
	void Render_BossRoom();
	void Render_Chess();
	void Render_Trap();

public:
	static CMap_Stage2* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr);
	virtual void Free() override;

private:
	class CStaticMesh*			m_pMesh[EMapType::End];
	class CTransform*			m_pTransform;
	class CShader*				m_pShader;
	class CGameObject*			m_pAreaObject[EMapArea::EndArea];
	class COptimization*        m_pOptimization;

	TRANSFORM_DESC				m_tTransform;

	// 맵 로딩
	class CLoading*				m_pLoading;
	_bool						m_bLoadingThreadStart;
	_bool						m_bLoadingThreadEnd;

	// 체스->보스 벽
	_bool						m_bChessToBoss;
};

#endif // !__MAP_STAGE2_H__