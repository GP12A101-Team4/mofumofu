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
#include "fragment_elephant.h"
#include "imgui.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX		(8)						// テクスチャの数

#define MAX_POLYGON		(6)						// キューブ１個あたりの面数

#define	VALUE_MOVE		(5.0f)					// 移動量
#define	VALUE_ROTATE	(XM_PI * 0.02f)			// 回転量

#define	SIZE_WH			(100.0f)				// 地面のサイズ

#define FRAGMENT_ROT_AMPLITUDE	(XM_PI / 10)
#define FRAGMENT_ANIM_TIME		(30)
#define ANIM_MOVE_SPEED			(2.0f)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


HRESULT MakeVertexFragment(void);

bool CheckPuzzleRestored(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************



static ID3D11Buffer			* g_VertexBuffer_Elph = NULL;	// 頂点情報
static ID3D11ShaderResourceView		* g_Texture_Elph[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static FRAGMENT				g_Fragment_Elph[TEXTURE_MAX];				// ポリゴンデータ
static FRAGMENT_RESTORED	g_FragmentRestored_Elph[TEXTURE_MAX];
static int				g_TexNo_Elph;				// テクスチャ番号


static XMFLOAT2 g_TargetScreenPos_Elph[TEXTURE_MAX] = {
	{510.5f, 191.2f},
	{522.7f, 197.8f},
	{537.8f, 203.9f},
	{522.1f, 202.2f},
	{480.3f, 187.0f},
	{520.3f, 203.3f},
	{481.0f, 191.7f},
	{0.0f, 0.0f}  // 第4张图是完整图，不需要判断
};

//static bool g_HasRecordedTarget = false;  // ← 直接设为 true


bool g_ShowFullImage_Elph = false;

static char* g_TextureName_Elph[] = {
	"data/TEXTURE/elephant_01.png",
	"data/TEXTURE/elephant_02.png",
	"data/TEXTURE/elephant_03.png",
	"data/TEXTURE/elephant_04.png",
	"data/TEXTURE/elephant_05.png",
	"data/TEXTURE/elephant_06.png",
	"data/TEXTURE/elephant_07.png",
	"data/TEXTURE/elephant.png",
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

void ComputePuzzleCenterAndScale_Elph(XMFLOAT3* outCenter, XMFLOAT3* outScale)
{
	XMFLOAT3 center = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 scale = { 0.0f, 0.0f, 0.0f };

	int count = TEXTURE_MAX - 1;

	for (int i = 0; i < count; i++)
	{
		center.x += g_Fragment_Elph[i].overallPos.x;
		center.y += g_Fragment_Elph[i].overallPos.y;
		center.z += g_Fragment_Elph[i].overallPos.z;

		scale.x += g_Fragment_Elph[i].scl.x;
		scale.y += g_Fragment_Elph[i].scl.y;
		scale.z += g_Fragment_Elph[i].scl.z;
	}

	float inv = 1.0f / count;

	if (outCenter)
	{
		outCenter->x = center.x * inv;
		outCenter->y = center.y * inv;
		outCenter->z = center.z * inv;
	}
	if (outScale)
	{
		outScale->x = scale.x * inv;
		outScale->y = scale.y * inv;
		outScale->z = 1.0f;  // 固定Z轴缩放
	}
}
//=============================================================================
// 初期化処理
//=============================================================================

HRESULT MakeVertexFragment_Elph(void)
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

	GetDevice()->CreateBuffer(&bd, &sd, &g_VertexBuffer_Elph);

	return S_OK;
}

HRESULT InitFragment_Elph(void)
{
	// 頂点バッファの作成
	MakeVertexFragment_Elph();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName_Elph[i],
			NULL,
			NULL,
			&g_Texture_Elph[i],
			NULL);
	}

	for (int i = 0; i < TEXTURE_MAX - 1; i++)

	{
		// 位置・回転・スケールの初期設定
		g_Fragment_Elph[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f); // 配置座標
		g_Fragment_Elph[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f); // 回転
		g_Fragment_Elph[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f); // 拡大縮小率 X,Y,Z
		g_Fragment_Elph[i].overallPos = XMFLOAT3(0.0f, 0.0f, 0.0f);

		//---------------------------------------------------------------------
		g_FragmentRestored_Elph[i].use = FALSE;
		g_FragmentRestored_Elph[i].AnimCnt = 0;
		g_FragmentRestored_Elph[i].alpha = 1.0f;
		g_FragmentRestored_Elph[i].Initialized = FALSE;
	}
	
	g_TexNo_Elph = 0;

	return S_OK;
}

