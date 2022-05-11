#pragma once
#ifndef _AFXDLL
#ifndef __SOUND_MANAGER_H__
#define __SOUND_MANAGER_H__

#include "Base.h"

BEGIN(Engine)

class CSound_Manager final : public CBase
{
	DECLARE_SINGLETON(CSound_Manager)
public:
	enum class ESoundID
	{
		BGM,
		Player, Player_Walk, Player_Weapon, Player_Attack_Wall, Player_Hit, Area,
		Monster_GetHit3D_0, Monster_Move3D_0, Monster_Parry3D_0,
		Monster_Effect3D_0, Monster_Weapon3D_0, Monster_Death3D_0,
		Monster_GetHit3D_1, Monster_Move3D_1, Monster_Parry3D_1,
		Monster_Effect3D_1, Monster_Weapon3D_1, Monster_Death3D_1,
		Monster_GetHit3D_2, Monster_Move3D_2, Monster_Parry3D_2,
		Monster_Effect3D_2, Monster_Weapon3D_2, Monster_Death3D_2,
		UI,
		Object, Object_1,
		Object3D_0, Object3D_1, Object3D_2, Object3D_3, Object3D_4, Object3D_5, Object3D_6, Object3D_7, Object3D_8, Object3D_9, Object3D_10, Object3D_11,
		Effect,
		End
	};
private:
	explicit CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Ready_Sound_Manager();

public:
	void PlayBGM(const wstring& _wstSoundKey, _float _fVolume = 1.f);
	void PlaySounds(const wstring& _wstSoundKey, ESoundID _eType, _float _fVolume = 1.f);
	void PlaySounds(const wstring& _wstSoundKey, _uint _iType, _float _fVolume = 1.f);
	void Play3DSounds(const wstring& _wstSoundKey, ESoundID _eType, _float3 _vSpeakerPos, _float _fMin, _float _fMax, _bool _bLoop = false, _float _fVolume = 1.f);
	void Play3DSounds(const wstring& _wstSoundKey, _uint _iType, _float3 _vSpeakerPos, _float _fMin, _float _fMax, _bool _bLoop = false, _float _fVolume = 1.f);
	void FadeOutSounds(ESoundID _eType, _float _fFadeTime);
	void Update_3DSounds(ESoundID _eType, _float3 _SpeakerPos);
	_bool PlaySoundEnd(ESoundID _eType);
	_bool PlaySoundEnd(_uint _iType);
	void SoundVolume(ESoundID _eType, _float _fVolume = 1.f);
	void SoundPitch(ESoundID _eType, _float _fVolume = 0.f);
	void StopMonsterSound();
	void StopSound(ESoundID _eType);
	void StopSound(_uint _iType);
	void PauseBGM();
	void StopAll(ESoundID _eType);

public:
	void Set_3DListener(_float3& _vPos, _float3& _vLook, _float3& _vUp, const _float& _fDeltaTime);

private:
	void LoadSoundFile();

public:
	virtual void Free() override;

private:
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;
	
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[(_uint)ESoundID::End];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem;

	FMOD_BOOL	 bBGm;
};

END
#endif // !_AFXDLL
#endif // !__SOUND_MANAGER_H__