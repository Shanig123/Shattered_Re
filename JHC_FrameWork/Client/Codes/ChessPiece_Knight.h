#pragma once

#ifndef __CHESSPIECE_KNIGHT_H__
#define __CHESSPIECE_KNIGHT_H__

#include "GameObject.h"

USING(Engine)

class CChessPiece_Knight final : public CGameObject
{
private:
	enum class EChessState
	{
		Ready,
		Awake,
		Idle,
		Dead,
		Sleep,
		End
	};

private:
	explicit CChessPiece_Knight(_DEVICE _pDevice, const wstring& _PrototypeName);
	explicit CChessPiece_Knight(const CChessPiece_Knight& _rOther);
	virtual ~CChessPiece_Knight() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

protected:
	virtual HRESULT Add_Component();
	virtual HRESULT SetUp_ConstantTable(_EFFECT& _pEffect);

private:
	//��ü ������ �����մϴ�. 1���� ȣ��Ǵ� �Լ����� �����մϴ�.
	HRESULT PatternTotalControl(const _float& _fDeltaTime);
	//�������� �ܺ� Ŭ���� ���� ���� �޴� �Լ��� �����մϴ�.
	HRESULT OutClassCheck(const _float& _fDeltaTime);
	/*���Ͽ� ���� �Լ��� ��� ȣ���ϵ��� �����մϴ�.*/
	HRESULT Do_TotalPattern(const _float& _fDeltaTime);

private:
	//�׻� ���� �Ǵ� �Լ�
	void Do_Always(const _float& _fDeltaTime);
	//���� Ȱ��ȭ �� �׻� �����Ǵ� �Լ�, �޸�� �������� ����.
	void Do_AfterActivation_Always(const _float& _fDeltaTime);
	//Ȱ��ȭ �� �ѹ� ����Ǵ� �Լ�
	void Do_Activation(const _float& _fDeltaTime);
	//Ȱ��ȭ ���� �� ȣ��� 
	void Do_Activating(const _float& _fDeltaTime);

private:
	//������Ʈ ���� �� ���� ȣ�� �Ǵ� �Լ��Դϴ�.
	void Update_Init(const _float& _fDeltaTime);
	void Create_RigidBody();
	void Delete_RigidBody();

	void DissolveTimer(const _float& _fDeltaTime);

public:
	void Set_ChessPos(_float3& _vPos, _float _RotateY = 0.f);

public:
	static CChessPiece_Knight* Create(_DEVICE _pDevice, const wstring& _PrototypeName);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;	

private:
	class CStaticMesh*			m_pMesh;
	class CTransform*			m_pTransform;
	class CShader*				m_pShader;

	_bool						m_bUpdateInit;
	_float						m_fRimPower;
	_uint						m_iShaderPass;

	EChessState					m_eCurChessState;
	EChessState					m_ePreChessState;

	_float						m_fDissolveTime, m_fDissolveSpeed;
	_float4						m_vDissolveColor;
};

#endif // !__CHESSPIECE_KNIGHT_H__