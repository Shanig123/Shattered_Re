#pragma once
#ifndef __EVENT_MANAGER__H__
#define __EVENT_MANAGER__H__


#include "Base.h"

const _EVENT g_iNULL = 0;
#pragma region VOLATILE_EVENT_FLAG

#define EVENT_VOLATILE_PLAYER_DEAD				(1<<2)
#define EVENT_VOLATILE_PLAYER_REVIVE			(1<<3)
#define EVENT_VOLATILE_PLAYER_LIFE				(1<<4)

#define EVENT_VOLATILE_DOOR_OPEN_STAGE_1		(1<<5)



#define EVENT_VOLATILE_PLAY_BOSS_CINE			(1<<6)
#define EVENT_VOLATILE_END_BOSS_CINE			(1<<7)
#define EVENT_VOLATILE_SKIP_BOSS_CINE			(1<<8)

#define EVENT_VOLATILE_CHANGE_SCENE				(1<<9)

#define EVENT_VOLATILE_QUEST_START_1			(1<<10)
#define EVENT_VOLATILE_QUEST_END_1				(1<<11)

#define EVENT_VOLATILE_STAGE_MEETBOSS			(1<<10)

#define EVENT_VOLATILE_STAGE_CHESSSTART			(1<<12)
#define EVENT_VOLATILE_STAGE_CHESSQUEEN			(1<<13)
#define EVENT_VOLATILE_STAGE_CHESSKNIGHT		(1<<14)

#define EVENT_VOLATILE_CINEMATICSTART			(1<<15)
#define EVENT_VOLATILE_CINEMATICEND				(1<<16)
#define EVENT_VOLATILE_ENDING					(1<<17)

#pragma endregion
#pragma region STATIC_EVENT_FLAG

#define EVENT_STATIC_AWAKEPLAYER				(1<<2)
#define EVENT_STATIC_GET_WEAPON					(1<<3)
#define EVENT_STATIC_GET_MOON_WEAPON			(1<<4)

#define EVENT_STATIC_DOOR_OPEN_SHRINE			(1<<7)

#define EVENT_STATIC_DEAD_BOSS_PRETO			(1<<8)
#define EVENT_STATIC_DEAD_BOSS_MALKHEL			(1<<9)

#define EVENT_STATIC_GET_QUEST_ITEM1			(1<<10)
#define EVENT_STATIC_GET_QUEST_ITEM2			(1<<11)

#define EVENT_STATIC_BRIDGE_STAGE2				(1<<12)
#define EVENT_STATIC_SHORTCUTDOOR_OPEN_STAGE2	(1<<13)
#define EVENT_STATIC_STAGE2_CHESS_END			(1<<14)

#define EVENT_STATIC_STAGE2_CHURCHDOOR_CLOSE	(1<<15)
#define EVENT_STATIC_STAGE2_BOSS_START			(1<<16)

#pragma endregion

#pragma region QUEST_FLAG
#define QUEST_START_1						(1<<1)
#define QUEST_END_1							(1<<2)

#define QUEST_START_2						(1<<3)
#define QUEST_END_2							(1<<4)

#define QUEST_START_3						(1<<5)
#define QUEST_END_3							(1<<6)

#define QUEST_START_4						(1<<7)
#define QUEST_END_4							(1<<8)

#pragma endregion

BEGIN(Engine)

class ENGINE_DLL CEvent_Manager : public CBase
{
	DECLARE_SINGLETON(CEvent_Manager)
private:
	explicit CEvent_Manager();
	virtual ~CEvent_Manager() = default;

public:
	HRESULT Ready_EventMgr(_DEVICE _pDevice);
	_EVENT	Update_EventMgr(const _float& _fDeltaTime);
public:/*Getter*/
	inline const _ulong& Get_CurEvent() const { return m_iEvent; }
	inline const _ulong& Get_CurStaticEvent() const { return m_iStaticEvent; }
	inline const _ulong& Get_QuestFlag() const { return m_iQuestFlag; }

public:/*Getter*/
	inline	const _uint& Get_KillCount_Total() { return m_iTotal_KillCount; };
	inline	const _uint& Get_KillCount_Legionnaire() { return m_iLegionnaire_KillCount; };
	inline	const _uint& Get_KillCount_Homonculus() { return m_iHomonculus_KillCount; };
	inline	const _uint& Get_KillCount_Boss() { return m_iBoss_KillCount; };

public:/*Setter*/
	inline void Add_Event(const _ulong& _iVolatieEvent) { m_iEvent |= _iVolatieEvent; }
	inline void Min_Event(const _ulong& _iVolatieEvent) { m_iEvent &= (~(_iVolatieEvent)); }
	inline void Reset_Event() { m_iEvent = 0; }
	inline void Set_Event(const _ulong& _iVolatieEvent) { m_iEvent = _iVolatieEvent; }

	inline void Add_StaticEvent(const _ulong& _iStaticEvent) { m_iStaticEvent |= _iStaticEvent; }
	inline void Min_StaticEvent(const _ulong& _iStaticEvent) { m_iStaticEvent &= (~_iStaticEvent); }
	inline void Reset_StaticEvent() { m_iStaticEvent = 0; }
	inline void Set_StaticEvent(const _ulong& _iStaticEvent) { m_iStaticEvent = _iStaticEvent; }

	inline void Add_Quest_Flag(const _ulong& _Quest_Flag) { m_iQuestFlag |= _Quest_Flag; }
	inline void Min_Quest_Flag(const _ulong& _Quest_Flag) { m_iQuestFlag &= (~_Quest_Flag); }
	inline void Reset_Quest_Flag() { m_iQuestFlag = 0; }
	inline void Set_Quest_Flag(const _ulong& _Quest_Flag) { m_iQuestFlag = _Quest_Flag; }

public:/*Setter*/

	inline void Add_KillCount_Legionnaire() { ++m_iLegionnaire_KillCount; ++m_iTotal_KillCount; }
	inline void Add_KillCount_Homonculus() { ++m_iHomonculus_KillCount; ++m_iTotal_KillCount; }
	inline void Add_KillCount_Boss() { ++m_iBoss_KillCount; }

private:
	void Stage1Event(const _float& _fDeltaTime);
	void Stage2Event(const _float& _fDeltaTime);
	void ShrineEvent(const _float& _fDeltaTime);
private:
	_DEVICE m_pDevice;

#pragma region EVENT_VALUE

	_ulong	m_iEvent;
	_ulong	m_iStaticEvent;
	_ulong	m_iQuestFlag;

#pragma endregion

#pragma region KILLCOUNT_VALUE

	_uint	m_iTotal_KillCount;
	_uint	m_iLegionnaire_KillCount;
	_uint	m_iHomonculus_KillCount;
	_uint	m_iBoss_KillCount;


	_uint	m_iStage1CreateCount;
#pragma endregion

#pragma region FLOATING_VALUE

	_float	m_fStage1EventTime;

#pragma endregion

private:
	virtual void Free() override;
};

END
#endif // !__EVENT_MANAGER__H__
