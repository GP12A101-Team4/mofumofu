#pragma once

//=============================================================================
//
// ��Q���̓�����Е`�� [fragment_obstacle.h]
//
//=============================================================================
#pragma once

#include "renderer.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define OBSTACLE_MAX      (50)   
#define OBSTACLE_TYPES    (5)    
#define TEXTURE_PER_TYPE  (7)    

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct {
    BOOL use;
    int type;   // 0=dog, 1=cat, 2=elephant, 3=mouse, 4=sheep
    int texIndex;
    XMFLOAT3 pos;
    XMFLOAT3 scl;
    XMFLOAT3 rot;
    XMFLOAT4X4 mtxWorld;
} OBSTACLE_FRAGMENT;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitObstacleFragment(void);
void    UninitObstacleFragment(void);
void    UpdateObstacleFragment(void);
void    DrawObstacleFragment(void);