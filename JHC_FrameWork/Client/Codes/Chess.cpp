#include "stdafx.h"
#include "Chess.h"
#include "Map_Stage2Chess.h"
#include "ChessPiece_Queen.h"
#include "ChessPiece_Knight.h"

CChess::CChess(_DEVICE _pDevice)
	:CGameObject(_pDevice)
	, m_bFirstStart(false)
	, m_bGameStart(false)
	, m_bOverlapCheck(false)
	, m_bOverlap(false)
	, m_bNaviView(false)
	, m_iSelectChess(0)
	, m_iOldSelectChess(-1)
	, m_iPiece(1)
	, m_iSelectQueenIndex(-1)
	, m_eGameType(EGameType::Queen)
	, m_pSelectQueen(nullptr)
	, m_pChessKnight(nullptr)
{
}

CChess::CChess(const CChess & _rhs)
	:CGameObject(_rhs)
	, m_bFirstStart(false)
	, m_bGameStart(false)
	, m_bOverlapCheck(false)
	, m_bOverlap(false)
	, m_bNaviView(false)
	, m_iSelectChess(0)
	, m_iOldSelectChess(-1)
	, m_iPiece(1)
	, m_iSelectQueenIndex(-1)
	, m_eGameType(EGameType::Queen)
	, m_pSelectQueen(nullptr)
	, m_pChessKnight(nullptr)
{
}

HRESULT CChess::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CChess::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Ready_GameObject_Clone(_iSceneIdx, _pArg), E_FAIL);

	m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type1].push_back(23);
	m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type1].push_back(9);
	m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type1].push_back(1);
	m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type1].push_back(15);
	m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type1].push_back(12);

	m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type2].push_back(21);
	m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type2].push_back(19);
	m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type2].push_back(3);
	m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type2].push_back(5);
	m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type2].push_back(12);

	return S_OK;
}

_uint CChess::Update_GameObject(const _float & _fDeltaTime)
{
	m_pStage2Chess = dynamic_cast<CMap_Stage2Chess*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Map", EFindGameObject::Proto, L"Map_Stage2Chess"));
	
	if (!m_bFirstStart)
	{
		InitChess();
		ResetChess();
		m_bFirstStart = true;
	}

	if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_CHESSSTART && !m_bGameStart)
	{
		m_pStage2Chess->Set_RanderPass(0, 14, _float3(0.f, 1.f, 0.f));
		m_bGameStart = true;
	}		

	if (Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_CHESSSTART)
	{
		if (EGameType::Knight == m_eGameType)
		{
			if (!m_bNaviView)
			{
				KnightNavi();
				m_bNaviView = true;
			}				
		}		

		Key_Input();
		ResultCheck();

		if (m_bOverlapCheck)
		{
			OverlapCheck();
			m_bOverlapCheck = false;
		}
	}

	return _uint();
}

HRESULT CChess::Render_GameObject()
{
	return S_OK;
}

