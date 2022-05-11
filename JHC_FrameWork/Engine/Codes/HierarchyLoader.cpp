#include "HierarchyLoader.h"
#include "Management.h"

USING(Engine)

CHierarchyLoader::CHierarchyLoader(_DEVICE _pDevice, const wstring& _pPath)
	: m_pDevice(_pDevice)
	, m_pPath(_pPath)
	, m_pVtxPos(nullptr)
	, m_dwVtxCount(0)
{
	m_pDevice->AddRef();
}

STDMETHODIMP CHierarchyLoader::CreateFrame(LPCSTR Name, LPD3DXFRAME * ppNewFrame)
{
	MY_FRAME* pFrame = new MY_FRAME;
	ZeroMemory(pFrame, sizeof(MY_FRAME));

	Allocate_Name(&pFrame->Name, Name);

	pFrame->CombinedTranformationMatrix = *D3DXMatrixIdentity(&pFrame->TransformationMatrix);

	*ppNewFrame = pFrame;

	return S_OK;
}

STDMETHODIMP CHierarchyLoader::CreateMeshContainer(LPCSTR Name,
													CONST D3DXMESHDATA * pMeshData,
													CONST D3DXMATERIAL * pMaterials,
													CONST D3DXEFFECTINSTANCE * pEffectInstances,
													DWORD NumMaterials, 
													CONST DWORD * pAdjacency,
													LPD3DXSKININFO pSkinInfo, 
													LPD3DXMESHCONTAINER * ppNewMeshContainer)
{
	MY_MESHCONTAINER* pMeshContainer = new MY_MESHCONTAINER;
	ZeroMemory(pMeshContainer, sizeof(MY_MESHCONTAINER));

	Allocate_Name(&pMeshContainer->Name, Name);

	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
	
	_MESH pMesh = pMeshData->pMesh;
	_ulong dwNumFaces = pMesh->GetNumFaces();	// 메쉬가 지닌 폴리곤의 개수를 반환

	pMeshContainer->pAdjacency = new _ulong[dwNumFaces * 3];	// 인접한 폴리곤 = 3
	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(_ulong) * dwNumFaces * 3);

	_ulong dwFVF = pMesh->GetFVF();

#ifndef _AFXDLL
	
#else
	// 메쉬의 노말 정보가 없는 경우 코드로 삽입
	if (!(dwFVF & D3DFVF_NORMAL))
	{
		pMesh->CloneMeshFVF(pMesh->GetOptions(), dwFVF | D3DFVF_NORMAL, m_pDevice, &pMeshContainer->MeshData.pMesh);
		D3DXComputeNormals(pMeshContainer->MeshData.pMesh, pMeshContainer->pAdjacency);
	}
	else
	{
		pMesh->CloneMeshFVF(pMesh->GetOptions(), dwFVF, m_pDevice, &pMeshContainer->MeshData.pMesh);
	}