bool CheckPuzzleRestored_Elph()
{
	D3D11_VIEWPORT vp;
	UINT num = 1;
	GetDeviceContext()->RSGetViewports(&num, &vp);

	CAMERA* cam = GetCamera();

	float tolerance = 18.0f;  // 可接受误差半径

	for (int i = 0; i < TEXTURE_MAX - 1; i++) {
		// 考虑缩放、旋转、平移后的世界坐标中心点
		XMMATRIX mtxScl = XMMatrixScaling(g_Fragment_Elph[i].scl.x, g_Fragment_Elph[i].scl.y, g_Fragment_Elph[i].scl.z);
		XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Fragment_Elph[i].rot.x, g_Fragment_Elph[i].rot.y, g_Fragment_Elph[i].rot.z);
		XMMATRIX mtxTranslate = XMMatrixTranslation(g_Fragment_Elph[i].overallPos.x, g_Fragment_Elph[i].overallPos.y + 60.0f, g_Fragment_Elph[i].overallPos.z + 200.0f);
		XMMATRIX mtxWorld = mtxScl * mtxRot * mtxTranslate;

		XMVECTOR worldCenter = XMVector3TransformCoord(XMVectorZero(), mtxWorld);

		XMVECTOR screen = XMVector3Project(
			worldCenter,
			0, 0,
			vp.Width, vp.Height,
			0.0f, 1.0f,
			XMLoadFloat4x4(&cam->mtxProjection),
			XMLoadFloat4x4(&cam->mtxView),
			XMMatrixIdentity()
		);

		float x = XMVectorGetX(screen);
		float y = XMVectorGetY(screen);

		float dx = x - g_TargetScreenPos_Elph[i].x;
		float dy = y - g_TargetScreenPos_Elph[i].y;
		float distance = sqrtf(dx * dx + dy * dy);

		if (distance > tolerance)
			return false;
	}

	return true;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitFragment_Elph(void)
{
	// 頂点バッファの解放
	if (g_VertexBuffer_Elph)
	{
		g_VertexBuffer_Elph->Release();
		g_VertexBuffer_Elph = NULL;
	}

	// テクスチャの解放
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture_Elph[i])
		{
			g_Texture_Elph[i]->Release();
			g_Texture_Elph[i] = NULL;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateFragment_Elph(void)
{
	CAMERA *cam = GetCamera();
	
	XMFLOAT3 center;
	ComputePuzzleCenterAndScale_Elph(&center, nullptr);

	if (!g_ShowFullImage_Elph && CheckPuzzleRestored_Elph())
	{
		g_ShowFullImage_Elph = true;
		g_ElphAnimationPlayed = true;
		OutputDebugStringA("✅ 判定成功，准备显示完整贴图\n");
	}

	if (g_ShowFullImage_Elph && !g_FragmentRestored_Elph[0].Initialized) {
		g_FragmentRestored_Elph[0].use = TRUE;

		XMFLOAT3 center, scale;
		ComputePuzzleCenterAndScale_Elph(&center, &scale);


		g_FragmentRestored_Elph[0].pos = center;
		g_FragmentRestored_Elph[0].scl = scale;
		g_FragmentRestored_Elph[0].rot = g_Fragment_Elph[0].rot;

		g_FragmentRestored_Elph[0].Initialized = TRUE;
	}

	if (g_FragmentRestored_Elph[0].use) {
		float moveX = cosf(g_FragmentRestored_Elph[0].rot.y) * ANIM_MOVE_SPEED;
		float moveZ = -sinf(g_FragmentRestored_Elph[0].rot.y) * ANIM_MOVE_SPEED;

		g_FragmentRestored_Elph[0].pos.x -= moveX;
		g_FragmentRestored_Elph[0].pos.z -= moveZ;

		float angle = (XM_PI / FRAGMENT_ANIM_TIME) * g_FragmentRestored_Elph[0].AnimCnt;

		g_FragmentRestored_Elph[0].AnimCnt++;

		float rot = FRAGMENT_ROT_AMPLITUDE * -cosf(angle);

		g_FragmentRestored_Elph[0].rot.z = rot;

		if (g_FragmentRestored_Elph[0].AnimCnt >= 2 * FRAGMENT_ANIM_TIME) {
			g_FragmentRestored_Elph[0].AnimCnt = 0;
		}

		g_FragmentRestored_Elph[0].alpha -= 0.005f;
	}

	if (g_FragmentRestored_Elph[0].alpha < 0) {
		g_FragmentRestored_Elph[0].use = FALSE;
		
	}
		
#ifdef _DEBUG
	

//Elephant
//if (GetKeyboardPress(DIK_LEFT))
//{
//	g_Fragment_Elph[6].overallPos.x -= 1.0f;
//}
//if (GetKeyboardPress(DIK_RIGHT))
//{
//	g_Fragment_Elph[6].overallPos.x += 1.0f;
//}
//if (GetKeyboardPress(DIK_UP))
//{
//	g_Fragment_Elph[6].overallPos.y += 1.0f;
//}
//if (GetKeyboardPress(DIK_DOWN))
//{
//	g_Fragment_Elph[6].overallPos.y -= 1.0f;
//}
//if (GetKeyboardPress(DIK_M))
//{
//	g_Fragment_Elph[6].overallPos.z -= 1.0f;
//}
//if (GetKeyboardPress(DIK_N))
//{
//	g_Fragment_Elph[6].overallPos.z += 1.0f;
//}
//if (GetKeyboardPress(DIK_O))
//{
//	g_Fragment_Elph[6].scl.x += 0.01f;
//}
//if (GetKeyboardPress(DIK_P))
//{
//	g_Fragment_Elph[6].scl.x -= 0.01f;
//}
//if (GetKeyboardPress(DIK_O))
//{
//	g_Fragment_Elph[6].scl.y += 0.01f;
//}
//if (GetKeyboardPress(DIK_P))
//{
//	g_Fragment_Elph[6].scl.y -= 0.01f;
//}


#endif



#ifdef _DEBUG	// デバッグ情報を表示する

	if (GetKeyboardTrigger(DIK_F2)) {
		D3D11_VIEWPORT vp;
		UINT num = 1;
		GetDeviceContext()->RSGetViewports(&num, &vp);

		CAMERA* cam = GetCamera();

		for (int i = 0; i < TEXTURE_MAX; i++) {
			XMVECTOR world = XMLoadFloat3(&g_Fragment_Elph[i].overallPos);
			XMVECTOR screen = XMVector3Project(
				world,
				0, 0,
				vp.Width, vp.Height,
				0.0f, 1.0f,
				XMLoadFloat4x4(&cam->mtxProjection),
				XMLoadFloat4x4(&cam->mtxView),
				XMMatrixIdentity()
			);

			g_TargetScreenPos_Elph[i].x = XMVectorGetX(screen);
			g_TargetScreenPos_Elph[i].y = XMVectorGetY(screen);
		}

		//g_HasRecordedTarget = true;
		OutputDebugStringA("✨ 已记录当前碎片的投影坐标作为正确答案\n");
	}


}
#endif

//=============================================================================
// 描画処理
//=============================================================================
void DrawFragment_Elph(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer_Elph, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// テクスチャ設定
	SetCullingMode(CULL_MODE_NONE);
	SetAlphaTestEnable(BLEND_MODE_ALPHABLEND);
	SetLightEnable(FALSE);
	SetDepthEnable(TRUE);

	if (!g_ShowFullImage_Elph)
	{
		//テクスチャー番号　自分調整できる
		int drawOrder[] = { 4, 6, 0, 1, 2, 3, 5 }; 

		for (int idx = 0; idx < TEXTURE_MAX - 1; idx++)
		{
			int i = drawOrder[idx];

			XMMATRIX mtxWorld = XMMatrixIdentity();
			XMMATRIX mtxScl = XMMatrixScaling(g_Fragment_Elph[i].scl.x, g_Fragment_Elph[i].scl.y, g_Fragment_Elph[i].scl.z);
			XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Fragment_Elph[i].rot.x, g_Fragment_Elph[i].rot.y, g_Fragment_Elph[i].rot.z);
			XMMATRIX mtxTranslate = XMMatrixTranslation(g_Fragment_Elph[i].overallPos.x, g_Fragment_Elph[i].overallPos.y + 60.0f, g_Fragment_Elph[i].overallPos.z + 200.0f);
			mtxWorld = mtxScl * mtxRot * mtxTranslate;

			/*g_Fragment_Elph[0].overallPos = XMFLOAT3(513.0f, -4.0f, 402.0f);
			g_Fragment_Elph[1].overallPos = XMFLOAT3(513.0f, -4.0f, 402.0f);
			g_Fragment_Elph[2].overallPos = XMFLOAT3(513.0f, -4.0f, 402.0f);
			g_Fragment_Elph[3].overallPos = XMFLOAT3(513.0f, -4.0f, 402.0f);
			g_Fragment_Elph[4].overallPos = XMFLOAT3(513.0f, -4.0f, 402.0f);
			g_Fragment_Elph[5].overallPos = XMFLOAT3(513.0f, -4.0f, 402.0f);
			g_Fragment_Elph[6].overallPos = XMFLOAT3(513.0f, -4.0f, 402.0f);


			g_Fragment_Elph[0].scl = XMFLOAT3(1.21f, 1.21f, 0.0f);
			g_Fragment_Elph[1].scl = XMFLOAT3(0.87f, 0.87f, 0.0f);
			g_Fragment_Elph[2].scl = XMFLOAT3(0.84f, 0.84f, 0.0f);
			g_Fragment_Elph[3].scl = XMFLOAT3(0.9f, 0.9f, 0.0f);
			g_Fragment_Elph[4].scl = XMFLOAT3(0.9f, 0.9f, 0.0f);
			g_Fragment_Elph[5].scl = XMFLOAT3(0.9f, 0.9f, 0.0f);
			g_Fragment_Elph[6].scl = XMFLOAT3(0.9f, 0.9f, 0.0f);
			*/

			g_Fragment_Elph[0].overallPos = XMFLOAT3(-504.0f, 117.0f, -726.0f);
			g_Fragment_Elph[1].overallPos = XMFLOAT3(-413.0f, 91.0f, -613.0f);
			g_Fragment_Elph[2].overallPos = XMFLOAT3(-756.0f, 128.0f, -901.0f);
			g_Fragment_Elph[3].overallPos = XMFLOAT3(-147.0f, 47.0f, -358.0f);
			g_Fragment_Elph[4].overallPos = XMFLOAT3(-597.0f, 141.0f, -818.0f);
			g_Fragment_Elph[5].overallPos = XMFLOAT3(-236.0f, 59.0f, -446.0f);
			g_Fragment_Elph[6].overallPos = XMFLOAT3(-623.0f, 137.0f, -842.0f);


			g_Fragment_Elph[0].scl = XMFLOAT3(2.27f, 2.27f, 1.0f);
			g_Fragment_Elph[1].scl = XMFLOAT3(2.17f, 2.17f, 1.0f);
			g_Fragment_Elph[2].scl = XMFLOAT3(2.28f, 2.28f, 1.0f);
			g_Fragment_Elph[3].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_Fragment_Elph[4].scl = XMFLOAT3(2.69f, 2.69f, 1.0f);
			g_Fragment_Elph[5].scl = XMFLOAT3(1.31f, 1.31f, 1.0f);
			g_Fragment_Elph[6].scl = XMFLOAT3(2.67f, 2.67f, 1.0f);
			

			g_Fragment_Elph[0].rot = XMFLOAT3(0.0f, -8.5f, 0.0f);
			g_Fragment_Elph[1].rot = XMFLOAT3(0.0f, -8.5f, 0.0f);
			g_Fragment_Elph[2].rot = XMFLOAT3(0.0f, -8.5f, 0.0f);
			g_Fragment_Elph[3].rot = XMFLOAT3(0.0f, -8.5f, 0.0f);
			g_Fragment_Elph[4].rot = XMFLOAT3(0.0f, -8.5f, 0.0f);
			g_Fragment_Elph[5].rot = XMFLOAT3(0.0f, -8.5f, 0.0f);
			g_Fragment_Elph[6].rot = XMFLOAT3(0.0f, -8.5f, 0.0f);


			// elephantの正しい見つかり位置座標
			// 

			// ワールドマトリックスの初期化
			SetWorldMatrix(&mtxWorld);
			XMStoreFloat4x4(&g_Fragment_Elph[i].mtxWorld, mtxWorld);

			// テクスチャー設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture_Elph[i]);

			//ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
	
	if (g_FragmentRestored_Elph[0].use)
	{
		XMMATRIX mtxWorld = XMMatrixIdentity();
		XMMATRIX mtxScl = XMMatrixScaling(g_FragmentRestored_Elph[0].scl.x, g_FragmentRestored_Elph[0].scl.y, g_FragmentRestored_Elph[0].scl.z);  // 你可以调整缩放
		XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_FragmentRestored_Elph[0].rot.x, g_FragmentRestored_Elph[0].rot.y, g_FragmentRestored_Elph[0].rot.z);
		XMMATRIX mtxTranslate = XMMatrixTranslation(g_FragmentRestored_Elph[0].pos.x, g_FragmentRestored_Elph[0].pos.y + 60.0f, g_FragmentRestored_Elph[0].pos.z + 200.0f);
		mtxWorld = mtxScl * mtxRot * mtxTranslate;

		SetWorldMatrix(&mtxWorld);

		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, g_FragmentRestored_Elph[0].alpha);
		SetMaterial(material);
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture_Elph[7]);  // 最后一张完整图

		GetDeviceContext()->Draw(4, 0);

	}

	SetLightEnable(TRUE);
	SetDepthEnable(TRUE);

	
	
}

