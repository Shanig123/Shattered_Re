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
	// DInput 컴객체를 생성하는 함수
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

	// 키보드 객체 생성
	FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr), E_FAIL);
	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, (클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수)
	m_pKeyBoard->SetCooperativeLevel(_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();

	// 마우스 객체 생성
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
	{
		ERROR_BOX(L"Failed to CreateDevice");
		return E_FAIL;
	}

	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
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
