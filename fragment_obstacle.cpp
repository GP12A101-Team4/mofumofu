//=============================================================================
//
// 障害物の動物碎片描画 [fragment_obstacle.cpp]
//
//=============================================================================
#include "main.h"
#include "fragment_obstacle.h"
#include "camera.h"
#include <time.h>

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_POLYGON (6)
#define SIZE_WH     (100.0f)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;
static ID3D11ShaderResourceView* g_Texture[OBSTACLE_TYPES][TEXTURE_PER_TYPE] = { NULL };

static OBSTACLE_FRAGMENT g_Obstacle[OBSTACLE_MAX];

static char* g_TextureNames[OBSTACLE_TYPES][TEXTURE_PER_TYPE] = {
    // Dog
    { "data/TEXTURE/dog_01.png", "data/TEXTURE/dog_02.png", "data/TEXTURE/dog_03.png", "data/TEXTURE/dog_04.png", "data/TEXTURE/dog_05.png" },
    // Cat
    { "data/TEXTURE/cat_01.png", "data/TEXTURE/cat_02.png", "data/TEXTURE/cat_03.png", "data/TEXTURE/cat_04.png" },
    // Elephant
    { "data/TEXTURE/elephant_01.png", "data/TEXTURE/elephant_02.png", "data/TEXTURE/elephant_03.png", "data/TEXTURE/elephant_04.png", "data/TEXTURE/elephant_05.png", "data/TEXTURE/elephant_06.png" },
    // Mouse
    { "data/TEXTURE/mouse_01.png", "data/TEXTURE/mouse_02.png", "data/TEXTURE/mouse_03.png", "data/TEXTURE/mouse_04.png", "data/TEXTURE/mouse_05.png" },
    // Sheep
    { "data/TEXTURE/sheep_01.png", "data/TEXTURE/sheep_02.png", "data/TEXTURE/sheep_03.png", "data/TEXTURE/sheep_04.png" },
};

//*****************************************************************************
// 頂点定義
//*****************************************************************************
static VERTEX_3D g_VertexArray[4] = {
    { XMFLOAT3(-SIZE_WH,  SIZE_WH, 0.0f), XMFLOAT3(0,0,-1), XMFLOAT4(1,1,1,1), XMFLOAT2(0,0) },
    { XMFLOAT3(SIZE_WH,  SIZE_WH, 0.0f), XMFLOAT3(0,0,-1), XMFLOAT4(1,1,1,1), XMFLOAT2(1,0) },
    { XMFLOAT3(-SIZE_WH, -SIZE_WH, 0.0f), XMFLOAT3(0,0,-1), XMFLOAT4(1,1,1,1), XMFLOAT2(0,1) },
    { XMFLOAT3(SIZE_WH, -SIZE_WH, 0.0f), XMFLOAT3(0,0,-1), XMFLOAT4(1,1,1,1), XMFLOAT2(1,1) },
};

void SetObstacle(int i, int type, int texIndex, XMFLOAT3 pos, XMFLOAT3 scl, XMFLOAT3 rot)
{
    g_Obstacle[i].use = TRUE;
    g_Obstacle[i].type = type;
    g_Obstacle[i].texIndex = texIndex;
    g_Obstacle[i].pos = pos;
    g_Obstacle[i].scl = scl;
    g_Obstacle[i].rot = rot;
}

