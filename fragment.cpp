//=============================================================================
//
// フィールド表示処理 [fragment.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "fragment.h"
#include "imgui.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX		(4)						// テクスチャの数

#define MAX_POLYGON		(6)						// キューブ１個あたりの面数

#define	VALUE_MOVE		(5.0f)					// 移動量
#define	VALUE_ROTATE	(XM_PI * 0.02f)			// 回転量

#define	SIZE_WH			(100.0f)				// 地面のサイズ

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexFragment(void);


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点情報
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static FRAGMENT						g_Fragment[TEXTURE_MAX];				// ポリゴンデータ
static int							g_TexNo;				// テクスチャ番号

static char* g_TextureName[] = {
	"data/TEXTURE/cat_01.png",
	"data/TEXTURE/cat_02.png",
	"data/TEXTURE/cat_03.png",
	"data/TEXTURE/cat_04.png",

};

// 頂点配列
static VERTEX_3D g_VertexArray[4 * MAX_POLYGON] = {
			// ３Ｄ座標							頂点の向き						ＲＧＢＡ					ＴＥＸ座標
	
   // 后面
   {XMFLOAT3(-SIZE_WH,  SIZE_WH, -SIZE_WH), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f)},
   {XMFLOAT3( SIZE_WH,  SIZE_WH, -SIZE_WH), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f)},
   {XMFLOAT3(-SIZE_WH, -SIZE_WH, -SIZE_WH), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f)},
   {XMFLOAT3( SIZE_WH, -SIZE_WH, -SIZE_WH), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f)},
  
   

};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitFragment(void)
{
	// 頂点バッファの作成
	MakeVertexFragment();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		// 位置・回転・スケールの初期設定
		g_Fragment[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f); // 配置座標
		g_Fragment[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f); // 回転
		g_Fragment[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f); // 拡大縮小率 X,Y,Z
		g_Fragment[i].overallPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	}

		
	
	
	

	g_TexNo = 0;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitFragment(void)
{
	// 頂点バッファの解放
	if (g_VertexBuffer) 
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	// テクスチャの解放
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
void UpdateFragment(void)
{
	CAMERA *cam = GetCamera();

#ifdef _DEBUG
	
	//猫の頭
	//if (GetKeyboardPress(DIK_LEFT))
	//{
	//	g_Fragment[0].overallPos.x -= 1.0f;
	//}
	//if (GetKeyboardPress(DIK_RIGHT))
	//{
	//	g_Fragment[0].overallPos.x += 1.0f;
	//}
	//if (GetKeyboardPress(DIK_UP))
	//{
	//	g_Fragment[0].overallPos.y += 1.0f;
	//}
	//if (GetKeyboardPress(DIK_DOWN))
	//{
	//	g_Fragment[0].overallPos.y -= 1.0f;
	//}
	//if (GetKeyboardPress(DIK_M))
	//{
	//	g_Fragment[0].overallPos.z -= 1.0f;
	//}
	//if (GetKeyboardPress(DIK_N))
	//{
	//	g_Fragment[0].overallPos.z += 1.0f;
	//}

	//猫の体
	/*if (GetKeyboardPress(DIK_LEFT))
	{
		g_Fragment[0].overallPos.x -= 1.0f;
	}
	if (GetKeyboardPress(DIK_RIGHT))
	{
		g_Fragment[0].overallPos.x += 1.0f;
	}
	if (GetKeyboardPress(DIK_UP))
	{
		g_Fragment[0].overallPos.y += 1.0f;
	}
	if (GetKeyboardPress(DIK_DOWN))
	{
		g_Fragment[0].overallPos.y -= 1.0f;
	}
	if (GetKeyboardPress(DIK_M))
	{
		g_Fragment[0].overallPos.z -= 1.0f;
	}
	if (GetKeyboardPress(DIK_N))
	{
		g_Fragment[0].overallPos.z += 1.0f;
	}*/

	//猫の手
	if (GetKeyboardPress(DIK_LEFT))
	{
		g_Fragment[2].overallPos.x -= 1.0f;
	}
	if (GetKeyboardPress(DIK_RIGHT))
	{
		g_Fragment[2].overallPos.x += 1.0f;
	}
	if (GetKeyboardPress(DIK_UP))
	{
		g_Fragment[2].overallPos.y += 1.0f;
	}
	if (GetKeyboardPress(DIK_DOWN))
	{
		g_Fragment[2].overallPos.y -= 1.0f;
	}
	if (GetKeyboardPress(DIK_M))
	{
		g_Fragment[2].overallPos.z -= 1.0f;
	}
	if (GetKeyboardPress(DIK_N))
	{
		g_Fragment[2].overallPos.z += 1.0f;
	}

	//猫の尻尾
	/*if (GetKeyboardPress(DIK_LEFT))
	{
		g_Fragment[0].overallPos.x -= 1.0f;
	}
	if (GetKeyboardPress(DIK_RIGHT))
	{
		g_Fragment[0].overallPos.x += 1.0f;
	}
	if (GetKeyboardPress(DIK_UP))
	{
		g_Fragment[0].overallPos.y += 1.0f;
	}
	if (GetKeyboardPress(DIK_DOWN))
	{
		g_Fragment[0].overallPos.y -= 1.0f;
	}
	if (GetKeyboardPress(DIK_M))
	{
		g_Fragment[0].overallPos.z -= 1.0f;
	}
	if (GetKeyboardPress(DIK_N))
	{
		g_Fragment[0].overallPos.z += 1.0f;
	}*/

	//猫の頭
	if (GetKeyboardPress(DIK_O))
	{
		g_Fragment[0].scl.x -= 0.01f;
	}
	if (GetKeyboardPress(DIK_P))
	{
		g_Fragment[0].scl.x += 0.01f;
	}
	if (GetKeyboardPress(DIK_O))
	{
		g_Fragment[0].scl.y -= 0.01f;
	}
	if (GetKeyboardPress(DIK_P))
	{
		g_Fragment[0].scl.y += 0.01f;
	}

	//猫の体
	if (GetKeyboardPress(DIK_U))
	{
		g_Fragment[1].scl.x -= 0.01f;
	}
	if (GetKeyboardPress(DIK_I))
	{
		g_Fragment[1].scl.x += 0.01f;
	}
	if (GetKeyboardPress(DIK_U))
	{
		g_Fragment[1].scl.y -= 0.01f;
	}
	if (GetKeyboardPress(DIK_I))
	{
		g_Fragment[1].scl.y += 0.01f;
	}

	//猫の手
	if (GetKeyboardPress(DIK_H))
	{
		g_Fragment[2].scl.x -= 0.01f;
	}
	if (GetKeyboardPress(DIK_J))
	{
		g_Fragment[2].scl.x += 0.01f;
	}
	if (GetKeyboardPress(DIK_H))
	{
		g_Fragment[2].scl.y -= 0.01f;
	}
	if (GetKeyboardPress(DIK_J))
	{
		g_Fragment[2].scl.y += 0.01f;
	}

	//猫の尻尾
	if (GetKeyboardPress(DIK_K))
	{
		g_Fragment[3].scl.x -= 0.01f;
	}
	if (GetKeyboardPress(DIK_L))
	{
		g_Fragment[3].scl.x += 0.01f;
	}
	if (GetKeyboardPress(DIK_K))
	{
		g_Fragment[3].scl.y -= 0.01f;
	}
	if (GetKeyboardPress(DIK_L))
	{
		g_Fragment[3].scl.y += 0.01f;
	}

#endif



#ifdef _DEBUG	// デバッグ情報を表示する

	/*PrintDebugProc("Field:↑→↓←\n");
	PrintDebugProc("cat head: X:%f Y:%f Z:%f\n", g_Fragment[0].overallPos.x, g_Fragment[0].overallPos.y, g_Fragment[0].overallPos.z);
	PrintDebugProc("cat body: X:%f Y:%f Z:%f\n", g_Fragment[1].overallPos.x, g_Fragment[1].overallPos.y, g_Fragment[1].overallPos.z);
	PrintDebugProc("cat head: X:%f Y:%f Z:%f\n", g_Fragment[2].overallPos.x, g_Fragment[2].overallPos.y, g_Fragment[2].overallPos.z);
	PrintDebugProc("cat tail: X:%f Y:%f Z:%f\n", g_Fragment[3].overallPos.x, g_Fragment[3].overallPos.y, g_Fragment[3].overallPos.z);
	PrintDebugProc("cat scale head: X:%f Y:%f Z:%f\n", g_Fragment[0].scl.x, g_Fragment[0].scl.y);
	PrintDebugProc("cat scale body: X:%f Y:%f Z:%f\n", g_Fragment[1].scl.x, g_Fragment[1].scl.y);
	PrintDebugProc("cat scale head: X:%f Y:%f Z:%f\n", g_Fragment[2].scl.x, g_Fragment[2].scl.y);
	PrintDebugProc("cat scale tail: X:%f Y:%f Z:%f\n", g_Fragment[3].scl.x, g_Fragment[3].scl.y);*/



#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawFragment(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	/*for (int i = 0; i < g_Field.historyCount; ++i) {
		XMMATRIX mtxTranslate = XMMatrixTranslation(g_Field.history[i].x, g_Field.history[i].y, g_Field.history[i].z);
		SetWorldMatrix(&mtxTranslate);
		GetDeviceContext()->Draw(4, 0); 
	}*/

	SetCullingMode(CULL_MODE_NONE);
	SetLightEnable(FALSE);
	SetDepthEnable(FALSE);

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		
		XMMATRIX mtxWorld = XMMatrixIdentity();
		XMMATRIX mtxScl = XMMatrixScaling(g_Fragment[i].scl.x, g_Fragment[i].scl.y, g_Fragment[i].scl.z);
		XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Fragment[i].rot.x, g_Fragment[i].rot.y, g_Fragment[i].rot.z);
		XMMATRIX mtxTranslate = XMMatrixTranslation(g_Fragment[i].overallPos.x, g_Fragment[i].overallPos.y + 60.0f, g_Fragment[i].overallPos.z+200.0f);
		mtxWorld = mtxScl * mtxRot * mtxTranslate;

		/*g_Fragment[0].overallPos = XMFLOAT3(10.0f, 0.0f, 60.0f);
		g_Fragment[1].overallPos = XMFLOAT3(30.0f, 0.0f, 40.0f);
		g_Fragment[2].overallPos = XMFLOAT3(5.0f,  0.0f, 80.0f);
		g_Fragment[3].overallPos = XMFLOAT3(70.0f, 0.0f, 90.0f);*/

		g_Fragment[0].overallPos = XMFLOAT3(-513.0f, -4.0f, 402.0f);
		g_Fragment[1].overallPos = XMFLOAT3(-511.0f, -4.0f, 291.0f);
		g_Fragment[2].overallPos = XMFLOAT3(-518.0f, -4.0f, 280.0f);
		g_Fragment[3].overallPos = XMFLOAT3(-518.0f, -5.0f, 223.0f);


		g_Fragment[0].scl = XMFLOAT3(1.21f, 1.21f, 0.0f);
		g_Fragment[1].scl = XMFLOAT3(0.87f, 0.87f, 0.0f);
		g_Fragment[2].scl = XMFLOAT3(0.84f, 0.84f, 0.0f);
		g_Fragment[3].scl = XMFLOAT3(0.9f, 0.9f, 0.0f);


		/*g_Fragment[0].scl = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Fragment[1].scl = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Fragment[2].scl = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Fragment[3].scl = XMFLOAT3(0.0f, 0.0f, 0.0f);*/

		// 猫の正しい見つかり位置座標
		// -507 , 57 , 70 - 80 

		// ワールドマトリックスの初期化
		SetWorldMatrix(&mtxWorld);
		XMStoreFloat4x4(&g_Fragment[i].mtxWorld, mtxWorld);

		// テクスチャー設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i]);

		//ポリゴン描画
		GetDeviceContext()->Draw(4, 0);  
	}

	SetLightEnable(TRUE);
	SetDepthEnable(TRUE);

	
	
}


