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
#include "menu.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(12)				// テクスチャの数

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

bool isHoveringStart;
bool isHoveringExit;
bool isHoveringSetting;

static bool wasHoveringStart = false;
static bool wasHoveringExit = false;
static bool wasHoveringSetting = false;




XMFLOAT3 StartButtonPos;
XMFLOAT3 ExitButtonPos;
XMFLOAT3 SettingButtonPos;
XMFLOAT3 PawsPos;

static float volume;

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title.png",						//0
	"data/TEXTURE/title_logo.png",					//1
	"data/TEXTURE/effect000.jpg",					//2
	"data/TEXTURE/start.png",						//3
	"data/TEXTURE/start_selected.png",				//4
	"data/TEXTURE/exit.png",						//5
	"data/TEXTURE/exit_selected.png",				//6
	"data/TEXTURE/setting.png",						//7
	"data/TEXTURE/setting_selected.png",			//8
};



static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号
static bool						isAudioPlayed;

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

	StartButtonPos		=	{ SCREEN_CENTER_X ,360.0f,0.0f };
	ExitButtonPos		=	{ SCREEN_CENTER_X ,450.0f,0.0f };
	SettingButtonPos	=	{ 900.0f ,500.0f,0.0f };
	PawsPos				=	{ 410.0f,220.0f,0.0f };


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

	long MouseDeltaX = GetMouseX();
	long MouseDeltaY = GetMouseY();
	XMFLOAT3 MousePos;
	MousePos.x = float(GetMousePosX());
	MousePos.y = float(GetMousePosY());

	if (MouseDeltaX != 0 || MouseDeltaY != 0) {
		inputMode = 0;
		menu = -1;
	}

	isHoveringStart		= CollisionBB(MousePos, 1.0f, 1.0f, StartButtonPos, 240.0f, 80.0f);
	isHoveringExit		= CollisionBB(MousePos, 1.0f, 1.0f, ExitButtonPos, 240.0f, 80.0f);
	isHoveringSetting	= CollisionBB(MousePos, 1.0f, 1.0f, SettingButtonPos, 60.0f, 60.0f);

	bool isPaws = CollisionBB(MousePos, 1.0f, 1.0f, PawsPos, 40.0f, 40.0f);

	MENU* MENU = GetMenu();
	if (MENU->use == TRUE) {
		return;
	}

	if (isHoveringStart && inputMode == 0) {
		menu = MENU_START;

		// 從非 hover → hover 時播放
		if (!wasHoveringStart) {
			PlaySound(SOUND_LABEL_SE_SWITCHBOTTON);
		}
	}
	if (isHoveringExit && inputMode == 0) {
		menu = MENU_EXIT;

		// 從非 hover → hover 時播放
		if (!wasHoveringExit) {
			PlaySound(SOUND_LABEL_SE_SWITCHBOTTON);
		}
	}

	if (isHoveringSetting && inputMode == 0) {
		menu = MENU_SETTING;

		// 從非 hover → hover 時播放
		if (!wasHoveringSetting) {
			PlaySound(SOUND_LABEL_SE_SWITCHBOTTON);
		}
	}

	//状態更新
	wasHoveringStart = isHoveringStart;
	wasHoveringExit = isHoveringExit;
	wasHoveringSetting = isHoveringSetting;


		if (GetKeyboardTrigger(DIK_UP))
		{
			PlaySound(SOUND_LABEL_SE_SWITCHBOTTON);
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
			PlaySound(SOUND_LABEL_SE_SWITCHBOTTON);
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
		menu = MENU_SETTING;
	}

	if (IsMouseLeftTriggered()&& isPaws) {
		PlaySound(SOUND_LABEL_EG_PAWS);
	}


	if (GetKeyboardTrigger(DIK_RETURN)|| IsMouseLeftTriggered())
	{// Enter押したら、ステージを切り替える
		
		if (menu == MENU_START)
		{
			PlaySound(SOUND_LABEL_SE_ENTERBOTTON);
			SetFade(FADE_OUT, MODE_TUTORIAL);
		}
		else if (menu == MENU_SETTING) {
			PlaySound(SOUND_LABEL_SE_ENTERBOTTON);
		}
		else if (menu == MENU_EXIT)
		{
			PlaySound(SOUND_LABEL_SE_ENTERBOTTON);
			PostQuitMessage(0);
		}
	}


	


#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	/*PrintDebugProc("menu:%d", menu);*/
	IXAudio2SubmixVoice* SubmixBGM = GetSubmixBGM();

	SubmixBGM->GetVolume(&volume);

	PrintDebugProc("Volume: %f", volume);
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

	//セッティングボタンを描画
	{
		
		if (menu == MENU_SETTING) {
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);
		}
		else {
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);
		}
		
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, 900.0f, 500.0f, 60.0f, 60.0f, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}

bool GetIsHovering(MENU_BUTTON target) {
	switch (target) {
	case MENU_START		: return isHoveringStart;
	case MENU_EXIT		: return isHoveringExit;
	case MENU_SETTING	: return isHoveringSetting;
	default: return false;
	}
}



