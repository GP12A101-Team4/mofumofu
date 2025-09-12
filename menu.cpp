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
#include "fade.h"
#include "debugproc.h"
#include "imgui.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(130)	// キャラサイズ
#define TEXTURE_HEIGHT				(TEXTURE_WIDTH*0.8)	// 
#define TEXTURE_MAX					(9)		// テクスチャの数
#define cnt_max						(5)
#define OFFSET						(60)
#define BAR_WIDTH					(300)
#define HB_OFFSET					(30.0f)
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/menubg.png",
	"data/TEXTURE/bar1.png",
	"data/TEXTURE/bar2.png",
	"data/TEXTURE/mute_button.png",
	"data/TEXTURE/mute_button_muted.png",
	"data/TEXTURE/bgmfont.png",
	"data/TEXTURE/sefont.png",
	"data/TEXTURE/back_to_title.png",
	"data/TEXTURE/back_to_title_selected.png",
	
};

static tex g_tex[TEXTURE_MAX];

static MENU g_menu;

static SPRITE g_sprite[MAX];


static float bgmVolume;
static float seVolume;

static float bgmVolume_org;
static float seVolume_org;

static bool muteBGM;
static bool muteSE;

bool isHoveringMuteBGM;
bool isHoveringMuteSE;
bool isHoveringBackToTitle;

static bool wasHoveringMuteBGM = false;
static bool wasHoveringMuteSE = false;
static bool wasHoveringBackToTitle = false;

