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
		Player,
		Monster,
		UI,
		Object,
		Object3D,
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
	void Play3DSounds(const wstring& _wstSoundKey, ESoundID _eType, _float3 _vSpeakerPos, _float _fMin, _float _fMax, _float _fVolume = 1.f);
	void Update_3DSounds(_float3 _vPlayerPos);
	_bool PlaySoundEnd(ESoundID _eType);
	void SoundVolume(ESoundID _eType, _float _fVolume = 1.f);
	void SoundPitch(ESoundID _eType, _float _fVolume = 0.f);
	void StopSound(ESoundID _eType);
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