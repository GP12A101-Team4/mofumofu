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
#include "fade.h"
#include "game.h"
#include "score.h"
#include "sprite.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(70)	// キャラサイズ
#define TEXTURE_HEIGHT				(70)	// 
#define TEXTURE_MAX					(5)		// テクスチャの数
#define ANIMAL_TEX_MAX				(5)

#define UI_ICON_COUNT				(5)
// 虫眼鏡
#define UI_ICON_W					(60.0f)	// 1個あたりの幅
#define UI_ICON_H					(60.0f)	// 1個あたりの高さ
#define UI_ICON_PAD					(2.0f)	// スロット同士の隙間
#define UI_ICON_MARGIN_RIGHT		(-25.0f)	// 画面右端からの余白
#define UI_ICON_MARGIN_TOP			(40.0f)	// 画面上端からの余白
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

static BOOL g_AnimalCounted[UI_ICON_COUNT] = { FALSE, FALSE, FALSE, FALSE, FALSE };
static int  g_NextSlotToLight = 0;

bool g_CatAnimationPlayed = false;
bool g_DogAnimationPlayed = false;
bool g_ElphAnimationPlayed = false;
bool g_MouseAnimationPlayed = false;
bool g_SheepAnimationPlayed = false;

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

	for (int i = 0; i < UI_ICON_COUNT; ++i) g_AnimalCounted[i] = FALSE;
	g_NextSlotToLight = 0;

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
	
	UI_UpdateSequential();

	if (GetShowFullImage())       g_TexNo = 1;
	if (GetShowFullImage_Dog())   g_TexNo = 1;
	if (GetShowFullImage_Elph())  g_TexNo = 1;
	if (GetShowFullImage_Mouse()) g_TexNo = 1;
	if (GetShowFullImage_Sheep()) g_TexNo = 1;

	int completed = 0;
	if (GetShowFullImage())       completed++;
	if (GetShowFullImage_Dog())   completed++;
	if (GetShowFullImage_Elph())  completed++;
	if (GetShowFullImage_Mouse()) completed++;
	if (GetShowFullImage_Sheep()) completed++;

	if (completed == 5)
	{
		SetFade(FADE_OUT, MODE_RESULT);
		StopTimer();
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// BAR処理
//===========================================================================

// 0.0～1.0 に収める安全関数
static inline float Safe01(float v) {
	if (!(v == v) || !isfinite(v)) return 0.0f;
	if (v < 0.0f) v = 0.0f;
	if (v > 1.0f) v = 1.0f;
	return v;
}

// 動物のパズル完成度を順次チェックして、UIに反映する
static void UI_UpdateSequential(void)
{
	// 0: ねこ
	if (!g_AnimalCounted[0] && GetShowFullImage()) {
		g_AnimalCounted[0] = TRUE;
		if (g_NextSlotToLight < UI_ICON_COUNT) ++g_NextSlotToLight;
	}
	// 1: いぬ
	if (!g_AnimalCounted[1] && GetShowFullImage_Dog()) {
		g_AnimalCounted[1] = TRUE;
		if (g_NextSlotToLight < UI_ICON_COUNT) ++g_NextSlotToLight;
	}
	// 2: ゾウ
	if (!g_AnimalCounted[2] && GetShowFullImage_Elph()) {
		g_AnimalCounted[2] = TRUE;
		if (g_NextSlotToLight < UI_ICON_COUNT) ++g_NextSlotToLight;
	}
	// 3: ネズミ
	if (!g_AnimalCounted[3] && GetShowFullImage_Mouse()) {
		g_AnimalCounted[3] = TRUE;
		if (g_NextSlotToLight < UI_ICON_COUNT) ++g_NextSlotToLight;
	}
	// 4: ひつじ
	if (!g_AnimalCounted[4] && GetShowFullImage_Sheep()) {
		g_AnimalCounted[4] = TRUE;
		if (g_NextSlotToLight < UI_ICON_COUNT) ++g_NextSlotToLight;
	}
}

// 現在アクティブな動物のパズル完成度を取得する
float GetActivePuzzleAlignmentRatio()
{
	float ratios[5] = {
		g_CatAnimationPlayed   ? 0.0f : GetPuzzleAlignmentRatio(),
		g_DogAnimationPlayed   ? 0.0f : GetPuzzleAlignmentRatio_Dog(),
		g_ElphAnimationPlayed  ? 0.0f : GetPuzzleAlignmentRatio_Elph(),
		g_MouseAnimationPlayed ? 0.0f : GetPuzzleAlignmentRatio_Mouse(),
		g_SheepAnimationPlayed ? 0.0f : GetPuzzleAlignmentRatio_Sheep()
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
	//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	//// 桁数分処理する
	//
	//	// １枚のポリゴンの頂点とテクスチャ座標を設定
	//	SetSpriteColor(g_VertexBuffer,
	//		g_Pos.x, g_Pos.y, g_w, g_h,     // position siz
	//		0.0f, 0.0f, 1.0f, 1.0f,         // texture coord
	//		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

	//	// ポリゴン描画
	//	GetDeviceContext()->Draw(4, 0);

		DrawSequentialSlotsRow();
}

static void DrawSequentialSlotsRow(void)
{
	if (!g_Texture[0] || !g_Texture[1]) return; 

	
	float totalW = UI_ICON_COUNT * UI_ICON_W + (UI_ICON_COUNT - 1) * UI_ICON_PAD;
	float leftX = SCREEN_WIDTH - UI_ICON_MARGIN_RIGHT - totalW;
	float y = UI_ICON_MARGIN_TOP;

	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	MATERIAL mat;
	ZeroMemory(&mat, sizeof(mat));
	mat.Diffuse = XMFLOAT4(1, 1, 1, 1);
	SetMaterial(mat);


	for (int i = 0; i < UI_ICON_COUNT; ++i)
	{
		ID3D11ShaderResourceView* tex = (i < g_NextSlotToLight) ? g_Texture[1] : g_Texture[0];

		float x = leftX + i * (UI_ICON_W + UI_ICON_PAD);

		SetSpriteColor(
			g_VertexBuffer,
			x, y, UI_ICON_W, UI_ICON_H,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1, 1, 1, 1)
		);

		GetDeviceContext()->PSSetShaderResources(0, 1, &tex);
		GetDeviceContext()->Draw(4, 0);
	}
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
		
		float iconW = 120.0f;
		float iconH = 120.0f;

		float padding = 24.0f; 
		
		// アイコンをゲージ左に配置するための座標
		float iconX = (screenX - barWidth * 0.5f) - padding - iconW * 0.3f;
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

void ResetPuzzleAnimationFlags(void)
{
	g_CatAnimationPlayed = g_DogAnimationPlayed =
		g_ElphAnimationPlayed = g_MouseAnimationPlayed =
		g_SheepAnimationPlayed = false;
}



