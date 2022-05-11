#pragma once
#ifndef __PARTICLEPATTERN_H__
#define __PARTICLEPATTERN_H__

//#include "Base.h"
#include "Particle_Base.h"
#include "Transform.h"
USING(Engine)


class CPaticlePattern
{
public:
	explicit CPaticlePattern(void) {}
	~CPaticlePattern(void) {}
public: // operator	
	//template <typename T>
	/*void operator () (PARTICLEPATTERNDESC* _pDesc, ePartilcePatternType _eType)
	{
		switch (_eType)
		{
		case ePartilcePatternType::NoAction:
			NoAction(nullptr);
			break;
		case ePartilcePatternType::Blink:
			Blink(_pDesc);
			break;
		case ePartilcePatternType::GoStraight:
			GoStraight(_pDesc);
			break;
		default:
			NoAction(nullptr);
			break;
		}
	}*/
	//template <typename T>
	//void operator () (T* _tPatternDesc)
	void operator () (PARTICLEPATTERNDESC* _tPatternDesc)
	{
		if (!_tPatternDesc)
			return; 
		((CTransform*)(_tPatternDesc->pTransform))->Set_Pos(_tPatternDesc->tParticleInfo.vPos);
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::NoAction)
		{
			NoAction(_tPatternDesc);
			return;
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::x2)
		{
			_tPatternDesc->fThisAccTime *= 2.f;
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::x3)
		{
			_tPatternDesc->fThisAccTime *= 3.f;
		}

		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::x4)
		{
			_tPatternDesc->fThisAccTime *= 4.f;
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::x5)
		{
			_tPatternDesc->fThisAccTime *= 5.f;
		}
		
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::OffStartBlink)
		{
			OffBlink(_tPatternDesc);
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::OnStartBlink)
		{
			OnBlink(_tPatternDesc);
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::OffStartBlinkPow)
		{
			OffBlinkPow(_tPatternDesc);
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::OnStartBlinkPow)
		{
			OnBlinkPow(_tPatternDesc);
		}
		
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::Gravity)
		{
			Gravity(_tPatternDesc);
		}
		((CTransform*)(_tPatternDesc->pTransform))->Update_Transform();
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::GoStraight)
		{
			GoStraight(_tPatternDesc);
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::GoStraightToTarget)
		{
			GoStraightToTarget(_tPatternDesc);
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::GoUp)
		{
			GoUp(_tPatternDesc);
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::GoUpToTarget)
		{
			GoUpToTarget(_tPatternDesc);
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::GoRight)
		{
			NoAction(nullptr);
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::GoRightpToTarget)
		{
			NoAction(nullptr);
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::ScaleUp)
		{
			ScaleUp(_tPatternDesc);
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::RotateX)
		{
			RotateX(_tPatternDesc);
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::RotateY)
		{
			RotateY(_tPatternDesc);
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::RotateZ)
		{
			RotateZ(_tPatternDesc);
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::RevRotX)
		{
			RevRotateX(_tPatternDesc);
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::RevRotY)
		{
			RevRotateY(_tPatternDesc);
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::RevRotZ)
		{
			RevRotateZ(_tPatternDesc);
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::BillBoard)
		{
			Billboarding(_tPatternDesc);
		}
		if ((_ulong)_tPatternDesc->ePatternType & (_ulong)ePartilcePatternType::YBillBoard)
		{
			YBillboarding(_tPatternDesc);
		}
		
	}
