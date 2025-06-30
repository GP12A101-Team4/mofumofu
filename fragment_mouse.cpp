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
#include "fragment_mouse.h"
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


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


HRESULT MakeVertexFragment(void);

bool CheckPuzzleRestored(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************



static ID3D11Buffer			* g_VertexBuffer_Mouse = NULL;	// 頂点情報
static ID3D11ShaderResourceView		* g_Texture_Mouse[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static FRAGMENT				g_Fragment_Mouse[TEXTURE_MAX];				// ポリゴンデータ
static FRAGMENT_RESTORED	g_FragmentRestored_Mouse[TEXTURE_MAX];
static int				g_TexNo_Mouse;				// テクスチャ番号


static XMFLOAT2 g_TargetScreenPos_Mouse[TEXTURE_MAX] = {
	{491.3f, 370.9f},
	{491.3f, 370.9f},
	{491.3f, 370.9f},
	{491.3f, 370.9f},
	{491.3f, 370.9f},
	{0.0f, 0.0f}  // 第4张图是完整图，不需要判断
};

//static bool g_HasRecordedTarget = false;  // ← 直接设为 true


bool g_ShowFullImage_Mouse = false;

static char* g_TextureName_Mouse[] = {
	"data/TEXTURE/mouse_01.png",
	"data/TEXTURE/mouse_02.png",
	"data/TEXTURE/mouse_03.png",
	"data/TEXTURE/mouse_04.png",
	"data/TEXTURE/mouse_05.png",
	"data/TEXTURE/mouse.png",
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

void ComputePuzzleCenterAndScale_Mouse(XMFLOAT3* outCenter, XMFLOAT3* outScale)
{
	XMFLOAT3 center = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 scale = { 0.0f, 0.0f, 0.0f };

	int count = TEXTURE_MAX - 1;

	for (int i = 0; i < count; i++)
	{
		center.x += g_Fragment_Mouse[i].overallPos.x;
		center.y += g_Fragment_Mouse[i].overallPos.y;
		center.z += g_Fragment_Mouse[i].overallPos.z;

		scale.x += g_Fragment_Mouse[i].scl.x;
		scale.y += g_Fragment_Mouse[i].scl.y;
		scale.z += g_Fragment_Mouse[i].scl.z;
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

HRESULT MakeVertexFragment_Mouse(void)
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

	GetDevice()->CreateBuffer(&bd, &sd, &g_VertexBuffer_Mouse);

	return S_OK;
}

HRESULT InitFragment_Mouse(void)
{
	// 頂点バッファの作成
	MakeVertexFragment_Mouse();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName_Mouse[i],
			NULL,
			NULL,
			&g_Texture_Mouse[i],
			NULL);
	}

	for (int i = 0; i < TEXTURE_MAX - 1; i++)

	{
		// 位置・回転・スケールの初期設定
		g_Fragment_Mouse[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f); // 配置座標
		g_Fragment_Mouse[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f); // 回転
		g_Fragment_Mouse[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f); // 拡大縮小率 X,Y,Z
		g_Fragment_Mouse[i].overallPos = XMFLOAT3(0.0f, 0.0f, 0.0f);

		//---------------------------------------------------------------------
		g_FragmentRestored_Mouse[i].use = FALSE;
		g_FragmentRestored_Mouse[i].AnimCnt = 0;
		g_FragmentRestored_Mouse[i].alpha = 1.0f;
		g_FragmentRestored_Mouse[i].Initialized = FALSE;
	}
	
	g_TexNo_Mouse = 0;

	return S_OK;
}

bool CheckPuzzleRestored_Mouse()
{
	D3D11_VIEWPORT vp;
	UINT num = 1;
	GetDeviceContext()->RSGetViewports(&num, &vp);

	CAMERA* cam = GetCamera();

	float tolerance = 50.0f;  // 可接受誤差半徑


	for (int i = 0; i < TEXTURE_MAX - 1; i++) {
		XMVECTOR world = XMLoadFloat3(&g_Fragment_Mouse[i].overallPos);
		XMVECTOR screen = XMVector3Project(
			world,
			0, 0,
			vp.Width, vp.Height,
			0.0f, 1.0f,
			XMLoadFloat4x4(&cam->mtxProjection),
			XMLoadFloat4x4(&cam->mtxView),
			XMMatrixIdentity()
		);

		float x = XMVectorGetX(screen);
		float y = XMVectorGetY(screen);

		float dx = x - g_TargetScreenPos_Mouse[i].x;
		float dy = y - g_TargetScreenPos_Mouse[i].y;
		float distance = sqrtf(dx * dx + dy * dy);

		if (distance > tolerance)
			return false;
	}

	return true;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitFragment_Mouse(void)
{
	// 頂点バッファの解放
	if (g_VertexBuffer_Mouse)
	{
		g_VertexBuffer_Mouse->Release();
		g_VertexBuffer_Mouse = NULL;
	}

	// テクスチャの解放
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture_Mouse[i])
		{
			g_Texture_Mouse[i]->Release();
			g_Texture_Mouse[i] = NULL;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateFragment_Mouse(void)
{
	CAMERA *cam = GetCamera();
	
	XMFLOAT3 center;
	ComputePuzzleCenterAndScale_Mouse(&center, nullptr);

	/*if (!g_ShowFullImage_Mouse && CheckPuzzleRestored_Mouse())
	{
		g_ShowFullImage_Mouse = true;
		OutputDebugStringA("✅ 判定成功，准备显示完整贴图\n");
	}

	if (g_ShowFullImage_Mouse && !g_FragmentRestored_Mouse[0].Initialized) {
		g_FragmentRestored_Mouse[0].use = TRUE;

		XMFLOAT3 center, scale;
		ComputePuzzleCenterAndScale_Mouse(&center, &scale);

		g_FragmentRestored_Mouse[0].pos = center;
		g_FragmentRestored_Mouse[0].scl = scale;

		g_FragmentRestored_Mouse[0].Initialized = TRUE;
	}

	if (g_FragmentRestored_Mouse[0].use) {
		g_FragmentRestored_Mouse[0].pos.x -= 1.0f;

		float angle = (XM_PI / FRAGMENT_ANIM_TIME) * g_FragmentRestored_Mouse[0].AnimCnt;

		g_FragmentRestored_Mouse[0].AnimCnt++;

		float rot = FRAGMENT_ROT_AMPLITUDE * -cosf(angle);

		g_FragmentRestored_Mouse[0].rot.z = rot;

		if (g_FragmentRestored_Mouse[0].AnimCnt >= 2 * FRAGMENT_ANIM_TIME) {
			g_FragmentRestored_Mouse[0].AnimCnt = 0;
		}

		g_FragmentRestored_Mouse[0].alpha -= 0.005f;
	}

	if (g_FragmentRestored_Mouse[0].alpha < 0) {
		g_FragmentRestored_Mouse[0].use = FALSE;
	}*/
		
#ifdef _DEBUG
	

//Mouse
//if (GetKeyboardPress(DIK_LEFT))
//{
//	g_Fragment_Mouse[4].overallPos.x -= 1.0f;
//}
//if (GetKeyboardPress(DIK_RIGHT))
//{
//	g_Fragment_Mouse[4].overallPos.x += 1.0f;
//}
//if (GetKeyboardPress(DIK_UP))
//{
//	g_Fragment_Mouse[4].overallPos.y += 1.0f;
//}
//if (GetKeyboardPress(DIK_DOWN))
//{
//	g_Fragment_Mouse[4].overallPos.y -= 1.0f;
//}
//if (GetKeyboardPress(DIK_M))
//{
//	g_Fragment_Mouse[4].overallPos.z -= 1.0f;
//}
//if (GetKeyboardPress(DIK_N))
//{
//	g_Fragment_Mouse[4].overallPos.z += 1.0f;
//}
//if (GetKeyboardPress(DIK_O))
//{
//	g_Fragment_Mouse[4].scl.x += 0.01f;
//}
//if (GetKeyboardPress(DIK_P))
//{
//	g_Fragment_Mouse[4].scl.x -= 0.01f;
//}
//if (GetKeyboardPress(DIK_O))
//{
//	g_Fragment_Mouse[4].scl.y += 0.01f;
//}
//if (GetKeyboardPress(DIK_P))
//{
//	g_Fragment_Mouse[4].scl.y -= 0.01f;
//}


#endif



#ifdef _DEBUG	// デバッグ情報を表示する

	if (GetKeyboardTrigger(DIK_F1)) {
		D3D11_VIEWPORT vp;
		UINT num = 1;
		GetDeviceContext()->RSGetViewports(&num, &vp);

		CAMERA* cam = GetCamera();

		for (int i = 0; i < TEXTURE_MAX; i++) {
			XMVECTOR world = XMLoadFloat3(&g_Fragment_Mouse[i].overallPos);
			XMVECTOR screen = XMVector3Project(
				world,
				0, 0,
				vp.Width, vp.Height,
				0.0f, 1.0f,
				XMLoadFloat4x4(&cam->mtxProjection),
				XMLoadFloat4x4(&cam->mtxView),
				XMMatrixIdentity()
			);

			g_TargetScreenPos_Mouse[i].x = XMVectorGetX(screen);
			g_TargetScreenPos_Mouse[i].y = XMVectorGetY(screen);
		}

		//g_HasRecordedTarget = true;
		OutputDebugStringA("✨ 已记录当前碎片的投影坐标作为正确答案\n");
	}


}
#endif

//=============================================================================
// 描画処理
//=============================================================================
void DrawFragment_Mouse(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer_Mouse, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// テクスチャ設定
	SetCullingMode(CULL_MODE_NONE);
	SetLightEnable(FALSE);
	SetDepthEnable(FALSE);

	if (!g_ShowFullImage_Mouse)
	{
		for (int i = 0; i < TEXTURE_MAX; i++)
		{

			XMMATRIX mtxWorld = XMMatrixIdentity();
			XMMATRIX mtxScl = XMMatrixScaling(g_Fragment_Mouse[i].scl.x, g_Fragment_Mouse[i].scl.y, g_Fragment_Mouse[i].scl.z);
			XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Fragment_Mouse[i].rot.x, g_Fragment_Mouse[i].rot.y, g_Fragment_Mouse[i].rot.z);
			XMMATRIX mtxTranslate = XMMatrixTranslation(g_Fragment_Mouse[i].overallPos.x, g_Fragment_Mouse[i].overallPos.y + 60.0f, g_Fragment_Mouse[i].overallPos.z + 200.0f);
			mtxWorld = mtxScl * mtxRot * mtxTranslate;

			/*g_Fragment_Mouse[0].overallPos = XMFLOAT3(513.0f, -4.0f, 402.0f);
			g_Fragment_Mouse[1].overallPos = XMFLOAT3(513.0f, -4.0f, 402.0f);
			g_Fragment_Mouse[2].overallPos = XMFLOAT3(513.0f, -4.0f, 402.0f);
			g_Fragment_Mouse[3].overallPos = XMFLOAT3(513.0f, -4.0f, 402.0f);
			g_Fragment_Mouse[4].overallPos = XMFLOAT3(513.0f, -4.0f, 402.0f);


			g_Fragment_Mouse[0].scl = XMFLOAT3(1.21f, 1.21f, 0.0f);
			g_Fragment_Mouse[1].scl = XMFLOAT3(0.87f, 0.87f, 0.0f);
			g_Fragment_Mouse[2].scl = XMFLOAT3(0.84f, 0.84f, 0.0f);
			g_Fragment_Mouse[3].scl = XMFLOAT3(0.9f, 0.9f, 0.0f);
			g_Fragment_Mouse[4].scl = XMFLOAT3(0.9f, 0.9f, 0.0f);*/


			g_Fragment_Mouse[0].overallPos = XMFLOAT3(-1.0f, -26.0f, -191.5f);
			g_Fragment_Mouse[1].overallPos = XMFLOAT3(107.0f, -49.0f, -193.0f);
			g_Fragment_Mouse[2].overallPos = XMFLOAT3(117.0f, -51.0f, -193.0f);
			g_Fragment_Mouse[3].overallPos = XMFLOAT3(42.0f, -36.0f, -192.0f);
			g_Fragment_Mouse[4].overallPos = XMFLOAT3(10.0f, -28.0f, -192.0f);


			g_Fragment_Mouse[0].scl = XMFLOAT3(0.09f, 0.09f, 1.0f);
			g_Fragment_Mouse[1].scl = XMFLOAT3(0.18f, 0.18f, 1.0f);
			g_Fragment_Mouse[2].scl = XMFLOAT3(0.19f, 0.19f, 1.0f);
			g_Fragment_Mouse[3].scl = XMFLOAT3(0.14f, 0.14f, 1.0f);
			g_Fragment_Mouse[4].scl = XMFLOAT3(0.10f, 0.10f, 0.0f); 



			g_Fragment_Mouse[0].rot = XMFLOAT3(0.0f, -4.75f, 0.0f);
			g_Fragment_Mouse[1].rot = XMFLOAT3(0.0f, -4.75f, 0.0f);
			g_Fragment_Mouse[2].rot = XMFLOAT3(0.0f, -4.75f, 0.0f);
			g_Fragment_Mouse[3].rot = XMFLOAT3(0.0f, -4.75f, 0.0f);
			g_Fragment_Mouse[4].rot = XMFLOAT3(0.0f, -4.75f, 0.0f);



			// dogの正しい見つかり位置座標
			// 

			// ワールドマトリックスの初期化
			SetWorldMatrix(&mtxWorld);
			XMStoreFloat4x4(&g_Fragment_Mouse[i].mtxWorld, mtxWorld);

			// テクスチャー設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture_Mouse[i]);

			//ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
	
	if (g_FragmentRestored_Mouse[0].use)
	{
		XMMATRIX mtxWorld = XMMatrixIdentity();
		XMMATRIX mtxScl = XMMatrixScaling(g_FragmentRestored_Mouse[0].scl.x, g_FragmentRestored_Mouse[0].scl.y, g_FragmentRestored_Mouse[0].scl.z);  // 你可以调整缩放
		XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_FragmentRestored_Mouse[0].rot.x, g_FragmentRestored_Mouse[0].rot.y, g_FragmentRestored_Mouse[0].rot.z);
		XMMATRIX mtxTranslate = XMMatrixTranslation(g_FragmentRestored_Mouse[0].pos.x, g_FragmentRestored_Mouse[0].pos.y + 60.0f, g_FragmentRestored_Mouse[0].pos.z + 200.0f);
		mtxWorld = mtxScl * mtxRot * mtxTranslate;

		SetWorldMatrix(&mtxWorld);

		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, g_FragmentRestored_Mouse[0].alpha);
		SetMaterial(material);
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture_Mouse[5]);  // 最后一张完整图

		GetDeviceContext()->Draw(4, 0);

	}

	SetLightEnable(TRUE);
	SetDepthEnable(TRUE);

	
	
}

float GetPuzzleAlignmentRatio_Mouse()
{
	D3D11_VIEWPORT vp;
	UINT num = 1;
	GetDeviceContext()->RSGetViewports(&num, &vp);

	CAMERA* cam = GetCamera();

	const float maxDistance = 200.0f;  // 误差最大可接受值（用于归一化）
	float ratioSum = 0.0f;

	for (int i = 0; i < TEXTURE_MAX - 1; i++) {
		XMVECTOR world = XMLoadFloat3(&g_Fragment_Mouse[i].overallPos);
		XMVECTOR screen = XMVector3Project(
			world,
			0, 0,
			vp.Width, vp.Height,
			0.0f, 1.0f,
			XMLoadFloat4x4(&cam->mtxProjection),
			XMLoadFloat4x4(&cam->mtxView),
			XMMatrixIdentity()
		);

		float x = XMVectorGetX(screen);
		float y = XMVectorGetY(screen);
		float dx = x - g_TargetScreenPos_Mouse[i].x;
		float dy = y - g_TargetScreenPos_Mouse[i].y;
		float distance = sqrtf(dx * dx + dy * dy);

		// 误差越小，比例越接近 1.0；误差 >= maxDistance 时，比例为 0
		float normalized = distance / maxDistance;
		if (normalized > 1.0f)
			normalized = 1.0f;

		float partRatio = 1.0f - normalized;
		ratioSum += partRatio;
	}

	return ratioSum / (TEXTURE_MAX - 1);  // 返回0～1之间的平均完成度
}


void DrawPartDebugUI_Mouse()
{
	ImGui::Begin("Part Debug (Elph)");

	CAMERA* cam = GetCamera();
	D3D11_VIEWPORT vp;
	UINT num = 1;
	GetDeviceContext()->RSGetViewports(&num, &vp);

	float ratio = GetPuzzleAlignmentRatio_Mouse();
	ImGui::Text("拼图完成度: %.1f%%", ratio * 100.0f);
	ImGui::ProgressBar(ratio, ImVec2(200, 20));

	ImGui::Separator();
	ImGui::Text("碎片中心位置（投影） vs 目标位置");

	for (int i = 0; i < TEXTURE_MAX - 1; i++) {
		// 构建世界矩阵
		XMMATRIX mtxScl = XMMatrixScaling(g_Fragment_Mouse[i].scl.x, g_Fragment_Mouse[i].scl.y, g_Fragment_Mouse[i].scl.z);
		XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Fragment_Mouse[i].rot.x, g_Fragment_Mouse[i].rot.y, g_Fragment_Mouse[i].rot.z);
		XMMATRIX mtxTranslate = XMMatrixTranslation(g_Fragment_Mouse[i].overallPos.x, g_Fragment_Mouse[i].overallPos.y + 60.0f, g_Fragment_Mouse[i].overallPos.z + 200.0f);
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
		float dx = sx - g_TargetScreenPos_Mouse[i].x;
		float dy = sy - g_TargetScreenPos_Mouse[i].y;
		float distance = sqrtf(dx * dx + dy * dy);

		ImGui::Text("Part %d: screen = (%.1f, %.1f) → target = (%.1f, %.1f) | Δ=%.1f",
			i, sx, sy, g_TargetScreenPos_Mouse[i].x, g_TargetScreenPos_Mouse[i].y, distance);
	}

	ImGui::Separator();
	ImGui::Text("Raw 3D Data:");
	for (int i = 0; i < TEXTURE_MAX - 1; i++) {
		ImGui::Text("Mouse Part %d Pos: (%.2f, %.2f, %.2f)", i, g_Fragment_Mouse[i].overallPos.x, g_Fragment_Mouse[i].overallPos.y, g_Fragment_Mouse[i].overallPos.z);
		ImGui::Text("Mouse Part %d Scl: (%.2f, %.2f, %.2f)", i, g_Fragment_Mouse[i].scl.x, g_Fragment_Mouse[i].scl.y, g_Fragment_Mouse[i].scl.z);
		ImGui::Text("Mouse Part %d Rot: (%.2f, %.2f, %.2f)", i, g_Fragment_Mouse[i].rot.x, g_Fragment_Mouse[i].rot.y, g_Fragment_Mouse[i].rot.z);
	}

	ImGui::End();

}

// right position of mouse -209. 57 , 5.54 //