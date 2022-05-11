#ifndef SkyBox_h__
#define SkyBox_h__

#include "GameObject.h"

USING(Engine);

class CSkyBox : public Engine::CGameObject
{
private:
	explicit CSkyBox(_DEVICE pGraphicDev);
	virtual ~CSkyBox(void);

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	//virtual _uint LateUpdate_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

private:
	HRESULT			Add_Component(void);
	HRESULT			SetUp_ConstantTable(LPD3DXEFFECT& _pEffect);

private:
	CVIBuffer_CubeTexture*		m_pVIBuffer ;
	CTexture*		m_pTexture;
	CTexture*		m_pFogTexture;
	CTransform*		m_pTransform;
	CShader*		m_pShader;
public:
	static CSkyBox*		Create(_DEVICE pGraphicDev);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
private:
	virtual void Free(void) override;




};



#endif // SkyBox_h__
