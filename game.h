//=============================================================================
//
// ゲーム画面処理 [game.h]
// Author : 
//
//=============================================================================
#pragma once

#define	NumBlockX	(100)
#define NumBlockZ	(100)
#define BlockSizeX	(13.0f)
#define BlockSizeZ	(13.0f)

#define	MAP_W			(1280.0f)
#define	MAP_H			(1280.0f)
#define	MAP_TOP			(MAP_H/2)
#define	MAP_DOWN		(-MAP_H/2)
#define	MAP_LEFT		(-MAP_W/2)
#define	MAP_RIGHT		(MAP_W/2)

#define PAD_CAMERA_SPD	(10)

struct RANDOM
{
	int seed;
};

struct UI {
	bool		pause;
};
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitGame(void);
void UninitGame(void);
void UpdateGame(void);
void DrawGame(void);

XMFLOAT3 GetCameraDir();
float GetCameraYaw(XMFLOAT3 dir, XMFLOAT3 pos);
float GetCameraPitch(XMFLOAT3 dir, XMFLOAT3 pos);



float turning(float target, float current);
float NormalizeAngle(float angle);


RANDOM* GetRandom();
//UI* GetUi();

