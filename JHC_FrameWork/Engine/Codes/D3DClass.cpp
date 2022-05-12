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
	float _fScreenFar,
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


	m_bVsync_enabled = _bVsunc;


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

	m_iVideoCardMemory = (int)tAdapterDesc.DedicatedVideoMemory / 1024 / 1024;

	iError = wcstombs_s(&sizeStringLenth,
		m_cVideoCardDescription, 128, tAdapterDesc.Description, 128);
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
	float fFieldOfView, fScreenAspect;

	ZeroMemory(&tSwapChainDesc, sizeof(tSwapChainDesc));
	tSwapChainDesc.BufferCount = 1;
	tSwapChainDesc.BufferDesc.Width = _iScreenWidth;
	tSwapChainDesc.BufferDesc.Height = _iScreenHeight;
	
	
	// Set regular 32-bit surface for the back buffer.
	tSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (m_bVsync_enabled)
	{
		tSwapChainDesc.BufferDesc.RefreshRate.Numerator = iNumerator;
		tSwapChainDesc.BufferDesc.RefreshRate.Denominator = iDenominator;
	}
	else
	{
		tSwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		tSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	tSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	tSwapChainDesc.OutputWindow = _hWnd;

	tSwapChainDesc.SampleDesc.Count = 1;
	tSwapChainDesc.SampleDesc.Quality = 0;

	if (_bFullScreen)
		tSwapChainDesc.Windowed = false;
	else
		tSwapChainDesc.Windowed = true;

	tSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	tSwapChainDesc.Flags = 0;

	featureLvl = D3D_FEATURE_LEVEL_11_0;

	FAILED_CHECK_RETURN(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		&featureLvl,
		1,
		D3D11_SDK_VERSION,
		&tSwapChainDesc,
		&m_pSwapChain, 
		&m_pDevice,
		NULL,
		&m_pDeviceContext), E_FAIL);

	FAILED_CHECK_RETURN(m_pSwapChain->GetBuffer(
		0,
		__uuidof(ID3D10Texture2D),
		(LPVOID*)&pBackBufferPtr), E_FAIL);

	FAILED_CHECK_RETURN(m_pDevice->CreateRenderTargetView(
		pBackBufferPtr,
		NULL,
		&m_pRenderTargetView), E_FAIL);

	pBackBufferPtr->Release();
	pBackBufferPtr = nullptr;

	ZeroMemory(&tDepthBufferDesc, sizeof(tDepthBufferDesc));
	tDepthBufferDesc.Width = _iScreenWidth;
	tDepthBufferDesc.Height = _iScreenHeight;
	tDepthBufferDesc.MipLevels = 1;
	tDepthBufferDesc.ArraySize = 1;
	tDepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tDepthBufferDesc.SampleDesc.Count = 1;
	tDepthBufferDesc.SampleDesc.Quality = 0;
	tDepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	tDepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tDepthBufferDesc.CPUAccessFlags = 0;
	tDepthBufferDesc.MiscFlags = 0;

	FAILED_CHECK_RETURN(m_pDevice->CreateTexture2D(
		&tDepthBufferDesc, 
		NULL,
		&m_pDepthStencilBuffer), E_FAIL);

	ZeroMemory(&tDepthStencilDesc, sizeof(tDepthStencilDesc));
	tDepthStencilDesc.DepthEnable = true;
	tDepthStencilDesc.DepthWriteMask= D3D11_DEPTH_WRITE_MASK_ALL;
	tDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	tDepthStencilDesc.StencilEnable = true;
	tDepthStencilDesc.StencilReadMask = 0xFF;
	tDepthStencilDesc.StencilWriteMask = 0xFF;

	tDepthStencilDesc.FrontFace.StencilFailOp= D3D11_STENCIL_OP_KEEP;
	tDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	tDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	tDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	tDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	tDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	tDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	tDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	FAILED_CHECK_RETURN(m_pDevice->CreateDepthStencilState(&tDepthStencilDesc,
		&m_pDepthStencilState), E_FAIL);

	m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);

	ZeroMemory(&tDepthStencilViewDesc, sizeof(tDepthStencilViewDesc));

	tDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	tDepthStencilViewDesc.Texture2D.MipSlice = 0;

	FAILED_CHECK_RETURN(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &tDepthStencilViewDesc,
		&m_pDepthStencilView), E_FAIL);

	tRasterDesc.AntialiasedLineEnable = false;
	tRasterDesc.CullMode = D3D11_CULL_BACK;
	tRasterDesc.DepthBias = 0;
	tRasterDesc.DepthBiasClamp = 0.f;
	tRasterDesc.DepthClipEnable = true;
	tRasterDesc.FillMode = D3D11_FILL_SOLID;
	tRasterDesc.FrontCounterClockwise = false;
	tRasterDesc.MultisampleEnable = false;
	tRasterDesc.ScissorEnable = false;
	tRasterDesc.SlopeScaledDepthBias = 0.f;

	FAILED_CHECK_RETURN(m_pDevice->CreateRasterizerState(&tRasterDesc, &m_pRasterState), E_FAIL);
	m_pDeviceContext->RSSetState(m_pRasterState);

	tViewPort.Width = _iScreenWidth;
	tViewPort.Height = _iScreenHeight;
	tViewPort.MinDepth = 0.f;
	tViewPort.MaxDepth = 1.f;
	tViewPort.TopLeftX = 0.f;
	tViewPort.TopLeftY = 0.f;
	
	m_pDeviceContext->RSSetViewports(1, &tViewPort);
	fFieldOfView = XM_PI / 4.f;
	fScreenAspect = (float)_iScreenWidth / (float)_iScreenHeight;

	m_matProjectionMatrix = XMMatrixPerspectiveFovLH(fFieldOfView, fScreenAspect, _fScreenNear, _fScreenFar);

	m_matWorldMatrix = XMMatrixIdentity();

	m_matOrthoMatrix = XMMatrixOrthographicLH((float)_iScreenWidth, (float)_iScreenHeight, _fScreenNear, _fScreenFar);

	return S_OK;
}

