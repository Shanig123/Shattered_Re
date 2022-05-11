#pragma once

#ifndef __ENGINE_INCLUDES_H__
#define __ENGINE_INCLUDES_H__

#pragma region _MFCDLL_INCLUDE



#ifdef _AFXDLL
#pragma warning(disable : 4005)
#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

//#include "targetver.h"
#include "sdkddkver.h"
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.
#include <afxext.h>         // MFC Ȯ���Դϴ�.

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE Ŭ�����Դϴ�.
#include <afxodlgs.h>       // MFC OLE ��ȭ ���� Ŭ�����Դϴ�.
#include <afxdisp.h>        // MFC �ڵ�ȭ Ŭ�����Դϴ�.
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>                      // MFC ODBC �����ͺ��̽� Ŭ�����Դϴ�.
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>                     // MFC DAO �����ͺ��̽� Ŭ�����Դϴ�.
#endif // _AFX_NO_DAO_SUPPORT

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>                     // Windows ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
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

// ����
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
