#include "ParticlePattern.h"
#include "Transform.h"

void CPaticlePattern::NoAction(PARTICLEPATTERNDESC * _pDesc)
{
	((CTransform*)(_pDesc->pTransform))->Update_Transform();
}
