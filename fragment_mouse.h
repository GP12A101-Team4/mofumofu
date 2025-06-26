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
HRESULT InitFragment_Mouse(void);
void UninitFragment_Mouse(void);
void UpdateFragment_Mouse(void);
void DrawFragment_Mouse(void);

float GetPuzzleAlignmentRatio_Mouse();
void DrawPartDebugUI_Mouse();

extern bool g_ShowFullImage_Mouse;

