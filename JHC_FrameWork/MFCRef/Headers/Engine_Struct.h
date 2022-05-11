#pragma once

#ifndef __ENGINE_STRUCT_H__
#define __ENGINE_STRUCT_H__

#pragma region Vertex
typedef struct tagVertex
{
	_float3		vPos;

}VTX;

typedef struct tagVertexColor : public tagVertex
{
	_uint		iColor;

}VTX_COLOR;

const _uint VTX_COLOR_FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;

typedef struct tagVertexTexture : public tagVertex
{
	_float2		vUV;

}VTX_TEXTURE;

const _uint VTX_TEXTURE_FVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);	//D3DFVF_TEXCOORDSIZE2 ����Ʈ��

typedef struct tagVertexTextureColor : public tagVertex
{
	_uint		iColor;
	_float2		vUV;

}VTX_TEXTURE_COLOR;

const _uint VTX_TEXTURE_COLOR_FVF = D3DFVF_TEX0 | D3DFVF_XYZ | D3DFVF_DIFFUSE |  D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0);	//D3DFVF_TEXCOORDSIZE2 ����Ʈ��




typedef struct tagVertexCubeTexture : public tagVertex
{
	_float3		vUV;

}VTX_CUBETEXTURE;

const _uint VTX_CUBETEXTURE_FVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);

typedef struct tagVertexScreen
{
	_float4		vPos;
	_float2		vUV;
}VTX_SCREEN;

const _uint VTX_SCREEN_FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;
#pragma endregion

#pragma region Index
typedef struct tagIndex16
{
	WORD _1, _2, _3;

}INDEX16;

typedef struct tagIndex32
{
	DWORD _1, _2, _3;

}INDEX32;
#pragma endregion

#pragma region Line
typedef struct tagLineDesc
{
	tagLineDesc()
	{
		ZeroMemory(this, sizeof(tagLineDesc));
	}

	_float3		vStart, vEnd;
	D3DCOLOR	Color;

}LINE_DESC;
#pragma endregion

#pragma region Transform
typedef struct tagTransformDesc
{
	tagTransformDesc()
	{
		ZeroMemory(this, sizeof(tagTransformDesc));
		vScale = _float3(1.f, 1.f, 1.f);
		ZeroMemory(matRotate, sizeof(_float4x4));
	}
	_float3		vScale, vRotate, vPos, vRev, vParent;
	_float4x4	matWorld, matRotate;
	_float		fSpeedPerSecond, fRotatePerSecond, fRevPerSecond;
	TCHAR		szFileTitle[MAX_PATH];		// FileTitle

}TRANSFORM_DESC;
#pragma endregion

#pragma region ItemTransform
typedef struct tagItemTransformDesc : public tagTransformDesc
{
	tagItemTransformDesc()
	{
		ZeroMemory(this, sizeof(tagItemTransformDesc));
	}

	eItemCode			ItemID;

}ITEMTRANSFORM_DESC;
#pragma endregion

#pragma region ResourceInfo
typedef struct tagResourceInfoDesc
{
	tagResourceInfoDesc()
	{
		ZeroMemory(this, sizeof(tagResourceInfoDesc));
	}

	TCHAR		szFilePath[MAX_PATH];		// FilePath	
	TCHAR		szFileName[MAX_PATH];		// FileName	
	TCHAR		szFileTitle[MAX_PATH];		// FileTitle	

	bool Find(const wstring& _wstrFilePullPath)
	{
		wstring wstrFilePullPath = szFilePath;
		wstrFilePullPath += szFileName;
		if (!lstrcmpi(wstrFilePullPath.c_str(), _wstrFilePullPath.c_str()))
		{
			return true;
		}

		return false;
	}

}RESOURCEINFO_DESC;
#pragma endregion

#pragma region ObjectInfo
typedef struct tagObjectInfoDesc
{
	tagObjectInfoDesc()
	{
		ZeroMemory(this, sizeof(tagObjectInfoDesc));
		vScale = _float3(1.f, 1.f, 1.f);
	}

	_float3		vScale, vRotate, vPos;
	EMeshType	eFileType;					// Static or Dynamic
	TCHAR		szFileTitle[MAX_PATH];		// FileTitle	

}OBJECTINFO_DESC;
#pragma endregion

#pragma region Collider
typedef struct tagCubeColliderDesc
{
	tagCubeColliderDesc()
	{
		ZeroMemory(this, sizeof(tagCubeColliderDesc));
	}

	_ulong		dwVtxCount;
	_float3		vPos;
	_float4x4	matWorld;
	_float4x4	matParentWorld;

}CUBECOLLIDER_DESC;

typedef struct tagSphereColliderDesc
{
	tagSphereColliderDesc()
	{
		ZeroMemory(this, sizeof(tagSphereColliderDesc));
		vScale = _float3(1.f, 1.f, 1.f);
	}

	_float3		vScale, vPos;
	_float4x4	matWorld;
	TCHAR		szBoneName[MAX_PATH];

}SPHERECOLLIDER_DESC;
#pragma endregion

#pragma region Camera
typedef struct tagCameraDesc
{
	tagCameraDesc()
	{
		ZeroMemory(this, sizeof(tagCameraDesc));
	}

	_float3		vEye, vAt, vUp;
	_float4x4	matView;
	_float4x4	matProj;
	_float		fFovY, fAspect, fNear, fFar;

} CAMERA_DESC;
#pragma endregion

#pragma region XYZLine
typedef struct tagXYZLine
{
	tagXYZLine()
	{
		ZeroMemory(this, sizeof(tagXYZLine));
		vScale = _float3(1.f, 1.f, 1.f);
	}

	_float3		vScale, vPos;
	_float4x4	matWorld;
	_float4x4	matParentWorld;

}XYZLINE_DESC;
#pragma endregion

