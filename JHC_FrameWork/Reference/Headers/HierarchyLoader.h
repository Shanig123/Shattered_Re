#pragma once

#ifndef __HIERARCHYLOADER_H__
#define __HIERARCHYLOADER_H__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CHierarchyLoader final : public ID3DXAllocateHierarchy
{
public:
	explicit CHierarchyLoader(_DEVICE _pDevice, const wstring& _pPath);
	virtual ~CHierarchyLoader() = default;

public:
	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);

	STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR Name,
									CONST D3DXMESHDATA *pMeshData,
									CONST D3DXMATERIAL *pMaterials,
									CONST D3DXEFFECTINSTANCE *pEffectInstances,
									DWORD NumMaterials,
									CONST DWORD *pAdjacency,
									LPD3DXSKININFO pSkinInfo,
									LPD3DXMESHCONTAINER *ppNewMeshContainer);


	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree);

private:
	void Allocate_Name(char** _ppName, const char* _pFrameName);

public:
	static CHierarchyLoader* Create(_DEVICE _pDevice, const wstring& _pPath);
	_ulong Release();

public:
	const _float3& Get_Max() const { return m_vMax; }
	const _float3& Get_Min() const { return m_vMin; }
	_float3* Get_VtxPos() { return m_pVtxPos; }
	_ulong Get_VtxCount() const { return m_dwVtxCount; }
private:
	_DEVICE				m_pDevice;
	const wstring		m_pPath;
	_float3				m_vMax;
	_float3				m_vMin;
	_ulong				m_dwVtxCount;	// 메쉬가 지닌 정점의 개수를 반환
	_float3*			m_pVtxPos;
};

END

#endif // !__HIERARCHYLOADER_H__
