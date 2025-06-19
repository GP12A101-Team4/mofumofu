//=============================================================================
//
// �T�E���h���� [sound.h]
//
//=============================================================================
#pragma once

#include <windows.h>
#include "xaudio2.h"						// �T�E���h�����ŕK�v

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
enum
{
	SOUND_LABEL_BGM_BGM1,

	SOUND_LABEL_SE_CAT,
	SOUND_LABEL_SE_DOG,
	SOUND_LABEL_SE_ELEPHANT,
	SOUND_LABEL_SE_MOUSE,
	SOUND_LABEL_SE_SHEEP,

	SOUND_LABEL_SE_COUNTDOWN,
	SOUND_LABEL_SE_DISCOVERED,
	SOUND_LABEL_SE_LOSING,
	SOUND_LABEL_SE_POINT,
	SOUND_LABEL_SE_TIMESUP,
	SOUND_LABEL_SE_WINNING,




	SOUND_LABEL_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