void CChess::Key_Input()
{
	if (Get_Manager()->Key_Down(DIK_W))
	{
		if (25 > (m_iSelectChess + 5))
			m_iSelectChess += 5;		

		SelectChess(*m_pStage2Chess, m_iSelectChess, 14, _float3(0.f, 1.f, 0.f));
	}

	if (Get_Manager()->Key_Down(DIK_A))
	{
		if ((m_iSelectChess % 5) != 0)
			m_iSelectChess -= 1;

		SelectChess(*m_pStage2Chess, m_iSelectChess, 14, _float3(0.f, 1.f, 0.f));
	}

	if (Get_Manager()->Key_Down(DIK_S))
	{
		if (0 <= (m_iSelectChess - 5))
			m_iSelectChess -= 5;

		SelectChess(*m_pStage2Chess, m_iSelectChess, 14, _float3(0.f, 1.f, 0.f));
	}

	if (Get_Manager()->Key_Down(DIK_D))
	{
		if ((m_iSelectChess % 5) != 4)
			m_iSelectChess += 1;

		SelectChess(*m_pStage2Chess, m_iSelectChess, 14, _float3(0.f, 1.f, 0.f));
	}

	if (Get_Manager()->Key_Down(DIK_F))
	{
		if (EGameType::Queen == m_eGameType)
		{
			_bool bOverlap = false;
			_int iIndex = 0;
			for (auto& iter : m_vecChessQueenIndex)
			{
				if (m_iSelectChess == iter)
				{
					bOverlap = true;
					break;
				}

				++iIndex;
			}

			if (bOverlap)
			{
				//중복
				if (m_iSelectChess == m_vecChessQueenIndex[0] || nullptr != m_pSelectQueen)
					return;

				m_pSelectQueen = m_vecChessQueen[iIndex];
				m_iSelectQueenIndex = iIndex;
				SelectQueen(*m_pSelectQueen, 15);
			}
			else
			{
				if (4 < m_iPiece && nullptr == m_pSelectQueen)
					return;

				if (nullptr == m_pSelectQueen)
				{
					m_vecChessQueen[m_iPiece]->Set_ChessPos(m_pStage2Chess->Get_StageMesh(m_iSelectChess)->Get_ObjectCenterPos());
					m_mapChessQueenCheck[m_vecChessQueen[m_iPiece]] = m_iSelectChess;
					m_vecChessQueenIndex[m_iPiece] = m_iSelectChess;
					++m_iPiece;
				}
				else
				{
					m_pSelectQueen->Set_ChessPos(m_pStage2Chess->Get_StageMesh(m_iSelectChess)->Get_ObjectCenterPos());
					m_mapChessQueenCheck[m_pSelectQueen] = m_iSelectChess;
					m_vecChessQueenIndex[m_iSelectQueenIndex] = m_iSelectChess;
					SelectQueen(*m_pSelectQueen, 2);
					m_pSelectQueen = nullptr;
					m_iSelectQueenIndex = -1;
				}

				m_bOverlapCheck = true;
			}
		}
		else
		{
			if (m_vecChessKnightNavi[m_iSelectChess])
			{
				auto iter_find = find(m_vecChessKnightIndex.begin(), m_vecChessKnightIndex.end(), m_iSelectChess);
				if (iter_find == m_vecChessKnightIndex.end())
				{
					m_pChessKnight->Set_ChessPos(m_pStage2Chess->Get_StageMesh(m_iSelectChess)->Get_ObjectCenterPos());
					m_vecChessKnightIndex.push_back(m_iSelectChess);
					m_iOldSelectChessRenderPass = 2;
					m_OldSelectChessColor = _float3(0.f, 0.f, 0.f);
					m_bNaviView = false;
				}				
			}

			/*m_vecChessQueen[0]->Set_ChessPos(m_pStage2Chess->Get_StageMesh(m_iSelectChess)->Get_ObjectCenterPos());
			m_mapChessQueenCheck[m_vecChessQueen[0]] = m_iSelectChess;
			m_vecChessQueenIndex[0] = m_iSelectChess;*/
		}
			
	}

	if (Get_Manager()->Key_Down(DIK_R))
	{
		// 리셋
		ResetChess();
	}

	if (Get_Manager()->Key_Down(DIK_ESCAPE))
	{
		// 복귀
		Get_EventMgr()->Min_Event(EVENT_VOLATILE_STAGE_CHESSSTART);	
		EndChess();
	}
}

void CChess::InitChess()
{
	list<CGameObject*> Monsterlist = *Get_Manager()->Get_LayerContainer(m_iSceneIdx, L"Layer_ChessQueen");

	_uint i = 0;
	for (auto& iter : Monsterlist)
	{
		m_vecChessQueen.push_back(dynamic_cast<CChessPiece_Queen*>(iter));
		m_mapChessQueenCheck.emplace(dynamic_cast<CChessPiece_Queen*>(iter), -1);
		m_vecChessQueenIndex.push_back(-1);
		m_vecChessOriPos.push_back(iter->Get_TransformDesc().vPos);
		++i;
	}

	Monsterlist = *Get_Manager()->Get_LayerContainer(m_iSceneIdx, L"Layer_ChessKnight");

	for (auto& iter : Monsterlist)
	{
		m_pChessKnight = dynamic_cast<CChessPiece_Knight*>(iter);
	}	
}

