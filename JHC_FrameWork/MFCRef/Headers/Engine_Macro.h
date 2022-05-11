#pragma once

#ifndef __ENGINE_MACRO_H__
#define __ENGINE_MACRO_H__

#define BEGIN(Name) namespace Name {
#define END }

#define USING(Name) using namespace Name;

#pragma region ENGINE_DLL
#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif  
#pragma endregion

#pragma region SINGLE_TON
#define NO_COPY(ClassName)							\
ClassName(const ClassName&)	= delete;				\
ClassName& operator=(const ClassName&) = delete;

#define DECLARE_SINGLETON(ClassName)				\
		NO_COPY(ClassName)							\
public:												\
	static ClassName* Get_Instance();				\
	static _uint ClassName::Destroy_Instance();		\
private:											\
	static ClassName* m_pInstance;

#define IMPLEMENT_SINGLETON(ClassName)				\
ClassName* ClassName::m_pInstance = nullptr;		\
ClassName* ClassName::Get_Instance()				\
{													\
	if (nullptr == m_pInstance)						\
		m_pInstance = new ClassName;				\
	return m_pInstance;								\
}													\
_uint ClassName::Destroy_Instance()					\
{													\
	_uint iRefCount = 0;							\
	if (m_pInstance)								\
		iRefCount = m_pInstance->Release();			\
	return iRefCount;								\
}  
#pragma endregion

#define ARGB(_a, _r, _g, _b) D3DCOLOR_ARGB(_a, _r, _g, _b)

#ifdef _WIN64
#define BREAK_POINT __debugbreak()
#else
#define BRAEAK_POINT {__asm { int 3 }
#endif // _WIN64

#pragma region MSG_BOX & ERROR_BOX
#ifndef _AFX
#define MSG_BOX(_message) MessageBoxW(NULL, _message, L"System Message", MB_OK);

#define ERROR_BOX(_message)								\
MessageBoxW(NULL, _message, L"System Message", MB_OK);	\
BREAK_POINT;
#else
#define MSG_BOX(_message) AfxMessageBox(_message);

#define ERROR_BOX(_message)		\
AfxMessageBox(_message);		\
BREAK_POINT;
#endif // !_AFX  
#pragma endregion

#define NULL_CHECK_RETURN(_ptr,	_return)	\
{if(!_ptr){__debugbreak();return _return;}}

#define FAILED_CHECK_RETURN(_hr,_return)	\
{if(FAILED((HRESULT)_hr)){__debugbreak();return _return;}}


#endif // !__ENGINE_MACRO_H__