#pragma region Mesh
typedef struct D3DFRAME_DERIVED : public D3DXFRAME
{
	_float4x4		CombinedTranformationMatrix;	// �θ� ���� ������ ���� ��� ���� �����ϱ� ���� ���
}MY_FRAME;

typedef struct D3DXMESHCONTAINER_DERIVED : public D3DXMESHCONTAINER, public D3DXBONECOMBINATION
{
	D3DXMESHCONTAINER_DERIVED()
	{
		ZeroMemory(this, sizeof(D3DXMESHCONTAINER_DERIVED));
	}
	_uint*			iTextureCount;
	_uint*			iEmissiveTextureCount;
	_TEXTURE9**		ppTexture;
	_TEXTURE9*		ppNormalTexture;
	_TEXTURE9*		ppSpacularTexture; // 1
	_TEXTURE9**		ppEmissiveTexture;
	_TEXTURE9*		ppOpacityTexture; // 1
	_TEXTURE9*		ppRoughnessTexture; // 1
	_MESH			pOriMesh;
	_ulong			dwNumBones;				// �޽��� ���� ��� ���� ���� ����
	_float4x4*		pFrameOffSetMatrix;		// �ִϸ��̼� ������ ������ ������ ��� ����
	_float4x4**		ppFrameCombineMatrix;	// �� ����ü�� ���� �ִ� CombinedTransformationMatrix�� �ּҰ����� �����ϴ� ������
	_float4x4*		pRenderingMatrix;		// ���������� ����� ���� ���, pFrameOffsetMatrix * (*ppFrameCombinedMatrix)�� �����
	// �ϵ���� ��Ű��
	LPD3DXBUFFER	pFrameCombinationBuf;
	DWORD			dwMaxVertinfluences;
	DWORD			dwBoneComboEntries;		// ��� �ȷ�Ʈ ��Ʈ��
	//_bool			UseHardwareVP;			// �ϵ���� ��Ű�� ���
	//DWORD			dwAttributeSW;			// ����Ʈ���� ��Ű�װ� �ϵ���� ��Ű���� �����ϱ� ���� ���� ����
} MY_MESHCONTAINER;
#pragma endregion

#pragma region ��ŷ ���� ����ü.
typedef struct tagMeshPickingVertex
{
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	FLOAT tu, tv;

}MESH_PICKING_VERTEX;
#pragma endregion

#pragma region Particle.
typedef struct tagARGB
{
	tagARGB() { ZeroMemory(this, sizeof(tagARGB)); };
	tagARGB(_ubyte _ubyA, _ubyte _ubyR, _ubyte _ubyG, _ubyte _ubyB) :ubyA(_ubyA), ubyR(_ubyR), ubyG(_ubyG), ubyB(_ubyB) {};
	_ubyte	ubyA;
	_ubyte	ubyR;
	_ubyte	ubyG;
	_ubyte	ubyB;
}_ARGB;

typedef struct tagParticleDesc
{
	tagParticleDesc()
		:
		vPos{ 0.f, 0.f, 0.f },
		vScale{ 0.f, 0.f, 0.f },
		vRotateAngle{ 0.f, 0.f, 0.f },
		vRevRotateAngle{ 0.f, 0.f, 0.f },
		vArgb{ 0.f,0.f,0.f,0.f },
		fMoveSpeed(0.f)
	{
	}
	_float4		vArgb;
	_float3		vPos;
	_float3		vTargetPos;
	_float3		vScale;
	_float3		vRotateAngle;
	_float3		vRevRotateAngle;

	_float		fMoveSpeed;
}PARTICLEDESC, PARTICLE_DESC;


typedef struct tagParticlePatternDesc
{
	tagParticlePatternDesc()
		:
		tParticleInfo(),
		pTransform(nullptr),
		fThisAccTime(0.f), fDeltaTime(0.f),
		ePatternType(ePartilcePatternType::End)
	{
	}
	PARTICLEDESC tParticleInfo;
	_float			fThisAccTime;
	_float			fDeltaTime;
	_float			fLifeTime;
	_float3*		pTargetPos;
	ePartilcePatternType	ePatternType;
	void*	 pTransform;
	_DEVICE			pDevice;
}PARTICLEPATTERNDESC, PARTICLE_PATTERN_DESC;
#pragma endregion

#pragma region State

typedef struct tagStatDesc
{
	_float	fMaxHP;
	_float	fHP;
	_float	fAtt;
	_float	fDef;
	_float	fMaxStamina;
	_float	fStamina;
	_float	fMaxPoise;
	_float	fPoise;
}STAT_DESC;

#pragma endregion

#pragma region TRAIL_EFFECT

typedef struct tagTrailDesc
{
	tagTrailDesc() { ZeroMemory(this, sizeof(tagTrailDesc)); }
	_float4x4		matLocal;			//�������
	_float3		 vColorRGB;				//�÷�
	_float        fTrailWidth;			//Ʈ���� �β�
	_float		fTrailChekRate;			//Ʈ���� ��� ���� �ð�
	void*		pObj;					//�θ� ������Ʈ
	_uint		iRenderID;				//Ʈ���� ������� �ؽ���id
	_tchar		szTransformComponentName[MAX_PATH];
	_bool		bRenderOnOff;
}TRAIL_DESC;

typedef struct tagTrailNode
{
	_float3 vPos;
	_float4 vColor;
}TRAIL_NODE;

#pragma endregion TRAIL_EFFECT


#endif // !__ENGINE_STRUCT_H__
