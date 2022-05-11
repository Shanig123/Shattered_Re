#include "DynamicMesh.h"

USING(Engine)

CDynamicMesh::CDynamicMesh(_DEVICE _pDevice)
	: CMesh(_pDevice)
	, m_pRootBone(nullptr)
	, m_pLoader(nullptr)
	, m_pAniControl(nullptr)
	, m_matBoneMatrix(nullptr)
	, m_longBoneCount(0)
{
	m_eMeshType = EMeshType::Dynamic;
}

CDynamicMesh::CDynamicMesh(const CDynamicMesh & _rOther)
	: CMesh(_rOther)
	, m_pRootBone(_rOther.m_pRootBone)
	, m_listMeshContaier(_rOther.m_listMeshContaier)
	, m_vecBoneName(_rOther.m_vecBoneName)
	, m_pLoader(_rOther.m_pLoader)
	, m_matBoneMatrix(_rOther.m_matBoneMatrix)
	, m_longBoneCount(_rOther.m_longBoneCount)
{
	m_eMeshType = _rOther.m_eMeshType;
	m_pAniControl = CAniControl::Create(*_rOther.m_pAniControl);
}

HRESULT CDynamicMesh::Ready_Meshes_Prototype(const wstring& _pFilePath, const wstring& _pFileName)
{
	_tchar szFullPath[MAX_PATH] = L"";
	lstrcpy(szFullPath, _pFilePath.c_str());
	lstrcat(szFullPath, _pFileName.c_str());

	m_pLoader = CHierarchyLoader::Create(m_pDevice, _pFilePath);

	if (nullptr == m_pLoader)
	{
		ERROR_BOX(L"m_pLoader is nullptr");
		return E_FAIL;
	}

	_ANICONTROLLER pAniControl = nullptr;

	if (FAILED(D3DXLoadMeshHierarchyFromX
	(
		szFullPath,
		D3DXMESH_MANAGED,
		m_pDevice,
		m_pLoader,			// HierarchyLoader
		NULL,
		&m_pRootBone,
		&pAniControl		// AniColtrol
	)))
	{
		ERROR_BOX(L"Failed to D3DXLoadMeshHierarchyFromX");
		return E_FAIL;
	}

	m_pAniControl = CAniControl::Create(pAniControl);
	if (nullptr == pAniControl)
	{
		ERROR_BOX(L"pAniControl is nullptr");
		return E_FAIL;
	}

	Safe_Release(pAniControl);

	_float4x4 matTemp;
	Update_FrameMatrices((MY_FRAME*)m_pRootBone, D3DXMatrixIdentity(&matTemp));

	Setup_FrameMatrixPointer((MY_FRAME*)m_pRootBone);

	m_vMax = new _float3[1];
	m_vMin = new _float3[1];

	m_vMax[0] = m_pLoader->Get_Max();
	m_vMin[0] = m_pLoader->Get_Min();
	

	m_dwVtxCount = m_pLoader->Get_VtxCount();
	m_pVtxPos = m_pLoader->Get_VtxPos();
	return S_OK;
}

