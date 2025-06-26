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

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(130)	// キャラサイズ
#define TEXTURE_HEIGHT				(TEXTURE_WIDTH*0.8)	// 
#define TEXTURE_MAX					(2)		// テクスチャの数
#define cnt_max						(5)


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
	"data/TEXTURE/cursor.png",

};

static tex g_tex[TEXTURE_MAX];

static MENU g_menu;


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
	if (GetKeyboardTrigger(DIK_P)) {
		g_menu.use = g_menu.use ? FALSE : TRUE;
	}
	


#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMenu(void)
{
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

			// カーソル
			
			//{
			//	// テクスチャ設定
			//	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			//	SetSpriteLTColor(g_VertexBuffer,
			//		float(GetMousePosX()), float(GetMousePosY()), 40.0f, 40.0f,
			//		0.0f, 0.0f, 1.0f, 1.0f,
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

