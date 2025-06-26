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


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitFragment_Sheep(void);
void UninitFragment_Sheep(void);
void UpdateFragment_Sheep(void);
void DrawFragment_Sheep(void);

float GetPuzzleAlignmentRatio_Sheep();
void DrawPartDebugUI_Sheep();

extern bool g_ShowFullImage_Sheep;

