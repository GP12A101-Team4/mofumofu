//=============================================================================
//
// �����蔻�菈�� [collision.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "collision.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************


//*****************************************************************************
// �\���̒�`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static inline float _minf(float a, float b) { return (a < b) ? a : b; }
static inline float _absf(float a) { return (a < 0.0f) ? -a : a; }

//=============================================================================
// BB�ɂ�铖���蔻�菈��
// ��]�͍l�����Ȃ�
// �߂�l�F�������Ă���true
//=============================================================================
BOOL CollisionBB(XMFLOAT3 mpos, float mw, float mh,
	XMFLOAT3 ypos, float yw, float yh)
{
	BOOL ans = FALSE;	// �O����Z�b�g���Ă���

	// ���W�����S�_�Ȃ̂Ōv�Z���₷�������ɂ��Ă���
	mw /= 2;
	mh /= 2;
	yw /= 2;
	yh /= 2;

	// �o�E���f�B���O�{�b�N�X(BB)�̏���
	if ((mpos.x + mw > ypos.x - yw) &&
		(mpos.x - mw < ypos.x + yw) &&
		(mpos.y + mh > ypos.y - yh) &&
		(mpos.y - mh < ypos.y + yh))
	{
		// �����������̏���
		ans = TRUE;
	}

	return ans;
}

//=============================================================================
// BC�ɂ�铖���蔻�菈��
// �T�C�Y�͔��a
// �߂�l�F�������Ă���TRUE
//=============================================================================
BOOL CollisionBC(XMFLOAT3 pos1, XMFLOAT3 pos2, float r1, float r2)
{
	BOOL ans = FALSE;						// �O����Z�b�g���Ă���

	float len = (r1 + r2) * (r1 + r2);		// ���a��2�悵����
	XMVECTOR temp = XMLoadFloat3(&pos1) - XMLoadFloat3(&pos2);
	temp = XMVector3LengthSq(temp);			// 2�_�Ԃ̋����i2�悵�����j
	float lenSq = 0.0f;
	XMStoreFloat(&lenSq, temp);

	// ���a��2�悵������苗�����Z���H
	if (len > lenSq)
	{
		ans = TRUE;	// �������Ă���
	}

	return ans;
}

//=============================================================================
// AABB_XZ�����蔻�菈��
//=============================================================================
BOOL ResolveCircleAABB_XZ(XMFLOAT3* pCenter, float radius,
	XMFLOAT3 aabbMin, XMFLOAT3 aabbMax)
{
	const float minx = aabbMin.x - radius;
	const float maxx = aabbMax.x + radius;
	const float minz = aabbMin.z - radius;
	const float maxz = aabbMax.z + radius;

	const float px = pCenter->x;
	const float pz = pCenter->z;

	if (px < minx || px > maxx || pz < minz || pz > maxz)
		return FALSE;

	const float penLeft = px - minx;
	const float penRight = maxx - px;
	const float penTop = pz - minz;
	const float penBottom = maxz - pz;

	
	const float pushX = (penLeft < penRight) ? -penLeft : +penRight;
	
	const float pushZ = (penTop < penBottom) ? -penTop : +penBottom;


	if (_absf(pushX) < _absf(pushZ))
		pCenter->x += pushX;
	else
		pCenter->z += pushZ;

	return TRUE;
}



