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
#define TEXTURE_MAX		(5)						// テクスチャの数

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



static ID3D11Buffer			*g_VertexBuffer = NULL;	// 頂点情報
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static FRAGMENT				g_Fragment[TEXTURE_MAX];				// ポリゴンデータ
static FRAGMENT_RESTORED	g_FragmentRestored[TEXTURE_MAX];
static int				g_TexNo;				// テクスチャ番号


static XMFLOAT2 g_TargetScreenPos[TEXTURE_MAX] = {
	{554.8f, 397.2f},
	{554.7f, 466.5f},
	{530.2f, 476.7f},
	{514.9f, 567.5f},
	{0.0f, 0.0f}  // 第4张图是完整图，不需要判断
};

//static bool g_HasRecordedTarget = false;  // ← 直接设为 true


bool g_ShowFullImage = false;

static char* g_TextureName[] = {
	"data/TEXTURE/cat_01.png",
	"data/TEXTURE/cat_02.png",
	"data/TEXTURE/cat_03.png",
	"data/TEXTURE/cat_04.png",
	"data/TEXTURE/cat.png",
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

void ComputePuzzleCenterAndScale(XMFLOAT3* outCenter, XMFLOAT3* outScale)
{
	XMFLOAT3 center = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 scale = { 0.0f, 0.0f, 0.0f };

	int count = TEXTURE_MAX - 1;

	for (int i = 0; i < count; i++)
	{
		center.x += g_Fragment[i].overallPos.x;
		center.y += g_Fragment[i].overallPos.y;
		center.z += g_Fragment[i].overallPos.z;

		scale.x += g_Fragment[i].scl.x;
		scale.y += g_Fragment[i].scl.y;
		scale.z += g_Fragment[i].scl.z;
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

	for (int i = 0; i < TEXTURE_MAX - 1; i++)

	{
		// 位置・回転・スケールの初期設定
		g_Fragment[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f); // 配置座標
		g_Fragment[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f); // 回転
		g_Fragment[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f); // 拡大縮小率 X,Y,Z
		g_Fragment[i].overallPos = XMFLOAT3(0.0f, 0.0f, 0.0f);

		//---------------------------------------------------------------------
		g_FragmentRestored[i].use = FALSE;
		g_FragmentRestored[i].AnimCnt = 0;
		g_FragmentRestored[i].alpha = 1.0f;
		g_FragmentRestored[i].Initialized = FALSE;
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
	
	XMFLOAT3 center;
	ComputePuzzleCenterAndScale(&center, nullptr);

	if (!g_ShowFullImage && CheckPuzzleRestored())
	{
		g_ShowFullImage = true;
		g_CatAnimationPlayed = true;
		OutputDebugStringA("✅ 判定成功，准备显示完整贴图\n");
	}

	if (g_ShowFullImage && !g_FragmentRestored[0].Initialized) {
		g_FragmentRestored[0].use = TRUE;

		XMFLOAT3 center, scale;
		ComputePuzzleCenterAndScale(&center, &scale);

		g_FragmentRestored[0].pos = center;
		g_FragmentRestored[0].scl = scale;

		g_FragmentRestored[0].Initialized = TRUE;
	}

	if (g_FragmentRestored[0].use) {

		float moveX = -cosf(g_FragmentRestored[0].rot.y) * ANIM_MOVE_SPEED;
		float moveZ = -sinf(g_FragmentRestored[0].rot.y) * ANIM_MOVE_SPEED;
		g_FragmentRestored[0].pos.x += moveX;
		g_FragmentRestored[0].pos.z += moveZ;
		

		float angle = (XM_PI / FRAGMENT_ANIM_TIME) * g_FragmentRestored[0].AnimCnt;

		g_FragmentRestored[0].AnimCnt++;

		float rot = FRAGMENT_ROT_AMPLITUDE * -cosf(angle);

		g_FragmentRestored[0].rot.z = rot;

		if (g_FragmentRestored[0].AnimCnt >= 2 * FRAGMENT_ANIM_TIME) {
			g_FragmentRestored[0].AnimCnt = 0;
		}

		g_FragmentRestored[0].alpha -= 0.005f;
	}

	if (g_FragmentRestored[0].alpha < 0) {
		g_FragmentRestored[0].use = FALSE;
		
	}
		
#ifdef _DEBUG
	

//猫
if (GetKeyboardPress(DIK_LEFT))
{
	g_Fragment[4].overallPos.x -= 1.0f;
}
if (GetKeyboardPress(DIK_RIGHT))
{
	g_Fragment[4].overallPos.x += 1.0f;
}
if (GetKeyboardPress(DIK_UP))
{
	g_Fragment[4].overallPos.y += 1.0f;
}
if (GetKeyboardPress(DIK_DOWN))
{
	g_Fragment[4].overallPos.y -= 1.0f;
}
if (GetKeyboardPress(DIK_M))
{
	g_Fragment[4].overallPos.z -= 1.0f;
}
if (GetKeyboardPress(DIK_N))
{
	g_Fragment[4].overallPos.z += 1.0f;
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

	//if (GetKeyboardTrigger(DIK_F1)) {
	//	D3D11_VIEWPORT vp;
	//	UINT num = 1;
	//	GetDeviceContext()->RSGetViewports(&num, &vp);

	//	CAMERA* cam = GetCamera();

	//	for (int i = 0; i < TEXTURE_MAX; i++) {
	//		XMVECTOR world = XMLoadFloat3(&g_Fragment[i].overallPos);
	//		XMVECTOR screen = XMVector3Project(
	//			world,
	//			0, 0,
	//			vp.Width, vp.Height,
	//			0.0f, 1.0f,
	//			XMLoadFloat4x4(&cam->mtxProjection),
	//			XMLoadFloat4x4(&cam->mtxView),
	//			XMMatrixIdentity()
	//		);

	//		g_TargetScreenPos[i].x = XMVectorGetX(screen);
	//		g_TargetScreenPos[i].y = XMVectorGetY(screen);
	//	}

	//	//g_HasRecordedTarget = true;
	//	OutputDebugStringA("✨ 已记录当前碎片的投影坐标作为正确答案\n");
	//}


}
#endif

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
	SetAlphaTestEnable(BLEND_MODE_ALPHABLEND);
	SetLightEnable(FALSE);
	SetDepthEnable(TRUE);

	if (!g_ShowFullImage)
	{
		for (int i = 0; i < TEXTURE_MAX; i++)
		{

			XMMATRIX mtxWorld = XMMatrixIdentity();
			XMMATRIX mtxScl = XMMatrixScaling(g_Fragment[i].scl.x, g_Fragment[i].scl.y, g_Fragment[i].scl.z);
			XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Fragment[i].rot.x, g_Fragment[i].rot.y, g_Fragment[i].rot.z);
			XMMATRIX mtxTranslate = XMMatrixTranslation(g_Fragment[i].overallPos.x, g_Fragment[i].overallPos.y + 60.0f, g_Fragment[i].overallPos.z + 200.0f);
			mtxWorld = mtxScl * mtxRot * mtxTranslate;

			/*g_Fragment[0].overallPos = XMFLOAT3(10.0f, 0.0f, 60.0f);
			g_Fragment[1].overallPos = XMFLOAT3(30.0f, 0.0f, 40.0f);
			g_Fragment[2].overallPos = XMFLOAT3(5.0f,  0.0f, 80.0f);
			g_Fragment[3].overallPos = XMFLOAT3(70.0f, 0.0f, 90.0f);*/

			//g_Fragment[4].overallPos = XMFLOAT3(70.0f, 0.0f, 90.0f);

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
	}
	

	//if (g_ShowFullImage)
	//{
	//	
	//	XMFLOAT3 center, scale;
	//	ComputePuzzleCenterAndScale(&center, &scale);


	//	XMMATRIX mtxWorld = XMMatrixIdentity();
	//	XMMATRIX mtxScl = XMMatrixScaling(scale.x, scale.y, scale.z);  // 你可以调整缩放
	//	XMMATRIX mtxTranslate = XMMatrixTranslation(center.x, center.y + 60.0f, center.z + 200.0f);
	//	mtxWorld = mtxScl * mtxTranslate;

	//	SetWorldMatrix(&mtxWorld);
	//	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);  // 最后一张完整图

	//	GetDeviceContext()->Draw(4, 0);
	//	
	//}

	if (g_FragmentRestored[0].use)
	{
		XMMATRIX mtxWorld = XMMatrixIdentity();
		XMMATRIX mtxScl = XMMatrixScaling(g_FragmentRestored[0].scl.x, g_FragmentRestored[0].scl.y, g_FragmentRestored[0].scl.z);  // 你可以调整缩放
		XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_FragmentRestored[0].rot.x, g_FragmentRestored[0].rot.y, g_FragmentRestored[0].rot.z);
		XMMATRIX mtxTranslate = XMMatrixTranslation(g_FragmentRestored[0].pos.x, g_FragmentRestored[0].pos.y + 60.0f, g_FragmentRestored[0].pos.z + 200.0f);
		mtxWorld = mtxScl * mtxRot * mtxTranslate;

		SetWorldMatrix(&mtxWorld);

		material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, g_FragmentRestored[0].alpha);
		SetMaterial(material);
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);  // 最后一张完整图

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


bool CheckPuzzleRestored()
{
	D3D11_VIEWPORT vp;
	UINT num = 1;
	GetDeviceContext()->RSGetViewports(&num, &vp);

	CAMERA* cam = GetCamera();

	float tolerance = 50.0f;  // 可接受誤差半徑


	for (int i = 0; i < TEXTURE_MAX - 1; i++) {
		XMVECTOR world = XMLoadFloat3(&g_Fragment[i].overallPos);
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

		float dx = x - g_TargetScreenPos[i].x;
		float dy = y - g_TargetScreenPos[i].y;
		float distance = sqrtf(dx * dx + dy * dy);

		if (distance > tolerance)
			return false;
	}

	return true;
}

float GetPuzzleAlignmentRatio()
{
	D3D11_VIEWPORT vp;
	UINT num = 1;
	GetDeviceContext()->RSGetViewports(&num, &vp);

	CAMERA* cam = GetCamera();

	const float maxDistance = 200.0f;  // 误差最大可接受值（用于归一化）
	float ratioSum = 0.0f;

	for (int i = 0; i < TEXTURE_MAX - 1; i++) {
		XMVECTOR world = XMLoadFloat3(&g_Fragment[i].overallPos);
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
		float dx = x - g_TargetScreenPos[i].x;
		float dy = y - g_TargetScreenPos[i].y;
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

	//if (g_HasRecordedTarget) {
	//	ImGui::Text("=== Target Screen Positions ===");
	//	for (int i = 0; i < TEXTURE_MAX; i++) {
	//		ImGui::Text("Part %d: (%.1f, %.1f)", i, g_TargetScreenPos[i].x, g_TargetScreenPos[i].y);
	//	}
	//}

	//if (CheckPuzzleRestored()) {
	//	ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Restore Success!");
	//}
	//else if (g_HasRecordedTarget) {
	//	ImGui::Text("Not yet aligned");
	//}

	//ImGui::Separator();
	//ImGui::Text("投影 vs 目標位置誤差 (CheckPuzzleRestored)");

	//CAMERA* cam = GetCamera();
	//D3D11_VIEWPORT vp;
	//UINT num = 1;
	//GetDeviceContext()->RSGetViewports(&num, &vp);

	//for (int i = 0; i < TEXTURE_MAX - 1; i++) {
	//	XMVECTOR world = XMLoadFloat3(&g_Fragment[i].overallPos);
	//	XMVECTOR screen = XMVector3Project(
	//		world,
	//		0, 0,
	//		vp.Width, vp.Height,
	//		0.0f, 1.0f,
	//		XMLoadFloat4x4(&cam->mtxProjection),
	//		XMLoadFloat4x4(&cam->mtxView),
	//		XMMatrixIdentity()
	//	);

	//	float x = XMVectorGetX(screen);
	//	float y = XMVectorGetY(screen);
	//	float dx = fabsf(x - g_TargetScreenPos[i].x);
	//	float dy = fabsf(y - g_TargetScreenPos[i].y);

	//	ImGui::Text("Part %d: (%.1f, %.1f) -> (%.1f, %.1f) | Δx=%.1f, Δy=%.1f",
	//		i, x, y, g_TargetScreenPos[i].x, g_TargetScreenPos[i].y, dx, dy);
	//}

	ImGui::Separator();
	ImGui::Text("拼圖對齊誤差 (歐幾里得距離)");

	ImGui::Separator();
	float ratio = GetPuzzleAlignmentRatio();
	ImGui::Text("拼圖完成度: %.1f%%", ratio * 100.0f);
	ImGui::ProgressBar(ratio, ImVec2(200, 20));

	CAMERA* cam = GetCamera();
	D3D11_VIEWPORT vp;
	UINT num = 1;
	GetDeviceContext()->RSGetViewports(&num, &vp);

	for (int i = 0; i < TEXTURE_MAX - 1; i++) {
		XMVECTOR world = XMLoadFloat3(&g_Fragment[i].overallPos);
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
		float dx = x - g_TargetScreenPos[i].x;
		float dy = y - g_TargetScreenPos[i].y;
		float distance = sqrtf(dx * dx + dy * dy);

		ImGui::Text("Part %d: screen = (%.1f, %.1f) → target = (%.1f, %.1f) | distance = %.1f",
			i, x, y, g_TargetScreenPos[i].x, g_TargetScreenPos[i].y, distance);
	}

}