HRESULT CDynamicMesh::Ready_Meshes_Clone(void * _pArg)
{
	if (CMesh::Ready_Meshes_Clone(_pArg))
	{
		ERROR_BOX(L"Failed to Ready_Meshes_Clone");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDynamicMesh::Render_Meshes()
{
	for (auto& iter : m_listMeshContaier)
	{
		MY_MESHCONTAINER* pMeshContainer = iter;

		for (_ulong i = 0; i < pMeshContainer->dwNumBones; ++i)
		{
			pMeshContainer->pRenderingMatrix[i] = pMeshContainer->pFrameOffSetMatrix[i] * (*pMeshContainer->ppFrameCombineMatrix[i]);
		}

		void* pDestVtx = nullptr;
		void* pSourVtx = nullptr;

		pMeshContainer->pOriMesh->LockVertexBuffer(0, &pDestVtx);
		pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pSourVtx);

		// 소프트웨어 스키닝을 수행하는 함수(스키닝 뿐 아니라 애니메이션 변경 시, 뼈대들과 정점 정보들의 변경 또한 동시에 수행)
		pMeshContainer->pSkinInfo->UpdateSkinnedMesh(pMeshContainer->pRenderingMatrix, NULL, pDestVtx, pSourVtx);

		for (_ulong i = 0; i < pMeshContainer->NumMaterials; ++i)
		{
			m_pDevice->SetTexture(0, pMeshContainer->ppTexture[i][0]);
			pMeshContainer->MeshData.pMesh->DrawSubset(i);
		}

		pMeshContainer->pOriMesh->UnlockVertexBuffer();
		pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();
	}

	return S_OK;
}

HRESULT CDynamicMesh::Render_Meshes(LPD3DXEFFECT & _pEffect, _uint _iIndex, MY_FRAME * _pFrame, _bool _bHardSkin)
{
	/*D3DXMESHCONTAINER_DERIVED* pMesh = nullptr;
	_bool bHardwareSkinning = false;
	for (auto& iter : m_listMeshContaier)
	{
		pMesh = iter;
		bHardwareSkinning = pMesh->UseHardwareVP;
	}*/

	if (_bHardSkin)	// 하드웨어 스키닝 렌더링
	{
		if (_pFrame == NULL)
			_pFrame = (MY_FRAME*)m_pRootBone;

		if (_pFrame->pMeshContainer != NULL)
		{
			MY_MESHCONTAINER* pMeshContainer = (MY_MESHCONTAINER*)_pFrame->pMeshContainer;

			if (pMeshContainer->pSkinInfo != NULL)
			{
				LPD3DXBONECOMBINATION pBoneComb = (LPD3DXBONECOMBINATION)pMeshContainer->pFrameCombinationBuf->GetBufferPointer();

				for (_ulong i = 0; i < pMeshContainer->dwBoneComboEntries; ++i)
				{
					_uint iPaletteEntry = 0;
					for (iPaletteEntry; iPaletteEntry < 70; ++iPaletteEntry)
					{
						DWORD iMatrixindex = pBoneComb[i].BoneId[iPaletteEntry];
						if (iMatrixindex != ULONG_MAX)
						{
							pMeshContainer->pRenderingMatrix[iPaletteEntry] = _float4x4();
							D3DXMatrixMultiply
							(
								&pMeshContainer->pRenderingMatrix[iPaletteEntry],
								&pMeshContainer->pFrameOffSetMatrix[iMatrixindex],
								pMeshContainer->ppFrameCombineMatrix[iMatrixindex]
							);
						}
						else
						{
							break;
						}
					}

					_uint bonecobo = pMeshContainer->dwBoneComboEntries;
					_pEffect->SetMatrixArray("g_MatrixPalette", pMeshContainer->pRenderingMatrix, iPaletteEntry);
					_pEffect->SetInt("g_numBonInfluences", pMeshContainer->dwMaxVertinfluences);

					DWORD iMtrlIndex = pBoneComb[i].AttribId;
					_pEffect->SetTexture("g_BaseTexture", pMeshContainer->ppTexture[iMtrlIndex][_iIndex]);
					_pEffect->SetTexture("g_NormalTexture", pMeshContainer->ppNormalTexture[iMtrlIndex]);

					_uint iEmissive = _iIndex;
					if (iEmissive >= pMeshContainer->iEmissiveTextureCount[iMtrlIndex])
						iEmissive = pMeshContainer->iEmissiveTextureCount[iMtrlIndex] - 1;
					_pEffect->SetTexture("g_EmissiveTexture", pMeshContainer->ppEmissiveTexture[iMtrlIndex][iEmissive]);
					_pEffect->SetTexture("g_SpecularTexture", pMeshContainer->ppSpacularTexture[iMtrlIndex]);
					_pEffect->SetTexture("g_OpacityTexture", pMeshContainer->ppOpacityTexture[iMtrlIndex]);
					_pEffect->SetTexture("g_RoughnessTexture", pMeshContainer->ppRoughnessTexture[iMtrlIndex]);
					_pEffect->CommitChanges();

					pMeshContainer->MeshData.pMesh->DrawSubset(iMtrlIndex);			
				}
			}			
		}

		if (_pFrame->pFrameSibling != NULL)
			Render_Meshes(_pEffect, _iIndex, (MY_FRAME*)_pFrame->pFrameSibling, _bHardSkin);

		if (_pFrame->pFrameFirstChild != NULL)
			Render_Meshes(_pEffect, _iIndex, (MY_FRAME*)_pFrame->pFrameFirstChild, _bHardSkin);
	}
	else	// 소프트웨어 스키닝 렌더링
	{
		for (auto& iter : m_listMeshContaier)
		{
			D3DXMESHCONTAINER_DERIVED*		pMeshContainer = iter;

			for (_ulong i = 0; i < pMeshContainer->dwNumBones; ++i)
			{
				pMeshContainer->pRenderingMatrix[i] = pMeshContainer->pFrameOffSetMatrix[i] * *pMeshContainer->ppFrameCombineMatrix[i];
			}

			void*		pDestVtx = nullptr;
			void*		pSourVtx = nullptr;

			pMeshContainer->pOriMesh->LockVertexBuffer(0, &pDestVtx);
			pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pSourVtx);

			// 소프트웨어 스키닝을 수행하는 함수(스키닝 뿐 아니라 애니메이션 변경 시, 뼈대들과 정점 정보들의 변경 또한 동시에 수행)
			pMeshContainer->pSkinInfo->UpdateSkinnedMesh(pMeshContainer->pRenderingMatrix, NULL, pDestVtx, pSourVtx);

			for (_ulong i = 0; i < pMeshContainer->NumMaterials; ++i)
			{
				if (nullptr == pMeshContainer->ppTexture[i][_iIndex])
					_iIndex = 0;

				_pEffect->SetTexture("g_BaseTexture", pMeshContainer->ppTexture[i][_iIndex]);
				_pEffect->SetTexture("g_NormalTexture", pMeshContainer->ppNormalTexture[i]);

				_pEffect->SetTexture("g_EmissiveTexture", pMeshContainer->ppEmissiveTexture[i][_iIndex]);

				_pEffect->SetTexture("g_SpecularTexture", pMeshContainer->ppSpacularTexture[i]);
				_pEffect->SetTexture("g_OpacityTexture", pMeshContainer->ppOpacityTexture[i]);
				_pEffect->SetTexture("g_RoughnessTexture", pMeshContainer->ppRoughnessTexture[i]);
				_pEffect->CommitChanges();

				pMeshContainer->MeshData.pMesh->DrawSubset(i);
			}

			pMeshContainer->pOriMesh->UnlockVertexBuffer();
			pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();
		}
	}

	return S_OK;
}

