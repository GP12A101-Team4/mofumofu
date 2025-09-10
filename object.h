//=============================================================================
//
// モデル処理 [player.h]
// Author : 
//
//=============================================================================
#pragma once

#include "model.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_OBJECT		(1)					// プレイヤーの数

#define	OBJECT_SIZE		(5.0f)				// 当たり判定の大きさ


//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct OBJECT
{
	XMFLOAT3		pos;		// ポリゴンの位置
	XMFLOAT3		rot;		// ポリゴンの向き(回転)
	XMFLOAT3		scl;		// ポリゴンの大きさ(スケール)

	XMFLOAT4X4		mtxWorld;	// ワールドマトリックス

	BOOL			load;
	DX11_MODEL		model;		// モデル情報

	float			size;		// 当たり判定の大きさ
	int				shadowIdx;	// 影のIndex
	BOOL			use;


};

enum object {
	tree_1,
	tree_2,
	tree_3,
	tree_4,
	tree_5,
	tree_6,
	tree_7,
	tree_8,
	tree_9,
	tree_last,
	lamp_1,
	lamp_2,
	lamp_3,
	lamp_4,
	lamp_5,
	lamp_6,
	lamp_7,
	lamp_last,
	plant_1,
	plant_2,
	plant_3,
	plant_4,
	plant_5,
	plant_last,
	bench_1,
	bench_2,
	bench_3,
	bench_4,
	bench_5,
	bench_last,
	shop_1,
	shop_2,
	shop_3,
	fountain,
	cafe,
	building,
	burger_shop,
	candy_shop,
	ice_truck,
	restaurant,
	
	
	
	
	
	object_max

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitObject(void);
void UninitObject(void);
void UpdateObject(void);
void DrawObject(void);

OBJECT *GetObject(void);
void ShowObjectDebugWindow();

