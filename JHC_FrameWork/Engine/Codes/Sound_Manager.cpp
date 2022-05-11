#include "Sound_Manager.h"
#ifndef _AFXDLL
USING(Engine)

IMPLEMENT_SINGLETON(CSound_Manager)

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Ready_Sound_Manager()
{
	FMOD_System_Create(&m_pSystem);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);
	FMOD_System_Set3DSettings(m_pSystem, 1.f, 1.f, 1.f);
	LoadSoundFile();
	return S_OK;
}

void CSound_Manager::PlayBGM(const wstring & _wstSoundKey, _float _fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(_wstSoundKey.c_str(), iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	bBGm = FALSE;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[(_uint)ESoundID::BGM], &bBGm))
		FMOD_System_PlaySound(m_pSystem, iter->second, FALSE, FALSE, &m_pChannelArr[(_uint)ESoundID::BGM]);

	FMOD_Channel_SetVolume(m_pChannelArr[(_uint)ESoundID::BGM], _fVolume);
	FMOD_Channel_SetMode(m_pChannelArr[(_uint)ESoundID::BGM], FMOD_LOOP_NORMAL);
	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::PlaySounds(const wstring & _wstSoundKey, ESoundID _eType, _float _fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(_wstSoundKey.c_str(), iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[(_uint)_eType], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, FALSE, FALSE, &m_pChannelArr[(_uint)_eType]);
		FMOD_Channel_SetVolume(m_pChannelArr[(_uint)_eType], _fVolume);
	}	
	
	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::PlaySounds(const wstring & _wstSoundKey, _uint _iType, _float _fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(_wstSoundKey.c_str(), iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[_iType], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, FALSE, FALSE, &m_pChannelArr[_iType]);
		FMOD_Channel_SetVolume(m_pChannelArr[_iType], _fVolume);
	}

	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::Play3DSounds(const wstring & _wstSoundKey, ESoundID _eType, _float3 _vSpeakerPos, _float _fMin, _float _fMax, _bool _bLoop, _float _fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(_wstSoundKey.c_str(), iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	if(_bLoop)
		FMOD_Sound_SetMode(iter->second, FMOD_LOOP_NORMAL);

	FMOD_Sound_SetMode(iter->second, FMOD_3D);
	FMOD_Sound_SetMode(iter->second, FMOD_3D_LINEARROLLOFF);

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[(_uint)_eType], &bPlay))
	{
		FMOD_RESULT result = FMOD_System_PlaySound(m_pSystem, iter->second, FALSE, TRUE, &m_pChannelArr[(_uint)_eType]);
		if (result != FMOD_OK)
		{

		}
		else
		{
			FMOD_VECTOR vSpeakerPos;
			vSpeakerPos.x = _vSpeakerPos.x;
			vSpeakerPos.y = _vSpeakerPos.y;
			vSpeakerPos.z = _vSpeakerPos.z;

			FMOD_Channel_Set3DAttributes(m_pChannelArr[(_uint)_eType], &vSpeakerPos, 0);
			FMOD_Channel_Set3DMinMaxDistance(m_pChannelArr[(_uint)_eType], _fMin, _fMax);
			FMOD_Channel_SetPaused(m_pChannelArr[(_uint)_eType], false);
			FMOD_Channel_SetVolume(m_pChannelArr[(_uint)_eType], _fVolume);
		}
	}
}

void CSound_Manager::Play3DSounds(const wstring & _wstSoundKey, _uint _iType, _float3 _vSpeakerPos, _float _fMin, _float _fMax, _bool _bLoop, _float _fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(_wstSoundKey.c_str(), iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	if (_bLoop)
		FMOD_Sound_SetMode(iter->second, FMOD_LOOP_NORMAL);

	FMOD_Sound_SetMode(iter->second, FMOD_3D);
	FMOD_Sound_SetMode(iter->second, FMOD_3D_LINEARROLLOFF);

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[_iType], &bPlay))
	{
		FMOD_RESULT result = FMOD_System_PlaySound(m_pSystem, iter->second, FALSE, TRUE, &m_pChannelArr[_iType]);
		if (result != FMOD_OK)
		{

		}
		else
		{
			FMOD_VECTOR vSpeakerPos;
			vSpeakerPos.x = _vSpeakerPos.x;
			vSpeakerPos.y = _vSpeakerPos.y;
			vSpeakerPos.z = _vSpeakerPos.z;

			FMOD_Channel_Set3DAttributes(m_pChannelArr[_iType], &vSpeakerPos, 0);
			FMOD_Channel_Set3DMinMaxDistance(m_pChannelArr[_iType], _fMin, _fMax);
			FMOD_Channel_SetPaused(m_pChannelArr[_iType], false);
			FMOD_Channel_SetVolume(m_pChannelArr[_iType], _fVolume);
		}
	}
}

void CSound_Manager::FadeOutSounds(ESoundID _eType, _float _fFadeTime)
{
	FMOD_BOOL bPlay = FALSE;
	FMOD_Channel_IsPlaying(m_pChannelArr[(_uint)_eType], &bPlay);
	
	if (bPlay)
	{
		unsigned long long dspclock = 0u;
		int rate = 0;
		FMOD_System_GetSoftwareFormat(m_pSystem, &rate, 0, 0);
		FMOD_Channel_GetDSPClock(m_pChannelArr[(_uint)_eType], nullptr, &dspclock);

		_float fVolume = 1.f;
		FMOD_Channel_GetVolume(m_pChannelArr[(_uint)_eType], &fVolume);
		FMOD_Channel_AddFadePoint(m_pChannelArr[(_uint)_eType], dspclock, fVolume);

		FMOD_Channel_AddFadePoint(m_pChannelArr[(_uint)_eType], dspclock + unsigned long long(rate * _fFadeTime), 0.f);
		FMOD_Channel_SetDelay(m_pChannelArr[(_uint)_eType], 0, dspclock + unsigned long long(rate * _fFadeTime), true);
	}
}

