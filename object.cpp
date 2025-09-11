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

#include <fstream>
#include <string>

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_TREE			"data/MODEL/tree.obj"			// 読み込むモデル名
#define	MODEL_LAMP			"data/MODEL/lamp.obj"			// 読み込むモデル名
#define	MODEL_PLANT			"data/MODEL/plant.obj"			// 読み込むモデル名
#define	MODEL_BENCH			"data/MODEL/bench.obj"			// 読み込むモデル名
#define	MODEL_SHOP_1		"data/MODEL/shop.obj"			// 読み込むモデル名
#define	MODEL_SHOP_2		"data/MODEL/shop_second.obj"			// 読み込むモデル名
#define	MODEL_SHOP_3		"data/MODEL/shop_third.obj"			// 読み込むモデル名
#define	MODEL_FOUNTAIN		"data/MODEL/fountain.obj"			// 読み込むモデル名
#define	MODEL_CAFE			"data/MODEL/cafe.obj"			// 読み込むモデル名
#define	MODEL_BUILDING		"data/MODEL/building.obj"			// 読み込むモデル名
#define	MODEL_BURGER_SHOP	"data/MODEL/burger.obj"			// 読み込むモデル名
#define	MODEL_CANDY_SHOP	"data/MODEL/candy_shop.obj"			// 読み込むモデル名
#define	MODEL_ICE_TRUCK		"data/MODEL/ice_truck.obj"			// 読み込むモデル名
#define	MODEL_RESTAURANT	"data/MODEL/restaurant.obj"			// 読み込むモデル名




#define	VALUE_MOVE			(2.0f)							// 移動量
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// 回転量

#define OBJECT_SHADOW_SIZE	(0.4f)							// 影の大きさ
#define OBJECT_OFFSET_Y		(7.0f)							// プレイヤーの足元をあわせる

#define OBJECT_MAX			(20)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void SaveObjectPositions(const char* filename);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static OBJECT				g_Object[object_max];						// プレイヤー

static int					Target_Object = 0;

const char* objectNames[] = {
	"tree_1",
	"tree_2",
	"tree_3",
	"tree_4",
	"tree_5",
	"tree_6",
	"tree_7",
	"tree_8",
	"tree_9",
	"tree_last",

	"lamp_1",
	"lamp_2",
	"lamp_3",
	"lamp_4",
	"lamp_5",
	"lamp_6",
	"lamp_7",
	"lamp_last",

	"plant_1",
	"plant_2",
	"plant_3",
	"plant_4",
	"plant_5",
	"plant_last",

	"bench_1",
	"bench_2",
	"bench_3",
	"bench_4",
	"bench_5",
	"bench_last",

	"shop_1",
	"shop_2",
	"shop_3",

	"fountain",
	"cafe",
	"building",
	"burger_shop",
	"candy_shop",
	"ice_truck",
	"restaurant"
};

static inline void ComputeAABB(OBJECT& o)
{
	XMFLOAT3 half = {
		o.aabbHalf.x * fabsf(o.scl.x),
		o.aabbHalf.y * fabsf(o.scl.y),
		o.aabbHalf.z * fabsf(o.scl.z),
	};
	o.aabbMin = { o.pos.x - half.x, o.pos.y - half.y, o.pos.z - half.z };
	o.aabbMax = { o.pos.x + half.x, o.pos.y + half.y, o.pos.z + half.z };
}

BOOL TestAABBOverlap(const OBJECT& a, const OBJECT& b)
{
	return !(a.aabbMax.x < b.aabbMin.x || a.aabbMin.x > b.aabbMax.x ||
		a.aabbMax.y < b.aabbMin.y || a.aabbMin.y > b.aabbMax.y ||
		a.aabbMax.z < b.aabbMin.z || a.aabbMin.z > b.aabbMax.z);
}

static ID3D11Buffer* s_pAABBLineVB = NULL;

static void EnsureAABBLineVB()
{
	if (s_pAABBLineVB) return;
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, nullptr, &s_pAABBLineVB);
}

