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
#include "ui.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(70)	// キャラサイズ
#define TEXTURE_HEIGHT				(70)	// 
#define TEXTURE_MAX					(5)		// テクスチャの数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/UI_black.png",
	"data/TEXTURE/UI_normal.png",
	"data/TEXTURE/successbarbottom.png",
	"data/TEXTURE/successbarcenter.png",
	"data/TEXTURE/successbartop.png",
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

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
	float screenX   = 480.0f;
	float screenY   = 475.0f;
	float barWidth  = 250.0f;
	float barHeight = 50.0f;
	float ratio = GetPuzzleAlignmentRatio();

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





