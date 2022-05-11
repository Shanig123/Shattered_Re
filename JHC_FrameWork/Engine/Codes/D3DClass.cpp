#include "D3DClass.h"
USING(Engine)

IMPLEMENT_SINGLETON(D3DClass)

D3DClass::D3DClass()
{
	//ZeroMemory(&m_vAspect, sizeof(_float2));
}


HRESULT D3DClass::Initialize(int _iScreenWidth,
	int _iScreenHeight,
	bool _bVsunc,
	HWND _hWnd,
	bool _bFullScreen,
	float _fScreenDepth,
	float _fScreenNear)
{
	//HRESULT  result;

	IDXGIFactory* pFactory;
	IDXGIAdapter* pAdapter;
	IDXGIOutput* pAdapterOutput;
	unsigned int iNumModes, iNumerator, iDenominator;
	size_t sizeStringLenth;

	DXGI_MODE_DESC* pDisplayModeList;
	DXGI_ADAPTER_DESC tAdapterDesc;
	int iError;


	m_vsync_enabled = _bVsunc;


	FAILED_CHECK_RETURN(
		CreateDXGIFactory(__uuidof(IDXGIFactory), 
		(void**)&pFactory), E_FAIL);

	
	FAILED_CHECK_RETURN(
		pFactory->EnumAdapters(0, &pAdapter), E_FAIL);

	FAILED_CHECK_RETURN(
		pAdapter->EnumOutputs(0, &pAdapterOutput), E_FAIL);



	FAILED_CHECK_RETURN(
		pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_ENUM_MODES_INTERLACED,
			&iNumModes, NULL), E_FAIL);


	pDisplayModeList = new DXGI_MODE_DESC[iNumModes];
	NULL_CHECK_RETURN(pDisplayModeList);


	FAILED_CHECK_RETURN(
		pAdapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_ENUM_MODES_INTERLACED,
			&iNumModes, pDisplayModeList), E_FAIL);
	


	for (int i = 0; i < iNumModes; ++i)
	{
		if (pDisplayModeList[i].Width == (UINT)_iScreenWidth)
		{
			if (pDisplayModeList[i].Height == (UINT)_iScreenHeight)
			{
				iNumerator = pDisplayModeList[i].RefreshRate.Numerator;
				iDenominator = pDisplayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	FAILED_CHECK_RETURN(pAdapter->GetDesc(&tAdapterDesc), E_FAIL);

	m_videoCardMemory = (int)tAdapterDesc.DedicatedVideoMemory / 1024 / 1024;

	iError = wcstombs_s(&sizeStringLenth,
		m_videoCardDescription, 128, tAdapterDesc.Description, 128);
	if (iError != 0)
	{
		ERROR_BOX(L"ERROR , change characterArr");
		return E_FAIL;
	}

	delete[] pDisplayModeList;
	pDisplayModeList = 0;

	Safe_Release(pAdapter);
	Safe_Release(pFactory);

	///===========================================================================

	DXGI_SWAP_CHAIN_DESC tSwapChainDesc;
	D3D_FEATURE_LEVEL featureLvl;
	ID3D11Texture2D* pBackBufferPtr;
	D3D11_TEXTURE2D_DESC tDepthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC tDepthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC tDepthStencilViewDesc;
	D3D11_RASTERIZER_DESC tRasterDesc;
	D3D11_VIEWPORT tViewPort;

	ZeroMemory(&tSwapChainDesc, sizeof(tSwapChainDesc));
	tSwapChainDesc.BufferCount = 1;
	tSwapChainDesc.BufferDesc.Width = _iScreenWidth;
	tSwapChainDesc.BufferDesc.Height = _iScreenHeight;
	
	
	// Set regular 32-bit surface for the back buffer.
	tSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (m_vsync_enabled)
	{
		tSwapChainDesc.BufferDesc.RefreshRate.Numerator = iNumerator;
		tSwapChainDesc.BufferDesc.RefreshRate.Denominator = iDenominator;
	}
	else
	{
		tSwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		tSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}



}

void D3DClass::Shutdown()
{
}

void D3DClass::BeginScene(float _fRed, float _fGreen, float _fBlue, float _fAlpha)
{
}

void D3DClass::EndScene()
{
}

ID3D11Device * D3DClass::GetDevice()
{
	return nullptr;
}

ID3D11DeviceContext * D3DClass::GetDeviceContext()
{
	return nullptr;
}

void D3DClass::Free()
{
}
