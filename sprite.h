//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : GP11B132 99 外岡高明
//
//=============================================================================
#pragma once


//*****************************************************************************
// マクロ定義
//*****************************************************************************
struct SPRITE {
	float px;
	float py;
	float pw;
	float ph;

	float tw;
	float th;
	float tx;
	float ty;
};
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void SetSprite(ID3D11Buffer *buf, float X, float Y, float Width, float Height, float U, float V, float UW, float VH);

void SetSpriteColor(ID3D11Buffer *buf, float X, float Y, float Width, float Height, float U, float V, float UW, float VH, XMFLOAT4 color);

void SetSpriteLTColor(ID3D11Buffer* buf,
	float X, float Y, float Width, float Height,
	float U, float V, float UW, float VH,
	XMFLOAT4 color);

void SetSpriteColorRotation(ID3D11Buffer *buf, float X, float Y, float Width, float Height,
	float U, float V, float UW, float VH,
	XMFLOAT4 Color, float Rot);

void SetSpriteLeftTop(ID3D11Buffer *buf, float X, float Y, float Width, float Height, float U, float V, float UW, float VH);

void DrawGaugeBarSprite(ID3D11Buffer* buf,ID3D11ShaderResourceView* texBack,ID3D11ShaderResourceView* texFill,
	float x, float y, float width, float height, float ratio);