#include "Key_Manager.h"

USING(Engine)

IMPLEMENT_SINGLETON(CKey_Manager)

CKey_Manager::CKey_Manager()
	: m_pInputSDK(nullptr)
	, m_pMouse(nullptr),m_pKeyBoard(nullptr)
{
	//ZeroMemory(&m_tMouseState, sizeof(m_tMouseState));
}

HRESULT CKey_Manager::Ready_Key_Manager(HINSTANCE _hInst, HWND _hWnd)
{
	// DInput �İ�ü�� �����ϴ� �Լ�
	if (FAILED(DirectInput8Create
	(
		_hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		NULL
	)))
	{
		ERROR_BOX(L"Failed to DirectInput8Create");
		return E_FAIL;
	}

	// Ű���� ��ü ����
	FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr), E_FAIL);
	// ������ Ű���� ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// ��ġ�� ���� �������� �������ִ� �Լ�, (Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�)
	m_pKeyBoard->SetCooperativeLevel(_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pKeyBoard->Acquire();

	// ���콺 ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
	{
		ERROR_BOX(L"Failed to CreateDevice");
		return E_FAIL;
	}

	// ������ ���콺 ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// ��ġ�� ���� �������� �������ִ� �Լ�, Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�
	m_pMouse->SetCooperativeLevel(_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pMouse->Acquire();

	return S_OK;
}

void CKey_Manager::Set_InputDev()
{
	m_pKeyBoard->GetDeviceState(MAX_KEY_VALUE, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}

void CKey_Manager::Update_Key_Manager()
{
	//m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
	

	
	for (int i = 0; i < MAX_KEY_VALUE; ++i)
	{
		if ((Get_DIKeyState(i) && 0x80) && !m_bKeyCheck[i])
			m_bKeyCheck[i] = !m_bKeyCheck[i];
		if ((!Get_DIKeyState(i) && 0x80) && m_bKeyCheck[i])
			m_bKeyCheck[i] = !m_bKeyCheck[i];
		if (i < 4)
		{
			if ((Get_DIMouseState((EMouseKey)i) && 0x80) && !m_bMouseCheck[i])
				m_bMouseCheck[i] = !m_bMouseCheck[i];
			if ((!Get_DIMouseState((EMouseKey)i) && 0x80) && m_bMouseCheck[i])
				m_bMouseCheck[i] = !m_bMouseCheck[i];
		}
	}

}

_bool CKey_Manager::Key_Pressing(_ubyte _iKey)
{
	if (Get_DIKeyState(_iKey) & 0x80)
		return true;
	return false;
}

_bool CKey_Manager::KeyPressing(EMouseKey eMouse)
{
	if (Get_DIMouseState(eMouse) & 0x80)
		return true;
	return false;
}

_bool CKey_Manager::Key_Down(_ubyte _iKey)
{
	if ((Get_DIKeyState(_iKey) & 0x80) && !m_bKeyCheck[_iKey])
	{
		m_bKeyCheck[_iKey] = !m_bKeyCheck[_iKey];
		return true;
	}

	return false;
}

_bool CKey_Manager::KeyDown(EMouseKey eMouse)
{
	if ((Get_DIMouseState(eMouse) & 0x80) && !m_bMouseCheck[(_ubyte)eMouse])
	{
		m_bMouseCheck[(_ubyte)eMouse] = !m_bMouseCheck[(_ubyte)eMouse];
		return true;
	}

	return false;
}

_bool CKey_Manager::Key_Up(_ubyte _iKey)
{
	if (!(Get_DIKeyState(_iKey) & 0x80) && m_bKeyCheck[_iKey])
	{
		m_bKeyCheck[_iKey] = !m_bKeyCheck[_iKey];
		return true;
	}

	return false;
}

_bool CKey_Manager::KeyUp(EMouseKey eMouse)
{
	if (!(Get_DIMouseState(eMouse) & 0x80) && m_bMouseCheck[(_ubyte)eMouse])
	{
		m_bMouseCheck[(_ubyte)eMouse] = !m_bMouseCheck[(_ubyte)eMouse];
		return true;
	}

	return false;
}

void CKey_Manager::Free()
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}
