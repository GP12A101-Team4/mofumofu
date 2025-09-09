//=============================================================================
//
// �X�R�A���� [ui.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitUI(void);
void UninitUI(void);
void UpdateUI(void);
void DrawUI(void);
void DrawGaugeBars(void);

float GetActivePuzzleAlignmentRatio(void);
void UI_UpdateSequential(void);
void DrawSequentialSlotsRow(void);

extern bool g_CatAnimationPlayed;
extern bool g_DogAnimationPlayed;
extern bool g_ElphAnimationPlayed;
extern bool g_MouseAnimationPlayed;
extern bool g_SheepAnimationPlayed;

void ResetPuzzleAnimationFlags(void);



