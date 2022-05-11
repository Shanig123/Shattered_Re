#include "AniControl.h"

USING(Engine)

CAniControl::CAniControl(_ANICONTROLLER _pAniControl)
	: m_pAniControl(_pAniControl)
	, m_iCurrentTrack(0)
	, m_iNewTrack(1)
	, m_fAccTime(0.f)
	, m_iOldAniIndex(999)
	, m_dPeriod(0)
{
	m_pAniControl->AddRef();
}

CAniControl::CAniControl(const CAniControl & _rOther)
	: m_iCurrentTrack(_rOther.m_iCurrentTrack)
	, m_iNewTrack(_rOther.m_iNewTrack)
	, m_fAccTime(_rOther.m_fAccTime)
	, m_iOldAniIndex(_rOther.m_iOldAniIndex)
	, m_dPeriod(_rOther.m_dPeriod)
{
	_rOther.m_pAniControl->CloneAnimationController
	(
		_rOther.m_pAniControl->GetMaxNumAnimationOutputs(),			// 복제시 원본 객체에서 제공되는 애니메이션 셋의 개수
		_rOther.m_pAniControl->GetMaxNumAnimationSets(),			// 구동 가능한 애니메이션 셋의 최대개수(대게는 1인자와 값이 같음)
		_rOther.m_pAniControl->GetMaxNumTracks(),					// 구동 가능한 트랙의 최대 개수
		_rOther.m_pAniControl->GetMaxNumEvents(),					// 메쉬 안에 적용되있는 독특한 효과, 우리는 사용 못함
		&m_pAniControl
	);
}

HRESULT CAniControl::Ready_AniControl()
{
	return S_OK;
}

void CAniControl::Set_Animationset(const _uint & _iIndex)
{
	if (m_iOldAniIndex == _iIndex)
		return;

	m_iNewTrack = (m_iCurrentTrack == 0 ? 1 : 0);

	LPD3DXANIMATIONSET pAnimationset = nullptr;
	m_pAniControl->GetAnimationSet(_iIndex, &pAnimationset);

	m_dPeriod = pAnimationset->GetPeriod();	// 현재 애니메이션 셋이 지닌 전체 재생 시간 반환

	// 0번째 트랙에 재생하고자하는 애니메이션 셋을 설정함
	m_pAniControl->SetTrackAnimationSet(m_iNewTrack, pAnimationset);

	// 사용하지 않고 있는 이벤트 정보들 때문에 선형 보간이 안되는 경우가 발생해서 호출
	m_pAniControl->UnkeyAllTrackEvents(m_iCurrentTrack);
	m_pAniControl->UnkeyAllTrackEvents(m_iNewTrack);

	// 지정한 트랙의 사용 유무를 결정하는 함수
	// 3인자 : 언제부터 트랙을 사용 또는 해제할 것인지 결정
	m_pAniControl->KeyTrackEnable(m_iCurrentTrack, FALSE, m_fAccTime + 0.1);
	// 트랙이 해제되는 시간동안 현재 애니메이션 셋은 어떤 속도로 움직일지 결정(속도의 상수 값은 각자 1)
	m_pAniControl->KeyTrackSpeed(m_iCurrentTrack, 1.f, m_fAccTime, 0.1, D3DXTRANSITION_LINEAR);
	// 트랙이 해제되는 시간동안 현재 애니메이션 셋은 어떤 가중치를 갖게 할지(속도의 상수 값은 각자 1)
	m_pAniControl->KeyTrackWeight(m_iCurrentTrack, 0.1f, m_fAccTime, 0.1, D3DXTRANSITION_LINEAR);

	// 트랙 활성화의 유무를 판단하는 함수
	m_pAniControl->SetTrackEnable(m_iNewTrack, TRUE);
	// 트랙이 시작하는 시간동안 현재 애니메이션 셋은 어떤 속도로 움직일지 결정(속도의 상수 값은 각자 1)
	m_pAniControl->KeyTrackSpeed(m_iNewTrack, 1.f, m_fAccTime, 0.1, D3DXTRANSITION_LINEAR);
	// 트랙이 시작하는 시간동안 현재 애니메이션 셋은 어떤 가중치를 갖게 할지(속도의 상수 값은 각자 1)
	m_pAniControl->KeyTrackWeight(m_iNewTrack, 0.9f, m_fAccTime, 0.1, D3DXTRANSITION_LINEAR);

	// 애니메이션이 재생되던 시간을 다시 세팅(advanced 함수 호출시 내부적으로 누적되던 시간 리셋)
	m_pAniControl->ResetTime();
	m_fAccTime = 0.f;

	m_pAniControl->SetTrackPosition(m_iNewTrack, 0.0);
	m_iOldAniIndex = _iIndex;

	m_iCurrentTrack = m_iNewTrack;
}