void CSound_Manager::Update_3DSounds(ESoundID _eType, _float3 _SpeakerPos)
{
	FMOD_VECTOR vSpeakerPos;
	vSpeakerPos.x = _SpeakerPos.x;
	vSpeakerPos.y = _SpeakerPos.y;
	vSpeakerPos.z = _SpeakerPos.z;

	FMOD_Channel_Set3DAttributes(m_pChannelArr[(_uint)_eType], &vSpeakerPos, 0);
	FMOD_System_Update(m_pSystem);
}

_bool CSound_Manager::PlaySoundEnd(ESoundID _eType)
{
	FMOD_BOOL bPlay = FALSE;
	FMOD_Channel_IsPlaying(m_pChannelArr[(_uint)_eType], &bPlay);

	FMOD_System_Update(m_pSystem);

	if (bPlay)
		return false;
	else
		return true;
}

_bool CSound_Manager::PlaySoundEnd(_uint _iType)
{
	FMOD_BOOL bPlay = FALSE;
	FMOD_Channel_IsPlaying(m_pChannelArr[_iType], &bPlay);

	FMOD_System_Update(m_pSystem);

	if (bPlay)
		return false;
	else
		return true;
}

void CSound_Manager::SoundVolume(ESoundID _eType, _float _fVolume)
{
	FMOD_Channel_SetVolume(m_pChannelArr[(_uint)_eType], _fVolume);
}

void CSound_Manager::SoundPitch(ESoundID _eType, _float _fVolume)
{
	FMOD_Channel_SetPitch(m_pChannelArr[(_uint)_eType], _fVolume);
}

void CSound_Manager::StopMonsterSound()
{
	for (_uint i = 7; i < 25; i++)
	{
		FMOD_Channel_Stop(m_pChannelArr[i]);
	}
}

void CSound_Manager::StopSound(ESoundID _eType)
{
	FMOD_Channel_Stop(m_pChannelArr[(_uint)_eType]);
}

void CSound_Manager::StopSound(_uint _iType)
{
	FMOD_Channel_Stop(m_pChannelArr[_iType]);
}

void CSound_Manager::PauseBGM()
{
	if (bBGm)
		bBGm = false;
	else
		bBGm = true;

	FMOD_Channel_SetPaused(m_pChannelArr[0], bBGm);
}

void CSound_Manager::StopAll(ESoundID _eType)
{
	for (_uint i = 0; i < (_uint)_eType; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSound_Manager::Set_3DListener(_float3& _vPos, _float3& _vLook, _float3& _vUp, const _float& _fDeltaTime)
{
	FMOD_VECTOR vlistenerPos;
	vlistenerPos.x = _vPos.x;
	vlistenerPos.y = _vPos.y;
	vlistenerPos.z = _vPos.z;

	//FMOD_VECTOR vlistenerPrePos;
	//vlistenerPrePos.x = (_vPos.x - m_vPrePlayerPos.x) / _fDeltaTime;
	//vlistenerPrePos.y = (_vPos.y - m_vPrePlayerPos.y) / _fDeltaTime;
	//vlistenerPrePos.z = (_vPos.z - m_vPrePlayerPos.z) / _fDeltaTime;

	//m_vPrePlayerPos = _vPos;

	FMOD_VECTOR test;
	FMOD_Channel_Get3DAttributes(m_pChannelArr[(_uint)ESoundID::Effect], &test, 0);

	FMOD_VECTOR vlistenerLook;
	/*vlistenerLook.x = test.x - vlistenerPos.x;
	vlistenerLook.y = test.y - vlistenerPos.y;
	vlistenerLook.z = test.z - vlistenerPos.z;*/
	vlistenerLook.x = _vLook.x;
	vlistenerLook.y = _vLook.y;
	vlistenerLook.z = _vLook.z;

	FMOD_VECTOR vlistenerUp;
	vlistenerUp.x = _vUp.x;
	vlistenerUp.y = _vUp.y;
	vlistenerUp.z = _vUp.z;

	
	FMOD_System_Set3DListenerAttributes(m_pSystem, 0, &vlistenerPos, 0, &vlistenerLook, &vlistenerUp);
	FMOD_System_Update(m_pSystem);
}

void CSound_Manager::LoadSoundFile()
{
	_finddata_t fd;

	intptr_t handle = _findfirst("../../Resources/Sound/*.*", &fd);

	if (handle == 0)
		return;

	int iResult = 0;

	char szCurPath[128] = "../../Resources/Sound/";
	char szFullPath[128] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);
		strcat_s(szFullPath, fd.name);
		FMOD_SOUND* pSound = nullptr;

		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_HARDWARE, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			_int iLength = _int(strlen(fd.name) + 1);

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		iResult = _findnext(handle, &fd);
	}
	FMOD_System_Update(m_pSystem);
	_findclose(handle);
}

void CSound_Manager::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();

	//Safe_Delete_Array(m_pChannelArr);

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}
#endif // !_AFXDLL