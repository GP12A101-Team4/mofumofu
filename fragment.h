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
struct FRAGMENT
{
	XMFLOAT3		pos;		// �|���S���̈ʒu
	XMFLOAT3		rot;		// �|���S���̌���(��])
	XMFLOAT3		scl;		// �|���S���̑傫��(�X�P�[��)

	XMFLOAT4X4		mtxWorld;	// ���[���h�}�g���b�N�X
	XMFLOAT3		overallPos;
	
	XMFLOAT3		history[MAX_HISTORY_SIZE];
	
};

struct FRAGMENT_RESTORED
{
	XMFLOAT3		pos;		// �|���S���̈ʒu
	XMFLOAT3		rot;		// �|���S���̌���(��])
	XMFLOAT3		scl;		// �|���S���̑傫��(�X�P�[��)

	XMFLOAT4X4		mtxWorld;	// ���[���h�}�g���b�N�X
	
	BOOL			use;
	BOOL			Initialized;

	float			alpha;

	int				AnimCnt;

};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitFragment(void);
void UninitFragment(void);
void UpdateFragment(void);
void DrawFragment(void);

void DrawPartDebugUI();

float GetPuzzleAlignmentRatio();

extern bool g_ShowFullImage;