void CAniControl::Change_Animationset(const _uint & _iIndex)
{
	if (m_iOldAniIndex == _iIndex)
		return;

	m_iNewTrack = (m_iCurrentTrack == 0 ? 1 : 0);

	LPD3DXANIMATIONSET pAnimationset = nullptr;
	m_pAniControl->GetAnimationSet(_iIndex, &pAnimationset);

	m_dPeriod = pAnimationset->GetPeriod();	// 현재 애니메이션 셋이 지닌 전체 재생 시간 반환

											// 0번째 트랙에 재생하고자하는 애니메이션 셋을 설정함
	m_pAniControl->SetTrackAnimationSet(m_iNewTrack, pAnimationset);

	// 사용하지 않고 있는 이벤트 정보들 때문에 선형 보간이 안되는 경우가 발생해서 호출
	m_pAniControl->UnkeyAllTrackEvents(m_iCurrentTrack);
	m_pAniControl->UnkeyAllTrackEvents(m_iNewTrack);

	// 지정한 트랙의 사용 유무를 결정하는 함수
	// 3인자 : 언제부터 트랙을 사용 또는 해제할 것인지 결정
	m_pAniControl->KeyTrackEnable(m_iCurrentTrack, FALSE, m_fAccTime + 0.1);
	// 트랙이 해제되는 시간동안 현재 애니메이션 셋은 어떤 속도로 움직일지 결정(속도의 상수 값은 각자 1)
	m_pAniControl->KeyTrackSpeed(m_iCurrentTrack, 1.f, m_fAccTime, 0.1, D3DXTRANSITION_LINEAR);
	// 트랙이 해제되는 시간동안 현재 애니메이션 셋은 어떤 가중치를 갖게 할지(속도의 상수 값은 각자 1)
	m_pAniControl->KeyTrackWeight(m_iCurrentTrack, 0.1f, m_fAccTime, 0.1, D3DXTRANSITION_LINEAR);

	// 트랙 활성화의 유무를 판단하는 함수
	m_pAniControl->SetTrackEnable(m_iNewTrack, TRUE);
	// 트랙이 시작하는 시간동안 현재 애니메이션 셋은 어떤 속도로 움직일지 결정(속도의 상수 값은 각자 1)
	m_pAniControl->KeyTrackSpeed(m_iNewTrack, 1.f, m_fAccTime, 0.1, D3DXTRANSITION_LINEAR);
	// 트랙이 시작하는 시간동안 현재 애니메이션 셋은 어떤 가중치를 갖게 할지(속도의 상수 값은 각자 1)
	m_pAniControl->KeyTrackWeight(m_iNewTrack, 0.9f, m_fAccTime, 0.1, D3DXTRANSITION_LINEAR);

	m_pAniControl->SetTrackPosition(m_iNewTrack, m_fAccTime);
	m_iOldAniIndex = _iIndex;
	m_iCurrentTrack = m_iNewTrack;
}

void CAniControl::Play_Animationset(const _float & _fDeltaTime)
{
	// 실제 애니메이션을 재생시키는 함수
	// _fDeltaTime와는 별개로 AdvanceTime함수 호출시 내부적으로 증가하는 시간 값이 따로 있음
	// 2인자 : 애니메이션 동작에 따른 이펙트에 대한 처리를 담당하는 객체의 주소(대게는 사용하지 않고 직접 만들어서 사용)
	if (Is_AnimationSetEnd())
	{
		m_pAniControl->ResetTime();
		m_fAccTime = 0.f;
	}

	m_pAniControl->AdvanceTime(_fDeltaTime, NULL);


	m_fAccTime += _fDeltaTime;

}

_bool CAniControl::Is_AnimationSetEnd()
{
	D3DXTRACK_DESC		TrackInfo;
	ZeroMemory(&TrackInfo, sizeof(D3DXTRACK_DESC));

	m_pAniControl->GetTrackDesc(m_iCurrentTrack, &TrackInfo);
	if (TrackInfo.Position >= m_dPeriod - 0.1)
		return true;

	return false;
}

_bool CAniControl::Get_AnimationFrame(_double _dMaxFrame, _double _dWantFrame)
{

	D3DXTRACK_DESC		TrackInfo;
	ZeroMemory(&TrackInfo, sizeof(D3DXTRACK_DESC));

	m_pAniControl->GetTrackDesc(m_iCurrentTrack, &TrackInfo);


	_double dWantFrame = (m_dPeriod / _dMaxFrame) * _dWantFrame;
	

	if (TrackInfo.Position >= dWantFrame)
		return true;

	return false;
	// MaxFrame(애니메이션의 최대 값)
	// WantFrame(원하는 애니메이션 값)

	// 원하는 프레임보다 작으면 false 같거나 크면 true;
}

CAniControl * CAniControl::Create(_ANICONTROLLER _pAniControl)
{
	CAniControl* pInstance = new CAniControl(_pAniControl);

	if (FAILED(pInstance->Ready_AniControl()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAniControl * CAniControl::Create(const CAniControl & _rOther)
{
	CAniControl* pInstance = new CAniControl(_rOther);

	if (FAILED(pInstance->Ready_AniControl()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAniControl::Free()
{
	Safe_Release(m_pAniControl);
}