void CChess::ResetChess()
{
	CTransform* pPicesTransform = nullptr;

	m_iSelectChess = 0;	
	m_iOldSelectChess = -1;
	m_iOldSelectChessRenderPass = 2;
	m_OldSelectChessColor = _float3(0.f, 0.f, 0.f);
	m_bGameStart = false;
	m_bNaviView = false;

	for (_int i = 0; i < 25; ++i)
	{
		m_pStage2Chess->Set_RanderPass(i, 2);
	}

	if (EGameType::Queen == m_eGameType)
	{
		m_vecChessQueen[0]->Set_ChessPos(m_pStage2Chess->Get_StageMesh(12)->Get_ObjectCenterPos());
		m_vecChessQueen[1]->Set_ChessPos(m_vecChessOriPos[1], 90.f);
		m_vecChessQueen[2]->Set_ChessPos(m_vecChessOriPos[2], 90.f);
		m_vecChessQueen[3]->Set_ChessPos(m_vecChessOriPos[3], 90.f);
		m_vecChessQueen[4]->Set_ChessPos(m_vecChessOriPos[4], 90.f);

		m_iPiece = 1;
		m_pSelectQueen = nullptr;

		for (_uint i = 0; i < 5; ++i)
			SelectQueen(*m_vecChessQueen[i], 15);
		
		for (auto& iter : m_mapChessQueenCheck)
			iter.second = -1;

		m_mapChessQueenCheck[m_vecChessQueen[0]] = 12;
		m_vecChessQueenIndex[0] = 12;
		m_vecChessQueenIndex[1] = -1;
		m_vecChessQueenIndex[2] = -1;
		m_vecChessQueenIndex[3] = -1;
		m_vecChessQueenIndex[4] = -1;
	}
	else
	{
		if(!(Get_EventMgr()->Get_CurEvent() & EVENT_VOLATILE_STAGE_CHESSKNIGHT))
			m_pChessKnight->Set_ChessPos(m_pStage2Chess->Get_StageMesh(20)->Get_ObjectCenterPos());

		m_vecChessKnightIndex.clear();
		m_vecChessKnightIndex.push_back(20);

		for (_uint i = 0; i < 25; ++i)
		{
			m_vecChessKnightNavi.push_back(false);
		}
	}
}

void CChess::EndChess()
{
	ResetChess();
}

void CChess::ResultCheck()
{
	if (EGameType::Queen == m_eGameType)
	{
		if (5 != m_iPiece)
			return;

		if (QueenResultCheck())
		{
			// 성공
			Get_EventMgr()->Add_Event(EVENT_VOLATILE_STAGE_CHESSQUEEN);
			m_eGameType = EGameType::Knight;
			ResetChess();
		}
		else
		{
			// 실패
			Print_Debug_Str(L"실패");
		}
	}
	else
	{
		if (24 < m_vecChessKnightIndex.size())
		{
			// 성공
			Get_EventMgr()->Add_Event(EVENT_VOLATILE_STAGE_CHESSKNIGHT);
			Get_EventMgr()->Min_Event(EVENT_VOLATILE_STAGE_CHESSSTART);
			Get_EventMgr()->Add_StaticEvent(EVENT_STATIC_STAGE2_CHESS_END);
			EndChess();
		}
	}
}

_bool CChess::QueenResultCheck()
{
	_uint iSuccess = 0;

	for (auto& iter : m_mapChessQueenCheck)
	{
		if (iter.second == m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type1][0] ||
			iter.second == m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type1][1] ||
			iter.second == m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type1][2] ||
			iter.second == m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type1][3] ||
			iter.second == m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type1][4])
			++iSuccess;
	}

	if (5 <= iSuccess)
		return true;
	else
	{
		iSuccess = 0;
		for (auto& iter : m_mapChessQueenCheck)
		{
			if (iter.second == m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type2][0] ||
				iter.second == m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type2][1] ||
				iter.second == m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type2][2] ||
				iter.second == m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type2][3] ||
				iter.second == m_vecChessQueenResult[(_uint)EGameType::Queen][(_uint)EQueenResult::Type2][4])
				++iSuccess;
		}

		if (5 <= iSuccess)
			return true;
		else
			return false;
	}
}

