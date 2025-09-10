//=============================================================================
//
// 当たり判定処理 [collision.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "collision.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************


//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static inline float _minf(float a, float b) { return (a < b) ? a : b; }
static inline float _absf(float a) { return (a < 0.0f) ? -a : a; }

//=============================================================================
// BBによる当たり判定処理
// 回転は考慮しない
// 戻り値：当たってたらtrue
//=============================================================================
BOOL CollisionBB(XMFLOAT3 mpos, float mw, float mh,
	XMFLOAT3 ypos, float yw, float yh)
{
	BOOL ans = FALSE;	// 外れをセットしておく

	// 座標が中心点なので計算しやすく半分にしている
	mw /= 2;
	mh /= 2;
	yw /= 2;
	yh /= 2;

	// バウンディングボックス(BB)の処理
	if ((mpos.x + mw > ypos.x - yw) &&
		(mpos.x - mw < ypos.x + yw) &&
		(mpos.y + mh > ypos.y - yh) &&
		(mpos.y - mh < ypos.y + yh))
	{
		// 当たった時の処理
		ans = TRUE;
	}

	return ans;
}

//=============================================================================
// BCによる当たり判定処理
// サイズは半径
// 戻り値：当たってたらTRUE
//=============================================================================
BOOL CollisionBC(XMFLOAT3 pos1, XMFLOAT3 pos2, float r1, float r2)
{
	BOOL ans = FALSE;						// 外れをセットしておく

	float len = (r1 + r2) * (r1 + r2);		// 半径を2乗した物
	XMVECTOR temp = XMLoadFloat3(&pos1) - XMLoadFloat3(&pos2);
	temp = XMVector3LengthSq(temp);			// 2点間の距離（2乗した物）
	float lenSq = 0.0f;
	XMStoreFloat(&lenSq, temp);

	// 半径を2乗した物より距離が短い？
	if (len > lenSq)
	{
		ans = TRUE;	// 当たっている
	}

	return ans;
}

//=============================================================================
// AABB_XZ当たり判定処理
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