//*****************************************************************************
// 初期化処理
//*****************************************************************************
HRESULT InitObstacleFragment(void)
{
    // 頂点バッファ作成
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(VERTEX_3D) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = g_VertexArray;

    GetDevice()->CreateBuffer(&bd, &sd, &g_VertexBuffer);

    // テクスチャ読み込み
    for (int t = 0; t < OBSTACLE_TYPES; t++) {
        for (int i = 0; i < TEXTURE_PER_TYPE; i++) {
            if (g_TextureNames[t][i] == NULL) continue;
            D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureNames[t][i], NULL, NULL, &g_Texture[t][i], NULL);
        }
    }

    // ランダム配置
    srand((unsigned int)time(NULL));
    for (int i = 0; i < OBSTACLE_MAX; i++) {
        g_Obstacle[i].use = TRUE;
        g_Obstacle[i].type = rand() % OBSTACLE_TYPES;
        g_Obstacle[i].texIndex = rand() % TEXTURE_PER_TYPE;
        g_Obstacle[i].pos = XMFLOAT3(rand() % 1000 - 500, 0.0f, rand() % 1000 - 500);
        g_Obstacle[i].scl = XMFLOAT3(0.5f + (rand() % 100) / 100.0f, 0.5f + (rand() % 100) / 100.0f, 1.0f);
        g_Obstacle[i].rot = XMFLOAT3(0.0f, XMConvertToRadians(rand() % 360), 0.0f);
    }

    //// 0番目の障害物を「猫」の0番テクスチャ（cat_01.png）として配置。位置は(-300, 0, 100)、スケールは(1,1,1)、回転なし。
    //SetObstacle(
    //    0,              // 障害物配列のインデックス（0番）
    //    1,              // 種類：1 = 猫（0=犬, 1=猫, 2=象, 3=鼠, 4=羊）
    //    0,              // 猫の0番目のテクスチャ（cat_01.png）
    //    { -300, 0, 100 }, // ワールド空間での配置座標（X, Y, Z）
    //    { 1, 1, 1 },       // スケール（X, Y, Z）＝原寸大
    //    { 0, 0, 0 }        // 回転（X, Y, Z）＝回転なし
    //);
    //// 1番目の障害物を「犬」の2番テクスチャ（dog_03.png）として配置。位置は(150, 0, -250)、スケールは(1.2, 1.2, 1.0)、Y軸に45度回転。
    //SetObstacle(
    //    1,                      // 障害物配列のインデックス（1番）
    //    0,                      // 種類：0 = 犬（0=犬, 1=猫, 2=象, 3=鼠, 4=羊）
    //    2,                      // 犬の2番目のテクスチャ（dog_03.png）
    //    { 150, 0, -250 },       // ワールド空間での配置座標（X, Y, Z）
    //    { 1.2f, 1.2f, 1.0f },   // スケール（X, Y, Z）
    //    { 0, XMConvertToRadians(45), 0 }  // 回転（X, Y, Z）＝Y軸に45度回転
    //);

    //for (int i = 2; i < OBSTACLE_MAX; i++) {
    //    g_Obstacle[i].use = FALSE;
    //}

    return S_OK;
}

//*****************************************************************************
// 終了処理
//*****************************************************************************
void UninitObstacleFragment(void)
{
    if (g_VertexBuffer) {
        g_VertexBuffer->Release();
        g_VertexBuffer = NULL;
    }

    for (int t = 0; t < OBSTACLE_TYPES; t++) {
        for (int i = 0; i < TEXTURE_PER_TYPE; i++) {
            if (g_Texture[t][i]) {
                g_Texture[t][i]->Release();
                g_Texture[t][i] = NULL;
            }
        }
    }
}

//*****************************************************************************
// 更新処理（必要なし）
//*****************************************************************************
void UpdateObstacleFragment(void)
{
    // 描画専用なら何もせずOK
}

//*****************************************************************************
// 描画処理
//*****************************************************************************
void DrawObstacleFragment(void)
{
    UINT stride = sizeof(VERTEX_3D);
    UINT offset = 0;
    GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);
    GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    SetLightEnable(FALSE);
    SetCullingMode(CULL_MODE_NONE);
    SetDepthEnable(FALSE);

    MATERIAL mat = {};
    mat.Diffuse = XMFLOAT4(1, 1, 1, 1);
    SetMaterial(mat);

    for (int i = 0; i < OBSTACLE_MAX; i++) {
        if (!g_Obstacle[i].use) continue;

        XMMATRIX mtxWorld = XMMatrixIdentity();
        XMMATRIX mtxScl = XMMatrixScaling(g_Obstacle[i].scl.x, g_Obstacle[i].scl.y, g_Obstacle[i].scl.z);
        XMMATRIX mtxRot = XMMatrixRotationRollPitchYaw(g_Obstacle[i].rot.x, g_Obstacle[i].rot.y, g_Obstacle[i].rot.z);
        XMMATRIX mtxTranslate = XMMatrixTranslation(g_Obstacle[i].pos.x, g_Obstacle[i].pos.y + 60.0f, g_Obstacle[i].pos.z + 200.0f);
        mtxWorld = mtxScl * mtxRot * mtxTranslate;

        SetWorldMatrix(&mtxWorld);
        XMStoreFloat4x4(&g_Obstacle[i].mtxWorld, mtxWorld);

        ID3D11ShaderResourceView* tex = g_Texture[g_Obstacle[i].type][g_Obstacle[i].texIndex];
        GetDeviceContext()->PSSetShaderResources(0, 1, &tex);

        GetDeviceContext()->Draw(4, 0);
    }

    SetLightEnable(TRUE);
}