void CChess::SelectChess(class CMap_Stage2Chess& _Chess, _int _iSelect, _uint _iPass, _float3 _Color)
{
	if (m_iOldSelectChess != _iSelect)
	{
		// 이전 체스판 변환
		if (-1 == m_iOldSelectChess)
			m_iOldSelectChess = 0;

		_Chess.Set_RanderPass(m_iOldSelectChess, m_iOldSelectChessRenderPass, m_OldSelectChessColor);

		m_iOldSelectChessRenderPass = _Chess.Get_RenderPass(_iSelect);
		m_OldSelectChessColor = _Chess.Get_RimColor(_iSelect);
		m_iOldSelectChess = _iSelect;

		// 선택 체스판 변환
		_float3 Color;
		if (_float3(0.f, 0.f, 0.f) != _Color)
			Color = _Color;

		_Chess.Set_RanderPass(_iSelect, _iPass, Color);
	}	
}

void CChess::SelectQueen(class CChessPiece_Queen& _ChessPiece, _uint _iPass, _float3 _Color)
{
	_ChessPiece.Set_ChangeRimLight(_iPass, _Color);
}

void CChess::OverlapCheck()
{
	for (_int i = 0; i < m_iPiece; ++i)
	{
		m_bOverlap = false;

		OverlapCheckUp(i);
		OverlapCheckDown(i);
		OverlapCheckLeft(i);
		OverlapCheckRight(i);
		OverlapCheckUpLeft(i);
		OverlapCheckUpRight(i);
		OverlapCheckDownLeft(i);
		OverlapCheckDownRight(i);

		if (!m_bOverlap)
		{
			SelectQueen(*m_vecChessQueen[i], 2);
		}
	}
}

void CChess::OverlapCheckUp(_uint _iIndex)
{
	_int iCheck = m_vecChessQueenIndex[_iIndex];
	while (true)
	{
		if (25 > (iCheck + 5))
		{
			iCheck += 5;
			_uint iVectorIndex = 0;
			for (auto& iter : m_vecChessQueenIndex)
			{
				if (iter == iCheck)
					break;

				++iVectorIndex;
			}

			if (5 > iVectorIndex)
			{
				SelectQueen(*m_vecChessQueen[iVectorIndex], 14, _float3(1.f, 0.f, 0.f));
				SelectQueen(*m_vecChessQueen[_iIndex], 14, _float3(1.f, 0.f, 0.f));

				m_bOverlap = true;
			}
		}
		else
			break;
	}	
}

void CChess::OverlapCheckDown(_uint _iIndex)
{
	_int iCheck = m_vecChessQueenIndex[_iIndex];
	while (true)
	{
		if (0 <= (iCheck - 5))
		{
			iCheck -= 5;
			_uint iVectorIndex = 0;
			for (auto& iter : m_vecChessQueenIndex)
			{
				if (iter == iCheck)
					break;

				++iVectorIndex;
			}

			if (5 > iVectorIndex)
			{
				SelectQueen(*m_vecChessQueen[iVectorIndex], 14, _float3(1.f, 0.f, 0.f));
				SelectQueen(*m_vecChessQueen[_iIndex], 14, _float3(1.f, 0.f, 0.f));

				m_bOverlap = true;
			}
		}
		else
			break;
	}
}