HRESULT CDynamicMesh::Render_ShadowMeshes(LPD3DXEFFECT & _pEffect, MY_FRAME * _pFrame)
{
	if (_pFrame == NULL)
		_pFrame = (MY_FRAME*)m_pRootBone;

	if (_pFrame->pMeshContainer != NULL)
	{
		MY_MESHCONTAINER* pMeshContainer = (MY_MESHCONTAINER*)_pFrame->pMeshContainer;

		if (pMeshContainer->pSkinInfo != NULL)
		{
			LPD3DXBONECOMBINATION pBoneComb = (LPD3DXBONECOMBINATION)pMeshContainer->pFrameCombinationBuf->GetBufferPointer();

			for (_ulong i = 0; i < pMeshContainer->dwBoneComboEntries; ++i)
			{
				_uint iPaletteEntry = 0;
				for (iPaletteEntry; iPaletteEntry < 70; ++iPaletteEntry)
				{
					DWORD iMatrixindex = pBoneComb[i].BoneId[iPaletteEntry];
					if (iMatrixindex != ULONG_MAX)
					{
						pMeshContainer->pRenderingMatrix[iPaletteEntry] = _float4x4();
						D3DXMatrixMultiply
						(
							&pMeshContainer->pRenderingMatrix[iPaletteEntry],
							&pMeshContainer->pFrameOffSetMatrix[iMatrixindex],
							pMeshContainer->ppFrameCombineMatrix[iMatrixindex]
						);
					}
					else
					{
						break;
					}
				}

				_uint bonecobo = pMeshContainer->dwBoneComboEntries;
				_pEffect->SetMatrixArray("g_MatrixPalette", pMeshContainer->pRenderingMatrix, iPaletteEntry);
				_pEffect->SetInt("g_numBonInfluences", pMeshContainer->dwMaxVertinfluences);
				_uint iMatrixI = pBoneComb[i].AttribId;
				_pEffect->SetTexture("g_BaseTexture", pMeshContainer->ppTexture[i][0]);
				_pEffect->SetTexture("g_NormalTexture", pMeshContainer->ppNormalTexture[i]);

				_pEffect->SetTexture("g_EmissiveTexture", pMeshContainer->ppEmissiveTexture[i][0]);

				_pEffect->SetTexture("g_SpecularTexture", pMeshContainer->ppSpacularTexture[i]);
				_pEffect->SetTexture("g_OpacityTexture", pMeshContainer->ppOpacityTexture[i]);
				_pEffect->SetTexture("g_RoughnessTexture", pMeshContainer->ppRoughnessTexture[i]);
				_pEffect->CommitChanges();

				pMeshContainer->MeshData.pMesh->DrawSubset(i);
			}
		}
	}

	if (_pFrame->pFrameSibling != NULL)
		Render_ShadowMeshes(_pEffect, (MY_FRAME*)_pFrame->pFrameSibling);

	if (_pFrame->pFrameFirstChild != NULL)
		Render_ShadowMeshes(_pEffect, (MY_FRAME*)_pFrame->pFrameFirstChild);
	return S_OK;
}

