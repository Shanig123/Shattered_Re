#pragma once

#ifndef __D3DCLASS_H__
#define __D3DCLASS_H__

#include "Base.h"

BEGIN(Engine)

class D3DClass final : public CBase
{
	DECLARE_SINGLETON(D3DClass)
private:
	explicit D3DClass();
	//explicit D3DClass(const D3DClass&);
	virtual ~D3DClass() = default;

public:
	HRESULT Initialize(int _iScreenWidth, 
		int _iScreenHeight,
		bool _bVsunc, 
		HWND _hWnd, 
		bool _bFullScreen,
		float _fScreenDepth,
		float _fScreenNear); 
	//void Shutdown();
	//void BeginScene(float _fRed, float _fGreen, float _fBlue, float _fAlpha); 
	//void EndScene(); 
	void Render_Begin(float _fRed, float _fGreen, float _fBlue, float _fAlpha);
	void Render_End();

	ID3D11Device* GetDevice(); 
	ID3D11DeviceContext* GetDeviceContext(); 
	void GetProjectionMatrix(XMMATRIX& _prjMat); 
	void GetWorldMatrix(XMMATRIX& _worldMat);
	void GetOrthoMatrix(XMMATRIX& _orthoMat);
	void GetVideoCardInfo(char* _charCardName, int& _iMemory);
private: 
	bool m_bVsync_enabled; 
	int m_iVideoCardMemory; 
	char m_cVideoCardDescription[128]; 
	IDXGISwapChain* m_pSwapChain; 
	_DEVICE m_pDevice; 
	ID3D11DeviceContext* m_pDeviceContext; 
	ID3D11RenderTargetView* m_pRenderTargetView; 
	ID3D11Texture2D* m_pDepthStencilBuffer; 
	ID3D11DepthStencilState* m_pDepthStencilState; 
	ID3D11DepthStencilView* m_pDepthStencilView; 
	ID3D11RasterizerState* m_pRasterState; 
	XMMATRIX m_matProjectionMatrix; 
	XMMATRIX m_matWorldMatrix; 
	XMMATRIX m_matOrthoMatrix;
public:
	virtual void Free() override;

};

END

#endif // !__D3DCLASS_H__

