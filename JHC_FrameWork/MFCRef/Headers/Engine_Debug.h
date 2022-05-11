#pragma once

#ifndef __ENGINE_DEBUG_H__
#define __ENGINE_DEBUG_H__

#pragma region MEMORY LEAK
#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef _AFX
#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif // !_AFX

#endif  
#pragma endregion

#endif // !__ENGINE_DEBUG_H__