HRESULT CDynamicMesh::Render_Meshes(LPD3DXEFFECT & _pEffect, COptimization* _pOptimization, const _float4x4* _matWolrd, _uint _iIndex)
{
	for (auto& iter : m_listMeshContaier)
	{
		D3DXMESHCONTAINER_DERIVED*		pMeshContainer = iter;

		for (_ulong i = 0; i < pMeshContainer->dwNumBones; ++i)
		{
			pMeshContainer->pRenderingMatrix[i] = pMeshContainer->pFrameOffSetMatrix[i] * *pMeshContainer->ppFrameCombineMatrix[i];
		}

		void*		pDestVtx = nullptr;
		void*		pSourVtx = nullptr;

		pMeshContainer->pOriMesh->LockVertexBuffer(0, &pDestVtx);
		pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pSourVtx);

		// 소프트웨어 스키닝을 수행하는 함수(스키닝 뿐 아니라 애니메이션 변경 시, 뼈대들과 정점 정보들의 변경 또한 동시에 수행)
		pMeshContainer->pSkinInfo->UpdateSkinnedMesh(pMeshContainer->pRenderingMatrix, NULL, pDestVtx, pSourVtx);

		for (_ulong i = 0; i < pMeshContainer->NumMaterials; ++i)
		{
			if (nullptr == pMeshContainer->ppTexture[i][_iIndex])
				_iIndex = 0;

			_pEffect->SetTexture("g_BaseTexture", pMeshContainer->ppTexture[i][_iIndex]);
			_pEffect->SetTexture("g_NormalTexture", pMeshContainer->ppNormalTexture[i]);

			_pEffect->SetTexture("g_EmissiveTexture", pMeshContainer->ppEmissiveTexture[i][_iIndex]);

			_pEffect->SetTexture("g_SpecularTexture", pMeshContainer->ppSpacularTexture[i]);    
			_pEffect->SetTexture("g_OpacityTexture", pMeshContainer->ppOpacityTexture[i]);
			_pEffect->SetTexture("g_RoughnessTexture", pMeshContainer->ppRoughnessTexture[i]);
			_pEffect->CommitChanges();

			pMeshContainer->MeshData.pMesh->DrawSubset(i);
		}

		pMeshContainer->pOriMesh->UnlockVertexBuffer();
		pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();
	}

	return S_OK;
}