float GetPuzzleAlignmentRatio_Elph()
{
	D3D11_VIEWPORT vp;
	UINT num = 1;
	GetDeviceContext()->RSGetViewports(&num, &vp);

	CAMERA* cam = GetCamera();

	const float maxDistance = 200.0f;  // 最大容忍误差
	float ratioSum = 0.0f;

	for (int i = 0; i < TEXTURE_MAX - 1; i++) {
		// 构建世界变换矩阵（缩放 → 旋转 → 平移）
		XMMATRIX mtxScl = XMMatrixScaling(g_Fragment_Elph[i].scl.x, g_Fragment_Elph[i].scl.y, g_Fragment_Elph[i].scl.z);
		XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Fragment_Elph[i].rot.x, g_Fragment_Elph[i].rot.y, g_Fragment_Elph[i].rot.z);
		XMMATRIX mtxTranslate = XMMatrixTranslation(g_Fragment_Elph[i].overallPos.x, g_Fragment_Elph[i].overallPos.y + 60.0f, g_Fragment_Elph[i].overallPos.z + 200.0f);
		XMMATRIX mtxWorld = mtxScl * mtxRot * mtxTranslate;

		XMVECTOR worldCenter = XMVector3TransformCoord(XMVectorZero(), mtxWorld);

		XMVECTOR screen = XMVector3Project(
			worldCenter,
			0, 0,
			vp.Width, vp.Height,
			0.0f, 1.0f,
			XMLoadFloat4x4(&cam->mtxProjection),
			XMLoadFloat4x4(&cam->mtxView),
			XMMatrixIdentity()
		);

		float x = XMVectorGetX(screen);
		float y = XMVectorGetY(screen);
		float dx = x - g_TargetScreenPos_Elph[i].x;
		float dy = y - g_TargetScreenPos_Elph[i].y;
		float distance = sqrtf(dx * dx + dy * dy);

		// 误差归一化处理
		float normalized = distance / maxDistance;
		if (normalized > 1.0f)
			normalized = 1.0f;

		float partRatio = 1.0f - normalized;
		ratioSum += partRatio;
	}

	return ratioSum / (TEXTURE_MAX - 1);  // 返回平均完成度 (0.0~1.0)
}



