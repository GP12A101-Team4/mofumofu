//=============================================================================
//
// スコア処理 [hpbaar.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "menu.h"
#include "model.h"
#include "player.h"
#include "sound.h"
#include "sprite.h"
#include "game.h"
#include "title.h"
#include "collision.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(130)	// キャラサイズ
#define TEXTURE_HEIGHT				(TEXTURE_WIDTH*0.8)	// 
#define TEXTURE_MAX					(3)		// テクスチャの数
#define cnt_max						(5)
#define OFFSET						(60)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/menu.png",
	"data/TEXTURE/bar1.png",
	"data/TEXTURE/bar2.png",
};

static tex g_tex[TEXTURE_MAX];

static MENU g_menu;

static SPRITE g_sprite[MAX];

static float masterVolume;
static float seVolume;




//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMenu(void)
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


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	g_sprite[BAR_MASTER] = {			SCREEN_CENTER_X + 20,
										SCREEN_CENTER_Y - 80,
										300.0f,
										20.0f,

										1.0f,
										1.0f,
										0.0f,
										0.0f};

	g_sprite[BAR_MASTER_CURRENT] = {	((SCREEN_CENTER_X + 20) - g_sprite[BAR_MASTER].pw/2),
										SCREEN_CENTER_Y - 80,
										5.0f,				
										20.0f,				

										1.0f,				
										1.0f,				
										0.0f,				
										0.0f};

	g_sprite[BAR_SE] = { SCREEN_CENTER_X + 20,
										SCREEN_CENTER_Y - 80 + OFFSET,
										300.0f,
										20.0f,

										1.0f,
										1.0f,
										0.0f,
										0.0f };

	g_sprite[BAR_SE_CURRENT] = { ((SCREEN_CENTER_X + 20) - g_sprite[BAR_MASTER].pw / 2),
										SCREEN_CENTER_Y - 80 + OFFSET,
										5.0f,
										20.0f,

										1.0f,
										1.0f,
										0.0f,
										0.0f };

	g_menu.use = FALSE;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitMenu(void)
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

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateMenu(void)
{
	

	if (GetKeyboardTrigger(DIK_B)) {
		g_menu.use = g_menu.use ? FALSE : TRUE;
	}
	else if (GetIsHovering(MENU_SETTING) && IsMouseLeftTriggered()) {
		g_menu.use = g_menu.use ? FALSE : TRUE;
	}
	
	if (!g_menu.use)return;

	/*if (GetKeyboardTrigger(DIK_ADD)) {
		masterVolume += 0.05f;
		if (masterVolume > 1.0f) {
			masterVolume = 1.0f;
		}
	}
	else if (GetKeyboardTrigger(DIK_SUBTRACT)) {
		masterVolume -= 0.05f;
		if (masterVolume < 0.0f) {
			masterVolume = 0.0f;
		}
	}*/




	XMFLOAT3 MousePos;
	MousePos.x = float(GetMousePosX());
	MousePos.y = float(GetMousePosY());

	//Master Volume Control
	if (CollisionBB(MousePos, 1.0f, 1.0f, XMFLOAT3(g_sprite[BAR_MASTER].px, g_sprite[BAR_MASTER].py, 0.0f),
		g_sprite[BAR_MASTER].pw, g_sprite[BAR_MASTER].ph) && IsMouseLeftPressed()) {
		
		g_sprite[BAR_MASTER_CURRENT].px = MousePos.x;

		masterVolume = CordinateToVolume(g_sprite[BAR_MASTER_CURRENT].px,
			g_sprite[BAR_MASTER].px - (g_sprite[BAR_MASTER].pw / 2),
			g_sprite[BAR_MASTER].pw);

	}

	g_sprite[BAR_MASTER_CURRENT].px = VolumeToCordinate(masterVolume, 
														g_sprite[BAR_MASTER].px - (g_sprite[BAR_MASTER].pw / 2), 
														g_sprite[BAR_MASTER].pw);


	//SE Volume Control
	if (CollisionBB(MousePos, 1.0f, 1.0f, XMFLOAT3(g_sprite[BAR_SE].px, g_sprite[BAR_SE].py, 0.0f),
		g_sprite[BAR_SE].pw, g_sprite[BAR_SE].ph) && IsMouseLeftPressed()) {

		g_sprite[BAR_SE_CURRENT].px = MousePos.x;

		seVolume = CordinateToVolume(g_sprite[BAR_SE_CURRENT].px,
			g_sprite[BAR_SE].px - (g_sprite[BAR_SE].pw / 2),
			g_sprite[BAR_SE].pw);

	}

	g_sprite[BAR_SE_CURRENT].px = VolumeToCordinate(seVolume,
		g_sprite[BAR_SE].px - (g_sprite[BAR_SE].pw / 2),
		g_sprite[BAR_SE].pw);
	
	SetBGMVolume(masterVolume);
	SetSEVolume(seVolume);

#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	PrintDebugProc("MasterVolume : %f", masterVolume);
	PrintDebugProc("SeVolume : %f", seVolume);
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMenu(void)
{
	if (!g_menu.use)return;
	for (int i = 0; i < TEXTURE_MAX; i++) {
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


			// ○○を描画
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

				float px = SCREEN_CENTER_X;
				float py = SCREEN_CENTER_Y;
				float pw = 400.0f;				// スコアの表示幅
				float ph = 400.0f;				// スコアの表示高さ

				float tw = 1.0f;			// テクスチャの幅
				float th = 1.0f;			// テクスチャの高さ
				float tx = 0.0f;			// テクスチャの左上X座標
				float ty = 0.0f;			// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

			// 音量バー
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

				float px = g_sprite[BAR_MASTER].px - (g_sprite[BAR_MASTER].pw/2);
				float py = g_sprite[BAR_MASTER].py;
				float pw = g_sprite[BAR_MASTER].pw;// スコアの表示幅
				float ph = g_sprite[BAR_MASTER].ph;// スコアの表示高さ
						  
				float tw = g_sprite[BAR_MASTER].tw;// テクスチャの幅
				float th = g_sprite[BAR_MASTER].th;// テクスチャの高さ
				float tx = g_sprite[BAR_MASTER].tx;// テクスチャの左上X座標
				float ty = g_sprite[BAR_MASTER].ty;// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

			// 音量バーNOW
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

				float px = g_sprite[BAR_MASTER_CURRENT].px;
				float py = g_sprite[BAR_MASTER_CURRENT].py;
				float pw = g_sprite[BAR_MASTER_CURRENT].pw;	// スコアの表示幅
				float ph = g_sprite[BAR_MASTER_CURRENT].ph;	// スコアの表示高さ
						   							   
				float tw = g_sprite[BAR_MASTER_CURRENT].tw;	// テクスチャの幅
				float th = g_sprite[BAR_MASTER_CURRENT].th;	// テクスチャの高さ
				float tx = g_sprite[BAR_MASTER_CURRENT].tx;	// テクスチャの左上X座標
				float ty = g_sprite[BAR_MASTER_CURRENT].ty;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

			// 音量バー
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

				float px = g_sprite[BAR_SE].px - (g_sprite[BAR_MASTER].pw / 2);
				float py = g_sprite[BAR_SE].py;
				float pw = g_sprite[BAR_SE].pw;// スコアの表示幅
				float ph = g_sprite[BAR_SE].ph;// スコアの表示高さ

				float tw = g_sprite[BAR_SE].tw;// テクスチャの幅
				float th = g_sprite[BAR_SE].th;// テクスチャの高さ
				float tx = g_sprite[BAR_SE].tx;// テクスチャの左上X座標
				float ty = g_sprite[BAR_SE].ty;// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

			// 音量バーNOW
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

				float px = g_sprite[BAR_SE_CURRENT].px;
				float py = g_sprite[BAR_SE_CURRENT].py;
				float pw = g_sprite[BAR_SE_CURRENT].pw;	// スコアの表示幅
				float ph = g_sprite[BAR_SE_CURRENT].ph;	// スコアの表示高さ

				float tw = g_sprite[BAR_SE_CURRENT].tw;	// テクスチャの幅
				float th = g_sprite[BAR_SE_CURRENT].th;	// テクスチャの高さ
				float tx = g_sprite[BAR_SE_CURRENT].tx;	// テクスチャの左上X座標
				float ty = g_sprite[BAR_SE_CURRENT].ty;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

			//// 音量バー
			//{
			//	// テクスチャ設定
			//	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			//	float px = SCREEN_CENTER_X + 20;
			//	float py = SCREEN_CENTER_Y - 80;
			//	float pw = 300.0f;				// スコアの表示幅
			//	float ph = 20.0f;				// スコアの表示高さ

			//	float tw = 1.0f;				// テクスチャの幅
			//	float th = 1.0f;				// テクスチャの高さ
			//	float tx = 0.0f;				// テクスチャの左上X座標
			//	float ty = 0.0f;				// テクスチャの左上Y座標

			//	// １枚のポリゴンの頂点とテクスチャ座標を設定
			//	SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			//		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			//	// ポリゴン描画
			//	GetDeviceContext()->Draw(4, 0);
			//}

			

		}
	}
}

MENU* GetMenu() {
	return &g_menu ;
}


float VolumeToCordinate(float volume, float startPos, float BarLength) {
	float deltaPos = BarLength * volume;
	float posX = startPos + deltaPos;
	return posX;
}

float CordinateToVolume(float pos,float startPos,float BarLength) {
	float volume = (pos - startPos) / BarLength;
	return volume;
}
