#pragma once

#ifndef __Map_Stage2Chess_H__
#define __Map_Stage2Chess_H__

#include "GameObject.h"

class CMap_Stage2Chess final : public CGameObject
{
private:
	enum EMapType
	{
		ChessPannel0,
		ChessPannel1,
		ChessPannel2,
		ChessPannel3,
		ChessPannel4,
		ChessPannel5,
		ChessPannel6,
		ChessPannel7,
		ChessPannel8,
		ChessPannel9,
		ChessPannel10,
		ChessPannel11,
		ChessPannel12,
		ChessPannel13,
		ChessPannel14,
		ChessPannel15,
		ChessPannel16,
		ChessPannel17,
		ChessPannel18,
		ChessPannel19,
		ChessPannel20,
		ChessPannel21,
		ChessPannel22,
		ChessPannel23,
		ChessPannel24,
		ChessBoard,
		ChessPice,
		ChessPlayerStand,
		End
	};

private:
	explicit CMap_Stage2Chess(_DEVICE _pDevice);
	explicit CMap_Stage2Chess(const CMap_Stage2Chess& _rOther);
	virtual ~CMap_Stage2Chess() = default;

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
	_uint& Get_RenderPass(_uint _iIndex);
	_float3& Get_RimColor(_uint _iIndex);
	void Set_RanderPass(_uint _iIndex, _uint _iPass, _float3& _Color = _float3(0.f, 0.f, 0.f));

public:
	static CMap_Stage2Chess* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr);
	virtual void Free() override;

private:
	class CStaticMesh*			m_pMesh[EMapType::End];
	class CTransform*			m_pTransform;
	class CShader*				m_pShader;

	TRANSFORM_DESC				m_tTransform;

	_float						m_fRimPower;
	_uint						m_iRanderPass[EMapType::End];
	vector<_float3>				m_vecRimColor;

};

#endif // !__Map_Stage2Chess_H__