void CChess::OverlapCheckLeft(_uint _iIndex)
{
	_int iCheck = m_vecChessQueenIndex[_iIndex];
	while (true)
	{
		if (((iCheck - 1) % 5) != 4 && 0 <= iCheck - 1)
		{
			iCheck -= 1;
			_uint iVectorIndex = 0;
			for (auto& iter : m_vecChessQueenIndex)
			{
				if (iter == iCheck)
					break;

				++iVectorIndex;
			}

			if (5 > iVectorIndex)
			{
				SelectQueen(*m_vecChessQueen[iVectorIndex], 14, _float3(1.f, 0.f, 0.f));
				SelectQueen(*m_vecChessQueen[_iIndex], 14, _float3(1.f, 0.f, 0.f));

				m_bOverlap = true;
			}
		}
		else
			break;
	}
}

void CChess::OverlapCheckRight(_uint _iIndex)
{
	_int iCheck = m_vecChessQueenIndex[_iIndex];
	while (true)
	{
		if (((iCheck + 1) % 5) != 0 && 25 > iCheck + 1)
		{
			iCheck += 1;
			_uint iVectorIndex = 0;
			for (auto& iter : m_vecChessQueenIndex)
			{
				if (iter == iCheck)
					break;

				++iVectorIndex;
			}

			if (5 > iVectorIndex)
			{
				SelectQueen(*m_vecChessQueen[iVectorIndex], 14, _float3(1.f, 0.f, 0.f));
				SelectQueen(*m_vecChessQueen[_iIndex], 14, _float3(1.f, 0.f, 0.f));

				m_bOverlap = true;
			}
		}
		else
			break;
	}
}

void CChess::OverlapCheckUpLeft(_uint _iIndex)
{
	_int iCheck = m_vecChessQueenIndex[_iIndex];
	while (true)
	{
		if (((iCheck + 4) % 5) != 4 && 25 > (iCheck + 4))
		{
			iCheck += 4;
			_uint iVectorIndex = 0;
			for (auto& iter : m_vecChessQueenIndex)
			{
				if (iter == iCheck)
					break;

				++iVectorIndex;
			}

			if (5 > iVectorIndex)
			{
				SelectQueen(*m_vecChessQueen[iVectorIndex], 14, _float3(1.f, 0.f, 0.f));
				SelectQueen(*m_vecChessQueen[_iIndex], 14, _float3(1.f, 0.f, 0.f));

				m_bOverlap = true;
			}
		}
		else
			break;
	}
}

void CChess::OverlapCheckUpRight(_uint _iIndex)
{
	_int iCheck = m_vecChessQueenIndex[_iIndex];
	while (true)
	{
		if (((iCheck + 6) % 5) != 0 && 25 > (iCheck + 6))
		{
			iCheck += 6;
			_uint iVectorIndex = 0;
			for (auto& iter : m_vecChessQueenIndex)
			{
				if (iter == iCheck)
					break;

				++iVectorIndex;
			}

			if (5 > iVectorIndex)
			{
				SelectQueen(*m_vecChessQueen[iVectorIndex], 14, _float3(1.f, 0.f, 0.f));
				SelectQueen(*m_vecChessQueen[_iIndex], 14, _float3(1.f, 0.f, 0.f));

				m_bOverlap = true;
			}
		}
		else
			break;
	}
}

void CChess::OverlapCheckDownLeft(_uint _iIndex)
{
	_int iCheck = m_vecChessQueenIndex[_iIndex];
	while (true)
	{
		if (((iCheck - 6) % 5) != 4 && 0 <= (iCheck - 6))
		{
			iCheck -= 6;
			_uint iVectorIndex = 0;
			for (auto& iter : m_vecChessQueenIndex)
			{
				if (iter == iCheck)
					break;

				++iVectorIndex;
			}

			if (5 > iVectorIndex)
			{
				SelectQueen(*m_vecChessQueen[iVectorIndex], 14, _float3(1.f, 0.f, 0.f));
				SelectQueen(*m_vecChessQueen[_iIndex], 14, _float3(1.f, 0.f, 0.f));

				m_bOverlap = true;
			}
		}
		else
			break;
	}
}

