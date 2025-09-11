//=============================================================================
//
// フェード処理 [fade.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "input.h"
#include "imgui.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(2)				// テクスチャの数

#define	FADE_RATE					(0.02f)			// フェード係数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
	

static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static	ID3D11VertexShader* g_VertexShader;	//頂点シェーダーオブジェクトポインタ
static	ID3D11PixelShader* g_PixelShader;	//ピクセルシェーダーオブジェクトポインタ
static	ID3D11InputLayout* g_VertexLayout;	//頂点レイアウトオブジェクトポインタ


static FADE		g_fade;
static PAWFADE	g_pawfade;


static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/fade_black.png",
	"data/TEXTURE/mask.png"
};


static BOOL						g_Use;						// TRUE:使っている  FALSE:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

int								g_Fade = FADE_IN;			// フェードの状態
int								g_ModeNext;					// 次のモード
XMFLOAT4						g_Color;					// フェードのカラー（α値）

static BOOL						g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitFade(void)
{
	//シェーダー読み込み
	CreateVertexShader(&g_VertexShader, &g_VertexLayout, "FadeVS.cso");
	CreatePixelShader(&g_PixelShader, "PawFadePS.cso");
	
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



	// プレイヤーの初期化
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 0.0f, 0.0f, 0.0f };
	g_TexNo = 0;

	g_Fade  = FADE_NONE;
	g_Color = { 0.0, 0.0, 0.0, 1.0 };	

	g_fade.Enable = 1;
	g_fade.center = XMFLOAT2(0.5f,0.5f);
	g_fade.radius = 1.0f;       // 半徑在 UV 空間
	g_fade.softness = 0.001f;     // 邊緣模糊範圍 (0~1)

	g_pawfade.g_FadeAmount = 15.0f;
	SetPawFadeBuffer(&g_pawfade);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitFade(void)
{
	if (g_Load == FALSE) return;

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

	g_VertexShader->Release();
	g_VertexLayout->Release();
	g_PixelShader->Release();



	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateFade(void)
{
	

	if (g_Fade != FADE_NONE)
	{// フェード処理中
		if (g_Fade == FADE_OUT)
		{// フェードアウト処理
			//g_fade.radius -= 0.01f;
			g_pawfade.g_FadeAmount -= 0.1f;
			if (g_pawfade.g_FadeAmount < 0.0f)
			{
				// 鳴っている曲を全部止める
				StopSound();

				SetFade(FADE_IN, g_ModeNext);

				// モードを設定
				SetMode(g_ModeNext);
			}

		}
		else if (g_Fade == FADE_IN)
		{// フェードイン処理
			g_pawfade.g_FadeAmount += 0.1f;		// α値を減算して画面を浮き上がらせる
			if (g_pawfade.g_FadeAmount >= 15.0f)
			{

				// フェード処理終了
				SetFade(FADE_NONE, g_ModeNext);
			}

		}
		
	}

	

	SetPawFadeBuffer(&g_pawfade);
	

#ifdef _DEBUG	// デバッグ情報を表示する
	// PrintDebugProc("\n");
	if (GetKeyboardPress(DIK_ADD)) {
		g_pawfade.g_FadeAmount += 0.02;

	}
	else if (GetKeyboardPress(DIK_SUBTRACT)) {
		g_pawfade.g_FadeAmount -= 0.02;
	}

	if (GetKeyboardTrigger(DIK_F10)) {
		g_Fade = FADE_OUT;
	}
	else if (GetKeyboardTrigger(DIK_F11)) {
		g_Fade = FADE_NONE;
	}

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawFade(void)
{
	if (g_Fade == FADE_NONE) return;	// フェードしないのなら描画しない

	
	//SetPawFadeBuffer(&g_pawfade);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	//頂点レイアウトを設定
	GetDeviceContext()->IASetInputLayout(g_VertexLayout);
	//頂点シェーダーをセット
	GetDeviceContext()->VSSetShader(g_VertexShader, nullptr, 0);
	//ピクセルシェーダーをセット
	GetDeviceContext()->PSSetShader(g_PixelShader, nullptr, 0);

	SetSamplerState_CLAMP();

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	ID3D11ShaderResourceView* textures[2] = { nullptr, nullptr };  // 確保先清空
	GetDeviceContext()->PSSetShaderResources(0, 2, textures);      // 先解除綁定全部貼圖

	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(1, 1, &g_Texture[1]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		//SetVertex(0.0f, 0.0f, SCREEN_WIDTH, TEXTURE_WIDTH, 0.0f, 0.0f, 1.0f, 1.0f);
		SetSpriteColor(g_VertexBuffer, SCREEN_CENTER_X, SCREEN_CENTER_Y, SCREEN_WIDTH , SCREEN_WIDTH , 0.0f, 0.0f, 1.0f, 1.0f,
			g_Color);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	SetSamplerState_WRAP();
	SetInputLayout();
	SetVertexShader();
	SetPixelShader();

}	


//=============================================================================
// フェードの状態設定
//=============================================================================
void SetFade(int fade, int modeNext)
{
	g_Fade = fade;
	g_ModeNext = modeNext;
}

//=============================================================================
// フェードの状態取得
//=============================================================================
int GetFade(void)
{
	return g_Fade;
}

void DrawFadeDebugUI()
{
	ImGui::Begin("Player Debug");
	ImGui::Text("Radius: (%.2f)", g_fade.radius);
	ImGui::Text("FadeAmount: (%.2f)", g_pawfade.g_FadeAmount);


	ImGui::End();
}

