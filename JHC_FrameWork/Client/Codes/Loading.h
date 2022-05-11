#pragma once

#ifndef __LOADING_H__
#define __LOADING_H__

#include "Base.h"
USING(Engine)

class CLoading final : public CBase
{
private:
	explicit CLoading(_DEVICE _pDevice, ESceneType _eSceneType);
	virtual ~CLoading() = default;
	enum eLoadingCategory
	{
		PlayerMesh,
		MonsterMesh,
		MapMesh,
		NoMeshObj,
		Sound,
		Texture,
		Other,
		End
	};
public:
	HRESULT Ready_Loading(_bool _bMultiThread = true);

public:
	static unsigned _stdcall LoadingByThread(void* _pParam);

private:
	HRESULT Load_PublicResources(ESceneType _eLoadingScene);
	HRESULT Load_Stage1Resources();
	HRESULT Load_ShrineResources();
	HRESULT Load_Stage2Resources();
	//void Load_Stage2AreaResources();

private:
	static unsigned _stdcall  Load_PlayerResource(void* _pParam);
	static unsigned _stdcall  Load_MonsterResource(void* _pParam);
	static unsigned _stdcall  Load_MapStage1Resource(void* _pParam);
	static unsigned _stdcall  Load_NoMeshObjResource(void* _pParam);
	static unsigned _stdcall  Load_TextureResource(void* _pParam);

public:
	_uint Get_Finish() { return m_iFinish; }
	wstring& Get_Text() { return m_wstrLoaing; }

private:
	HRESULT Add_Player_Layer(const wstring& _szLayer, ESceneType _eSceneType);
	HRESULT Add_Player_Weapon_Layer(const wstring& _szLayer);
	HRESULT Add_NPC_Layer(const wstring& _szLayer);
	HRESULT Add_MapStage1_Layer(const wstring& _szLayer);
	HRESULT Add_MapShrine_Layer(const wstring& _szLayer);
	HRESULT Add_MapStage2_Layer(const wstring& _szLayer);
	HRESULT Add_NPCStage2_Layer(const wstring& _szLayer);
	HRESULT Add_MonsterStage1_Layer(const wstring& _szLayer);
	HRESULT Add_MonsterStage2_Layer(const wstring& _szLayer);
	HRESULT Add_MonsterStage2ChessQueen_Layer(const wstring& _szLayer);
	HRESULT Add_MonsterStage2ChessKnight_Layer(const wstring& _szLayer);
	HRESULT Add_Cinematic_Camera_Layer(const wstring& _szLayer);
	HRESULT Add_Camera_Layer(const wstring& _szLayer);
	HRESULT Add_Light();
	HRESULT Add_Light_Stage2();
	HRESULT Add_EffectGenerator_Layer(const wstring & _szLayer);
	HRESULT Add_SkyBox(const wstring & _szLayer);

public:
	static CLoading* Create(_DEVICE _pDevice, ESceneType _eSceneType, _bool _bMultiThread = true);
	virtual void Free() override;

private:
	_DEVICE				m_pDevice;
	HANDLE				m_hLoadingThread;
	HANDLE				m_hResourceThread[(_uint)eLoadingCategory::End];
	CRITICAL_SECTION	m_tCriticalSection;
	ESceneType			m_eLoadingScene;

	//_uint				m_iCheckFinish;
	_uint				m_iFinish;
	wstring				m_wstrLoaing;
};

#endif // !__LOADING_H__