#endif // !_AFXDLL

	// 메쉬 사이즈에 맞는 바운딩박스 생성을 위한 작업
	void* pVertices = nullptr;
	m_dwVtxCount = pMesh->GetNumVertices();	// 메쉬가 지닌 정점의 개수를 반환
	m_pVtxPos = new _float3[m_dwVtxCount];
	_long m_iTriCount = pMesh->GetNumFaces();

	pMesh->LockVertexBuffer(0, &pVertices);

	D3DVERTEXELEMENT9 tDecl[MAX_FVF_DECL_SIZE];
	ZeroMemory(tDecl, sizeof(D3DVERTEXELEMENT9) * MAX_FVF_DECL_SIZE);

	pMesh->GetDeclaration(tDecl);

	_ubyte byOffSet = 0;

	for (_ulong i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		if (tDecl[i].Usage == D3DDECLUSAGE_POSITION)
		{
			byOffSet = (_ubyte)tDecl[i].Offset;
			break;
		}
	}

	// 정점이 지닌 FVF정보를 기중으로 정점의 크기 계산
	_ulong m_dwStride = D3DXGetFVFVertexSize(dwFVF);

	for (_ulong i = 0; i < m_dwVtxCount; ++i)
	{
		m_pVtxPos[i] = *((_float3*)((_ubyte*)pVertices + (i * m_dwStride + byOffSet)));
	}

	D3DXComputeBoundingBox(m_pVtxPos, m_dwVtxCount, sizeof(_float3), &m_vMin, &m_vMax);


	pMesh->UnlockVertexBuffer();

	_ulong dwNumMaterials;
	dwNumMaterials = pMeshContainer->NumMaterials = (NumMaterials == 0 ? 1 : NumMaterials);
	pMeshContainer->pMaterials = new D3DXMATERIAL[dwNumMaterials];
	ZeroMemory(pMeshContainer->pMaterials, sizeof(D3DXMATERIAL) * dwNumMaterials);

	pMeshContainer->ppTexture = new _TEXTURE9*[dwNumMaterials];
	ZeroMemory(pMeshContainer->ppTexture, sizeof(_TEXTURE9) * dwNumMaterials);
	pMeshContainer->ppNormalTexture = new _TEXTURE9[dwNumMaterials];
	ZeroMemory(pMeshContainer->ppNormalTexture, sizeof(_TEXTURE9) * dwNumMaterials);
	pMeshContainer->ppSpacularTexture = new _TEXTURE9[dwNumMaterials];
	ZeroMemory(pMeshContainer->ppSpacularTexture, sizeof(_TEXTURE9) * dwNumMaterials);
	pMeshContainer->ppOpacityTexture = new _TEXTURE9[dwNumMaterials];
	ZeroMemory(pMeshContainer->ppOpacityTexture, sizeof(_TEXTURE9) * dwNumMaterials);
	pMeshContainer->ppRoughnessTexture = new _TEXTURE9[dwNumMaterials];
	ZeroMemory(pMeshContainer->ppRoughnessTexture, sizeof(_TEXTURE9) * dwNumMaterials);
	pMeshContainer->ppEmissiveTexture = new _TEXTURE9*[dwNumMaterials];
	ZeroMemory(pMeshContainer->ppEmissiveTexture, sizeof(_TEXTURE9) * dwNumMaterials);

	pMeshContainer->iTextureCount = new _uint[dwNumMaterials];
	pMeshContainer->iEmissiveTextureCount = new _uint[dwNumMaterials];

	if (0 != NumMaterials)
	{
		memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * dwNumMaterials);
	
		for (_ulong iMtrIndex = 0; iMtrIndex < dwNumMaterials; ++iMtrIndex)
		{
			_tchar szFullPath[MAX_PATH] = L"";
			_tchar szFullPathDymmy[MAX_PATH] = L"";
			_tchar szFileName[MAX_PATH] = L"";
			_tchar szFileNameDummy[MAX_PATH] = L"";

			lstrcpy(szFullPath, m_pPath.c_str());

			MultiByteToWideChar
			(
				CP_ACP,
				0,
				pMeshContainer->pMaterials[iMtrIndex].pTextureFilename,
				(_uint)strlen(pMeshContainer->pMaterials[iMtrIndex].pTextureFilename),
				szFileName,
				MAX_PATH
			);

			vector<wstring> vecFileName;
			pMeshContainer->iTextureCount[iMtrIndex] = 0;
			pMeshContainer->iEmissiveTextureCount[iMtrIndex] = 0;

#pragma region Diffuse
			while (true)
			{
				lstrcpy(szFullPathDymmy, szFullPath);

				vector<wstring> tok;
				SplitW(L'_', szFileName, &tok);

				_uint count = lstrlen(tok[1].c_str());
				tok[1].erase(count - 1, count);

				wstring wstrPath = tok[0] + L"_" + tok[1] + to_wstring(pMeshContainer->iTextureCount[iMtrIndex]) + L"_" + tok[2];

				lstrcpy(szFileNameDummy, wstrPath.c_str());
				lstrcat(szFullPathDymmy, szFileNameDummy);

				if (FileFind(szFullPathDymmy))
				{
					vecFileName.push_back(szFileNameDummy);
					++pMeshContainer->iTextureCount[iMtrIndex];
				}
				else
				{
					pMeshContainer->ppTexture[iMtrIndex] = new _TEXTURE9[pMeshContainer->iTextureCount[iMtrIndex]];
					ZeroMemory(pMeshContainer->ppTexture[iMtrIndex], sizeof(_TEXTURE9)*pMeshContainer->iTextureCount[iMtrIndex]);
					break;
				}
			}

			if (pMeshContainer->iTextureCount[iMtrIndex] > 0)
			{
				for (_uint j = 0; j < pMeshContainer->iTextureCount[iMtrIndex]; ++j)
				{
					lstrcpy(szFullPathDymmy, szFullPath);
					lstrcat(szFullPathDymmy, vecFileName[j].c_str());
					pMeshContainer->ppTexture[iMtrIndex][j] = Get_Manager()->Find_MeshTexture(szFullPathDymmy, vecFileName[j]);
					/*if (FAILED(D3DXCreateTextureFromFile(m_pDevice, szFullPathDymmy, &pMeshContainer->ppTexture[iMtrIndex][j])))
					{
						TCHAR szBuff[MAX_PATH] = L"";
						swprintf_s(szBuff, L"Failed to %s\n", szFullPathDymmy);
						ERROR_BOX(szBuff);
						return E_FAIL;
					}*/
				}
			}
			else
				pMeshContainer->ppTexture[iMtrIndex][0] = nullptr;

#pragma endregion

			_uint count = lstrlen(vecFileName[0].c_str());

#pragma region Normal
			wcsncpy_s(szFileNameDummy, vecFileName[0].c_str(), count - 5);
			lstrcat(szFileNameDummy, L"N.png");
			lstrcpy(szFullPathDymmy, szFullPath);
			lstrcat(szFullPathDymmy, szFileNameDummy);
			pMeshContainer->ppNormalTexture[iMtrIndex] = Get_Manager()->Find_MeshTexture(szFullPathDymmy, szFileNameDummy);
			//D3DXCreateTextureFromFile(m_pDevice, szFullPathDymmy, &pMeshContainer->ppNormalTexture[iMtrIndex]);
#pragma endregion

#pragma region Spacular
			wcsncpy_s(szFileNameDummy, vecFileName[0].c_str(), count - 5);
			lstrcat(szFileNameDummy, L"S.png");
			lstrcpy(szFullPathDymmy, szFullPath);
			lstrcat(szFullPathDymmy, szFileNameDummy);
			pMeshContainer->ppSpacularTexture[iMtrIndex] = Get_Manager()->Find_MeshTexture(szFullPathDymmy, szFileNameDummy);
			//D3DXCreateTextureFromFile(m_pDevice, szFullPathDymmy, &pMeshContainer->ppSpacularTexture[iMtrIndex]);
#pragma endregion

#pragma region Opacity
			wcsncpy_s(szFileNameDummy, vecFileName[0].c_str(), count - 5);
			lstrcat(szFileNameDummy, L"O.png");
			lstrcpy(szFullPathDymmy, szFullPath);
			lstrcat(szFullPathDymmy, szFileNameDummy);
			pMeshContainer->ppOpacityTexture[iMtrIndex] = Get_Manager()->Find_MeshTexture(szFullPathDymmy, szFileNameDummy);
			//D3DXCreateTextureFromFile(m_pDevice, szFullPathDymmy, &pMeshContainer->ppOpacityTexture[iMtrIndex]);
#pragma endregion

#pragma region Roughness
			wcsncpy_s(szFileNameDummy, vecFileName[0].c_str(), count - 5);
			lstrcat(szFileNameDummy, L"R.png");
			lstrcpy(szFullPathDymmy, szFullPath);
			lstrcat(szFullPathDymmy, szFileNameDummy);
			pMeshContainer->ppRoughnessTexture[iMtrIndex] = Get_Manager()->Find_MeshTexture(szFullPathDymmy, szFileNameDummy);
			//D3DXCreateTextureFromFile(m_pDevice, szFullPathDymmy, &pMeshContainer->ppRoughnessTexture[iMtrIndex]);
#pragma endregion

#pragma region Emissive
			vecFileName.clear();

			while (true)
			{
				vector<wstring> tok;				
				wcsncpy_s(szFileNameDummy, szFileName, count - 5);
				lstrcat(szFileNameDummy, L"E.png");
				lstrcpy(szFullPathDymmy, szFullPath);
				lstrcat(szFullPathDymmy, szFileNameDummy);
				SplitW(L'_', szFileNameDummy, &tok);

				_uint count = lstrlen(tok[1].c_str());
				tok[1].erase(count - 1, count);

				wstring wstrPath = tok[0] + L"_" + tok[1] + to_wstring(pMeshContainer->iEmissiveTextureCount[iMtrIndex]) + L"_" + tok[2];

				lstrcpy(szFileNameDummy, wstrPath.c_str());
				lstrcpy(szFullPathDymmy, szFullPath);
				lstrcat(szFullPathDymmy, szFileNameDummy);

				if (FileFind(szFullPathDymmy))
				{
					vecFileName.push_back(szFileNameDummy);
					++pMeshContainer->iEmissiveTextureCount[iMtrIndex];
				}
				else
				{
					if (pMeshContainer->iEmissiveTextureCount[iMtrIndex] == 0)
						pMeshContainer->iEmissiveTextureCount[iMtrIndex] = 1;

					pMeshContainer->ppEmissiveTexture[iMtrIndex] = new _TEXTURE9[pMeshContainer->iEmissiveTextureCount[iMtrIndex]];
					ZeroMemory(pMeshContainer->ppEmissiveTexture[iMtrIndex], sizeof(_TEXTURE9)*pMeshContainer->iEmissiveTextureCount[iMtrIndex]);
					break;
				}
			}

			if (0 == vecFileName.size())
			{
				pMeshContainer->ppEmissiveTexture[iMtrIndex][0] = nullptr;
			}
			else
			{
				for (_uint j = 0; j < pMeshContainer->iEmissiveTextureCount[iMtrIndex]; ++j)
				{
					lstrcpy(szFullPathDymmy, szFullPath);
					lstrcat(szFullPathDymmy, vecFileName[j].c_str());
					pMeshContainer->ppEmissiveTexture[iMtrIndex][j] = Get_Manager()->Find_MeshTexture(szFullPathDymmy, vecFileName[j]);
					/*if (FAILED(D3DXCreateTextureFromFile(m_pDevice, szFullPathDymmy, &pMeshContainer->ppEmissiveTexture[iMtrIndex][j])))
					{
						TCHAR szBuff[MAX_PATH] = L"";
						swprintf_s(szBuff, L"Failed to %s\n", szFullPathDymmy);
						ERROR_BOX(szBuff);
					}*/
				}
			}
#pragma endregion

		}
	}
	else
	{
		pMeshContainer->pMaterials[0].MatD3D.Diffuse = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
		pMeshContainer->pMaterials[0].MatD3D.Specular = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
		pMeshContainer->pMaterials[0].MatD3D.Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
		pMeshContainer->pMaterials[0].MatD3D.Emissive = D3DXCOLOR(0.f, 0.f, 0.f, 0.f);
		pMeshContainer->pMaterials[0].MatD3D.Power = 0.f;

		pMeshContainer->ppTexture[0] = nullptr;
	}

	if (nullptr == pSkinInfo)
		return S_OK;

	pMeshContainer->pSkinInfo = pSkinInfo;
	pMeshContainer->pSkinInfo->AddRef();

