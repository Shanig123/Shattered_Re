#pragma once

#ifndef __KEY_MANAGER_H__
#define __KEY_MANAGER_H__

#include "Base.h"

BEGIN(Engine)
//const _uint KEY_W			= 0x00000001;		/* 00000000 00000000 00000000 00000001 */
//const _uint KEY_A			= 0x00000002;		/* 00000000 00000000 00000000 00000010 */
//const _uint KEY_S			= 0x00000004;		/* 00000000 00000000 00000000 00000100 */
//const _uint KEY_D			= 0x00000008;		/* 00000000 00000000 00000000 00001000 */
//
//const _uint KEY_UP			= 0x00000010;		/* 00000000 00000000 00000000 00010000 */
//const _uint KEY_LEFT		= 0x00000020;		/* 00000000 00000000 00000000 00100000 */
//const _uint KEY_DOWN		= 0x00000040;		/* 00000000 00000000 00000000 01000000 */
//const _uint KEY_RIGHT		= 0x00000080;		/* 00000000 00000000 00000000 10000000 */
//
//const _uint KEY_Z			= 0x00000100;		/* 00000000 00000000 00000001 00000000 */
//const _uint KEY_CTRL		= 0x00000200;		/* 00000000 00000000 00000010 00000000 */
//const _uint KEY_SHIFT		= 0x00000400;		/* 00000000 00000000 00000100 00000000 */
//const _uint KEY_SPACE		= 0x00000800;		/* 00000000 00000000 00001000 00000000 */
//
//const _uint KEY_1			= 0x00001000;		/* 00000000 00000000 00010000 00000000 */
//const _uint KEY_2			= 0x00002000;		/* 00000000 00000000 00100000 00000000 */
//const _uint KEY_3			= 0x00004000;		/* 00000000 00000000 01000000 00000000 */
//const _uint KEY_4			= 0x00008000;		/* 00000000 00000000 10000000 00000000 */
//
//const _uint KEY_LBUTTON		= 0x00010000;		/* 00000000 00000001 00000000 00000000 */
//const _uint KEY_RBUTTON		= 0x00020000;		/* 00000000 00000010 00000000 00000000 */
//const _uint KEY_F			= 0x00040000;		/* 00000000 00000100 00000000 00000000 */
////const _uint KEY_MINUS		= 0x00080000;		/* 00000000 00001000 00000000 00000000 */
//
////const _uint KEY_LBUTTON	= 0x00100000;		/* 00000000 00010000 00000000 00000000 */
////const _uint KEY_RBUTTON	= 0x00200000;		/* 00000000 00100000 00000000 00000000 */
////const _uint KEY_Q			= 0x00400000;		/* 00000000 01000000 00000000 00000000 */
////const _uint KEY_W			= 0x00800000;		/* 00000000 10000000 00000000 00000000 */
//
////const _uint KEY_E			= 0x01000000;		/* 00000001 00000000 00000000 00000000 */
////const _uint KEY_SPACE		= 0x02000000;		/* 00000010 00000000 00000000 00000000 */
////const _uint KEY_2			= 0x04000000;		/* 00000100 00000000 00000000 00000000 */
////const _uint KEY_3			= 0x08000000;		/* 00001000 00000000 00000000 00000000 */
//
////const _uint KEY_4			= 0x10000000;		/* 00010000 00000000 00000000 00000000 */
////const _uint KEY_Z			= 0x20000000;		/* 00100000 00000000 00000000 00000000 */
////const _uint KEY_CTRL		= 0x40000000;		/* 01000000 00000000 00000000 00000000 */
//const _uint KEY_TAB			= 0x80000000;		/* 10000000 00000000 00000000 00000000 */

#define MAX_KEY_VALUE (256)


class ENGINE_DLL CKey_Manager final : public CBase
{
	DECLARE_SINGLETON(CKey_Manager)

private:
	explicit CKey_Manager();
	virtual ~CKey_Manager() = default;

public:
	HRESULT Ready_Key_Manager(HINSTANCE _hInst, HWND _hWnd);
	void Set_InputDev();
	void Update_Key_Manager();
public:
	_byte	Get_DIKeyState(_ubyte byKeyID) { return m_byKeyState[byKeyID]; }
	_byte	Get_DIMouseState(EMouseKey eMouse) { return m_tMouseState.rgbButtons[(_uint)eMouse]; }
	_long	Get_DIMouseMove(EAxis eMouseState) { return *(((_long*)&m_tMouseState) + (_uint)eMouseState); }

public:
	_bool Key_Pressing(_ubyte _iKey);
	_bool KeyPressing(EMouseKey eMouse);
	_bool Key_Down(_ubyte _iKey);
	_bool KeyDown(EMouseKey eMouse);
	_bool Key_Up(_ubyte _iKey);
	_bool KeyUp(EMouseKey eMouse);
	_byte Mouse_Down(EMouseKey _eMouseKey) { return m_tMouseState.rgbButtons[(_uint)_eMouseKey]; }
	_long Mouse_Move(EAxis _eMouseMove) { return *(((_long*)&m_tMouseState) + (_uint)_eMouseMove); }

public:
	virtual void Free() override;

private:
	LPDIRECTINPUT8			m_pInputSDK ;
	LPDIRECTINPUTDEVICE8	m_pMouse ;
	LPDIRECTINPUTDEVICE8	m_pKeyBoard;

	DIMOUSESTATE			m_tMouseState;


	_byte					m_byKeyState[MAX_KEY_VALUE];
	_bool					m_bKeyCheck[MAX_KEY_VALUE];
	_bool					m_bMouseCheck[4];

};

END

#endif // !__KEY_MANAGER_H__