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
HRESULT InitFragment_Dog(void);
void UninitFragment_Dog(void);
void UpdateFragment_Dog(void);
void DrawFragment_Dog(void);

float GetPuzzleAlignmentRatio_Dog();
void DrawPartDebugUI_Dog();

extern bool g_ShowFullImage_Dog;
extern bool g_DogAnimationPlayed;

void ShowDogDebugWindow();