#ifndef _AFXDLL
	if (FAILED(pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh
	(
		pMeshData->pMesh,
		D3DXMESH_MANAGED | D3DXMESHOPT_VERTEXCACHE,
		70,
		pMeshContainer->pAdjacency,
		NULL,
		NULL,
		NULL,
		&pMeshContainer->dwMaxVertinfluences,
		&pMeshContainer->dwBoneComboEntries,
		&pMeshContainer->pFrameCombinationBuf,
		&pMeshContainer->MeshData.pMesh
	)))
	{
		BREAK_POINT;
	}
	/*LPD3DXBONECOMBINATION pBoneComb = (LPD3DXBONECOMBINATION)pMeshContainer->pFrameCombinationBuf->GetBufferPointer();

	for (_ulong i = 0; i < pMeshContainer->dwBoneComboEntries; ++i)
	{
		for (_uint iPaletteEntry = 0; iPaletteEntry < 64; ++iPaletteEntry)
		{
			DWORD iMatrixindex = pBoneComb[i].BoneId[iPaletteEntry];*/

	/*for (pMeshContainer->dwAttributeSW = 0; pMeshContainer->dwAttributeSW < pMeshContainer->dwBoneComboEntries; ++pMeshContainer->dwAttributeSW)
	{
		DWORD cInfl = 0;
		LPD3DXBONECOMBINATION pBoneComb = (LPD3DXBONECOMBINATION)pMeshContainer->pFrameCombinationBuf->GetBufferPointer();
		for (DWORD i = 0; i < pMeshContainer->dwMaxVertinfluences; ++i)
		{
			if (pBoneComb[pMeshContainer->dwAttributeSW].BoneId[i] != UINT_MAX)
			{
				++cInfl;
			}
		}
	}

	if (pMeshContainer->dwAttributeSW < pMeshContainer->dwBoneComboEntries)
	{
		pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), dwFVF, m_pDevice, &pMeshContainer->pOriMesh);
		pMeshContainer->UseHardwareVP = false;
	}
	else
	{
		pMeshContainer->UseHardwareVP = true;
	}*/

