//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "title.h"
#include "collision.h"
#include "debugproc.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(10)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(480)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(480)		

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

XMFLOAT3 MousePos;
XMFLOAT3 StartButtonPos;
XMFLOAT3 ExitButtonPos;

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title.png",		//0
	"data/TEXTURE/title_logo.png",	//1
	"data/TEXTURE/effect000.jpg",	//2
	"data/TEXTURE/start.png",		//3
	"data/TEXTURE/start_selected.png",		//4
	"data/TEXTURE/exit.png",		//5
	"data/TEXTURE/exit_selected.png",		//6
	"data/TEXTURE/yuki001.png",		//7
	"data/TEXTURE/yuki002.png",		//8
	"data/TEXTURE/cursor.png"		//9

};



static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static BOOL						g_Load = FALSE;

int menu = 1;
int inputMode;

static SNOW	g_SNOW;



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
{
	ID3D11Device* pDevice = GetDevice();

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


	// 変数の初期化
	g_Use = TRUE;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = XMFLOAT3(g_w / 2, g_h / 2, 0.0f);
	g_TexNo = 0;

	g_SNOW.scrl = 0.0f;			// TEXスクロール
	g_SNOW.scrl2 = 0.0f;		// TEXスクロール
	g_SNOW.scrl2 = 0.0f;		// TEXスクロール

	StartButtonPos = { SCREEN_CENTER_X ,360.0f,0.0f };
	ExitButtonPos  = { SCREEN_CENTER_X ,450.0f,0.0f };

	//PlaySound(SOUND_LABEL_BGM_title);

	g_Load = TRUE;
	return S_OK;

}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
{
	if (g_Load == FALSE) return;

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

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTitle(void)
{

	
	//// ゲームパッドで入力処理
	//else if (IsButtonTriggered(0, BUTTON_START))
	//{
	//	SetFade(FADE_OUT, MODE_TUTORIAL);
	//}
	//else if (IsButtonTriggered(0, BUTTON_B))
	//{
	//	SetFade(FADE_OUT, MODE_TUTORIAL);
	//}

	long MouseDeltaX = GetMouseX();
	long MouseDeltaY = GetMouseY();
	MousePos.x = float(GetMousePosX());
	MousePos.y = float(GetMousePosY());

	if (MouseDeltaX != 0 || MouseDeltaY != 0) {
		inputMode = 0;
		menu = -1;
	}

	if (CollisionBB(MousePos, 1.0f, 1.0f, StartButtonPos, 240.0f, 80.0f) && inputMode == 0) {
		menu = MENU_START;
	}
	if (CollisionBB(MousePos, 1.0f, 1.0f, ExitButtonPos , 240.0f, 80.0f) && inputMode == 0) {
		menu = MENU_EXIT;
	}
	

		if (GetKeyboardTrigger(DIK_UP))
		{
			if (inputMode == 0) {
				menu = 1;
				inputMode = 1;
			}
			else{ 
				menu--; 
				inputMode = 1;
			}
			
		}

		if (GetKeyboardTrigger(DIK_DOWN))
		{
			if (inputMode == 0) {
				menu = 1;
				inputMode = 1;
			}
			else{
				menu++;
				inputMode = 1;
			}
		}




	if (menu == MENU_MAX) {
		menu = MENU_START;
	}
	else if (menu == MENU_MIN) {
		menu = MENU_EXIT;
	}

	if (GetKeyboardTrigger(DIK_RETURN)|| IsMouseLeftPressed())
	{// Enter押したら、ステージを切り替える
		if (menu == MENU_START)
		{
			SetFade(FADE_OUT, MODE_GAME);
		}
		else if (menu == MENU_EXIT)
		{
			PostQuitMessage(0);
		}
	}

	g_SNOW.old_pos = g_SNOW.pos;	// １フレ前の情報を保存


	g_SNOW.scrl -= 0.005f;		// 0.005f;		// スクロール

	


#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	/*PrintDebugProc("menu:%d", menu);*/
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
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

	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	// スタートボタン描画
	{
		// テクスチャ設定
		if (menu == MENU_START) {
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);
		}
		else {
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);
		}

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, StartButtonPos.x, StartButtonPos.y, 240, 80, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// エグジットボタンを描画
	{
		// テクスチャ設定
		if (menu == MENU_EXIT) {
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);
		}
		else {
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);
		}

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, ExitButtonPos.x, ExitButtonPos.y, 240, 80, 0.0f, 0.0f, 1.0f, 1.0);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// 空を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

		//１枚のポリゴンの頂点とテクスチャ座標を設定
		g_SNOW.scrl3 -= 0.001f;

		SetSpriteLTColor(g_VertexBuffer,
			0.0f, 0.0f, SCREEN_WIDTH, 540.0f,
			0.0f, g_SNOW.scrl3, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// 空を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

		//１枚のポリゴンの頂点とテクスチャ座標を設定
		g_SNOW.scrl += 0.001f;

		SetSpriteLTColor(g_VertexBuffer,
			0.0f, 0.0f, SCREEN_WIDTH, 540.0f,
			0.0f, g_SNOW.scrl, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// カーソル
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

		SetSpriteLTColor(g_VertexBuffer,
			MousePos.x, MousePos.y, 40.0f, 40.0f,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}