private:
	void NoAction(PARTICLEPATTERNDESC* _pDesc)
	{
		if (!_pDesc)
			return;
		((CTransform*)(_pDesc->pTransform))->Set_Pos(_pDesc->tParticleInfo.vPos);
		/*((CTransform*)(_pDesc->pTransform))->Update_Transform();*/
	}
	void OffBlink(PARTICLEPATTERNDESC* _pDesc)
	{			
		_pDesc->tParticleInfo.vArgb.w = sinf(_pDesc->fThisAccTime - 0.5f)*2.f*0.5f;
		((CTransform*)(_pDesc->pTransform))->Set_Pos(_pDesc->tParticleInfo.vPos);
	}
	void OnBlink(PARTICLEPATTERNDESC* _pDesc)
	{
		_pDesc->tParticleInfo.vArgb.w = sinf(_pDesc->fThisAccTime)*2.f*0.5f;
		((CTransform*)(_pDesc->pTransform))->Set_Pos(_pDesc->tParticleInfo.vPos);
	}
	void OffBlinkPow(PARTICLEPATTERNDESC* _pDesc)
	{
 		_pDesc->tParticleInfo.vArgb.w = sinf(_pDesc->fThisAccTime-0.5f)*2.f*0.5f;
		_pDesc->tParticleInfo.vArgb.w -= sinf(_pDesc->fThisAccTime)*pow(0.5f, _pDesc->fThisAccTime);
		((CTransform*)(_pDesc->pTransform))->Set_Pos(_pDesc->tParticleInfo.vPos);
	}
	void OnBlinkPow(PARTICLEPATTERNDESC* _pDesc)
	{
		_pDesc->tParticleInfo.vArgb.w = sinf(_pDesc->fThisAccTime)*2.f*0.5f;
		_pDesc->tParticleInfo.vArgb.w -= sinf(_pDesc->fThisAccTime)*pow(0.5f, _pDesc->fThisAccTime);
		((CTransform*)(_pDesc->pTransform))->Set_Pos(_pDesc->tParticleInfo.vPos);
	}
	//void Decal(PARTICLEPATTERNDESC* _pDesc)
	//{
	//	_pDesc->tParticleInfo.vArgb.w = sinf(_pDesc->fThisAccTime)*2.f*0.5f;
	//	//_pDesc->tParticleInfo.vArgb.w -= sinf(_pDesc->fThisAccTime)*pow(0.5f, _pDesc->fThisAccTime);
	//	((CTransform*)(_pDesc->pTransform))->Set_Pos(_pDesc->tParticleInfo.vPos);

	//	_pDesc->tParticleInfo.vScale.x += ((sinf(_pDesc->fThisAccTime) + 1.f)*0.1f);
	//	_pDesc->tParticleInfo.vScale.y += ((sinf(_pDesc->fThisAccTime) + 1.f)*0.1f);
	//	_pDesc->tParticleInfo.vScale.z += ((sinf(_pDesc->fThisAccTime) + 1.f)*0.1f);
	//	((CTransform*)(_pDesc->pTransform))->Set_Scale(_pDesc->tParticleInfo.vScale);
	//	((CTransform*)(_pDesc->pTransform))->Go_Straight(((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().fSpeedPerSecond, _pDesc->fDeltaTime);

	//}
	void ScaleUp(PARTICLEPATTERNDESC* _pDesc)
	{	
		//_pDesc->tParticleInfo.vScale.x += ((sinf(_pDesc->fThisAccTime) + 1.f)*0.5f);
		//_pDesc->tParticleInfo.vScale.y += ((sinf(_pDesc->fThisAccTime) + 1.f)*0.5f);
		//_pDesc->tParticleInfo.vScale.z += ((sinf(_pDesc->fThisAccTime) + 1.f)*0.5f);
		((CTransform*)(_pDesc->pTransform))->Set_Scale(_pDesc->tParticleInfo.vScale* ((cosf(_pDesc->fThisAccTime) + 2.f )));
	}
	void GoStraight(PARTICLEPATTERNDESC* _pDesc)
	{		
		//((CTransform*)(_pDesc->pTransform))->Set_Pos(_pDesc->tParticleInfo.vPos);
		((CTransform*)(_pDesc->pTransform))->Go_Straight(_pDesc->tParticleInfo.fMoveSpeed, _pDesc->fDeltaTime);

	}
	void GoStraightToTarget(PARTICLEPATTERNDESC* _pDesc)
	{
		if (_pDesc->fThisAccTime < _pDesc->fLifeTime / 2.f)
		{
			//_pDesc->tParticleInfo.vArgb.w = sinf(_pDesc->fThisAccTime)*2.f*0.5f;
			//((CTransform*)(_pDesc->pTransform))->Set_Pos(_pDesc->tParticleInfo.vPos);
			((CTransform*)(_pDesc->pTransform))->Go_Straight(_pDesc->tParticleInfo.fMoveSpeed, _pDesc->fDeltaTime);
		}
		else
		{
			//_pDesc->tParticleInfo.vArgb.w -= _pDesc->fDeltaTime;
			_float3 vPos,vLookCur;
			((CTransform*)(_pDesc->pTransform))->Get_Info(EInfo::Pos, &vPos);
			((CTransform*)(_pDesc->pTransform))->Get_Info(EInfo::Look, &vLookCur);
			_float3 vHalfLook = (*(_pDesc->pTargetPos) - (vLookCur+ vPos));
		
			const _float3 vHalfLookNormal= *D3DXVec3Normalize(&vHalfLook, &vHalfLook);
			const _float fSpeed = _pDesc->fDeltaTime*_pDesc->tParticleInfo.fMoveSpeed;
			const _float3 vToDir = vHalfLookNormal * fSpeed;
			const _float3 vOut = vLookCur + vToDir;
			//_float3 vOutNormal;
	
			//D3DXVec3Normalize(&vOutNormal, &vOut);
			((CTransform*)(_pDesc->pTransform))->Set_Info(EInfo::Look, vOut);
		
			((CTransform*)(_pDesc->pTransform))->Go_Straight(_pDesc->tParticleInfo.fMoveSpeed, _pDesc->fDeltaTime);
		}
	
	}
	void GoUp(PARTICLEPATTERNDESC* _pDesc)
	{
		//_float fSinValue = sinf(_pDesc->fThisAccTime * _pDesc->fLifeTime);
		//_pDesc->tParticleInfo.vArgb.w = fSinValue*2.f*0.5f;
		//((CTransform*)(_pDesc->pTransform))->Set_Scale(_pDesc->tParticleInfo.vScale* fSinValue);
		//((CTransform*)(_pDesc->pTransform))->Set_Pos(_pDesc->tParticleInfo.vPos);
		//((CTransform*)(_pDesc->pTransform))->Go_Up(((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().fSpeedPerSecond, _pDesc->fDeltaTime);
		((CTransform*)(_pDesc->pTransform))->Go_Up(((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().fSpeedPerSecond, _pDesc->fDeltaTime);

	}
	void GoUpToTarget(PARTICLEPATTERNDESC* _pDesc)
	{
		if (_pDesc->fThisAccTime < _pDesc->fLifeTime / 2.f)
		{
		//	_pDesc->tParticleInfo.vArgb.w = sinf(_pDesc->fThisAccTime)*2.f*0.5f;
		//	((CTransform*)(_pDesc->pTransform))->Set_Pos(_pDesc->tParticleInfo.vPos);
			((CTransform*)(_pDesc->pTransform))->Go_Up(((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().fSpeedPerSecond, _pDesc->fDeltaTime);
		}
		else
		{
			_float3 vPos, vLookCur;
			((CTransform*)(_pDesc->pTransform))->Get_Info(EInfo::Pos, &vPos);
			((CTransform*)(_pDesc->pTransform))->Get_Info(EInfo::Up, &vLookCur);
			_float3 vHalfLook = (*(_pDesc->pTargetPos) - (vLookCur + vPos));

			const _float3 vHalfLookNormal = *D3DXVec3Normalize(&vHalfLook, &vHalfLook);
			const _float fSpeed = _pDesc->fDeltaTime*_pDesc->tParticleInfo.fMoveSpeed;
			const _float3 vToDir = vHalfLookNormal * fSpeed;
			const _float3 vOut = vLookCur + vToDir;
			((CTransform*)(_pDesc->pTransform))->Set_Info(EInfo::Up, vOut);

			((CTransform*)(_pDesc->pTransform))->Go_Up(((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().fSpeedPerSecond, _pDesc->fDeltaTime);
		}

	}
	void GoRight(PARTICLEPATTERNDESC* _pDesc)
	{
		//_float fSinValue = sinf(_pDesc->fThisAccTime * _pDesc->fLifeTime);
		//_pDesc->tParticleInfo.vArgb.w = fSinValue*2.f*0.5f;
		//((CTransform*)(_pDesc->pTransform))->Set_Scale(_pDesc->tParticleInfo.vScale* fSinValue);
		//((CTransform*)(_pDesc->pTransform))->Set_Pos(_pDesc->tParticleInfo.vPos);
		//((CTransform*)(_pDesc->pTransform))->Go_Up(((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().fSpeedPerSecond, _pDesc->fDeltaTime);
		((CTransform*)(_pDesc->pTransform))->Go_Side(((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().fSpeedPerSecond, _pDesc->fDeltaTime);

	}
	void GoRightToTarget(PARTICLEPATTERNDESC* _pDesc)
	{
		if (_pDesc->fThisAccTime < _pDesc->fLifeTime / 2.f)
		{
			//	_pDesc->tParticleInfo.vArgb.w = sinf(_pDesc->fThisAccTime)*2.f*0.5f;
			//	((CTransform*)(_pDesc->pTransform))->Set_Pos(_pDesc->tParticleInfo.vPos);
			((CTransform*)(_pDesc->pTransform))->Go_Side(((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().fSpeedPerSecond, _pDesc->fDeltaTime);
		}
		else
		{
			_float3 vPos, vLookCur;
			((CTransform*)(_pDesc->pTransform))->Get_Info(EInfo::Pos, &vPos);
			((CTransform*)(_pDesc->pTransform))->Get_Info(EInfo::Right, &vLookCur);
			_float3 vHalfLook = (*(_pDesc->pTargetPos) - (vLookCur + vPos));

			const _float3 vHalfLookNormal = *D3DXVec3Normalize(&vHalfLook, &vHalfLook);
			const _float fSpeed = _pDesc->fDeltaTime*_pDesc->tParticleInfo.fMoveSpeed;
			const _float3 vToDir = vHalfLookNormal * fSpeed;
			const _float3 vOut = vLookCur + vToDir;
			((CTransform*)(_pDesc->pTransform))->Set_Info(EInfo::Right, vOut);

			((CTransform*)(_pDesc->pTransform))->Go_Side(((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().fSpeedPerSecond, _pDesc->fDeltaTime);
		}

	}
	void RevRotateX(PARTICLEPATTERNDESC* _pDesc)
	{
		//_float fSinValue = sinf(_pDesc->fThisAccTime * _pDesc->fLifeTime);
		//_pDesc->tParticleInfo.vArgb.w = fSinValue*2.f*0.5f;
		//((CTransform*)(_pDesc->pTransform))->Set_Scale(_pDesc->tParticleInfo.vScale* fSinValue);
		
		//((CTransform*)(_pDesc->pTransform))->Set_Revolution(EAxis::X, ((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().vRotate.x);
		//((CTransform*)(_pDesc->pTransform))->Set_Revolution(EAxis::Y, ((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().vRotate.y);
		//((CTransform*)(_pDesc->pTransform))->Set_Revolution(EAxis::Z, ((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().vRotate.z);
		//_pDesc->tParticleInfo.vRevRotateAngle.x += (_pDesc->fDeltaTime)*D3DXToRadian(1.f)*0.1f;
		//((CTransform*)(_pDesc->pTransform))->Set_Revolution(EAxis::X, _pDesc->tParticleInfo.vRotateAngle.x);
		((CTransform*)(_pDesc->pTransform))->Revolution(EAxis::X, D3DXToRadian(((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().fSpeedPerSecond), 0.1f, _pDesc->fDeltaTime);
		//((CTransform*)(_pDesc->pTransform))->Revolution(EAxis::Y, D3DXToRadian(((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().fSpeedPerSecond), 0.1f, _pDesc->fDeltaTime);
		//((CTransform*)(_pDesc->pTransform))->Revolution(EAxis::Z, D3DXToRadian(((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().fSpeedPerSecond), 0.1f, _pDesc->fDeltaTime);
		_float3 vLength = ((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().vPos - *_pDesc->pTargetPos;

		((CTransform*)(_pDesc->pTransform))->Set_Pos(vLength);
		((CTransform*)(_pDesc->pTransform))->Set_Parent(*_pDesc->pTargetPos);
		//((CTransform*)(_pDesc->pTransform))->Revolution(EAxis::Y, 1.f, D3DXToRadian(10.f));
		//((CTransform*)(_pDesc->pTransform))->Revolution(EAxis::Z, 1.f, D3DXToRadian(10.f));
		//((CTransform*)(_pDesc->pTransform))->Set_RevolutionSpeed(((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().fSpeedPerSecond);
		
		//((CTransform*)(_pDesc->pTransform))->Set_Pos(_pDesc->tParticleInfo.vPos);
		//((CTransform*)(_pDesc->pTransform))->Go_Up(((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().fSpeedPerSecond, _pDesc->fDeltaTime);
	}
	void RevRotateY(PARTICLEPATTERNDESC* _pDesc)
	{	
		((CTransform*)(_pDesc->pTransform))->Revolution(EAxis::Y, D3DXToRadian(((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().fSpeedPerSecond), 0.1f, _pDesc->fDeltaTime);
		_float3 vLength = ((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().vPos - *_pDesc->pTargetPos;

		((CTransform*)(_pDesc->pTransform))->Set_Pos(vLength);
		((CTransform*)(_pDesc->pTransform))->Set_Parent(*_pDesc->pTargetPos);
	}
	void RevRotateZ(PARTICLEPATTERNDESC* _pDesc)
	{
		((CTransform*)(_pDesc->pTransform))->Revolution(EAxis::Z, D3DXToRadian(((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().fSpeedPerSecond), 0.1f, _pDesc->fDeltaTime);
		_float3 vLength = ((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().vPos - *_pDesc->pTargetPos;

		((CTransform*)(_pDesc->pTransform))->Set_Pos(vLength);
		((CTransform*)(_pDesc->pTransform))->Set_Parent(*_pDesc->pTargetPos);
	}
	void RotateX(PARTICLEPATTERNDESC* _pDesc)
	{
		_pDesc->tParticleInfo.vRotateAngle.x += (_pDesc->fThisAccTime);
		((CTransform*)(_pDesc->pTransform))->Set_Rotate(EAxis::X,_pDesc->tParticleInfo.vRotateAngle.x);
	}
	void RotateY(PARTICLEPATTERNDESC* _pDesc)
	{
		_pDesc->tParticleInfo.vRotateAngle.y += (_pDesc->fThisAccTime);
		((CTransform*)(_pDesc->pTransform))->Set_Rotate(EAxis::Y, _pDesc->tParticleInfo.vRotateAngle.y);
	}
	void RotateZ(PARTICLEPATTERNDESC* _pDesc)
	{
		_pDesc->tParticleInfo.vRotateAngle.z += (_pDesc->fThisAccTime);
		((CTransform*)(_pDesc->pTransform))->Set_Rotate(EAxis::Z, _pDesc->tParticleInfo.vRotateAngle.z);
	}
	void Billboarding(PARTICLEPATTERNDESC* _pDesc)
	{
		_float4x4 matBill;
		D3DXMatrixIdentity(&matBill);
		//Get_Manager()->Get_Device()
		_pDesc->pDevice->GetTransform(D3DTS_VIEW, &matBill);
		D3DXMatrixInverse(&matBill, NULL, &matBill);
		matBill._41 = 0, matBill._42 = 0, matBill._43 = 0, matBill._44 = 1;

		((CTransform*)(_pDesc->pTransform))->Set_RotateMatrix(&matBill);
	}
	void YBillboarding(PARTICLEPATTERNDESC* _pDesc)
	{
		_float4x4 matBill;
		D3DXMatrixIdentity(&matBill);
		//Get_Manager()->Get_Device()
		_pDesc->pDevice->GetTransform(D3DTS_VIEW, &matBill);
		D3DXMatrixInverse(&matBill, NULL, &matBill);
		matBill._12 = 0;
		matBill._21 = 0, matBill._22 = 1, matBill._23 = 0;
		matBill._32 = 0;
		matBill._41 = 0, matBill._42 = 0, matBill._43 = 0, matBill._44 = 1;

		((CTransform*)(_pDesc->pTransform))->Set_RotateMatrix(&matBill);
	}
	void Gravity(PARTICLEPATTERNDESC* _pDesc)
	{
		_float3 vPos = _pDesc->tParticleInfo.vInitPos;
		vPos.x = _pDesc->tParticleInfo.vPos.x;
		vPos.z = _pDesc->tParticleInfo.vPos.z;
		//vPos.y = Jump_Function_To_Y(&vPos.y, _pDesc->fThisAccTime, 5.f);
		_float fPower = _pDesc->tParticleInfo.fMoveSpeed*2.f;
		if (fPower < 5.f)
			fPower = 5.f;
		vPos.y = Jump_Function_To_Y(&vPos.y, _pDesc->fThisAccTime, fPower);
		//vPos.y = vPos.y + (1.f *_pDesc->fThisAccTime - 0.5f * _float(GRAVITY) *_pDesc->fThisAccTime * _pDesc->fThisAccTime);


		((CTransform*)(_pDesc->pTransform))->Set_Pos(vPos);
		//((CTransform*)(_pDesc->pTransform))->Go_Up(((CTransform*)(_pDesc->pTransform))->Get_TransformDesc().fSpeedPerSecond, _pDesc->fDeltaTime);
	}
};



#endif // !__PARTICLEPATTERN_H__
