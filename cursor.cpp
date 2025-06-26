//=============================================================================
//
// スコア処理 [cursor.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "cursor.h"
#include "model.h"
#include "player.h"
#include "sound.h"
#include "sprite.h"
#include "game.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(130)	// キャラサイズ
#define TEXTURE_HEIGHT				(TEXTURE_WIDTH*0.8)	// 
#define TEXTURE_PATTERN_DIVIDE_X	(1.0f/2)
#define TEXTURE_PATTERN_DIVIDE_Y	(1.0f)
#define TEXTURE_MAX					(2)		// テクスチャの数
#define cnt_max						(5)

#define	CURSOR_NORMAL				(0.0f)
#define	CURSOR_CLICK				(1.0f/2*1)	
#define	CURSOR_MIDDLE_NORMAL		(1.0f/6*2)
#define	CURSOR_MIDDLE_CLICK			(1.0f/6*3)
#define	CURSOR_GOLDEN_NORMAL		(1.0f/6*4)
#define	CURSOR_GOLDEN_CLICK			(1.0f/6*5)


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


static BOOL		isClick = FALSE;
static float	cursorUV = 0.0f;

static int		isF = 0;


static MODE g_mode;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitCursor(void)
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


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitCursor(void)
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
void UpdateCursor(void)
{
	if (IsMouseLeftTriggered()) {
		PlaySound(SOUND_LABEL_SE_MOUSECLICK);
	}

	/*if (GetKeyboardTrigger(DIK_F)) {
		isF++ ;
		if (isF > 2 ) {
			isF = 0;
		}
	}*/
	
	switch (isF)
	{
	case 0:
		if (IsMouseLeftPressed()) {

			cursorUV = CURSOR_CLICK;
		}
		else {
			cursorUV = CURSOR_NORMAL;
		}
		break;
	case 1:
		if (IsMouseLeftPressed()) {

			cursorUV = CURSOR_MIDDLE_CLICK;
		}
		else {
			cursorUV = CURSOR_MIDDLE_NORMAL;
		}
		break;
	case 2:
		if (IsMouseLeftPressed()) {

			cursorUV = CURSOR_GOLDEN_CLICK;
		}
		else {
			cursorUV = CURSOR_GOLDEN_NORMAL;
		}
		break;
	}
	

	

#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	PrintDebugProc("isF:%d \n", isF);
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawCursor(void)
{
	PLAYER* player = GetPlayer();
	for (int i = 0; i < TEXTURE_MAX; i++) {
		
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
			

				//カーソル描画
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

				SetSpriteLTColor(g_VertexBuffer,
					float(GetMousePosX()), float(GetMousePosY()), 60.0f, 60.0f,
					cursorUV, 0.0f, TEXTURE_PATTERN_DIVIDE_X, TEXTURE_PATTERN_DIVIDE_Y,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));


				/*switch (isF)
				{
				case 0:
					SetSpriteLTColor(g_VertexBuffer,
						float(GetMousePosX()), float(GetMousePosY()), 30.0f, 50.0f,
						cursorUV, 0.0f, TEXTURE_PATTERN_DIVIDE_X, TEXTURE_PATTERN_DIVIDE_Y,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
					break;
				case 1:
					SetSpriteLTColor(g_VertexBuffer,
						float(GetMousePosX()), float(GetMousePosY()), 60.0f, 100.0f,
						cursorUV, 0.0f, TEXTURE_PATTERN_DIVIDE_X, TEXTURE_PATTERN_DIVIDE_Y,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
					break;
				case 2:
					SetSpriteLTColor(g_VertexBuffer,
						float(GetMousePosX()), float(GetMousePosY()), 60.0f, 100.0f,
						cursorUV, 0.0f, TEXTURE_PATTERN_DIVIDE_X, TEXTURE_PATTERN_DIVIDE_Y,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
					break;
				}*/
				
				

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

	}
}


