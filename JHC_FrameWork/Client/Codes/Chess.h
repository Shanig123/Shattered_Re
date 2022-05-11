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

	vector<_float3>							m_vecChessOriPos;		// 초기화 위치
	vector<class CChessPiece_Queen*>		m_vecChessQueen;		// 퀸 오브젝트
	vector<_int>							m_vecChessQueenIndex;	// 퀸의 체스판 인덱스	

	map<class CChessPiece_Queen*, _int>		m_mapChessQueenCheck;	// 결과 체크를 위한
	vector<_int>							m_vecChessQueenResult[(_uint)EGameType::End][(_uint)EQueenResult::End];		// 정답

	class CChessPiece_Knight*				m_pChessKnight;			// 나이트 오브젝트
	vector<_int>							m_vecChessKnightIndex;	// 나이트의 체스판 인덱스
	vector<_bool>							m_vecChessKnightNavi;

	class CChessPiece_Queen*				m_pSelectQueen;

	_bool									m_bFirstStart;					// 첫 시작
	_bool									m_bGameStart;					// 게임 시작
	_bool									m_bOverlapCheck;				// 중복 체크;
	_bool									m_bOverlap;						// 중복 확인
	_bool									m_bNaviView;					// 나이트 네비 표시
	_int									m_iSelectChess;					// 체스판 선택
	_int									m_iOldSelectChess;				// 이전 체스판 선택
	_int									m_iOldSelectChessRenderPass;	// 이전 체스판 랜더패스
	_float3									m_OldSelectChessColor;			// 이전 체스판 컬러
	_int									m_iPiece;						// 말 인덱스들
	_int									m_iSelectQueenIndex;			// 선택한 퀸 인덱스
};

#endif // !__CHESS_H__

