//=============================================================================
//
// �n�ʏ��� [fragment.h]
// Author : 
//
//=============================================================================
#pragma once

#include "renderer.h"

#define MAX_HISTORY_SIZE 100

//*****************************************************************************
// �}�N����`
//*****************************************************************************


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitFragment_Elph(void);
void UninitFragment_Elph(void);
void UpdateFragment_Elph(void);
void DrawFragment_Elph(void);

float GetPuzzleAlignmentRatio_Elph();
void DrawPartDebugUI_Elph();

extern bool g_ShowFullImage_Elph;

