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
#include "fragment_dog.h"
#include "imgui.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX		(6)						// テクスチャの数

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



static ID3D11Buffer			* g_VertexBuffer_Dog = NULL;	// 頂点情報
static ID3D11ShaderResourceView		* g_Texture_Dog[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static FRAGMENT				g_Fragment_Dog[TEXTURE_MAX];				// ポリゴンデータ
static FRAGMENT_RESTORED	g_FragmentRestored_Dog[TEXTURE_MAX];
static int					g_TexNo_Dog;				// テクスチャ番号


static XMFLOAT2 g_TargetScreenPos_Dog[TEXTURE_MAX] = {
	{447.2f, 214.1f},
	{444.3f, 211.6f},
	{459.3f, 206.2f},
	{483.5f, 197.5f},
	{417.3f, 231.4f},
	{0.0f, 0.0f} // 第4张图是完整图，不需要判断
};

//static bool g_HasRecordedTarget = false;  // ← 直接设为 true


bool g_ShowFullImage_Dog = false;

static char* g_TextureName_Dog[] = {
	"data/TEXTURE/dog_01.png",
	"data/TEXTURE/dog_02.png",
	"data/TEXTURE/dog_03.png",
	"data/TEXTURE/dog_04.png",
	"data/TEXTURE/dog_05.png",
	"data/TEXTURE/dog.png",
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

void ComputePuzzleCenterAndScale_Dog(XMFLOAT3* outCenter, XMFLOAT3* outScale)
{
	XMFLOAT3 center = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 scale = { 0.0f, 0.0f, 0.0f };

	int count = TEXTURE_MAX - 1;

	for (int i = 0; i < count; i++)
	{
		center.x += g_Fragment_Dog[i].overallPos.x;
		center.y += g_Fragment_Dog[i].overallPos.y;
		center.z += g_Fragment_Dog[i].overallPos.z;

		scale.x += g_Fragment_Dog[i].scl.x;
		scale.y += g_Fragment_Dog[i].scl.y;
		scale.z += g_Fragment_Dog[i].scl.z;
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

HRESULT MakeVertexFragment_Dog(void)
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

	GetDevice()->CreateBuffer(&bd, &sd, &g_VertexBuffer_Dog);

	return S_OK;
}

HRESULT InitFragment_Dog(void)
{
	// 頂点バッファの作成
	MakeVertexFragment_Dog();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName_Dog[i],
			NULL,
			NULL,
			&g_Texture_Dog[i],
			NULL);
	}

	for (int i = 0; i < TEXTURE_MAX - 1; i++)

	{
		// 位置・回転・スケールの初期設定
		g_Fragment_Dog[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f); // 配置座標
		g_Fragment_Dog[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f); // 回転
		g_Fragment_Dog[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f); // 拡大縮小率 X,Y,Z
		g_Fragment_Dog[i].overallPos = XMFLOAT3(0.0f, 0.0f, 0.0f);

		//---------------------------------------------------------------------
		g_FragmentRestored_Dog[i].use = FALSE;
		g_FragmentRestored_Dog[i].AnimCnt = 0;
		g_FragmentRestored_Dog[i].alpha = 1.0f;
		g_FragmentRestored_Dog[i].Initialized = FALSE;
	}
	
	g_TexNo_Dog = 0;

	return S_OK;
}

bool CheckPuzzleRestored_Dog()
{
	D3D11_VIEWPORT vp;
	UINT num = 1;
	GetDeviceContext()->RSGetViewports(&num, &vp);

	CAMERA* cam = GetCamera();

	float tolerance = 10.0f;  // 可接受误差半径

	for (int i = 0; i < TEXTURE_MAX - 1; i++) {
		// 考虑缩放、旋转、平移后的世界坐标中心点
		XMMATRIX mtxScl = XMMatrixScaling(g_Fragment_Dog[i].scl.x, g_Fragment_Dog[i].scl.y, g_Fragment_Dog[i].scl.z);
		XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Fragment_Dog[i].rot.x, g_Fragment_Dog[i].rot.y, g_Fragment_Dog[i].rot.z);
		XMMATRIX mtxTranslate = XMMatrixTranslation(g_Fragment_Dog[i].overallPos.x, g_Fragment_Dog[i].overallPos.y + 60.0f, g_Fragment_Dog[i].overallPos.z + 200.0f);
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

		float dx = x - g_TargetScreenPos_Dog[i].x;
		float dy = y - g_TargetScreenPos_Dog[i].y;
		float distance = sqrtf(dx * dx + dy * dy);

		if (distance > tolerance)
			return false;
	}

	return true;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitFragment_Dog(void)
{
	// 頂点バッファの解放
	if (g_VertexBuffer_Dog)
	{
		g_VertexBuffer_Dog->Release();
		g_VertexBuffer_Dog = NULL;
	}

	// テクスチャの解放
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture_Dog[i])
		{
			g_Texture_Dog[i]->Release();
			g_Texture_Dog[i] = NULL;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateFragment_Dog(void)
{
	CAMERA *cam = GetCamera();
	
	XMFLOAT3 center;
	ComputePuzzleCenterAndScale_Dog(&center, nullptr);

	if (!g_ShowFullImage_Dog && CheckPuzzleRestored_Dog())
	{
		g_ShowFullImage_Dog = true;
		g_DogAnimationPlayed = true;
		OutputDebugStringA("✅ 判定成功，准备显示完整贴图\n");
	}

	if (g_ShowFullImage_Dog && !g_FragmentRestored_Dog[0].Initialized) {
		g_FragmentRestored_Dog[0].use = TRUE;

		XMFLOAT3 center, scale;
		ComputePuzzleCenterAndScale_Dog(&center, &scale);
	
		g_FragmentRestored_Dog[0].pos = center;
		g_FragmentRestored_Dog[0].scl = scale;
		g_FragmentRestored_Dog[0].rot = g_Fragment_Dog[0].rot;;


		g_FragmentRestored_Dog[0].Initialized = TRUE;
	}

	if (g_FragmentRestored_Dog[0].use) {

		float moveX = cosf(g_FragmentRestored_Dog[0].rot.y) * ANIM_MOVE_SPEED;
		float moveZ = -sinf(g_FragmentRestored_Dog[0].rot.y) * ANIM_MOVE_SPEED;

		g_FragmentRestored_Dog[0].pos.x += moveX;
		g_FragmentRestored_Dog[0].pos.z += moveZ;

		float angle = (XM_PI / FRAGMENT_ANIM_TIME) * g_FragmentRestored_Dog[0].AnimCnt;

		g_FragmentRestored_Dog[0].AnimCnt++;

		float rot = FRAGMENT_ROT_AMPLITUDE * -cosf(angle);

		g_FragmentRestored_Dog[0].rot.z = rot;

		if (g_FragmentRestored_Dog[0].AnimCnt >= 2 * FRAGMENT_ANIM_TIME) {
			g_FragmentRestored_Dog[0].AnimCnt = 0;
		}

		g_FragmentRestored_Dog[0].alpha -= 0.005f;
	}

	if (g_FragmentRestored_Dog[0].alpha < 0) {
		g_FragmentRestored_Dog[0].use = FALSE;
	}
		
#ifdef _DEBUG
	

//Dog
//if (GetKeyboardPress(DIK_LEFT))
//{
//	g_Fragment_Dog[4].overallPos.x -= 1.0f;
//}
//if (GetKeyboardPress(DIK_RIGHT))
//{
//	g_Fragment_Dog[4].overallPos.x += 1.0f;
//}
//if (GetKeyboardPress(DIK_UP))
//{
//	g_Fragment_Dog[4].overallPos.y += 1.0f;
//}
//if (GetKeyboardPress(DIK_DOWN))
//{
//	g_Fragment_Dog[4].overallPos.y -= 1.0f;
//}
//if (GetKeyboardPress(DIK_M))
//{
//	g_Fragment_Dog[4].overallPos.z -= 1.0f;
//}
//if (GetKeyboardPress(DIK_N))
//{
//	g_Fragment_Dog[4].overallPos.z += 1.0f;
//}
//if (GetKeyboardPress(DIK_O))
//{
//	g_Fragment_Dog[4].scl.x += 0.01f;
//}
//if (GetKeyboardPress(DIK_P))
//{
//	g_Fragment_Dog[4].scl.x -= 0.01f;
//}
//if (GetKeyboardPress(DIK_O))
//{
//	g_Fragment_Dog[4].scl.y += 0.01f;
//}
//if (GetKeyboardPress(DIK_P))
//{
//	g_Fragment_Dog[4].scl.y -= 0.01f;
//}
//if (GetKeyboardPress(DIK_Z))
//{
//	g_Fragment_Dog[4].rot.y += 0.01f;
//}
//if (GetKeyboardPress(DIK_X))
//{
//	g_Fragment_Dog[4].rot.y -= 0.01f;
//}


#endif



#ifdef _DEBUG	// デバッグ情報を表示する

	if (GetKeyboardTrigger(DIK_F1)) {
		D3D11_VIEWPORT vp;
		UINT num = 1;
		GetDeviceContext()->RSGetViewports(&num, &vp);

		CAMERA* cam = GetCamera();

		for (int i = 0; i < TEXTURE_MAX; i++) {
			XMVECTOR world = XMLoadFloat3(&g_Fragment_Dog[i].overallPos);
			XMVECTOR screen = XMVector3Project(
				world,
				0, 0,
				vp.Width, vp.Height,
				0.0f, 1.0f,
				XMLoadFloat4x4(&cam->mtxProjection),
				XMLoadFloat4x4(&cam->mtxView),
				XMMatrixIdentity()
			);

			g_TargetScreenPos_Dog[i].x = XMVectorGetX(screen);
			g_TargetScreenPos_Dog[i].y = XMVectorGetY(screen);
		}

		//g_HasRecordedTarget = true;
		OutputDebugStringA("✨ 已记录当前碎片的投影坐标作为正确答案\n");
	}


}
#endif

//=============================================================================
// 描画処理
//=============================================================================
void DrawFragment_Dog(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer_Dog, &stride, &offset);

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

	if (!g_ShowFullImage_Dog)
	{
		for (int i = 0; i < TEXTURE_MAX; i++)
		{

			XMMATRIX mtxWorld = XMMatrixIdentity();
			XMMATRIX mtxScl = XMMatrixScaling(g_Fragment_Dog[i].scl.x, g_Fragment_Dog[i].scl.y, g_Fragment_Dog[i].scl.z);
			XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Fragment_Dog[i].rot.x, g_Fragment_Dog[i].rot.y, g_Fragment_Dog[i].rot.z);
			XMMATRIX mtxTranslate = XMMatrixTranslation(g_Fragment_Dog[i].overallPos.x, g_Fragment_Dog[i].overallPos.y + 60.0f, g_Fragment_Dog[i].overallPos.z + 200.0f);
			mtxWorld = mtxScl * mtxRot * mtxTranslate;

			/*g_Fragment_Dog[0].overallPos = XMFLOAT3(513.0f, -4.0f, 402.0f);
			g_Fragment_Dog[1].overallPos = XMFLOAT3(430.0f, 9.0f, 188.0f);
			g_Fragment_Dog[2].overallPos = XMFLOAT3(513.0f, -4.0f, 402.0f);
			g_Fragment_Dog[3].overallPos = XMFLOAT3(513.0f, -4.0f, 402.0f);
			g_Fragment_Dog[4].overallPos = XMFLOAT3(513.0f, -4.0f, 402.0f);


			g_Fragment_Dog[0].scl = XMFLOAT3(1.21f, 1.21f, 0.0f);
			g_Fragment_Dog[1].scl = XMFLOAT3(0.87f, 0.87f, 0.0f);
			g_Fragment_Dog[2].scl = XMFLOAT3(0.84f, 0.84f, 0.0f);
			g_Fragment_Dog[3].scl = XMFLOAT3(0.9f, 0.9f, 0.0f);
			g_Fragment_Dog[4].scl = XMFLOAT3(0.9f, 0.9f, 0.0f);
			
			*/

			g_Fragment_Dog[0].overallPos = XMFLOAT3(1185.0f, 27.0f, 234.0f);
			g_Fragment_Dog[1].overallPos = XMFLOAT3(1247.0f, 34.0f, 246.0f);
			g_Fragment_Dog[2].overallPos = XMFLOAT3(1244.0f, 37.0f, 236.0f);
			g_Fragment_Dog[3].overallPos = XMFLOAT3(1125.0f, 29.0f, 208.0f);
			g_Fragment_Dog[4].overallPos = XMFLOAT3(1022.0f, 8.0f, 216.0f);

			g_Fragment_Dog[0].scl = XMFLOAT3(1.09f, 1.09f, 1.0f);
			g_Fragment_Dog[1].scl = XMFLOAT3(1.36f, 1.36f, 1.0f);
			g_Fragment_Dog[2].scl = XMFLOAT3(1.35f, 1.35f, 1.0f);
			g_Fragment_Dog[3].scl = XMFLOAT3(1.15f, 1.15f, 0.0f);
			g_Fragment_Dog[4].scl = XMFLOAT3(0.32f, 0.32f, 1.0f);


			g_Fragment_Dog[0].rot = XMFLOAT3(0.0f, -5.0f, 0.0f);
			g_Fragment_Dog[1].rot = XMFLOAT3(0.0f, -5.0f, 0.0f);
			g_Fragment_Dog[2].rot = XMFLOAT3(0.0f, -5.0f, 0.0f);
			g_Fragment_Dog[3].rot = XMFLOAT3(0.0f, -5.0f, 0.0f);
			g_Fragment_Dog[4].rot = XMFLOAT3(0.0f, -5.0f, 0.0f);
			
			
			



			// dogの正しい見つかり位置座標
			// 

			// ワールドマトリックスの初期化
			SetWorldMatrix(&mtxWorld);
			XMStoreFloat4x4(&g_Fragment_Dog[i].mtxWorld, mtxWorld);

			// テクスチャー設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture_Dog[i]);

			//ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
	
	if (g_FragmentRestored_Dog[0].use)
	{
		XMMATRIX mtxWorld = XMMatrixIdentity();
		XMMATRIX mtxScl = XMMatrixScaling(g_FragmentRestored_Dog[0].scl.x, g_FragmentRestored_Dog[0].scl.y, g_FragmentRestored_Dog[0].scl.z);  // 你可以调整缩放
		XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_FragmentRestored_Dog[0].rot.x, g_FragmentRestored_Dog[0].rot.y, g_FragmentRestored_Dog[0].rot.z);
		XMMATRIX mtxTranslate = XMMatrixTranslation(g_FragmentRestored_Dog[0].pos.x, g_FragmentRestored_Dog[0].pos.y + 60.0f, g_FragmentRestored_Dog[0].pos.z + 200.0f);
		mtxWorld = mtxScl * mtxRot * mtxTranslate;

		SetWorldMatrix(&mtxWorld);

		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, g_FragmentRestored_Dog[0].alpha);
		SetMaterial(material);
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture_Dog[5]);  // 最后一张完整图

		GetDeviceContext()->Draw(4, 0);

	}

	SetLightEnable(TRUE);
	SetDepthEnable(TRUE);

	
	
}