const MY_FRAME* CDynamicMesh::Get_FrameByName(const char* pFrameName)
{
	return (MY_FRAME*)D3DXFrameFind(m_pRootBone, pFrameName);
}

void CDynamicMesh::Set_Animationset(const _uint & _iIndex)
{
	if (nullptr == m_pAniControl)
		return;

	m_pAniControl->Set_Animationset(_iIndex);
}

void CDynamicMesh::Change_Animationset(const _uint & _iIndex)
{

	if (nullptr == m_pAniControl)
		return;

	m_pAniControl->Change_Animationset(_iIndex);
}

_uint CDynamicMesh::Get_Animationset()
{
	if (nullptr == m_pAniControl)
		return 999;

	return m_pAniControl->Get_Animationset();
}

_bool CDynamicMesh::Get_AnimationFrame(_double _dMaxFrame, _double _dWantFrame)
{
	if (nullptr == m_pAniControl)
		return false;

	return m_pAniControl->Get_AnimationFrame(_dMaxFrame, _dWantFrame);
}

void CDynamicMesh::Play_Animationset(const _float & _fDeltaTime)
{
	if (nullptr == m_pAniControl)
		return;

	m_pAniControl->Play_Animationset(_fDeltaTime);

	_float4x4		matTemp;
	Update_FrameMatrices((MY_FRAME*)m_pRootBone, D3DXMatrixIdentity(&matTemp));
}

_bool CDynamicMesh::Is_AnimationSetEnd()
{
	return m_pAniControl->Is_AnimationSetEnd();
}

void CDynamicMesh::Get_AllBoneMetrix(_float4x4 ** _OutMatrix, _ulong * _OutBoneCount)
{
	/*if ((*_OutBoneCount) || (*_OutMatrix))
		return;
	*_OutMatrix = m_matBoneMatrix;
	*_OutBoneCount = m_longBoneCount;*/

	if ((*_OutBoneCount) || (*_OutMatrix))
		return;

	_ulong iBoneCount = 0;
	for (auto& iter : m_listMeshContaier)
	{
		D3DXMESHCONTAINER_DERIVED*		pMeshContainer = iter;

		void*		pDestVtx = nullptr;
		void*		pSourVtx = nullptr;

		pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pDestVtx);
		pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pSourVtx);

		for (_ulong i = 0; i < pMeshContainer->dwNumBones; ++i)
		{
			pMeshContainer->pRenderingMatrix[i] = pMeshContainer->pFrameOffSetMatrix[i] * *pMeshContainer->ppFrameCombineMatrix[i];
		}

		iBoneCount = pMeshContainer->dwNumBones;
		memcpy(m_matBoneMatrix, pMeshContainer->pRenderingMatrix, sizeof(_float4x4) * pMeshContainer->dwNumBones);

		pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();
		pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();
	}

	*_OutMatrix = m_matBoneMatrix;
	*_OutBoneCount = iBoneCount;
}

void CDynamicMesh::Update_FrameMatrices(MY_FRAME * _pFrame, const _float4x4 * _pParentMatrix)
{
	if (nullptr == _pFrame)
		return;

	_pFrame->CombinedTranformationMatrix = _pFrame->TransformationMatrix * (*_pParentMatrix);

	if (nullptr != _pFrame->pFrameSibling)
		Update_FrameMatrices((MY_FRAME*)_pFrame->pFrameSibling, _pParentMatrix);

	if (nullptr != _pFrame->pFrameFirstChild)
		Update_FrameMatrices((MY_FRAME*)_pFrame->pFrameFirstChild, &(_pFrame->CombinedTranformationMatrix));
}

