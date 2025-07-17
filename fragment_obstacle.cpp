//=============================================================================
//
// ��Q���̓�����Е`�� [fragment_obstacle.cpp]
//
//=============================================================================
#include "main.h"
#include "fragment_obstacle.h"
#include "camera.h"
#include <time.h>

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_POLYGON (6)
#define SIZE_WH     (100.0f)

//*****************************************************************************
// �O���[�o���ϐ�
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
// ���_��`
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
// ����������
//*****************************************************************************
HRESULT InitObstacleFragment(void)
{
    // ���_�o�b�t�@�쐬
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(VERTEX_3D) * 4;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = g_VertexArray;

    GetDevice()->CreateBuffer(&bd, &sd, &g_VertexBuffer);

    // �e�N�X�`���ǂݍ���
    for (int t = 0; t < OBSTACLE_TYPES; t++) {
        for (int i = 0; i < TEXTURE_PER_TYPE; i++) {
            if (g_TextureNames[t][i] == NULL) continue;
            D3DX11CreateShaderResourceViewFromFile(GetDevice(), g_TextureNames[t][i], NULL, NULL, &g_Texture[t][i], NULL);
        }
    }

    // �����_���z�u
    srand((unsigned int)time(NULL));
    for (int i = 0; i < OBSTACLE_MAX; i++) {
        g_Obstacle[i].use = TRUE;
        g_Obstacle[i].type = rand() % OBSTACLE_TYPES;
        g_Obstacle[i].texIndex = rand() % TEXTURE_PER_TYPE;
        g_Obstacle[i].pos = XMFLOAT3(rand() % 1000 - 500, 0.0f, rand() % 1000 - 500);
        g_Obstacle[i].scl = XMFLOAT3(0.5f + (rand() % 100) / 100.0f, 0.5f + (rand() % 100) / 100.0f, 1.0f);
        g_Obstacle[i].rot = XMFLOAT3(0.0f, XMConvertToRadians(rand() % 360), 0.0f);
    }

    //// 0�Ԗڂ̏�Q�����u�L�v��0�ԃe�N�X�`���icat_01.png�j�Ƃ��Ĕz�u�B�ʒu��(-300, 0, 100)�A�X�P�[����(1,1,1)�A��]�Ȃ��B
    //SetObstacle(
    //    0,              // ��Q���z��̃C���f�b�N�X�i0�ԁj
    //    1,              // ��ށF1 = �L�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
    //    0,              // �L��0�Ԗڂ̃e�N�X�`���icat_01.png�j
    //    { -300, 0, 100 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
    //    { 1, 1, 1 },       // �X�P�[���iX, Y, Z�j��������
    //    { 0, 0, 0 }        // ��]�iX, Y, Z�j����]�Ȃ�
    //);
    //// 1�Ԗڂ̏�Q�����u���v��2�ԃe�N�X�`���idog_03.png�j�Ƃ��Ĕz�u�B�ʒu��(150, 0, -250)�A�X�P�[����(1.2, 1.2, 1.0)�AY����45�x��]�B
    //SetObstacle(
    //    1,                      // ��Q���z��̃C���f�b�N�X�i1�ԁj
    //    0,                      // ��ށF0 = ���i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
    //    2,                      // ����2�Ԗڂ̃e�N�X�`���idog_03.png�j
    //    { 150, 0, -250 },       // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
    //    { 1.2f, 1.2f, 1.0f },   // �X�P�[���iX, Y, Z�j
    //    { 0, XMConvertToRadians(45), 0 }  // ��]�iX, Y, Z�j��Y����45�x��]
    //);

    //for (int i = 2; i < OBSTACLE_MAX; i++) {
    //    g_Obstacle[i].use = FALSE;
    //}

    return S_OK;
}

//*****************************************************************************
// �I������
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
// �X�V�����i�K�v�Ȃ��j
//*****************************************************************************
void UpdateObstacleFragment(void)
{
    // �`���p�Ȃ牽������OK
}

//*****************************************************************************
// �`�揈��
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