void DrawPartDebugUI_Elph()
{
	ImGui::Begin("Part Debug (Elph)");

	CAMERA* cam = GetCamera();
	D3D11_VIEWPORT vp;
	UINT num = 1;
	GetDeviceContext()->RSGetViewports(&num, &vp);

	float ratio = GetPuzzleAlignmentRatio_Elph();
	ImGui::Text("拼图完成度: %.1f%%", ratio * 100.0f);
	ImGui::ProgressBar(ratio, ImVec2(200, 20));

	ImGui::Separator();
	ImGui::Text("碎片中心位置（投影） vs 目标位置");

	for (int i = 0; i < TEXTURE_MAX - 1; i++) {
		// 构建世界矩阵
		XMMATRIX mtxScl = XMMatrixScaling(g_Fragment_Elph[i].scl.x, g_Fragment_Elph[i].scl.y, g_Fragment_Elph[i].scl.z);
		XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Fragment_Elph[i].rot.x, g_Fragment_Elph[i].rot.y, g_Fragment_Elph[i].rot.z);
		XMMATRIX mtxTranslate = XMMatrixTranslation(g_Fragment_Elph[i].overallPos.x, g_Fragment_Elph[i].overallPos.y + 60.0f, g_Fragment_Elph[i].overallPos.z + 200.0f);
		XMMATRIX mtxWorld = mtxScl * mtxRot * mtxTranslate;

		// 世界中心点
		XMVECTOR worldPos = XMVector3TransformCoord(XMVectorZero(), mtxWorld);

		// 投影到屏幕
		XMVECTOR screen = XMVector3Project(
			worldPos,
			0, 0,
			vp.Width, vp.Height,
			0.0f, 1.0f,
			XMLoadFloat4x4(&cam->mtxProjection),
			XMLoadFloat4x4(&cam->mtxView),
			XMMatrixIdentity()
		);

		float sx = XMVectorGetX(screen);
		float sy = XMVectorGetY(screen);
		float dx = sx - g_TargetScreenPos_Elph[i].x;
		float dy = sy - g_TargetScreenPos_Elph[i].y;
		float distance = sqrtf(dx * dx + dy * dy);

		ImGui::Text("Part %d: screen = (%.1f, %.1f) → target = (%.1f, %.1f) | Δ=%.1f",
			i, sx, sy, g_TargetScreenPos_Elph[i].x, g_TargetScreenPos_Elph[i].y, distance);
	}

	ImGui::Separator();
	ImGui::Text("Raw 3D Data:");
	for (int i = 0; i < TEXTURE_MAX - 1; i++) {
		ImGui::Text("Elph Part %d Pos: (%.2f, %.2f, %.2f)", i, g_Fragment_Elph[i].overallPos.x, g_Fragment_Elph[i].overallPos.y, g_Fragment_Elph[i].overallPos.z);
		ImGui::Text("Elph Part %d Scl: (%.2f, %.2f, %.2f)", i, g_Fragment_Elph[i].scl.x, g_Fragment_Elph[i].scl.y, g_Fragment_Elph[i].scl.z);
		ImGui::Text("Elph Part %d Rot: (%.2f, %.2f, %.2f)", i, g_Fragment_Elph[i].rot.x, g_Fragment_Elph[i].rot.y, g_Fragment_Elph[i].rot.z);
	}

	ImGui::End();

}

