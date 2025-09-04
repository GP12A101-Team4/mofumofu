//=============================================================================
//
// スコア処理 [ui.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "fragment.h"
#include "fragment_dog.h"
#include "fragment_elephant.h"
#include "fragment_mouse.h"
#include "fragment_sheep.h"
#include "ui.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(70)	// キャラサイズ
#define TEXTURE_HEIGHT				(70)	// 
#define TEXTURE_MAX					(5)		// テクスチャの数
#define ANIMAL_TEX_MAX				(5)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static ID3D11ShaderResourceView	*g_AnimalTex[ANIMAL_TEX_MAX] = { NULL };

static char *g_TexturName[] = {
	"data/TEXTURE/UI_black.png",
	"data/TEXTURE/UI_normal.png",
	"data/TEXTURE/successbarbottom.png",
	"data/TEXTURE/successbarcenter.png",
	"data/TEXTURE/successbartop.png",
};

static const char* g_AnimalTexName[ANIMAL_TEX_MAX] = {
	"data/TEXTURE/cat.png",
	"data/TEXTURE/dog.png",
	"data/TEXTURE/elephant.png",
	"data/TEXTURE/mouse.png",
	"data/TEXTURE/sheep.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号
static int g_ActiveAnimalIdx = -1;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitUI(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	for (int i = 0; i < ANIMAL_TEX_MAX; ++i) {
		g_AnimalTex[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_AnimalTexName[i],
			NULL,
			NULL, 
			&g_AnimalTex[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// プレイヤーの初期化
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 925.0f, 35.0f, 0.0f };
	g_TexNo = 0;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitUI(void)
{
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	for (int i = 0; i < ANIMAL_TEX_MAX; ++i) {
		if (g_AnimalTex[i]) 
		{
			g_AnimalTex[i]->Release();
			g_AnimalTex[i] = NULL;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateUI(void)
{
	if (g_ShowFullImage)
	{
		g_TexNo = 1;
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// BAR処理
//===========================================================================
static inline float Safe01(float v) {
	if (!(v == v) || !isfinite(v)) return 0.0f; // NaN/Inf -> 0
	if (v < 0.0f) v = 0.0f;
	if (v > 1.0f) v = 1.0f;
	return v;
}

// 現在アクティブな動物のパズル完成度を取得する
float GetActivePuzzleAlignmentRatio()
{
	float ratios[5] = {
		GetPuzzleAlignmentRatio(),        // 猫
		GetPuzzleAlignmentRatio_Dog(),    // 狗
		GetPuzzleAlignmentRatio_Elph(),   // 象
		GetPuzzleAlignmentRatio_Mouse(),  // 鼠
		GetPuzzleAlignmentRatio_Sheep()   // 羊
	};

	// 取得した値を [0,1] に収める
	for (int i = 0; i < 5; ++i) {
		ratios[i] = Safe01(ratios[i]);
	}

	// 最大値を持つ動物を探す
	int   bestIdx = -1;
	float bestVal = 0.0f;
	for (int i = 0; i < 5; ++i) {
		if (ratios[i] > bestVal) {
			bestVal = ratios[i];
			bestIdx = i;
		}
	}

	// ゲージ表示を開始する最小しきい値
	const float ENTER_EPS = 0.005f;
#ifdef _DEBUG
	{
		// デバッグ用に各動物の比率と最大値を出力
		char* str = GetDebugStr();
		sprintf(&str[strlen(str)], " [GB] cat=%.3f dog=%.3f elph=%.3f mou=%.3f sheep=%.3f | best=%d val=%.3f",
			ratios[0], ratios[1], ratios[2], ratios[3], ratios[4], bestIdx, bestVal);
	}
#endif

	// 一定以上の比率がある場合はアクティブ動物として登録
	if (bestIdx >= 0 && bestVal > ENTER_EPS) {
		g_ActiveAnimalIdx = bestIdx;  
		return bestVal;
	}

	// いずれの動物も対象外の場合
	g_ActiveAnimalIdx = -1;

	return 0.0f;
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawUI(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	// 桁数分処理する
	
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer,
			g_Pos.x, g_Pos.y, g_w, g_h,     // position siz
			0.0f, 0.0f, 1.0f, 1.0f,         // texture coord
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
}

void DrawGaugeBars()
{
	// 現在のアクティブ動物の完成度を取得
	float ratio = GetActivePuzzleAlignmentRatio();
	
	// ゲージバーの表示位置とサイズ
	float screenX   = 480.0f;
	float screenY   = 475.0f;
	float barWidth  = 250.0f;
	float barHeight = 50.0f;

	// ===== アクティブ動物のアイコン描画 =====
	if (g_ActiveAnimalIdx >= 0 && g_ActiveAnimalIdx < ANIMAL_TEX_MAX) {
		
		float iconW = 64.0f;
		float iconH = 64.0f;

		float padding = 24.0f; 
		
		// アイコンをゲージ左に配置するための座標
		float iconX = (screenX - barWidth * 0.5f) - padding - iconW * 0.5f;
		float iconY = screenY; 

		// 動物アイコンのテクスチャを設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_AnimalTex[g_ActiveAnimalIdx]);

		// アイコンのスプライト描画
		SetSpriteColor(g_VertexBuffer,
			iconX, iconY, iconW, iconH,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1, 1, 1, 1));
		GetDeviceContext()->Draw(4, 0);
	}

	// ===== Bar =====
	// bottom layer (background)
	SetSpriteColor(g_VertexBuffer, screenX, screenY, barWidth, barHeight,
		0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1, 1, 1, 1));
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);
	GetDeviceContext()->Draw(4, 0);

	// center layer (ratio-filled bar)
	float filledWidth = barWidth * ratio;
	SetSpriteColor(g_VertexBuffer, screenX - (barWidth - filledWidth) / 2.0f, screenY, filledWidth, barHeight,
		0.0f, 0.0f, ratio, 1.0f, XMFLOAT4(1, 1, 1, 1));
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);
	GetDeviceContext()->Draw(4, 0);

	// top layer (outline)
	SetSpriteColor(g_VertexBuffer, screenX, screenY, barWidth, barHeight,
		0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1, 1, 1, 1));
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);
	GetDeviceContext()->Draw(4, 0);
}