XMFLOAT3 MuteBGMPos;
XMFLOAT3 MuteSEPos;
XMFLOAT3 BackToTitlePos;


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

	g_sprite[BAR_MASTER] = {			(SCREEN_CENTER_X +10.0f ) - BAR_WIDTH/2,
										SCREEN_CENTER_Y - 80.0f,
										BAR_WIDTH,
										20.0f,

										1.0f,
										1.0f,
										0.0f,
										0.0f};

	g_sprite[BAR_MASTER_CURRENT] = {	(SCREEN_CENTER_X +10.0f ) - BAR_WIDTH / 2,
										SCREEN_CENTER_Y - 80.0f,
										BAR_WIDTH,
										20.0f,

										1.0f,				
										1.0f,				
										0.0f,				
										0.0f};

	g_sprite[BAR_SE] = {				(SCREEN_CENTER_X +10.0f ) - BAR_WIDTH / 2,
										SCREEN_CENTER_Y - 80 + OFFSET,
										BAR_WIDTH,
										20.0f,

										1.0f,
										1.0f,
										0.0f,
										0.0f };

	g_sprite[BAR_SE_CURRENT] = {		(SCREEN_CENTER_X + 10.0f ) - BAR_WIDTH / 2,
										SCREEN_CENTER_Y - 80.0f + OFFSET,
										BAR_WIDTH,
										20.0f,

										1.0f,
										1.0f,
										0.0f,
										0.0f };

	g_sprite[BUTTON_BGM_MUTE] = {		SCREEN_CENTER_X + 30.0f,
										SCREEN_CENTER_Y ,
										90.0f,
										90.0f,

										1.0f/2,
										1.0f/2,
										0.5f,
										0.0f };

	g_sprite[BUTTON_SE_MUTE] = { 		SCREEN_CENTER_X + 90.0f,
										SCREEN_CENTER_Y ,
										90.0f,
										90.0f,

										1.0f/2,
										1.0f/2,
										0.0f,
										0.0f };

	g_sprite[BGM_FONT] = {				SCREEN_CENTER_X - 140.0f,
										SCREEN_CENTER_Y - 105.0f ,
										174.0f / 3,
										66.0f / 3,

										1.0f,
										1.0f,
										0.0f,
										0.0f };

	g_sprite[SE_FONT] = {				SCREEN_CENTER_X - 140.0f,
										SCREEN_CENTER_Y - 105.0f + OFFSET,
										104.0f / 3,
										62.0f / 3,

										1.0f,
										1.0f,
										0.0f,
										0.0f };

	g_sprite[BACK_TO_TITLE_BUTTON] = {	SCREEN_CENTER_X - 461.0f / 4,
										SCREEN_CENTER_Y  + 50.0f + OFFSET,
										461.0f / 2.0f,
										109.0f / 2.0f,

										1.0f,
										1.0f,
										0.0f,
										0.0f };



	MuteBGMPos = { g_sprite[BUTTON_BGM_MUTE].px + g_sprite[BUTTON_BGM_MUTE].pw / 2, g_sprite[BUTTON_BGM_MUTE].py + g_sprite[BUTTON_BGM_MUTE].ph / 2 , 0 };
	MuteSEPos = { g_sprite[BUTTON_SE_MUTE].px + g_sprite[BUTTON_SE_MUTE].pw / 2, g_sprite[BUTTON_SE_MUTE].py + g_sprite[BUTTON_SE_MUTE].ph / 2 , 0 };
	BackToTitlePos = { g_sprite[BACK_TO_TITLE_BUTTON].px + g_sprite[BACK_TO_TITLE_BUTTON].pw / 2, g_sprite[BACK_TO_TITLE_BUTTON].py + g_sprite[BACK_TO_TITLE_BUTTON].ph / 2 , 0 };
	
	IXAudio2SubmixVoice* SubmixBGM = GetSubmixBGM();
	IXAudio2SubmixVoice* SubmixSE = GetSubmixSE();
	SubmixBGM->GetVolume(&bgmVolume);
	SubmixSE->GetVolume(&seVolume);

	muteBGM = false;
	muteSE = false;

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

	XMFLOAT3 MousePos;
	MousePos.x = float(GetMousePosX());
	MousePos.y = float(GetMousePosY());

	//ボタン当たり判定
	isHoveringMuteBGM = CollisionBB(MousePos, 1.0f, 1.0f, MuteBGMPos, g_sprite[BUTTON_BGM_MUTE].pw, g_sprite[BUTTON_BGM_MUTE].ph);
	isHoveringMuteSE = CollisionBB(MousePos, 1.0f, 1.0f, MuteSEPos, g_sprite[BUTTON_SE_MUTE].pw, g_sprite[BUTTON_SE_MUTE].ph);
	isHoveringBackToTitle = CollisionBB(MousePos, 1.0f, 1.0f, BackToTitlePos, g_sprite[BACK_TO_TITLE_BUTTON].pw, g_sprite[BACK_TO_TITLE_BUTTON].ph);

	//当たったら
	if (isHoveringMuteBGM) {

		//左クリックしたらMute/Unmute
		if (IsMouseLeftTriggered()) { 
			muteBGM = muteBGM ? false : true; 
			PlaySound(SOUND_LABEL_SE_ENTERBOTTON);

			if(muteBGM){
			bgmVolume_org = bgmVolume;
			bgmVolume = 0.0f;
			}
			else {
				bgmVolume = bgmVolume_org;
			}

		}

		// !hover → hoverで流す 
		if (!wasHoveringMuteBGM) {
			PlaySound(SOUND_LABEL_SE_SWITCHBOTTON);
		}
	}

	//当たったら
	if (isHoveringMuteSE) {

		//左クリックしたらMute/Unmute
		if (IsMouseLeftTriggered()) {
			muteSE = muteSE ? false : true;
			PlaySound(SOUND_LABEL_SE_ENTERBOTTON);

			if (muteSE == true) {
				seVolume_org = seVolume;
				seVolume = 0.0f;
			}
			else if (muteSE == false){
				seVolume = seVolume_org;
			}
		}

		// !hover → hoverで流す 
		if (!wasHoveringMuteSE) {
			PlaySound(SOUND_LABEL_SE_SWITCHBOTTON);
		}
	}
	//int mode = GetMode();
	if(GetMode()== MODE_GAME){
		//当たったら
		if (isHoveringBackToTitle) {

			if (IsMouseLeftTriggered()) {
				PlaySound(SOUND_LABEL_SE_ENTERBOTTON);
				SetFade(FADE_OUT, MODE_TITLE);
			}

			// !hover → hoverで流す 
			if (!wasHoveringBackToTitle) {
				PlaySound(SOUND_LABEL_SE_SWITCHBOTTON);
			}
		}
	}


	//状態更新
	wasHoveringMuteBGM = isHoveringMuteBGM;
	wasHoveringMuteSE = isHoveringMuteSE;
	wasHoveringBackToTitle = isHoveringBackToTitle;


	if (CollisionBB(MousePos, 1.0f, 1.0f,
		XMFLOAT3(g_sprite[BAR_MASTER].px + g_sprite[BAR_MASTER].pw / 2,
			g_sprite[BAR_MASTER].py + g_sprite[BAR_MASTER].ph / 2, 0.0f),
		g_sprite[BAR_MASTER].pw + HB_OFFSET, g_sprite[BAR_MASTER].ph) && IsMouseLeftPressed()) {

		bgmVolume = (MousePos.x - g_sprite[BAR_MASTER].px) / g_sprite[BAR_MASTER].pw;

		if (bgmVolume < 0.0f) bgmVolume = 0.0f;
		if (bgmVolume > 1.0f) bgmVolume = 1.0f;

		g_sprite[BAR_MASTER_CURRENT].pw = BAR_WIDTH * bgmVolume;
	}

	if (CollisionBB(MousePos, 1.0f, 1.0f,
		XMFLOAT3(g_sprite[BAR_SE].px + g_sprite[BAR_SE].pw / 2,
			g_sprite[BAR_SE].py + g_sprite[BAR_SE].ph / 2, 0.0f),
		g_sprite[BAR_SE].pw + HB_OFFSET, g_sprite[BAR_SE].ph) && IsMouseLeftPressed()) {

		seVolume = (MousePos.x - g_sprite[BAR_SE].px) / g_sprite[BAR_SE].pw;

		if (seVolume < 0.0f) seVolume = 0.0f;
		if (seVolume > 1.0f) seVolume = 1.0f;

		g_sprite[BAR_SE_CURRENT].pw = BAR_WIDTH * seVolume;
	}

	g_sprite[BAR_MASTER_CURRENT].pw = BAR_WIDTH * bgmVolume;
	g_sprite[BAR_SE_CURRENT].pw = BAR_WIDTH * seVolume;
	
	SetBGMVolume(bgmVolume);
	SetSEVolume(seVolume);