void CDynamicMesh::Setup_FrameMatrixPointer(MY_FRAME * _pFrame)
{
	//if (nullptr != _pFrame->pMeshContainer)
	//{
	//	MY_MESHCONTAINER* pMeshContainer = (MY_MESHCONTAINER*)_pFrame->pMeshContainer;
	//	
	//	m_longBoneCount = pMeshContainer->dwNumBones;
	//	m_matBoneMatrix = new _float4x4[m_longBoneCount];
	//	for (_ulong i = 0; i < m_longBoneCount; ++i)
	//	{
	//		// 뼈의 이름을 얻어옴
	//		const char* pBoneName = pMeshContainer->pSkinInfo->GetBoneName(i);
	//		m_vecBoneName.push_back(CharToWChar(pBoneName));
	//		// 이름과 일치하는 뼈를 찾음
	//		MY_FRAME* pFineBone = (MY_FRAME*)D3DXFrameFind(m_pRootBone, pBoneName);

	//		pMeshContainer->ppFrameCombineMatrix[i] = &pFineBone->CombinedTranformationMatrix;
	//		m_matBoneMatrix[i] = *pMeshContainer->ppFrameCombineMatrix[i];
	//	}
	//	
	//	m_listMeshContaier.push_back(pMeshContainer);
	//}

	//if (nullptr != _pFrame->pFrameSibling)
	//	Setup_FrameMatrixPointer((MY_FRAME*)_pFrame->pFrameSibling);

	//if (nullptr != _pFrame->pFrameFirstChild)
	//	Setup_FrameMatrixPointer((MY_FRAME*)_pFrame->pFrameFirstChild);

	if (nullptr != _pFrame->pMeshContainer)
	{
		MY_MESHCONTAINER* pMeshContainer = (MY_MESHCONTAINER*)_pFrame->pMeshContainer;

		_ulong dwNumBones = pMeshContainer->dwNumBones;
		m_matBoneMatrix = new _float4x4[dwNumBones];
		for (_ulong i = 0; i < dwNumBones; ++i)
		{
			// 뼈의 이름을 얻어옴
			const char* pBoneName = pMeshContainer->pSkinInfo->GetBoneName(i);
			m_vecBoneName.push_back(CharToWChar(pBoneName));
			// 이름과 일치하는 뼈를 찾음
			MY_FRAME* pFineBone = (MY_FRAME*)D3DXFrameFind(m_pRootBone, pBoneName);

			pMeshContainer->ppFrameCombineMatrix[i] = &pFineBone->CombinedTranformationMatrix;
			m_matBoneMatrix[i] = *pMeshContainer->ppFrameCombineMatrix[i];
		}

		m_listMeshContaier.push_back(pMeshContainer);
	}

	if (nullptr != _pFrame->pFrameSibling)
		Setup_FrameMatrixPointer((MY_FRAME*)_pFrame->pFrameSibling);

	if (nullptr != _pFrame->pFrameFirstChild)
		Setup_FrameMatrixPointer((MY_FRAME*)_pFrame->pFrameFirstChild);
}

CDynamicMesh * CDynamicMesh::Create(_DEVICE _pDevice, const wstring& _pFilePath, const wstring& _pFileName)
{
	CDynamicMesh* pInstance = new CDynamicMesh(_pDevice);

	if (FAILED(pInstance->Ready_Meshes_Prototype(_pFilePath, _pFileName)))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMesh * CDynamicMesh::Clone(void * _pArg)
{
	CDynamicMesh* pClone = new CDynamicMesh(*this);

	if (FAILED(pClone->Ready_Meshes_Clone(_pArg)))
	{
		ERROR_BOX(L"Failed to pClone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CDynamicMesh::Free()
{
	Safe_Release(m_pAniControl);

	if (false == m_bClone)
	{
		m_pLoader->DestroyFrame(m_pRootBone);
		Safe_Release(m_pLoader);

		m_listMeshContaier.clear();
		m_vecBoneName.clear();
		Safe_Delete_Array(m_matBoneMatrix);
		Safe_Delete_Array(m_pVtxPos);
	}

	CMesh::Free();
}
