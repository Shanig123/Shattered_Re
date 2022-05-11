#include "Graphic_Device.h"
#include "Management.h"

USING(Engine)

IMPLEMENT_SINGLETON(CGraphic_Device)

CGraphic_Device::CGraphic_Device()
{
	ZeroMemory(&m_vAspect, sizeof(_float2));
}

HRESULT CGraphic_Device::Ready_Graphic_Device(HWND _hWnd, _uint _iWinCX, _uint _iWinCY, EDisplayMode _eDisplayMode,_bool _bLowGraphicMode)
{
	//다이렉트 생성
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);
	if (nullptr == m_pSDK)
	{
		ERROR_BOX(L"m_pSDK is nullptr");
		return E_FAIL;
	}

	D3DCAPS9 tDeviceCaps;
	ZeroMemory(&tDeviceCaps, sizeof(D3DCAPS9));

	//그래픽 장치 정보를 가져오기.
	//D3DDEVTYPE_HAL - 하드웨어(그래픽카드) 와 소프트웨어(램)을 사용한다.
	if (FAILED(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &tDeviceCaps)))
	{
		ERROR_BOX(L"GetDevice Caps Failed");
		return E_FAIL;
	}

	//하드웨어가 버텍스 프로세싱을 사용 할 수 있는지 검사.
	//버텍스 프로세싱 : 정점변환 + 조명연산
	DWORD dwVP = 0;

	if (tDeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		dwVP = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	else
		dwVP = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;

	//그래픽 장치 설정
	D3DPRESENT_PARAMETERS tD3dpp;
	ZeroMemory(&tD3dpp, sizeof(tD3dpp));

	tD3dpp.BackBufferWidth = _iWinCX;								//후면 버퍼 너비
	tD3dpp.BackBufferHeight = _iWinCY;								//후면 버퍼 높이
	m_vAspect = { _float(_iWinCX), _float(_iWinCY) };
	tD3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;						//후면 버퍼 픽셀 종류. A8R8G8B8 : ARGB(255,255,255,255)
	tD3dpp.BackBufferCount = 1;										//후면 버퍼의 수. 체인버퍼를 사용할거라 1을 사용.

	tD3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	tD3dpp.MultiSampleQuality = 0;

	tD3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	tD3dpp.hDeviceWindow = _hWnd;									//윈도우 핸들
	tD3dpp.Windowed = (_uint)_eDisplayMode;							// TRUE : 창모드, FALSE : 전체화면. 
	tD3dpp.EnableAutoDepthStencil = TRUE;							//깊이 설정. TRUE : 자동
	tD3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	tD3dpp.Flags = 0/*D3DPRESENTFLAG_LOCKABLE_BACKBUFFER*/;

	tD3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	tD3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	//그래픽 장치 생성
	if (FAILED(m_pSDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, _hWnd, dwVP, &tD3dpp, &m_pDevice)))
	{
		ERROR_BOX(L"GraphicDevice Creating Failed");
		return E_FAIL;
	}
	m_bLowGraphicMode = _bLowGraphicMode;
	return S_OK;
}

void CGraphic_Device::Render_Begin(D3DCOLOR _Color)
{
	if (FAILED(m_pDevice->Clear(0, nullptr, D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, _Color, 1.f, 0)))
	{
		ERROR_BOX(L"Failed to Clear");
		return;
	}

	if (FAILED(m_pDevice->BeginScene()))
	{
		ERROR_BOX(L"Failed to BeginScene");
		return;
	}
}

void CGraphic_Device::Render_End()
{
	if (FAILED(m_pDevice->EndScene()))
	{
		ERROR_BOX(L"Failed to EndScene");
		return;
	}

	if (FAILED(m_pDevice->Present(nullptr, nullptr, nullptr, nullptr)))
	{
		ERROR_BOX(L"Failed to Present");
		return;
	}
}

void CGraphic_Device::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pSDK);
}
