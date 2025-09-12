#include "main.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "bg.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX    (1)                     // テクスチャの数
#define MAX_POLYGON    (36 * 36 * 6)           // 最大ポリゴン数（頂点数）
#define VALUE_MOVE     (5.0f)                  // 移動量
#define VALUE_ROTATE   (XM_PI * 0.02f)         // 回転量
#define SIZE_WH_Y      (2800.0f)               // 球体の半径

//*****************************************************************************
// グローバル変数
//*****************************************************************************
HRESULT MakeVertexBG(void);

static ID3D11Buffer* g_VertexBuffer = NULL;    // 頂点バッファ
static ID3D11Buffer* g_IndexBuffer = NULL;     // インデックスバッファ
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL }; // テクスチャ情報
static BG g_bgc;                              // ポリゴンデータ
static int g_TexNo;                            // テクスチャ番号

static char* g_TextureName[] = {
    "data/TEXTURE/5.png",
};

static VERTEX_3D g_VertexArray[MAX_POLYGON];  // 球体の頂点データ
static unsigned int g_Indices[MAX_POLYGON];   // インデックス配列

//*****************************************************************************
// 初期化処理
//*****************************************************************************
HRESULT InitBG(void)
{
    // 頂点バッファの作成
    HRESULT hr = MakeVertexBG();
    if (FAILED(hr))
    {
        return hr;
    }

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

    // 位置・回転・スケールの初期設定
    g_bgc.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    g_bgc.rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
    g_bgc.scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
    g_bgc.rotSpeed = XMFLOAT3(0.0f, XM_PI * 0.0001f, 0.0f); // Y轴方向的自转速度

    g_TexNo = 0;

    return S_OK;
}

//*****************************************************************************
// 終了処理
//*****************************************************************************
void UninitBG(void)
{
    if (g_VertexBuffer)
    {
        g_VertexBuffer->Release();
        g_VertexBuffer = NULL;
    }

    if (g_IndexBuffer)
    {
        g_IndexBuffer->Release();
        g_IndexBuffer = NULL;
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

//*****************************************************************************
// 更新処理
//*****************************************************************************
void UpdateBG(void)
{
    g_bgc.rot.y += g_bgc.rotSpeed.y;

    // 回転角の制限
    if (g_bgc.rot.y > XM_2PI) g_bgc.rot.y -= XM_2PI;
    if (g_bgc.rot.y < 0.0f) g_bgc.rot.y += XM_2PI;

}

//*****************************************************************************
// 描画処理
//*****************************************************************************

void DrawBG(void)
{
    UINT stride = sizeof(VERTEX_3D);
    UINT offset = 0;

    // 頂点バッファ

    GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);
    GetDeviceContext()->IASetIndexBuffer(g_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    MATERIAL material;
    ZeroMemory(&material, sizeof(material));
    material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    SetMaterial(material);

    GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

    // ワールドマトリックスを計算する
    XMMATRIX mtxWorld = XMMatrixIdentity();
    XMMATRIX mtxScl = XMMatrixScaling(g_bgc.scl.x, g_bgc.scl.y, g_bgc.scl.z);
    XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_bgc.rot.x, g_bgc.rot.y, g_bgc.rot.z);
    XMMATRIX mtxTranslate = XMMatrixTranslation(g_bgc.pos.x, g_bgc.pos.y, g_bgc.pos.z);
    mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);
    mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);
    mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);
    SetWorldMatrix(&mtxWorld);

    // 描画
    GetDeviceContext()->DrawIndexed(MAX_POLYGON, 0, 0);
}

//*****************************************************************************
// 頂点バッファ生成処理
//*****************************************************************************
HRESULT MakeVertexBG(void)
{
    const int slices = 36;  // 球の横分割数（経度）
    const int stacks = 18;  // 球の縦分割数（緯度）
    const float radius = SIZE_WH_Y; // 球の半径

    int index = 0, idx = 0;

    // 頂点データの生成
    for (int i = 0; i <= stacks; i++)
    {
        float phi = XM_PI * i / stacks; // 緯度角（垂直方向の角度）
        for (int j = 0; j <= slices; j++)
        {
            float theta = XM_2PI * j / slices; // 経度角（水平方向の角度）

            float x = radius * sinf(phi) * cosf(theta);
            float y = radius * cosf(phi);
            float z = radius * sinf(phi) * sinf(theta);

            float u = -(float)j / slices; // テクスチャ座標 U
            float v = (float)i / stacks;  // テクスチャ座標 V

            g_VertexArray[index++] = { XMFLOAT3(x, y, z), XMFLOAT3(x / radius, y / radius, z / radius), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT2(u, v) };
        }
    }

    // インデックスデータの生成
    for (int i = 0; i < stacks; i++)
    {
        for (int j = 0; j < slices; j++)
        {
            g_Indices[idx++] = i * (slices + 1) + j;
            g_Indices[idx++] = (i + 1) * (slices + 1) + j;
            g_Indices[idx++] = i * (slices + 1) + (j + 1);

            g_Indices[idx++] = (i + 1) * (slices + 1) + j;
            g_Indices[idx++] = (i + 1) * (slices + 1) + (j + 1);
            g_Indices[idx++] = i * (slices + 1) + (j + 1);
        }
    }

    // 頂点バッファの作成
    D3D11_BUFFER_DESC vertexBufferDesc = {};
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VERTEX_3D) * index;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = g_VertexArray;

    HRESULT hr = GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &g_VertexBuffer);
    if (FAILED(hr)) return hr;

    // インデックスバッファの作成
    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned int) * idx;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = g_Indices;

    return GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &g_IndexBuffer);
}


////=============================================================================
//// BG構造体の先頭アドレスを取得
////=============================================================================
//BG* GetBG(void)
//{
//    return &g_BG;
//}
