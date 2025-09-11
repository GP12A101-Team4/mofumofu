//=============================================================================
//
// タイマー処理 [score.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "score.h"
#include "sprite.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH   (27)   // キャラサイズ
#define TEXTURE_HEIGHT  (43)
#define TEXTURE_MAX     (1)    // テクスチャ数

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };

static char* g_TexturName[] = {
    "data/TEXTURE/Numberrr.png",
};

static float g_w, g_h;
static XMFLOAT3 g_Pos;
static int g_TexNo;

// タイマー用
static DWORD g_StartTime = 0;
static DWORD g_ElapsedTime = 0;
static BOOL  g_Running = FALSE;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitScore(void)
{
    // テクスチャ生成
    for (int i = 0; i < TEXTURE_MAX; i++)
    {
        g_Texture[i] = NULL;
        D3DX11CreateShaderResourceViewFromFile(
            GetDevice(), g_TexturName[i], NULL, NULL, &g_Texture[i], NULL);
    }

    // 頂点バッファ生成
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(VERTEX_3D) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

    g_w = TEXTURE_WIDTH;
    g_h = TEXTURE_HEIGHT;
    g_Pos = { 360.0f, 20.0f, 0.0f };  // 表示位置（必要なら調整）
    g_TexNo = 0;

    g_StartTime = 0;
    g_ElapsedTime = 0;
    g_Running = FALSE;

    return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitScore(void)
{
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
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateScore(void)
{
    if (g_Running)
    {
        g_ElapsedTime = timeGetTime() - g_StartTime;
    }
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawScore(void)
{
    // 頂点バッファ設定
    UINT stride = sizeof(VERTEX_3D);
    UINT offset = 0;
    GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

    SetWorldViewProjection2D();
    GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    MATERIAL material{};
    material.Diffuse = XMFLOAT4(1, 1, 1, 1);
    SetMaterial(material);

    GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

    // 経過時間 → 分, 秒, ミリ秒
    DWORD msAll = g_ElapsedTime;
    int mm = msAll / 60000;
    int ss = (msAll / 1000) % 60;
    int ms = (msAll % 1000) / 10; // 2桁ミリ秒

    // 桁ごとの配列：MM:SS:MS
    // -1 を「冒号」にする（テクスチャの最後の列を使用）
    int layout[8] = {
        mm / 10, mm % 10,
        -1,                 // 冒号
        ss / 10, ss % 10,
        -1,                 // 冒号
        ms / 10, ms % 10
    };

    float startX = g_Pos.x;
    float startY = g_Pos.y;
    float tw = 1.0f / 11.0f;  // 数字(0-9) + 冒号(10) → 11列

    for (int i = 0; i < 8; i++)
    {
        int d = layout[i];
        float tx;

        if (d == -1) {
            // 冒号：最后一列 (index = 10)
            tx = 10 * tw;
        }
        else {
            // 普通数字
            tx = d * tw;
        }

        SetSpriteLeftTop(g_VertexBuffer,
            startX + i * g_w, startY,
            g_w, g_h,
            tx, 0.0f, tw, 1.0f);

        GetDeviceContext()->Draw(4, 0);
    }
}


//=============================================================================
// タイマー制御
//=============================================================================
void StartTimer()
{
    g_StartTime = timeGetTime();
    g_ElapsedTime = 0;
    g_Running = TRUE;
}

void StopTimer()
{
    if (g_Running)
    {
        g_ElapsedTime = timeGetTime() - g_StartTime;
        g_Running = FALSE;
    }
}

int GetElapsedTime(void)
{
    return g_ElapsedTime / 1000;
}

int GetElapsedTimeMs()
{
    return (int)g_ElapsedTime;
}

void Score_SetPosition(float x, float y)
{
    // g_Pos 在 score.cpp 内部就是显示起点坐标
    // 默认为 {450, 5, 0}，改成传入坐标即可
    // g_Pos.z 不用动
    extern XMFLOAT3 g_Pos; // 如果在同文件顶端可见则无需 extern
    g_Pos.x = x;
    g_Pos.y = y;
}

void Score_SetScale(float scale)
{
    // g_w/g_h 是每个字符的绘制尺寸；默认 22x38
    // 用统一缩放即可得到想要的大小
    extern float g_w, g_h;
    g_w = 22.0f * scale;  // TEXTURE_WIDTH = 22
    g_h = 38.0f * scale;  // TEXTURE_HEIGHT = 38
}
