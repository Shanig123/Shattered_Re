#pragma once

#ifndef __D3DCLASS_H__
#define __D3DCLASS_H__

#include "Base.h"
#include "DirectXMath.h"

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
	void Shutdown();
	void BeginScene(float _fRed, float _fGreen, float _fBlue, float _fAlpha); 
	void EndScene(); 
	ID3D11Device* GetDevice(); 
	ID3D11DeviceContext* GetDeviceContext(); 
	void GetProjectionMatrix(XMMATRIX& _prjMat); 
	void GetWorldMatrix(XMMATRIX& _worldMat);
	void GetOrthoMatrix(XMMATRIX& _orthoMat);
	void GetVideoCardInfo(char* _charCardName, int& _iMemory);
private: 
	bool m_vsync_enabled; 
	int m_videoCardMemory; 
	char m_videoCardDescription[128]; 
	IDXGISwapChain* m_pswapChain; 
	ID3D11Device* m_pdevice; 
	ID3D11DeviceContext* m_pdeviceContext; 
	ID3D11RenderTargetView* m_prenderTargetView; 
	ID3D11Texture2D* m_pdepthStencilBuffer; 
	ID3D11DepthStencilState* m_pdepthStencilState; 
	ID3D11DepthStencilView* m_pdepthStencilView; 
	ID3D11RasterizerState* m_prasterState; 
	XMMATRIX m_projectionMatrix; 
	XMMATRIX m_worldMatrix; 
	XMMATRIX m_orthoMatrix;
public:
	virtual void Free() override;

};

END

#endif // !__D3DCLASS_H__

