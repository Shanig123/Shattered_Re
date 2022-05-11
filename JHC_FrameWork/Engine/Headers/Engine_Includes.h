#pragma once

#ifndef __ENGINE_INCLUDES_H__
#define __ENGINE_INCLUDES_H__

#pragma region _MFCDLL_INCLUDE



#ifdef _AFXDLL
#pragma warning(disable : 4005)
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

//#include "targetver.h"
#include "sdkddkver.h"
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE 클래스입니다.
#include <afxodlgs.h>       // MFC OLE 대화 상자 클래스입니다.
#include <afxdisp.h>        // MFC 자동화 클래스입니다.
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC 데이터베이스 클래스입니다.
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO 데이터베이스 클래스입니다.
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#endif // _AFXDLL


#pragma endregion

#pragma warning(disable : 4251)

/* Standard Headers */
#include <list>
#include <vector>
#include <unordered_map>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <typeinfo>
#include <random>
#include <ctime>
#include <memory>
#include <process.h>
#include <sstream>
//#include <iostream>

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

using namespace std;
using namespace std::tr1;

/* dx9 */
//#include "d3d9.h"
//#include "d3dx9.h"
#include "D3D11.h"
#include "D3Dcompiler.h"
#include "DirectXMath.h"

#pragma comment(lib,"dxgi.lib")



#ifndef _AFXDLL
/* PhysX */
#include "PxPhysicsAPI.h"
#endif // _AFXDLL

// 사운드
#include <io.h>
#include "fmod.h"
//#pragma comment (lib, "fmod_vc.lib")

/* User Headers */
#include "Engine_Typedef.h"
#include "Engine_Enum.h"
#include "Engine_Struct.h"
#include "Engine_Define.h"
#include "Engine_Functions.h"
#include "Engine_Macro.h"
#include "Engine_Debug.h"



#endif // !__ENGINE_INCLUDES_H__
