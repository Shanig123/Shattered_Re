#pragma once

#ifndef __ENGINE_DEFINE_H__
#define __ENGINE_DEFINE_H__

extern HWND g_hWnd;
extern _uint g_Win_GCD;
extern HINSTANCE g_hInst;

#define TIMER_MAIN			(L"MainTimer")
#define GRAVITY				(9.81)
#define GRAVITYPOWER		(1.6)
#ifdef _DEBUG
#define SHADOW_DETAIL		(4)
#else
#define SHADOW_DETAIL		(6)
#endif // _DEBUG


#endif // !__ENGINE_DEFINE_H__