void ShowElephantDebugWindow()
{
	ImGui::Begin("Elph Debug");

	ImGui::Text("Restored Elph Pos: (%.2f, %.2f, %.2f)",
		g_Fragment_Elph[0].overallPos.x,
		g_Fragment_Elph[0].overallPos.y,
		g_Fragment_Elph[0].overallPos.z);

	ImGui::Text("Restored Elph Rot: (%.2f, %.2f, %.2f)",
		g_Fragment_Elph[0].rot.x,
		g_Fragment_Elph[0].rot.y,
		g_Fragment_Elph[0].rot.z);


	ImGui::Text("Restored Elph Pos: (%.2f, %.2f, %.2f)",
		g_FragmentRestored_Elph[0].pos.x,
		g_FragmentRestored_Elph[0].pos.y,
		g_FragmentRestored_Elph[0].pos.z);

	ImGui::Text("Restored Elph Rot: (%.2f, %.2f, %.2f)",
		g_FragmentRestored_Elph[0].rot.x,
		g_FragmentRestored_Elph[0].rot.y,
		g_FragmentRestored_Elph[0].rot.z);

	ImGui::Text("Restored Elph Alpha: (%.2f)",
		g_FragmentRestored_Elph[0].alpha);

	ImGui::Text("Restored Elph Use: (%d)",
		g_FragmentRestored_Elph[0].use);


	ImGui::End();
}


// right position for elphant 200, 57 ,176 //