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
#include "object.h"
#include "shadow.h"
#include "light.h"
#include "collision.h"
#include "sound.h"
#include "imgui.h"
#include "game.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_TREE			"data/MODEL/tree.obj"			// 読み込むモデル名
#define	MODEL_LAMP			"data/MODEL/lamp.obj"			// 読み込むモデル名
#define	MODEL_FOUNTAIN			"data/MODEL/fountain.obj"			// 読み込むモデル名

#define	VALUE_MOVE			(2.0f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define OBJECT_SHADOW_SIZE	(0.4f)							// 影の大きさ
#define OBJECT_OFFSET_Y		(7.0f)							// プレイヤーの足元をあわせる

#define OBJECT_MAX			(20)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static OBJECT				g_Object[object_max];						// プレイヤー

static int					Target_Object = 0;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitObject(void)
{
	for(int i = 0;i< object_max;i++){
		if (i < tree_first) {
			LoadModel(MODEL_FOUNTAIN, &g_Object[i].model);
		}
		else if(i<lamp_first){
		LoadModel(MODEL_TREE, &g_Object[i].model);
		}
		else if (i < object_max ) {
		LoadModel(MODEL_LAMP, &g_Object[i].model);
		}
		g_Object[i].load = TRUE;

		g_Object[i].pos = { 0.0f, 0.0f, 0.0f };
		g_Object[i].rot = { 0.0f, 0.0f, 0.0f };
		g_Object[i].scl = { 0.1f, 0.1f, 0.1f };

		g_Object[i].size = OBJECT_SIZE;	// 当たり判定の大きさ
		g_Object[i].use = TRUE;

		// ここでプレイヤー用の影を作成している
		XMFLOAT3 pos = g_Object[i].pos;
		pos.y -= (OBJECT_OFFSET_Y - 0.1f);
		g_Object[i].shadowIdx = CreateShadow(pos, OBJECT_SHADOW_SIZE, OBJECT_SHADOW_SIZE);
		//          ↑
		//        このメンバー変数が生成した影のIndex番号
	}

	g_Object[fountain].pos =	{ 0.0f,0.0f,60.0f };
	g_Object[tree_first].pos =	{ 290.0f,0.0f,2150.0f };
	g_Object[tree_2].pos =		{ -360.0f,0.0f,1800.0f };
	g_Object[tree_3].pos =		{ -1380.0f,0.0f,330.0f };
	g_Object[tree_4].pos =		{ -2070.0f,0.0f,-330.0f };
	g_Object[tree_5].pos =		{ 1980.0f,0.0f,230.0f };
	g_Object[tree_6].pos =		{ 320.0f,0.0f,-728.0f };
	g_Object[tree_7].pos =		{ 730.0f,0.0f,428.0f };
	g_Object[tree_last].pos =	{ -360.0f,0.0f,-900.0f };
	g_Object[lamp_first].pos =		{ -1910.0f,0.0f,-180.0f };
	g_Object[lamp_2].pos =		{ -650.0f,0.0f,320.0f };
	g_Object[lamp_3].pos =		{ -320.0f,0.0f,2030.0f };
	g_Object[lamp_4].pos =		{ 310.0f,0.0f,618.0f };
	g_Object[lamp_5].pos =		{ 1550.0f,0.0f,0268.0f };
	g_Object[lamp_6].pos =		{ 610.0f,0.0f,-230.0f };
	g_Object[lamp_7].pos =		{ -410.0f,0.0f,520.0f };
	g_Object[lamp_last].pos =	{ 200.0f,0.0f,-1270.0f };


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitObject(void)
{
	for (int i = 0; i < OBJECT_MAX; i++) {
		// モデルの解放処理
		if (g_Object[i].load)
		{
			UnloadModel(&g_Object[i].model);
			g_Object[i].load = FALSE;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateObject(void)
{

	

	//g_Object[Target_Object].




#ifdef _DEBUG	// デバッグ情報を表示する
	if(GetIsSetting()){
		if (GetKeyboardTrigger(DIK_TAB)) {
			Target_Object += 1;
			if (Target_Object > object_max) {
				Target_Object = 0;
			}
		}

		if (GetKeyboardPress(DIK_UP)) {
			g_Object[Target_Object].pos.z += 10.0f;
		}
		else if (GetKeyboardPress(DIK_DOWN)) {
			g_Object[Target_Object].pos.z -= 10.0f;
		}
		else if (GetKeyboardPress(DIK_RIGHT)) {
			g_Object[Target_Object].pos.x += 10.0f;
		}
		else if (GetKeyboardPress(DIK_LEFT)) {
			g_Object[Target_Object].pos.x -= 10.0f;
		}
	}

	PrintDebugProc("Target: %d", Target_Object);
#endif
}

//=========================================:====================================
// 描画処理
//=============================================================================
void DrawObject(void)
{

	for (int i = 0; i < object_max; i++) {
		// カリング無効
		SetCullingMode(CULL_MODE_NONE);

		XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

		// ワールドマトリックスの初期化
		mtxWorld = XMMatrixIdentity();

		// スケールを反映
		mtxScl = XMMatrixScaling(g_Object[i].scl.x, g_Object[i].scl.y, g_Object[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// 回転を反映
		mtxRot = XMMatrixRotationRollPitchYaw(g_Object[i].rot.x, g_Object[i].rot.y + XM_PI, g_Object[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// 移動を反映
		mtxTranslate = XMMatrixTranslation(g_Object[i].pos.x, g_Object[i].pos.y, g_Object[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ワールドマトリックスの設定
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Object[i].mtxWorld, mtxWorld);

		if(GetIsSetting()){
			if (i == Target_Object) {
				for (int j = 0; j < g_Object[i].model.SubsetNum; j++)
				{
					SetModelDiffuse(&g_Object[i].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
				}
			}
		}
		

		// モデル描画
		DrawModel(&g_Object[i].model);

		SetFuchi(0);
		for (int j = 0; j < g_Object[i].model.SubsetNum; j++)
		{
			SetModelDiffuse(&g_Object[i].model, j, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}
	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
OBJECT *GetObject(void)
{
	return &g_Object[0];
}

void ShowObjectDebugWindow()
{
	ImGui::Begin("Object Debug");

	for (int i = 0; i < object_max; i++)
	{
		ImGui::Text("Object %d Pos: (%.2f, %.2f, %.2f)",
			i,
			g_Object[i].pos.x,
			g_Object[i].pos.y,
			g_Object[i].pos.z);
	}

	ImGui::End();
}


