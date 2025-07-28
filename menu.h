//=============================================================================
//
// スコア処理 [menu.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
struct tex {
	BOOL					Use;						// TRUE:使っている  FALSE:未使用
	float					w,h;					// 幅と高さ
	XMFLOAT3				Pos;						// ポリゴンの座標
	int						TexNo;					// テクスチャ番号
	};

struct MENU
{
	BOOL					use;
};

enum {
	BAR_MASTER,
	BAR_MASTER_CURRENT,
	BAR_SE,
	BAR_SE_CURRENT,
	MAX
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitMenu(void);
void UninitMenu(void);
void UpdateMenu(void);
void DrawMenu(void);

MENU* GetMenu();

float VolumeToCordinate(float volume,float startPos,float BarLength);
float CordinateToVolume(float pos, float StartPos, float BarLength);