#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	PrintDebugProc("MasterVolume : %f", bgmVolume);
	PrintDebugProc("SeVolume : %f", seVolume);
	PrintDebugProc("isHoveringMuteSE : %d", isHoveringMuteSE);

	
	
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


			// メニューを描画
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

				float px = g_sprite[BAR_MASTER].px;
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

				float px = g_sprite[BAR_SE].px;
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

			// BGM MUTE
			{
				// テクスチャ設定
				if (!muteBGM) {
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

				}
				else {
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

				}

				float px = g_sprite[BUTTON_BGM_MUTE].px;
				float py = g_sprite[BUTTON_BGM_MUTE].py;
				float pw = g_sprite[BUTTON_BGM_MUTE].pw;	// スコアの表示幅
				float ph = g_sprite[BUTTON_BGM_MUTE].ph;	// スコアの表示高さ

				float tw = g_sprite[BUTTON_BGM_MUTE].tw;	// テクスチャの幅
				float th = g_sprite[BUTTON_BGM_MUTE].th;	// テクスチャの高さ
				float tx = g_sprite[BUTTON_BGM_MUTE].tx;	// テクスチャの左上X座標
				float ty = g_sprite[BUTTON_BGM_MUTE].ty;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

			// SE MUTE
			{
				// テクスチャ設定
				if (!muteSE) {
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);
				}
				else {
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);
				}
				

				float px = g_sprite[BUTTON_SE_MUTE].px;
				float py = g_sprite[BUTTON_SE_MUTE].py;
				float pw = g_sprite[BUTTON_SE_MUTE].pw;	// スコアの表示幅
				float ph = g_sprite[BUTTON_SE_MUTE].ph;	// スコアの表示高さ

				float tw = g_sprite[BUTTON_SE_MUTE].tw;	// テクスチャの幅
				float th = g_sprite[BUTTON_SE_MUTE].th;	// テクスチャの高さ
				float tx = g_sprite[BUTTON_SE_MUTE].tx;	// テクスチャの左上X座標
				float ty = g_sprite[BUTTON_SE_MUTE].ty;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

			// BGM FONT
			{
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);
	


				float px = g_sprite[BGM_FONT].px;
				float py = g_sprite[BGM_FONT].py;
				float pw = g_sprite[BGM_FONT].pw;	// スコアの表示幅
				float ph = g_sprite[BGM_FONT].ph;	// スコアの表示高さ

				float tw = g_sprite[BGM_FONT].tw;	// テクスチャの幅
				float th = g_sprite[BGM_FONT].th;	// テクスチャの高さ
				float tx = g_sprite[BGM_FONT].tx;	// テクスチャの左上X座標
				float ty = g_sprite[BGM_FONT].ty;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}

			// SE FONT
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);


				float px = g_sprite[SE_FONT].px;
				float py = g_sprite[SE_FONT].py;
				float pw = g_sprite[SE_FONT].pw;	// スコアの表示幅
				float ph = g_sprite[SE_FONT].ph;	// スコアの表示高さ

				float tw = g_sprite[SE_FONT].tw;	// テクスチャの幅
				float th = g_sprite[SE_FONT].th;	// テクスチャの高さ
				float tx = g_sprite[SE_FONT].tx;	// テクスチャの左上X座標
				float ty = g_sprite[SE_FONT].ty;	// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
			//int mode = GetMode();
			//Back To Title
			if (GetMode() == MODE_GAME) {
					
					// テクスチャ設定
					if (isHoveringBackToTitle){
					GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);
					}
					else {
						GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);
					}

					float px = g_sprite[BACK_TO_TITLE_BUTTON].px;
					float py = g_sprite[BACK_TO_TITLE_BUTTON].py;
					float pw = g_sprite[BACK_TO_TITLE_BUTTON].pw;	// スコアの表示幅
					float ph = g_sprite[BACK_TO_TITLE_BUTTON].ph;	// スコアの表示高さ

					float tw = g_sprite[BACK_TO_TITLE_BUTTON].tw;	// テクスチャの幅
					float th = g_sprite[BACK_TO_TITLE_BUTTON].th;	// テクスチャの高さ
					float tx = g_sprite[BACK_TO_TITLE_BUTTON].tx;	// テクスチャの左上X座標
					float ty = g_sprite[BACK_TO_TITLE_BUTTON].ty;	// テクスチャの左上Y座標

					// １枚のポリゴンの頂点とテクスチャ座標を設定
					SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
						XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

					// ポリゴン描画
					GetDeviceContext()->Draw(4, 0);
					}
			}

}

MENU* GetMenu() {
	return &g_menu ;
}




void DrawDebugMenu()
{
	ImGui::Begin("Menu Debug");

		ImGui::Text("IsMuteBGM : %d",isHoveringMuteBGM );
		ImGui::Text("IsMuteSE : %d", isHoveringMuteSE);

		ImGui::Text("IsMuteBGM o/f : %d", muteBGM);
		ImGui::Text("IsMuteSE  o/f : %d", muteSE);

		ImGui::Text("MuteBGMPos : %f, %f, %f", MuteBGMPos.x, MuteBGMPos.y, MuteBGMPos.z);
		ImGui::Text("MuteSEPos : %f, %f, %f", MuteSEPos.x, MuteSEPos.y, MuteSEPos.z);

		/*ImGui::Text("MODE : %d", mode);*/
	ImGui::End();

}