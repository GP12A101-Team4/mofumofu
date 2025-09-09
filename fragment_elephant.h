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
HRESULT InitFragment_Elph(void);
void UninitFragment_Elph(void);
void UpdateFragment_Elph(void);
void DrawFragment_Elph(void);

float GetPuzzleAlignmentRatio_Elph();
void DrawPartDebugUI_Elph();

extern bool g_ShowFullImage_Elph;
extern bool g_ElphAnimationPlayed;

void ShowElephantDebugWindow();