static void DrawAABB_Wire(const OBJECT& o)
{
	if (!s_pAABBLineVB) EnsureAABBLineVB();
	if (!s_pAABBLineVB) return;

	GetDeviceContext()->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	XMFLOAT3 mn = o.aabbMin, mx = o.aabbMax;
	XMFLOAT3 p[8] = {
		{mn.x,mn.y,mn.z},{mx.x,mn.y,mn.z},{mx.x,mx.y,mn.z},{mn.x,mx.y,mn.z},
		{mn.x,mn.y,mx.z},{mx.x,mn.y,mx.z},{mx.x,mx.y,mx.z},{mn.x,mx.y,mx.z}
	};
	int e[24] = { 0,1,1,2,2,3,3,0, 4,5,5,6,6,7,7,4, 0,4,1,5,2,6,3,7 };

	VERTEX_3D v[24]{};
	for (int i = 0; i < 24; ++i) {
		v[i].Position = p[e[i]];
		v[i].Normal = XMFLOAT3(0, 0, 0);
		v[i].Diffuse = XMFLOAT4(1, 1, 1, 1); // 不依赖：材质里会上色
		v[i].TexCoord = XMFLOAT2(0, 0);
	}

	D3D11_MAPPED_SUBRESOURCE msr{};
	if (SUCCEEDED(GetDeviceContext()->Map(s_pAABBLineVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr)))
	{
		memcpy(msr.pData, v, sizeof(v));
		GetDeviceContext()->Unmap(s_pAABBLineVB, 0);
	}
	else {
		OutputDebugStringA("Map AABB VB failed\n");
		return;
	}

	XMMATRIX I = XMMatrixIdentity();
	SetWorldMatrix(&I);

	MATERIAL m{};
	m.Diffuse = o.isColliding ? XMFLOAT4(0, 0, 0, 1) : XMFLOAT4(0, 0, 0, 1);
	m.Ambient = m.Diffuse;
	m.noTexSampling = 1;
	SetMaterial(m);

	UINT stride = sizeof(VERTEX_3D), offset = 0;
	ID3D11Buffer* vb = s_pAABBLineVB;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	GetDeviceContext()->Draw(24, 0);
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitObject(void)
{
	for (int i = 0; i < object_max; i++) {
		if (i < tree_last + 1) {
			LoadModel(MODEL_TREE, &g_Object[i].model);
		}
		else if (i < lamp_last + 1) {
			LoadModel(MODEL_LAMP, &g_Object[i].model);
		}
		else if (i < plant_last + 1) {
			LoadModel(MODEL_PLANT, &g_Object[i].model);
		}
		else if (i < bench_last + 1) {
			LoadModel(MODEL_BENCH, &g_Object[i].model);
		}
		g_Object[i].load = TRUE;

		g_Object[i].pos = { 0.0f + i * 90.0f, 0.0f, 0.0f };
		g_Object[i].rot = { 0.0f, 0.0f, 0.0f };
		//g_Object[i].scl = { 0.1f, 0.1f, 0.1f };
		g_Object[i].scl = { 1.0f, 1.0f, 1.0f };

		g_Object[i].size = OBJECT_SIZE;	// 当たり判定の大きさ
		g_Object[i].use = TRUE;

		g_Object[i].aabbHalf = { 100.0f, 100.0f, 100.0f };

		if (i <= tree_last)                 g_Object[i].aabbHalf = { 70.0f, 200.0f, 70.0f };
		else if (i <= lamp_last)            g_Object[i].aabbHalf = { 30.0f, 200.0f,  30.0f };
		else if (i <= plant_last)           g_Object[i].aabbHalf = { 40.0f, 160.0f, 40.0f };
		else if (i <= bench_last)           g_Object[i].aabbHalf = { 100.0f,  100.0f,  60.0f };

		else if (i == shop_1 || i == shop_2 || i == shop_3)
			g_Object[i].aabbHalf = { 350.0f, 250.0f, 250.0f };
		else if (i == fountain)             g_Object[i].aabbHalf = { 90.0f, 220.0f, 90.0f };
		else if (i == cafe)                 g_Object[i].aabbHalf = { 420.0f, 260.0f, 320.0f };
		else if (i == building)             g_Object[i].aabbHalf = { 200.0f, 400.0f, 200.0f };
		else if (i == burger_shop)          g_Object[i].aabbHalf = { 380.0f, 260.0f, 300.0f };
		else if (i == candy_shop)           g_Object[i].aabbHalf = { 380.0f, 260.0f, 300.0f };
		else if (i == ice_truck)            g_Object[i].aabbHalf = { 220.0f, 160.0f, 120.0f };
		else if (i == restaurant)           g_Object[i].aabbHalf = { 200.0f, 300.0f, 200.0f };

		g_Object[i].showAABB = TRUE;
		g_Object[i].isColliding = FALSE;
		ComputeAABB(g_Object[i]);

		//// ここでプレイヤー用の影を作成している
		//XMFLOAT3 pos = g_Object[i].pos;
		//pos.y -= (OBJECT_OFFSET_Y - 0.1f);
		//g_Object[i].shadowIdx = CreateShadow(pos, OBJECT_SHADOW_SIZE, OBJECT_SHADOW_SIZE);
		////          ↑
		////        このメンバー変数が生成した影のIndex番号
	}

	LoadModel(MODEL_SHOP_1, &g_Object[shop_1].model);
	LoadModel(MODEL_SHOP_2, &g_Object[shop_2].model);
	LoadModel(MODEL_SHOP_3, &g_Object[shop_3].model);
	LoadModel(MODEL_FOUNTAIN, &g_Object[fountain].model);
	LoadModel(MODEL_CAFE, &g_Object[cafe].model);
	LoadModel(MODEL_BUILDING, &g_Object[building].model);
	LoadModel(MODEL_BURGER_SHOP, &g_Object[burger_shop].model);
	LoadModel(MODEL_CANDY_SHOP, &g_Object[candy_shop].model);
	LoadModel(MODEL_ICE_TRUCK, &g_Object[ice_truck].model);
	LoadModel(MODEL_RESTAURANT, &g_Object[restaurant].model);
	
	g_Object[bench_1].scl = { 0.6f,0.6f,0.6f };
	g_Object[bench_2].scl = { 0.6f,0.6f,0.6f };
	g_Object[bench_3].scl = { 0.6f,0.6f,0.6f };
	g_Object[bench_4].scl = { 0.6f,0.6f,0.6f };
	g_Object[bench_5].scl = { 0.6f,0.6f,0.6f };
	g_Object[bench_last].scl = { 0.6f,0.6f,0.6f };
	


	g_Object[tree_1].pos = { -430.0f,0.0f,1980.0f };
	g_Object[tree_2].pos = { -1710.0f,0.0f,950.0f };
	g_Object[tree_3].pos = { 1140.0f,0.0f,1600.0f };
	g_Object[tree_4].pos = { -600.0f,0.0f,490.0f };
	g_Object[tree_5].pos = { -1980.0f,0.0f,-280.0f };
	g_Object[tree_6].pos = { 1950.0f,0.0f,480.0f };
	g_Object[tree_7].pos = { 660.0f,0.0f,-310.0f };
	g_Object[tree_8].pos = { -1480.0f,0.0f,-1360.0f };
	g_Object[tree_9].pos = { 1640.0f,0.0f,-880.0f };
	g_Object[tree_last].pos = { 330.0f,0.0f,-1800.0f };
	
	g_Object[lamp_1].pos = { 510.0f,0.0f,2100.0f };
	g_Object[lamp_2].pos = { -2020.0f,0.0f,390.0f };
	g_Object[lamp_3].pos = { -310.0f,0.0f,650.0f };
	g_Object[lamp_4].pos = { -700.0f,0.0f,-200.0f };
	g_Object[lamp_5].pos = { 310.0f,0.0f,-520.0f };
	g_Object[lamp_6].pos = { -440.0f,0.0f,-1580.0f };
	g_Object[lamp_7].pos = { 1930.0f,0.0f,-230.0f };
	g_Object[lamp_last].pos = { -310.0f,0.0f,-1920.0f };

	g_Object[plant_1].pos = { -270.0f,0.0f,1350.0f };
	g_Object[plant_2].pos = { 1810.0f,0.0f,1980.0f };
	g_Object[plant_3].pos = { 340.0f,0.0f,690.0f };
	g_Object[plant_4].pos = { -310.0f,0.0f,-550.0f };
	g_Object[plant_5].pos = { -2000.0f,0.0f,-1760.0f };
	g_Object[plant_last].pos = { 1960.0f,0.0f,-1760.0f };

	g_Object[bench_1].pos = { -300.0f,0.0f,1550.0f };
	g_Object[bench_1].rot = { 0.0f,4.60767f,0.0f };
	g_Object[bench_2].pos = { -1460.0f,0.0f,310.0f };
	g_Object[bench_2].rot = { 0.0f,0.f,0.0f };
	g_Object[bench_3].pos = { 490.0f,0.0f,520.0f };
	g_Object[bench_3].rot = { 0.0f,0.767947f,0.0f };
	g_Object[bench_4].pos = { -500.0f,0.0f,-450.0f };
	g_Object[bench_4].rot = { 0.0f,3.90954f,0.0f };
	g_Object[bench_5].pos = { 1400.0f,0.0f,-280.0f };
	g_Object[bench_5].rot = { 0.0f,3.21141f,0.0f };
	g_Object[bench_last].pos = { 270.0f,0.0f,-1390.0f };
	g_Object[bench_last].rot = { 0.0f,1.53589f,0.0f };

	g_Object[shop_1].pos = { -970.0f, 0.0f, 2020.0f };
	g_Object[shop_1].rot = { 0.0f, 0.0f, 0.0f };                 

	g_Object[shop_2].pos = { 840.0f, 0.0f, 2070.0f };
	g_Object[shop_2].rot = { 0.0f, -XM_PI / 2.0f, 0.0f };        

	g_Object[shop_3].pos = { 1290.0f, 0.0f, 2040.0f };
	g_Object[shop_3].rot = { 0.0f, XM_PI / 2, 0.0f };                

	g_Object[fountain].pos = { 0.0f, 0.0f, 50.0f };
	g_Object[fountain].rot = { 0.0f, 0.0f, 0.0f };               

	g_Object[cafe].pos = { -2080.0f,0.0f,-1120.0f };
	g_Object[cafe].rot = { 0.0f,XM_PI,0.0f };



	g_Object[building].pos = { -1480.0f,0.0f,2000.0f };
	g_Object[building].rot = { 0.0f,0.0f,0.0f };


	g_Object[burger_shop].pos = { 2020.0f,0.0f,850.0f };
	g_Object[burger_shop].rot = { 0.0f, XM_PI, 0.0f }; 


	g_Object[candy_shop].pos = { -2060.0f, 0.0f, -830.0f };
	g_Object[candy_shop].rot = { 0.0f, XM_PI, 0.0f };            

	g_Object[ice_truck].pos = { 1250.0f, 0.0f, -1190.0f };
	g_Object[ice_truck].rot = { 0.0f, -3.0f * XM_PI / 4.0f, 0.0f }; 

	g_Object[restaurant].pos = { -1040.0f,0.0f,-2000.0f };
	g_Object[restaurant].rot = { 0.0f,0.0f,0.0f };





	//g_Object[]



	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitObject(void)
{
	for (int i = 0; i < object_max; i++) {
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

	for (int i = 0; i < object_max; ++i) {
		g_Object[i].isColliding = FALSE;
		ComputeAABB(g_Object[i]);
	}

	for (int i = 0; i < object_max; ++i) {
		for (int j = i + 1; j < object_max; ++j) {
			if (TestAABBOverlap(g_Object[i], g_Object[j])) {
				g_Object[i].isColliding = TRUE;
				g_Object[j].isColliding = TRUE;
			}
		}
	}


#ifdef _DEBUG	// デバッグ情報を表示する
	if(GetIsSetting()){
		if (GetKeyboardTrigger(DIK_1)) {
			Target_Object += 1;
			if (Target_Object > object_max) {
				Target_Object = 0;
			}
		}
		else if(GetKeyboardTrigger(DIK_2)) {
			Target_Object -= 1;
			if (Target_Object < 0) {
				Target_Object = object_max;
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
		else if (GetKeyboardPress(DIK_O)) {
			g_Object[Target_Object].rot.y -= XM_PI / 45;
		}
		else if (GetKeyboardPress(DIK_P)) {
			g_Object[Target_Object].rot.y += XM_PI / 45;
		}
		else if (GetKeyboardPress(DIK_U)) {
			XMVECTOR scale = XMLoadFloat3(&g_Object[Target_Object].scl);
			scale *= 0.99f; 
			XMStoreFloat3(&g_Object[Target_Object].scl, scale);
		}
		else if (GetKeyboardPress(DIK_I)) {
			XMVECTOR scale = XMLoadFloat3(&g_Object[Target_Object].scl);
			scale *= 1.01f;
			XMStoreFloat3(&g_Object[Target_Object].scl, scale);
		}

		if (GetKeyboardTrigger(DIK_L)) // 按 L 鍵存檔
		{
			SaveObjectPositions("object_positions.txt");
		}
		
		g_Object[Target_Object].rot.y = fmodf(g_Object[Target_Object].rot.y, XM_2PI);

		if (g_Object[Target_Object].rot.y < 0.0f)
		{
			g_Object[Target_Object].rot.y += XM_2PI;
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

#ifdef _DEBUG
		if (GetIsSetting())
		{
			for (int j = 0; j < g_Object[i].model.SubsetNum; j++)
			{
				if (i == Target_Object)
				{
					// 選中 → 改紅色
					SetModelDiffuse(&g_Object[i].model, j, XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
				}
				else
				{
					// 未選中 → 還原原始顏色
					SetModelDiffuse(&g_Object[i].model, j, g_Object[i].model.SubsetArray[j].Diffuse_Original);
				}
			}

			
			
		}
		else {
			for (int j = 0; j < g_Object[i].model.SubsetNum; j++)
			{
					SetModelDiffuse(&g_Object[i].model, j, g_Object[i].model.SubsetArray[j].Diffuse_Original);
			}
		}
#endif

		

		// モデル描画
		DrawModel(&g_Object[i].model);
	}

	XMMATRIX I = XMMatrixIdentity();
	SetWorldMatrix(&I);

	/*for (int i = 0; i < object_max; i++) {
		DrawAABB_Wire(g_Object[i]);
	}*/

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
		ImGui::Text("%s Pos: (%.2f, %.2f, %.2f)",
			objectNames[i],
			g_Object[i].pos.x,
			g_Object[i].pos.y,
			g_Object[i].pos.z);

		/*ImGui::Text("%s Rot: (%.2f, %.2f, %.2f)",
			objectNames[i],
			g_Object[i].rot.x,
			g_Object[i].rot.y,
			g_Object[i].rot.z);*/

		/*ImGui::Text("%s Scl: (%.2f, %.2f, %.2f)",
			objectNames[i],
			g_Object[i].scl.x,
			g_Object[i].scl.y,
			g_Object[i].scl.z);*/
	}

	ImGui::End();
}

void SaveObjectPositions(const char* filename)
{
	std::ofstream ofs(filename);
	if (!ofs.is_open())
	{
		MessageBoxA(NULL, "Failed to open file for writing!", "Error", MB_OK);
		return;
	}

	for (int i = 24; i < object_max; i++)
	{
		ofs << "g_Object[" << objectNames[i] << "].pos = {"
			<< g_Object[i].pos.x << ".0f,"
			<< g_Object[i].pos.y << ".0f,"
			<< g_Object[i].pos.z << ".0f};\n";

		ofs << "g_Object[" << objectNames[i] << "].rot = {"
			<< g_Object[i].rot.x << "f,"
			<< g_Object[i].rot.y << "f,"
			<< g_Object[i].rot.z << "f};\n";

		ofs << "g_Object[" << objectNames[i] << "].scl = {"
			<< g_Object[i].scl.x << "f,"
			<< g_Object[i].scl.y << "f,"
			<< g_Object[i].scl.z << "f};\n";
	}

	ofs.close();
}

void SetObjectAABBHalf(int idx, XMFLOAT3 half)
{
	if (idx < 0 || idx >= object_max) return;
	g_Object[idx].aabbHalf = half;
	ComputeAABB(g_Object[idx]);
}
