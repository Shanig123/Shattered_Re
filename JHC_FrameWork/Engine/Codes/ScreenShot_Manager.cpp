#include "ScreenShot_Manager.h"

USING(Engine)

IMPLEMENT_SINGLETON(CScreenShot_Manager)

CScreenShot_Manager::CScreenShot_Manager()
	: m_iScreenShot(0)
	, m_wstrScreenShotPath(L"../../Resources/ScreenShot/Endding%d.png")
{
}

HRESULT CScreenShot_Manager::CreateSurface(_DEVICE _Device, const _float& _fScreenX, const _float& _fScreenY)
{
	IDirect3DSurface9* surface = NULL;
	//_Device->CreateOffscreenPlainSurface((UINT)_fScreenX, (UINT)_fScreenY, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &surface, NULL);
	_Device->CreateOffscreenPlainSurface((UINT)_fScreenX, (UINT)_fScreenY, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &surface, NULL);

	_Device->SetRenderTarget(0, surface);

	LPDIRECT3DSURFACE9 back = NULL;
	HRESULT hr = _Device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back);

	if (hr != D3D_OK)
	{
		surface->Release();
		return E_FAIL;
	}

	_Device->StretchRect(surface, NULL, back, NULL, D3DTEXF_NONE);

	TCHAR _szFullPath[MAX_PATH] = L"";
	swprintf_s(_szFullPath, m_wstrScreenShotPath.c_str(), m_iScreenShot);

	D3DXSaveSurfaceToFile(_szFullPath, D3DXIFF_PNG, back, NULL, NULL);

	++m_iScreenShot;

	if (back)
		back->Release();

	return S_OK;
}

void CScreenShot_Manager::Free()
{
}
