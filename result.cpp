//=============================================================================
//
// リザルト画面処理 [result.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "result.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "score.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH              (SCREEN_WIDTH)   // 背景サイズ
#define TEXTURE_HEIGHT             (SCREEN_HEIGHT)
#define TEXTURE_MAX                (4)              // テクスチャの数

#define TEXTURE_WIDTH_LOGO         (409)           // ロゴサイズ
#define TEXTURE_HEIGHT_LOGO        (246)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;      // 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL }; // テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
    "data/TEXTURE/result.png",
    "data/TEXTURE/result_logo.png",
    "data/TEXTURE/Numberrr.png",
    "data/TEXTURE/press_any_key.png",
};

static BOOL      g_Use;                     // TRUE:使っている  FALSE:未使用
static float     g_w, g_h;                  // 幅と高さ
static XMFLOAT3  g_Pos;                     // ポリゴンの座標
static int       g_TexNo;                   // テクスチャ番号

static BOOL      g_Load = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitResult(void)
{
    // --- Score をこの画面で使うため初期化（重要！）---
    // DrawScore() は score.cpp 内の自前の頂点バッファ/テクスチャを使うため、
    // ここで確実に初期化しておく（リザルト期間中は生存させる）
    //InitScore();  // ★ 追加（RESULT 管理）

    // テクスチャ生成
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

    // 頂点バッファ生成（RESULT 自身の 2D 全画面用）
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(VERTEX_3D) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

    // 変数の初期化
    g_Use = TRUE;
    g_w = TEXTURE_WIDTH;
    g_h = TEXTURE_HEIGHT;
    g_Pos = { g_w / 2, 50.0f, 0.0f };
    g_TexNo = 0;

    // BGM再生（必要なら）
    //PlaySound(SOUND_LABEL_BGM_result);

    g_Load = TRUE;
    return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitResult(void)
{
    if (g_Load == FALSE) return;

    // --- Score の後始末（RESULT 管理）---
    //UninitScore(); // ★ 追加

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

    g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateResult(void)
{
    // ※ ここで UpdateScore() を毎フレーム回す必要はありません。
    // StopTimer() で凍結していれば値は固定ですし、
    // 走らせても g_Running==FALSE なら値は変わりません。:contentReference[oaicite:3]{index=3}

    if (IsAnyKeyTriggered())
    {// Enter押したら、ステージを切り替える
        SetFade(FADE_OUT, MODE_TITLE);
    }
   

#ifdef _DEBUG
    // デバッグ表示があればここに
#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawResult(void)
{
    // 頂点バッファ設定（RESULT 自身の全画面ポリゴン用）
    UINT stride = sizeof(VERTEX_3D);
    UINT offset = 0;
    GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

    // マトリクス設定
    SetWorldViewProjection2D();

    // プリミティブトポロジ設定
    GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // マテリアル設定
    MATERIAL material;
    ZeroMemory(&material, sizeof(material));
    material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    SetMaterial(material);

    // --- 背景 ---
    {
        ID3D11ShaderResourceView* srv = g_Texture[0];
        GetDeviceContext()->PSSetShaderResources(0, 1, &srv);

        SetSpriteLeftTop(g_VertexBuffer, 0.0f, 0.0f, g_w, g_h,
            0.0f, 0.0f, 1.0f, 1.0f);
        GetDeviceContext()->Draw(4, 0);
    }

    // --- ロゴ ---
    {
        ID3D11ShaderResourceView* srv = g_Texture[1];
        GetDeviceContext()->PSSetShaderResources(0, 1, &srv);

        const float w = TEXTURE_WIDTH_LOGO;   // 480
        const float h = TEXTURE_HEIGHT_LOGO;  // 80
        const float x = 260.0f;               // 位置はお好みで
        const float y = 25.0f;

        SetSpriteLeftTop(g_VertexBuffer, x, y, w, h,
            0.0f, 0.0f, 1.0f, 1.0f);
        GetDeviceContext()->Draw(4, 0);
    }

    {
        ID3D11ShaderResourceView* srv = g_Texture[3];
        GetDeviceContext()->PSSetShaderResources(0, 1, &srv);

        const float w = 300.0f;   // 480
        const float h = 40.0f;  // 80
        const float x = 320.0f;               // 位置はお好みで
        const float y = 450.0f;

        SetSpriteLeftTop(g_VertexBuffer, x, y, w, h,
            0.0f, 0.0f, 1.0f, 1.0f);
        GetDeviceContext()->Draw(4, 0);
    }

    Score_SetScale(3.3f);

    float scale = 2.0f;
    float totalWidth = 13 * 22.0f * scale;
    float x = (SCREEN_WIDTH - totalWidth) * 0.5f;
    float y = 300.0f;

    Score_SetPosition(x, y);
    DrawScore();

}
