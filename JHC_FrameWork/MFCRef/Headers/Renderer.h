#pragma once

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "Base.h"
#include "VIBuffer_Screen.h"

BEGIN(Engine)

class CRenderer final : public CBase
{
	DECLARE_SINGLETON(CRenderer)

private:
	explicit CRenderer();
	virtual ~CRenderer() = default;

public:
	HRESULT Ready_Renderer(_DEVICE _pDevice);
	HRESULT Add_RenderList(ERenderPriority _eRenderPriority, class CGameObject* _pGameObject);
	HRESULT Add_RenderSceneList(class CScene* _pScene);
	void Render_Begin(D3DXCOLOR _Color);
	void Render_End();
	HRESULT Render_RenderList(HWND _hWnd = nullptr);
public:
	const _uint& GetFadeEvent() const { return m_iFadeEventFlag; }
public:
	void Debug_Render() { m_bDebug = !m_bDebug; }
	void FadeEvent(const _uint& _Event_012);
	void Set_AccTimer(const _float& _fDeltaTime = 0.f) { m_fAccTime = _fDeltaTime; }
private:
	HRESULT Render_Priority(class CRenderTarget_Manager* _RenderTargetManager);
	HRESULT Render_NoAlpha();
	HRESULT Render_Alpha();
	HRESULT Render_UI(class CRenderTarget_Manager* _RenderTargetManager,HWND _hWnd = nullptr);
	HRESULT Render_Scene();

	HRESULT Render_Environment(class CRenderTarget_Manager* _RenderTargetManager);

	HRESULT Render_Deferred(class CRenderTarget_Manager* _RenderTargetManager);
	HRESULT Render_Shadow(class CRenderTarget_Manager* _RenderTargetManager);
	HRESULT Render_LightAcc(class CRenderTarget_Manager* _RenderTargetManager);
	//HRESULT Render_Effect(class CRenderTarget_Manager* _RenderTargetManager);
	HRESULT Render_HDR_Blend(class CRenderTarget_Manager* _RenderTargetManager);
	HRESULT Render_HorizontalBlur(class CRenderTarget_Manager* _RenderTargetManager);
	HRESULT Render_VerticalBlur(class CRenderTarget_Manager* _RenderTargetManager);
	HRESULT Render_ToneDown(class CRenderTarget_Manager* _RenderTargetManager);


public:
	virtual void Free() override;

private:
#pragma region STL_VALUE
	typedef list<class CGameObject*>		GAMEOBJECTS;
	GAMEOBJECTS								m_GameObjects[(_uint)ERenderPriority::End];

	list<class CScene*>						m_Scenes;

	map<const _tchar*, CVIBuffer_Screen*>	m_mapRenderScreen;
#pragma endregion

#pragma region VALUE

	_float				m_fFadeOutTime;
	_float				m_fFadeInTime;
	_float				m_fAccTime;

	_bool				m_bDebug;

	_uint				m_iFadeEventFlag;

#pragma endregion

#pragma region POINTER_VALUE

	LPDIRECT3DSURFACE9			m_pTargetSurface;
	LPDIRECT3DSURFACE9			m_pOldTargetSurface;

	_DEVICE						m_pDevice;

#pragma endregion


	//_VertexBuffer		m_pVB;
//_IndexBuffer		m_pIB;
//map<wstring, CVIBuffer_Screen*> m_mapRenderScreen;

};

END

#endif // !__RENDERER_H__