HRESULT MakeVertexFragment(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4 * MAX_POLYGON;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// グローバルの頂点配列から頂点バッファをつくる
	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = g_VertexArray;

	GetDevice()->CreateBuffer(&bd, &sd, &g_VertexBuffer);

	return S_OK;
}

void DrawPartDebugUI()
{
	ImGui::Begin("Part Debug");
	ImGui::Text("Position Head Part: (%.2f, %.2f, %.2f)", g_Fragment[0].overallPos.x, g_Fragment[0].overallPos.y, g_Fragment[0].overallPos.z);
	ImGui::Text("Position Body Part: (%.2f, %.2f, %.2f)", g_Fragment[1].overallPos.x, g_Fragment[1].overallPos.y, g_Fragment[1].overallPos.z);
	ImGui::Text("Position Hand Part: (%.2f, %.2f, %.2f)", g_Fragment[2].overallPos.x, g_Fragment[2].overallPos.y, g_Fragment[2].overallPos.z);
	ImGui::Text("Position Tail Part: (%.2f, %.2f, %.2f)", g_Fragment[3].overallPos.x, g_Fragment[3].overallPos.y, g_Fragment[3].overallPos.z);
	ImGui::Text("Scl Head Part: (%.2f, %.2f, %.2f)", g_Fragment[0].scl.x, g_Fragment[0].scl.y);
	ImGui::Text("Scl Body Part: (%.2f, %.2f, %.2f)", g_Fragment[1].scl.x, g_Fragment[1].scl.y);
	ImGui::Text("Scl Hand Part: (%.2f, %.2f, %.2f)", g_Fragment[2].scl.x, g_Fragment[2].scl.y);
	ImGui::Text("Scl Tail Part: (%.2f, %.2f, %.2f)", g_Fragment[3].scl.x, g_Fragment[3].scl.y);
	ImGui::End();
}