#else
	pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), dwFVF, m_pDevice, &pMeshContainer->pOriMesh);
#endif // !_AFXDLL	

	// 메쉬 컨테이너에 영향을 미치는 뼈의 개수를 반환
	_ulong dwNumBones;
	dwNumBones = pMeshContainer->dwNumBones = pMeshContainer->pSkinInfo->GetNumBones();
	
	pMeshContainer->pFrameOffSetMatrix = new _float4x4[dwNumBones];
	ZeroMemory(pMeshContainer->pFrameOffSetMatrix, sizeof(_float4x4) * dwNumBones);

	pMeshContainer->ppFrameCombineMatrix = new _float4x4*[dwNumBones];
	ZeroMemory(pMeshContainer->ppFrameCombineMatrix, sizeof(_float4x4*) * dwNumBones);

	pMeshContainer->pRenderingMatrix = new _float4x4[dwNumBones];
	ZeroMemory(pMeshContainer->pRenderingMatrix, sizeof(_float4x4) * dwNumBones);
	
	for (_ulong i = 0; i < dwNumBones; ++i)
		pMeshContainer->pFrameOffSetMatrix[i] = *pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i);

	*ppNewMeshContainer = pMeshContainer;
	
	return S_OK;
}

STDMETHODIMP CHierarchyLoader::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	Safe_Delete_Array(pFrameToFree->Name);

	if (nullptr != pFrameToFree->pMeshContainer)
		DestroyMeshContainer(pFrameToFree->pMeshContainer);

	if (nullptr != pFrameToFree->pFrameSibling)
		DestroyFrame(pFrameToFree->pFrameSibling);

	if (nullptr != pFrameToFree->pFrameFirstChild)
		DestroyFrame(pFrameToFree->pFrameFirstChild);

	Safe_Delete(pFrameToFree);

	return S_OK;
}

