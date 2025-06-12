//=============================================================================
//
// 地面処理 [fragment.h]
// Author : 
//
//=============================================================================
#pragma once

#include "renderer.h"

#define MAX_HISTORY_SIZE 100

//*****************************************************************************
// マクロ定義
//*****************************************************************************
struct FRAGMENT
{
	XMFLOAT3		pos;		// ポリゴンの位置
	XMFLOAT3		rot;		// ポリゴンの向き(回転)
	XMFLOAT3		scl;		// ポリゴンの大きさ(スケール)

	XMFLOAT4X4		mtxWorld;	// ワールドマトリックス
	XMFLOAT3		overallPos;
	
	XMFLOAT3		history[MAX_HISTORY_SIZE];
	
};

struct FRAGMENT_RESTORED
{
	XMFLOAT3		pos;		// ポリゴンの位置
	XMFLOAT3		rot;		// ポリゴンの向き(回転)
	XMFLOAT3		scl;		// ポリゴンの大きさ(スケール)

	XMFLOAT4X4		mtxWorld;	// ワールドマトリックス
	
	BOOL			use;
	BOOL			Initialized;

	float			alpha;

	int				AnimCnt;

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitFragment(void);
void UninitFragment(void);
void UpdateFragment(void);
void DrawFragment(void);

void DrawPartDebugUI();

float GetPuzzleAlignmentRatio();

extern bool g_ShowFullImage;