//void D3DClass::Shutdown()
//{
//}

//void D3DClass::BeginScene(float _fRed, float _fGreen, float _fBlue, float _fAlpha)
//{
//}

//void D3DClass::EndScene()
//{
//}

void D3DClass::Render_Begin(float _fRed, float _fGreen, float _fBlue, float _fAlpha)
{
	float color[4] = { _fRed,_fGreen,_fBlue,_fAlpha };
	m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
	return;
	
}

void D3DClass::Render_End()
{
	if (m_bVsync_enabled)
		m_pSwapChain->Present(1, 0);
	else
		m_pSwapChain->Present(0, 0);
	return;
}

ID3D11Device * D3DClass::GetDevice()
{
	return m_pDevice;
}

ID3D11DeviceContext * D3DClass::GetDeviceContext()
{
	return m_pDeviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX & _prjMat)
{
	_prjMat = m_matProjectionMatrix;
	return;
}

void D3DClass::GetWorldMatrix(XMMATRIX & _worldMat)
{
	_worldMat = m_matWorldMatrix;
	return;
}

void D3DClass::GetOrthoMatrix(XMMATRIX & _orthoMat)
{
	_orthoMat = m_matOrthoMatrix;
	return;
}

void D3DClass::GetVideoCardInfo(char * _charCardName, int & _iMemory)
{
	strcpy_s(_charCardName, 128, m_cVideoCardDescription);
	_iMemory = m_iVideoCardMemory;
	return;
}

void D3DClass::Free()
{
	if (m_pSwapChain)
		m_pSwapChain->SetFullscreenState(false, NULL);

	Safe_Release(m_pRasterState);
	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pDepthStencilState);
	Safe_Release(m_pDepthStencilBuffer);
	Safe_Release(m_pRenderTargetView);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pSwapChain);

}
