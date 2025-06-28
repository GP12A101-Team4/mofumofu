//=============================================================================
//
// モデル処理 [player.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "collision.h"
#include "sound.h"
#include "imgui.h"
#include "game.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_PLAYER		"data/MODEL/player.obj"			// 読み込むモデル名

#define	VALUE_MOVE			(2.0f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define PLAYER_SHADOW_SIZE	(0.4f)							// 影の大きさ
#define PLAYER_OFFSET_Y		(7.0f)							// プレイヤーの足元をあわせる


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER				g_Player;						// プレイヤー


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	LoadModel(MODEL_PLAYER, &g_Player.model);
	g_Player.load = TRUE;

	g_Player.pos = { 0.0f +130.0f , PLAYER_OFFSET_Y + 50.0f, 0.0f -10.0f};
	g_Player.rot = { 0.0f, 0.0f, 0.0f };
	g_Player.scl = { 1.0f, 1.0f, 1.0f };

	g_Player.spd = 0.0f;			// 移動スピードクリア
	g_Player.size = PLAYER_SIZE;	// 当たり判定の大きさ

	g_Player.ammor = 0.0f;

	g_Player.use = TRUE;

	// ここでプレイヤー用の影を作成している
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);
	//          ↑
	//        このメンバー変数が生成した影のIndex番号


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	// モデルの解放処理
	if (g_Player.load)
	{
		UnloadModel(&g_Player.model);
		g_Player.load = FALSE;
	}


}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{

	float inputX = 0.0f;
	float inputY = 0.0f;

	// 移动输入
	if (GetKeyboardPress(DIK_A)) inputX += 1.0f;
	if (GetKeyboardPress(DIK_D)) inputX -= 1.0f;
	if (GetKeyboardPress(DIK_W)) inputY -= 1.0f;
	if (GetKeyboardPress(DIK_S)) inputY += 1.0f;

	// 合成输入向量的长度
	float len = sqrtf(inputX * inputX + inputY * inputY);

	// 若有输入
	if (len > 0.0f)
	{
		// 单位化向量
		inputX /= len;
		inputY /= len;

		// 设置方向(以摄像机角度为基准 + 输入方向)
		CAMERA* camera = GetCamera();
		float cameraYaw = GetCameraYaw(camera->dir, camera->pos);
		float inputDir = atan2f(inputX, inputY); // ← 注意是YX顺序

		g_Player.dir = inputDir;
		g_Player.rot.y = cameraYaw + inputDir;

		// 移动
		float moveX = -sinf(g_Player.rot.y) * VALUE_MOVE;
		float moveZ = -cosf(g_Player.rot.y) * VALUE_MOVE;
		g_Player.pos.x += moveX;
		g_Player.pos.z += moveZ;
	}
	
#ifdef _DEBUG
	if (GetKeyboardPress(DIK_R))
	{
		g_Player.pos.z = g_Player.pos.x = 0.0f;
		g_Player.rot.y = g_Player.dir = 0.0f;
		g_Player.spd = 0.0f;
	}
#endif

	

	CAMERA* camera = GetCamera();
	g_Player.rot.y = GetCameraYaw(camera->dir, camera->pos)+ g_Player.dir;
	//	// Key入力があったら移動処理する
	if (g_Player.spd != 0.0f)
	{
		float moveX = -sinf(g_Player.rot.y) * g_Player.spd;
		float moveY = -cosf(g_Player.rot.y) * g_Player.spd;
		

		// 入力のあった方向へプレイヤーを向かせて移動させる
		g_Player.pos.x += moveX;
		g_Player.pos.z += moveY;

		
	}

	// 影もプレイヤーの位置に合わせる
	XMFLOAT3 pos = g_Player.pos;
	pos.y -= (PLAYER_OFFSET_Y - 0.1f);
	SetPositionShadow(g_Player.shadowIdx, pos);


	g_Player.spd *= 0.5f;



	//{	// ポイントライトのテスト
	//	LIGHT *light = GetLightData(1);
	//	XMFLOAT3 pos = g_Player.pos;
	//	pos.y += 20.0f;

	//	light->Position = pos;
	//	light->Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//	light->Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	//	light->Type = LIGHT_TYPE_POINT;
	//	light->Enable = TRUE;
	//	SetLightData(1, light);
	//}

	long X = GetMouseX();
	long Y = GetMouseY();
	long Z = GetMouseZ();

	

#ifdef _DEBUG	// デバッグ情報を表示する
	//PrintDebugProc("Player:↑ → ↓ ←　Space\n");
	//PrintDebugProc("Player:X:%f Y:%f Z:%f\n", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	//PrintDebugProc("X:%d Y:%d Z:%d \n", X, Y, Z);
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// ワールドマトリックスの初期化
	mtxWorld = XMMatrixIdentity();

	// スケールを反映
	mtxScl = XMMatrixScaling(g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

	// 回転を反映
	mtxRot = XMMatrixRotationRollPitchYaw(g_Player.rot.x, g_Player.rot.y + XM_PI, g_Player.rot.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

	// 移動を反映
	mtxTranslate = XMMatrixTranslation(g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	XMStoreFloat4x4(&g_Player.mtxWorld, mtxWorld);


	// モデル描画
	DrawModel(&g_Player.model);

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

void DrawPlayerDebugUI()
{
	ImGui::Begin("Player Debug");
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	ImGui::End();
}