STDMETHODIMP CHierarchyLoader::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	MY_MESHCONTAINER* pMeshContainer = (MY_MESHCONTAINER*)pMeshContainerToFree;

	_ulong dwNumMaterials = pMeshContainer->NumMaterials;

	for (_ulong i = 0; i < dwNumMaterials; ++i)
	{
		for (_uint j = 0; j < pMeshContainer->iTextureCount[i]; ++j)
		{
			Safe_Delete(pMeshContainer->ppTexture[i]);
		}

		Safe_Release(pMeshContainer->ppNormalTexture[i]);
		Safe_Release(pMeshContainer->ppSpacularTexture[i]);
		Safe_Release(pMeshContainer->ppOpacityTexture[i]);
		Safe_Release(pMeshContainer->ppRoughnessTexture[i]);

		for (_uint j = 0; j < pMeshContainer->iEmissiveTextureCount[i]; ++j)
		{
			Safe_Delete(pMeshContainer->ppEmissiveTexture[i]);
		}
	}
		
	Safe_Delete_Array(pMeshContainer->iTextureCount);
	Safe_Delete_Array(pMeshContainer->iEmissiveTextureCount);
	Safe_Delete_Array(pMeshContainer->ppTexture);
	Safe_Delete_Array(pMeshContainer->ppNormalTexture);
	Safe_Delete_Array(pMeshContainer->ppSpacularTexture);
	Safe_Delete_Array(pMeshContainer->ppOpacityTexture);
	Safe_Delete_Array(pMeshContainer->ppRoughnessTexture);
	Safe_Delete_Array(pMeshContainer->ppEmissiveTexture);
	Safe_Delete_Array(pMeshContainer->pMaterials);
	Safe_Delete_Array(pMeshContainer->pAdjacency);

	Safe_Release(pMeshContainer->MeshData.pMesh);
	Safe_Release(pMeshContainer->pOriMesh);
	Safe_Release(pMeshContainer->pSkinInfo);
	Safe_Release(pMeshContainer->pFrameCombinationBuf);

	Safe_Delete_Array(pMeshContainer->Name);
	Safe_Delete_Array(pMeshContainer->pFrameOffSetMatrix);
	Safe_Delete_Array(pMeshContainer->ppFrameCombineMatrix);
	Safe_Delete_Array(pMeshContainer->pRenderingMatrix);

	Safe_Delete(pMeshContainer);

	return S_OK;
}

void CHierarchyLoader::Allocate_Name(char ** _ppName, const char * _pFrameName)
{
	if (nullptr == _pFrameName)
		return;

	_uint iLength = (_uint)strlen(_pFrameName);

	*_ppName = new char[iLength + 1];
	strcpy_s(*_ppName, iLength + 1, _pFrameName);
}

CHierarchyLoader * CHierarchyLoader::Create(_DEVICE _pDevice, const wstring& _pPath)
{
	return new CHierarchyLoader(_pDevice, _pPath);
}

_ulong CHierarchyLoader::Release()
{
	Safe_Release(m_pDevice);
	delete this;

	return 0;
}
