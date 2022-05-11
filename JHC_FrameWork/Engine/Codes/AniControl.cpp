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
		_rOther.m_pAniControl->GetMaxNumAnimationOutputs(),			// ������ ���� ��ü���� �����Ǵ� �ִϸ��̼� ���� ����
		_rOther.m_pAniControl->GetMaxNumAnimationSets(),			// ���� ������ �ִϸ��̼� ���� �ִ밳��(��Դ� 1���ڿ� ���� ����)
		_rOther.m_pAniControl->GetMaxNumTracks(),					// ���� ������ Ʈ���� �ִ� ����
		_rOther.m_pAniControl->GetMaxNumEvents(),					// �޽� �ȿ� ������ִ� ��Ư�� ȿ��, �츮�� ��� ����
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

	m_dPeriod = pAnimationset->GetPeriod();	// ���� �ִϸ��̼� ���� ���� ��ü ��� �ð� ��ȯ

	// 0��° Ʈ���� ����ϰ����ϴ� �ִϸ��̼� ���� ������
	m_pAniControl->SetTrackAnimationSet(m_iNewTrack, pAnimationset);

	// ������� �ʰ� �ִ� �̺�Ʈ ������ ������ ���� ������ �ȵǴ� ��찡 �߻��ؼ� ȣ��
	m_pAniControl->UnkeyAllTrackEvents(m_iCurrentTrack);
	m_pAniControl->UnkeyAllTrackEvents(m_iNewTrack);

	// ������ Ʈ���� ��� ������ �����ϴ� �Լ�
	// 3���� : �������� Ʈ���� ��� �Ǵ� ������ ������ ����
	m_pAniControl->KeyTrackEnable(m_iCurrentTrack, FALSE, m_fAccTime + 0.1);
	// Ʈ���� �����Ǵ� �ð����� ���� �ִϸ��̼� ���� � �ӵ��� �������� ����(�ӵ��� ��� ���� ���� 1)
	m_pAniControl->KeyTrackSpeed(m_iCurrentTrack, 1.f, m_fAccTime, 0.1, D3DXTRANSITION_LINEAR);
	// Ʈ���� �����Ǵ� �ð����� ���� �ִϸ��̼� ���� � ����ġ�� ���� ����(�ӵ��� ��� ���� ���� 1)
	m_pAniControl->KeyTrackWeight(m_iCurrentTrack, 0.1f, m_fAccTime, 0.1, D3DXTRANSITION_LINEAR);

	// Ʈ�� Ȱ��ȭ�� ������ �Ǵ��ϴ� �Լ�
	m_pAniControl->SetTrackEnable(m_iNewTrack, TRUE);
	// Ʈ���� �����ϴ� �ð����� ���� �ִϸ��̼� ���� � �ӵ��� �������� ����(�ӵ��� ��� ���� ���� 1)
	m_pAniControl->KeyTrackSpeed(m_iNewTrack, 1.f, m_fAccTime, 0.1, D3DXTRANSITION_LINEAR);
	// Ʈ���� �����ϴ� �ð����� ���� �ִϸ��̼� ���� � ����ġ�� ���� ����(�ӵ��� ��� ���� ���� 1)
	m_pAniControl->KeyTrackWeight(m_iNewTrack, 0.9f, m_fAccTime, 0.1, D3DXTRANSITION_LINEAR);

	// �ִϸ��̼��� ����Ǵ� �ð��� �ٽ� ����(advanced �Լ� ȣ��� ���������� �����Ǵ� �ð� ����)
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

	m_dPeriod = pAnimationset->GetPeriod();	// ���� �ִϸ��̼� ���� ���� ��ü ��� �ð� ��ȯ

											// 0��° Ʈ���� ����ϰ����ϴ� �ִϸ��̼� ���� ������
	m_pAniControl->SetTrackAnimationSet(m_iNewTrack, pAnimationset);

	// ������� �ʰ� �ִ� �̺�Ʈ ������ ������ ���� ������ �ȵǴ� ��찡 �߻��ؼ� ȣ��
	m_pAniControl->UnkeyAllTrackEvents(m_iCurrentTrack);
	m_pAniControl->UnkeyAllTrackEvents(m_iNewTrack);

	// ������ Ʈ���� ��� ������ �����ϴ� �Լ�
	// 3���� : �������� Ʈ���� ��� �Ǵ� ������ ������ ����
	m_pAniControl->KeyTrackEnable(m_iCurrentTrack, FALSE, m_fAccTime + 0.1);
	// Ʈ���� �����Ǵ� �ð����� ���� �ִϸ��̼� ���� � �ӵ��� �������� ����(�ӵ��� ��� ���� ���� 1)
	m_pAniControl->KeyTrackSpeed(m_iCurrentTrack, 1.f, m_fAccTime, 0.1, D3DXTRANSITION_LINEAR);
	// Ʈ���� �����Ǵ� �ð����� ���� �ִϸ��̼� ���� � ����ġ�� ���� ����(�ӵ��� ��� ���� ���� 1)
	m_pAniControl->KeyTrackWeight(m_iCurrentTrack, 0.1f, m_fAccTime, 0.1, D3DXTRANSITION_LINEAR);

	// Ʈ�� Ȱ��ȭ�� ������ �Ǵ��ϴ� �Լ�
	m_pAniControl->SetTrackEnable(m_iNewTrack, TRUE);
	// Ʈ���� �����ϴ� �ð����� ���� �ִϸ��̼� ���� � �ӵ��� �������� ����(�ӵ��� ��� ���� ���� 1)
	m_pAniControl->KeyTrackSpeed(m_iNewTrack, 1.f, m_fAccTime, 0.1, D3DXTRANSITION_LINEAR);
	// Ʈ���� �����ϴ� �ð����� ���� �ִϸ��̼� ���� � ����ġ�� ���� ����(�ӵ��� ��� ���� ���� 1)
	m_pAniControl->KeyTrackWeight(m_iNewTrack, 0.9f, m_fAccTime, 0.1, D3DXTRANSITION_LINEAR);

	m_pAniControl->SetTrackPosition(m_iNewTrack, m_fAccTime);
	m_iOldAniIndex = _iIndex;
	m_iCurrentTrack = m_iNewTrack;
}

void CAniControl::Play_Animationset(const _float & _fDeltaTime)
{
	// ���� �ִϸ��̼��� �����Ű�� �Լ�
	// _fDeltaTime�ʹ� ������ AdvanceTime�Լ� ȣ��� ���������� �����ϴ� �ð� ���� ���� ����
	// 2���� : �ִϸ��̼� ���ۿ� ���� ����Ʈ�� ���� ó���� ����ϴ� ��ü�� �ּ�(��Դ� ������� �ʰ� ���� ���� ���)
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
	// MaxFrame(�ִϸ��̼��� �ִ� ��)
	// WantFrame(���ϴ� �ִϸ��̼� ��)

	// ���ϴ� �����Ӻ��� ������ false ���ų� ũ�� true;
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