float GetPuzzleAlignmentRatio_Dog()
{
	D3D11_VIEWPORT vp;
	UINT num = 1;
	GetDeviceContext()->RSGetViewports(&num, &vp);

	CAMERA* cam = GetCamera();

	const float maxDistance = 200.0f;  // 最大容忍误差
	float ratioSum = 0.0f;

	for (int i = 0; i < TEXTURE_MAX - 1; i++) {
		// 构建世界变换矩阵（缩放 → 旋转 → 平移）
		XMMATRIX mtxScl = XMMatrixScaling(g_Fragment_Dog[i].scl.x, g_Fragment_Dog[i].scl.y, g_Fragment_Dog[i].scl.z);
		XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Fragment_Dog[i].rot.x, g_Fragment_Dog[i].rot.y, g_Fragment_Dog[i].rot.z);
		XMMATRIX mtxTranslate = XMMatrixTranslation(g_Fragment_Dog[i].overallPos.x, g_Fragment_Dog[i].overallPos.y + 60.0f, g_Fragment_Dog[i].overallPos.z + 200.0f);
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
		float dx = x - g_TargetScreenPos_Dog[i].x;
		float dy = y - g_TargetScreenPos_Dog[i].y;
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


void DrawPartDebugUI_Dog()
{
	ImGui::Begin("Part Debug (Dog)");

	CAMERA* cam = GetCamera();
	D3D11_VIEWPORT vp;
	UINT num = 1;
	GetDeviceContext()->RSGetViewports(&num, &vp);

	float ratio = GetPuzzleAlignmentRatio_Dog();
	ImGui::Text("拼图完成度: %.1f%%", ratio * 100.0f);
	ImGui::ProgressBar(ratio, ImVec2(200, 20));

	ImGui::Separator();
	ImGui::Text("碎片中心位置（投影） vs 目标位置");

	for (int i = 0; i < TEXTURE_MAX - 1; i++) {
		// 构建世界矩阵
		XMMATRIX mtxScl = XMMatrixScaling(g_Fragment_Dog[i].scl.x, g_Fragment_Dog[i].scl.y, g_Fragment_Dog[i].scl.z);
		XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Fragment_Dog[i].rot.x, g_Fragment_Dog[i].rot.y, g_Fragment_Dog[i].rot.z);
		XMMATRIX mtxTranslate = XMMatrixTranslation(g_Fragment_Dog[i].overallPos.x, g_Fragment_Dog[i].overallPos.y + 60.0f, g_Fragment_Dog[i].overallPos.z + 200.0f);
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
		float dx = sx - g_TargetScreenPos_Dog[i].x;
		float dy = sy - g_TargetScreenPos_Dog[i].y;
		float distance = sqrtf(dx * dx + dy * dy);

		ImGui::Text("Part %d: screen = (%.1f, %.1f) → target = (%.1f, %.1f) | Δ=%.1f",
			i, sx, sy, g_TargetScreenPos_Dog[i].x, g_TargetScreenPos_Dog[i].y, distance);
	}

	ImGui::Separator();
	ImGui::Text("Raw 3D Data:");
	for (int i = 0; i < TEXTURE_MAX - 1; i++) {
		ImGui::Text("Dog Part %d Pos: (%.2f, %.2f, %.2f)", i, g_Fragment_Dog[i].overallPos.x, g_Fragment_Dog[i].overallPos.y, g_Fragment_Dog[i].overallPos.z);
		ImGui::Text("Dog Part %d Scl: (%.2f, %.2f, %.2f)", i, g_Fragment_Dog[i].scl.x, g_Fragment_Dog[i].scl.y, g_Fragment_Dog[i].scl.z);
		ImGui::Text("Dog Part %d Rot: (%.2f, %.2f, %.2f)", i, g_Fragment_Dog[i].rot.x, g_Fragment_Dog[i].rot.y, g_Fragment_Dog[i].rot.z);
	}

	ImGui::End();

}

void ShowDogDebugWindow()
{
	ImGui::Begin("Dog Debug");

	ImGui::Text("Restored Dog Pos: (%.2f, %.2f, %.2f)",
		g_Fragment_Dog[0].pos.x,
		g_Fragment_Dog[0].pos.y,
		g_Fragment_Dog[0].pos.z);

	ImGui::Text("Restored Dog Rot: (%.2f, %.2f, %.2f)",
		g_Fragment_Dog[0].rot.x,
		g_Fragment_Dog[0].rot.y,
		g_Fragment_Dog[0].rot.z);


	ImGui::Text("Restored Dog Pos: (%.2f, %.2f, %.2f)",
		g_FragmentRestored_Dog[0].pos.x,
		g_FragmentRestored_Dog[0].pos.y,
		g_FragmentRestored_Dog[0].pos.z);

	ImGui::Text("Restored Dog Rot: (%.2f, %.2f, %.2f)",
		g_FragmentRestored_Dog[0].rot.x,
		g_FragmentRestored_Dog[0].rot.y,
		g_FragmentRestored_Dog[0].rot.z);

	ImGui::End();
}

// right position for dog 235, 57 , 376 //