void CChess::OverlapCheckDownRight(_uint _iIndex)
{
	_int iCheck = m_vecChessQueenIndex[_iIndex];
	while (true)
	{
		if (((iCheck - 4) % 5) != 0 && 0 <= (iCheck - 4))
		{
			iCheck -= 4;
			_uint iVectorIndex = 0;
			for (auto& iter : m_vecChessQueenIndex)
			{
				if (iter == iCheck)
					break;

				++iVectorIndex;
			}

			if (5 > iVectorIndex)
			{
				SelectQueen(*m_vecChessQueen[iVectorIndex], 14, _float3(1.f, 0.f, 0.f));
				SelectQueen(*m_vecChessQueen[_iIndex], 14, _float3(1.f, 0.f, 0.f));

				m_bOverlap = true;
			}
		}
		else
			break;
	}
}

void CChess::KnightNavi()
{
	_int iNowIndex = m_vecChessKnightIndex.back();

	for (_int i = 0; i < 25; ++i)
		m_vecChessKnightNavi[i] = false;

	if (25 > iNowIndex + 9 && (((iNowIndex + 9) % 5) != 4)) // 위 왼
	{
		m_vecChessKnightNavi[iNowIndex + 9] = true;
	}

	if (25 > iNowIndex + 11 && (((iNowIndex + 11) % 5) != 0)) // 위 오른
	{
		m_vecChessKnightNavi[iNowIndex + 11] = true;
	}

	if (0 <= iNowIndex - 11 && (((iNowIndex - 11) % 5) != 4)) // 아래 왼
	{
		m_vecChessKnightNavi[iNowIndex - 11] = true;
	}

	if (0 <= iNowIndex - 9 && (((iNowIndex - 9) % 5) != 0)) // 아래 오른
	{
		m_vecChessKnightNavi[iNowIndex - 9] = true;
	}

	if ((0 <= iNowIndex + 7) && (25 > iNowIndex + 7) && (((iNowIndex + 7) % 5) != 0) && (((iNowIndex + 7) % 5) != 1)) // 오른 위
	{
		m_vecChessKnightNavi[iNowIndex + 7] = true;
	}

	if ((0 <= iNowIndex - 3) && (25 > iNowIndex - 3) && (((iNowIndex - 3) % 5) != 0) && (((iNowIndex - 3) % 5) != 1)) // 오른 아래
	{
		m_vecChessKnightNavi[iNowIndex - 3] = true;
	}

	if ((0 <= iNowIndex + 3) && (25 > iNowIndex + 3) && (((iNowIndex + 3) % 5) != 4) && (((iNowIndex + 3) % 5) != 3)) // 왼 위
	{
		m_vecChessKnightNavi[iNowIndex + 3] = true;
	}

	if ((0 <= iNowIndex - 7) && (25 > iNowIndex - 7) && (((iNowIndex - 7) % 5) != 4) && (((iNowIndex - 7) % 5) != 3)) // 왼 아래
	{
		m_vecChessKnightNavi[iNowIndex - 7] = true;
	}

	for (_int i = 0; i < 25; ++i)
	{
		if(m_vecChessKnightNavi[i])
			ViewNavi(*m_pStage2Chess, i, 14, _float3(0.f, 0.f, 1.f));
		else
			ViewNavi(*m_pStage2Chess, i, 1);
	}

	for (_uint i = 0; i < m_vecChessKnightIndex.size(); ++i)
	{
		ViewNavi(*m_pStage2Chess, m_vecChessKnightIndex[i], 14, _float3(1.f, 0.f, 0.f)); // 색상 추가
	}
}

void CChess::ViewNavi(CMap_Stage2Chess & _Chess, _int _iSelect, _uint _iPass, _float3 _Color)
{
	if(_iSelect != m_iSelectChess)
		_Chess.Set_RanderPass(_iSelect, _iPass, _Color);
}

CChess * CChess::Create(_DEVICE _pDevice)
{
	CChess* pInstance = nullptr;
	pInstance = new CChess(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX(L"Failed to Create CChess.");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject * CChess::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CChess* pInstance = nullptr;
	pInstance = new CChess(*this);
	if (FAILED(pInstance->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		MSG_BOX(L"Failed to Clone CChess.");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CChess::Free()
{
	CGameObject::Free();
}
