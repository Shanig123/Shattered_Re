#pragma once
#ifndef __CHESS_H__
#define __CHESS_H__

#include "GameObject.h"

USING(Engine)

class CChess : public CGameObject
{
	enum class EGameType
	{
		Queen,
		Knight,
		End
	};

	enum class EQueenResult
	{
		Type1,
		Type2,
		End
	};

protected:
	explicit CChess(_DEVICE _pDevice);
	explicit CChess(const CChess& _rhs);
	virtual ~CChess() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

public:
	void Add_ChessPiece(class CChessPiece_Queen& _ChessPiece) { m_vecChessQueen.push_back(&_ChessPiece); }

private:
	void Key_Input();
	void InitChess();
	void ResetChess();
	void EndChess();
	void ResultCheck();
	_bool QueenResultCheck();
	void SelectChess(class CMap_Stage2Chess& _Chess, _int _iSelect, _uint _iPass, _float3 _Color = _float3(0.f, 0.f, 0.f));
	void SelectQueen(class CChessPiece_Queen& _ChessPiece, _uint _iPass, _float3 _Color = _float3(0.f, 0.f, 0.f));
	void OverlapCheck();
	void OverlapCheckUp(_uint _iIndex);
	void OverlapCheckDown(_uint _iIndex);
	void OverlapCheckLeft(_uint _iIndex);
	void OverlapCheckRight(_uint _iIndex);
	void OverlapCheckUpLeft(_uint _iIndex);
	void OverlapCheckUpRight(_uint _iIndex);
	void OverlapCheckDownLeft(_uint _iIndex);
	void OverlapCheckDownRight(_uint _iIndex);

	void KnightNavi();
	void ViewNavi(class CMap_Stage2Chess& _Chess, _int _iSelect, _uint _iPass, _float3 _Color = _float3(0.f, 0.f, 0.f));

public:
	static CChess* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
protected:
	virtual void Free()override;

private:
	class CMap_Stage2Chess*					m_pStage2Chess;
	EGameType								m_eGameType;
	EQueenResult							m_eQueenResultType;

	vector<_float3>							m_vecChessOriPos;		// �ʱ�ȭ ��ġ
	vector<class CChessPiece_Queen*>		m_vecChessQueen;		// �� ������Ʈ
	vector<_int>							m_vecChessQueenIndex;	// ���� ü���� �ε���	

	map<class CChessPiece_Queen*, _int>		m_mapChessQueenCheck;	// ��� üũ�� ����
	vector<_int>							m_vecChessQueenResult[(_uint)EGameType::End][(_uint)EQueenResult::End];		// ����

	class CChessPiece_Knight*				m_pChessKnight;			// ����Ʈ ������Ʈ
	vector<_int>							m_vecChessKnightIndex;	// ����Ʈ�� ü���� �ε���
	vector<_bool>							m_vecChessKnightNavi;

	class CChessPiece_Queen*				m_pSelectQueen;

	_bool									m_bFirstStart;					// ù ����
	_bool									m_bGameStart;					// ���� ����
	_bool									m_bOverlapCheck;				// �ߺ� üũ;
	_bool									m_bOverlap;						// �ߺ� Ȯ��
	_bool									m_bNaviView;					// ����Ʈ �׺� ǥ��
	_int									m_iSelectChess;					// ü���� ����
	_int									m_iOldSelectChess;				// ���� ü���� ����
	_int									m_iOldSelectChessRenderPass;	// ���� ü���� �����н�
	_float3									m_OldSelectChessColor;			// ���� ü���� �÷�
	_int									m_iPiece;						// �� �ε�����
	_int									m_iSelectQueenIndex;			// ������ �� �ε���
};

#endif // !__CHESS_H__

