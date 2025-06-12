//=============================================================================
//
// ゲーム画面処理 [game.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "sound.h"
#include "player.h"
#include "meshfield.h"
#include "shadow.h"
#include "score.h"
#include "ui.h"
#include "collision.h"
#include "debugproc.h"
#include "meshwall.h"
#include "fragment.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************





//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;


//static BOOL	g_bPause = FALSE;	// ポーズON/OFF

RANDOM	g_random;
UI		g_Ui;


float yaw;
float pitch;



//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGame(void)
{
	g_Ui.pause = FALSE;

	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// フィールドの初期化
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 100, 13.0f, 13.0f);

	// ライトを有効化	// 影の初期化処理
	InitShadow();

	// 欠片の初期処理
	InitFragment();

	// プレイヤーの初期化
	InitPlayer();


	// スコアの初期化
	InitScore();

	// スコアの初期化
	InitUI();

	// BGM再生
	//PlaySound(SOUND_LABEL_BGM_game);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGame(void)
{
	// スコアの終了処理
	UninitScore();

	UninitUI();

	// 地面の終了処理
	UninitMeshField();

	// プレイヤーの終了処理
	UninitPlayer();

	// 影の終了処理
	UninitShadow();
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Game);
	}
#endif
	if (GetKeyboardTrigger(DIK_P))
	{
		g_Ui.pause = g_Ui.pause ? FALSE : TRUE;
	}

	if (GetKeyboardTrigger(DIK_RETURN)) {
		SetFade(FADE_OUT, MODE_RESULT);
	}
	

	/*if (g_Ui.pause == TRUE) {
		UpdateUi();
		return;
	}*/
	
	

	// 地面処理の更新
	UpdateMeshField();

	// プレイヤーの更新処理
	UpdatePlayer();

	//欠片の更新処理
	UpdateFragment();

	// 影の更新処理
	//UpdateShadow();

	// スコアの更新処理
	UpdateScore();

	UpdateUI();

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGame0(void)
{
	// 3Dの物を描画する処理
	// 地面の描画処理
	DrawMeshField();

	// 影の描画処理
	//DrawShadow();

	// プレイヤーの描画処理
	DrawPlayer();

	
	// 壁の描画処理
	DrawMeshWall();

	//欠片の描画処理
	DrawFragment();
	
	// 2Dの物を描画する処理
	// Z比較なし
	SetDepthEnable(FALSE);

	// ライティングを無効
	SetLightEnable(FALSE);

	// スコアの描画処理
	DrawScore();

	DrawUI();

	DrawGaugeBars();

	
	/*if(g_Ui.pause == TRUE)
	DrawUi();*/

	// ライティングを有効に
	SetLightEnable(TRUE);

	// Z比較あり
	SetDepthEnable(TRUE);
}


void DrawGame(void)
{

#ifdef _DEBUG
	// デバッグ表示
	/*PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);*/

#endif
	CAMERA* camera = GetCamera();
	PLAYER* player = GetPlayer();

	{
		float dist = 10.0f;
		XMFLOAT3 dir = GetCameraDir();
		
		//ベクトルを拡大する　Camera.atとCamera.posの値が同じになるバグを防ぐため 
		dir.x *= dist;
		dir.y *= dist;
		dir.z *= dist;

		camera->dir = { camera->pos.x + dir.x,
						camera->pos.y + dir.y,
						camera->pos.z + dir.z };

		SetCameraAT(camera->dir);
		SetCamera();
		


		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();	//OBJ描画処理
	}

	

}

//回転平滑化
float turning(float target, float current) {
	target = NormalizeAngle(target);
	current = NormalizeAngle(current);

	float diff = target - current;
	diff = NormalizeAngle(diff);

	return current + diff / 10.0f;
}

float NormalizeAngle(float angle) {
	while (angle > XM_PI) angle -= 2 * XM_PI;
	while (angle < -XM_PI) angle += 2 * XM_PI;
	return angle;
}

//カメラ角度制御
XMFLOAT3 GetCameraDir() {

	int deltaX = GetMouseX();
	/*int deltaZ = GetMouseY();*/

	XMFLOAT3 dir;

	yaw += deltaX * 0.4f;
	/*pitch -= deltaZ * 0.4f;*/

	if (pitch > 89.0f) { pitch = 89.0f; }
	if (pitch < -89.0f) { pitch = -89.0f; }

	float yawRad = DirectX::XMConvertToRadians(yaw);
	float pitchRad = DirectX::XMConvertToRadians(pitch);

	dir = { sinf(yawRad) * cosf((pitchRad)),
		sinf(pitchRad), 
		cosf(yawRad) * cosf(pitchRad) };

	//正規化
	XMVECTOR v = XMVector3Normalize(XMLoadFloat3(&dir));
	XMStoreFloat3(&dir, v);

	return dir;
}

//カメラ上下角度計算
float GetCameraYaw(XMFLOAT3 dir, XMFLOAT3 pos) {
	float deltaX = dir.x - pos.x;
	float deltaZ = dir.z - pos.z;

	float yaw = atan2f(deltaX, deltaZ);

	return yaw;
}

////カメラ左右角度計算
float GetCameraPitch(XMFLOAT3 dir, XMFLOAT3 pos) {
	float deltaY = dir.y - pos.y;
	float deltaX = dir.x - pos.x;
	float deltaZ = dir.z - pos.z;

	float XY = (float)sqrt(deltaX * deltaX + deltaZ * deltaZ);

	float pitch = atan2f(deltaY, XY);



	return pitch;
}


RANDOM* GetRandom() {
	return &g_random;
}

UI* GetUi() {
	return &g_Ui;
}