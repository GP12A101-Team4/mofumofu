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
    //srand((unsigned int)time(NULL));
    //for (int i = 0; i < OBSTACLE_MAX; i++) {
    //    g_Obstacle[i].use = TRUE;
    //    g_Obstacle[i].type = rand() % OBSTACLE_TYPES;
    //    g_Obstacle[i].texIndex = rand() % TEXTURE_PER_TYPE;
    //    g_Obstacle[i].pos = XMFLOAT3(rand() % 1000 - 500, 0.0f, rand() % 1000 - 500);
    //    g_Obstacle[i].scl = XMFLOAT3(0.5f + (rand() % 100) / 100.0f, 0.5f + (rand() % 100) / 100.0f, 1.0f);
    //    g_Obstacle[i].rot = XMFLOAT3(0.0f, XMConvertToRadians(rand() % 360), 0.0f);
    //}

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
    // 0�Ԗڂ̏�Q�����u�ہv��3�ԃe�N�X�`���ielephant_04.png�j�Ƃ��Ĕz�u�B�ʒu��(-200, -10, 100)�A�X�P�[����(1.2, 1.2, 1.0)�AY����30�x��]�B
    SetObstacle(
        0,              // ��Q���z��̃C���f�b�N�X�i0�ԁj
        2,              // ��ށF2 = �ہi0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        3,              // �ۂ�3�Ԗڂ̃e�N�X�`���ielephant_04.png�j
        { -200, 20, 100 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.2f, 1.2f, 1.0f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(30), 0 }  // ��]�iX, Y, Z�j��Y����30�x��]
    );

    // 1�Ԗڂ̏�Q�����u�L�v��1�ԃe�N�X�`���icat_02.png�j�Ƃ��Ĕz�u�B�ʒu��(600, 50, 300)�A�X�P�[����(1.1, 1.1, 1.1)�AY����90�x��]�B
    SetObstacle(
        1,              // ��Q���z��̃C���f�b�N�X�i1�ԁj
        1,              // ��ށF1 = �L�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        1,              // �L��1�Ԗڂ̃e�N�X�`���icat_02.png�j
        { 600, 50, 300 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.1f, 1.1f, 1.1f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(90), 0 }  // ��]�iX, Y, Z�j��Y����90�x��]
    );

    // 2�Ԗڂ̏�Q�����u���v��2�ԃe�N�X�`���idog_03.png�j�Ƃ��Ĕz�u�B�ʒu��(-500, 70, -500)�A�X�P�[����(1.3, 1.3, 1.2)�AY����180�x��]�B
    SetObstacle(
        2,              // ��Q���z��̃C���f�b�N�X�i2�ԁj
        0,              // ��ށF0 = ���i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        2,              // ����2�Ԗڂ̃e�N�X�`���idog_03.png�j
        { -500, 70, -500 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.3f, 1.3f, 1.2f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(180), 0 }  // ��]�iX, Y, Z�j��Y����180�x��]
    );

    // 3�Ԗڂ̏�Q�����u�r�v��2�ԃe�N�X�`���isheep_03.png�j�Ƃ��Ĕz�u�B�ʒu��(800, -20, 50)�A�X�P�[����(1.4, 1.4, 1.3)�A��]�Ȃ��B
    SetObstacle(
        3,              // ��Q���z��̃C���f�b�N�X�i3�ԁj
        4,              // ��ށF4 = �r�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        2,              // �r��2�Ԗڂ̃e�N�X�`���isheep_03.png�j
        { 800, -20, 50 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.4f, 1.4f, 1.3f }, // �X�P�[���iX, Y, Z�j
        { 0, 0, 0 }     // ��]�iX, Y, Z�j����]�Ȃ�
    );

    // 4�Ԗڂ̏�Q�����u�l�v��1�ԃe�N�X�`���imouse_02.png�j�Ƃ��Ĕz�u�B�ʒu��(-1200, -4, 600)�A�X�P�[����(0.9, 0.9, 0.9)�AY����45�x��]�B
    SetObstacle(
        4,              // ��Q���z��̃C���f�b�N�X�i4�ԁj
        3,              // ��ށF3 = �l�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        1,              // �l��1�Ԗڂ̃e�N�X�`���imouse_02.png�j
        { -1200, -4, 600 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 0.9f, 0.9f, 0.9f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(45), 0 }  // ��]�iX, Y, Z�j��Y����45�x��]
    );

    // 5�Ԗڂ̏�Q�����u�ہv��5�ԃe�N�X�`���ielephant_06.png�j�Ƃ��Ĕz�u�B�ʒu��(-100, 100, -200)�A�X�P�[����(1.6, 1.6, 1.4)�AY����135�x��]�B
    SetObstacle(
        5,              // ��Q���z��̃C���f�b�N�X�i5�ԁj
        2,              // ��ށF2 = �ہi0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        5,              // �ۂ�5�Ԗڂ̃e�N�X�`���ielephant_06.png�j
        { -100, 40, -200 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.6f, 1.6f, 1.4f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(135), 0 }  // ��]�iX, Y, Z�j��Y����135�x��]
    );

    // 6�Ԗڂ̏�Q�����u�L�v��0�ԃe�N�X�`���icat_01.png�j�Ƃ��Ĕz�u�B�ʒu��(1400, 60, 400)�A�X�P�[����(1.0, 1.0, 1.0)�AY����270�x��]�B
    SetObstacle(
        6,              // ��Q���z��̃C���f�b�N�X�i6�ԁj
        1,              // ��ށF1 = �L�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        0,              // �L��0�Ԗڂ̃e�N�X�`���icat_01.png�j
        { 1400, 60, 400 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.0f, 1.0f, 1.0f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(270), 0 }  // ��]�iX, Y, Z�j��Y����270�x��]
    );

    // 8�Ԗ�
    SetObstacle(
        8,              // ��Q���z��̃C���f�b�N�X�i8�ԁj
        4,              // ��ށF4 = �r�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        3,              // �r��3�Ԗڂ̃e�N�X�`���isheep_04.png�j
        { 900, 30, -100 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 0.8f, 0.8f, 0.8f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(150), 0 }  // ��]�iX, Y, Z�j��Y����150�x��]
    );

    // 9�Ԗڂ̏�Q�����u�l�v��4�ԃe�N�X�`���imouse_05.png�j�Ƃ��Ĕz�u�B�ʒu��(-800, 80, -300)�A�X�P�[����(1.5, 1.5, 1.3)�A��]�Ȃ��B
    SetObstacle(
        9,              // ��Q���z��̃C���f�b�N�X�i9�ԁj
        3,              // ��ށF3 = �l�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        4,              // �l��4�Ԗڂ̃e�N�X�`���imouse_05.png�j
        { -800, 80, -300 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.5f, 1.5f, 1.3f }, // �X�P�[���iX, Y, Z�j
        { 0, 0, 0 }     // ��]�iX, Y, Z�j����]�Ȃ�
    );

    // 10�Ԗڂ̏�Q�����u�ہv��2�ԃe�N�X�`���ielephant_03.png�j�Ƃ��Ĕz�u�B�ʒu��(1100, -10, 500)�A�X�P�[����(1.7, 1.7, 1.5)�AY����210�x��]�B
    SetObstacle(
        10,             // ��Q���z��̃C���f�b�N�X�i10�ԁj
        2,              // ��ށF2 = �ہi0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        2,              // �ۂ�2�Ԗڂ̃e�N�X�`���ielephant_03.png�j
        { 1100, 30, 500 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.7f, 1.7f, 1.5f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(210), 0 }  // ��]�iX, Y, Z�j��Y����210�x��]
    );

    // 11�Ԗڂ̏�Q�����u�L�v��3�ԃe�N�X�`���icat_04.png�j�Ƃ��Ĕz�u�B�ʒu��(-50, 40, 150)�A�X�P�[����(1.3, 1.3, 1.2)�AY����300�x��]�B
    SetObstacle(
        11,             // ��Q���z��̃C���f�b�N�X�i11�ԁj
        1,              // ��ށF1 = �L�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        3,              // �L��3�Ԗڂ̃e�N�X�`���icat_04.png�j
        { -50, 40, 150 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.3f, 1.3f, 1.2f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(300), 0 }  // ��]�iX, Y, Z�j��Y����300�x��]
    );

    // 12�Ԗڂ̏�Q�����u���v��1�ԃe�N�X�`���idog_02.png�j�Ƃ��Ĕz�u�B�ʒu��(650, -15, -700)�A�X�P�[����(1.4, 1.4, 1.3)�AY����75�x��]�B
    SetObstacle(
        12,             // ��Q���z��̃C���f�b�N�X�i12�ԁj
        0,              // ��ށF0 = ���i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        1,              // ����1�Ԗڂ̃e�N�X�`���idog_02.png�j
        { 650, -15, -700 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.4f, 1.4f, 1.3f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(75), 0 }  // ��]�iX, Y, Z�j��Y����75�x��]
    );

    // 13�Ԗڂ̏�Q�����u�r�v��0�ԃe�N�X�`���isheep_01.png�j�Ƃ��Ĕz�u�B�ʒu��(-600, 10, 400)�A�X�P�[����(1.1, 1.1, 1.0)�AY����120�x��]�B
    SetObstacle(
        13,             // ��Q���z��̃C���f�b�N�X�i13�ԁj
        4,              // ��ށF4 = �r�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        0,              // �r��0�Ԗڂ̃e�N�X�`���isheep_01.png�j
        { -600, 10, 400 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.1f, 1.1f, 1.0f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(120), 0 }  // ��]�iX, Y, Z�j��Y����120�x��]
    );

    // 14�Ԗڂ̏�Q�����u�l�v��0�ԃe�N�X�`���imouse_01.png�j�Ƃ��Ĕz�u�B�ʒu��(1300, 20, -150)�A�X�P�[����(1.6, 1.6, 1.4)�AY����240�x��]�B
    SetObstacle(
        14,             // ��Q���z��̃C���f�b�N�X�i14�ԁj
        3,              // ��ށF3 = �l�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        0,              // �l��0�Ԗڂ̃e�N�X�`���imouse_01.png�j
        { 1300, 20, -150 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.6f, 1.6f, 1.4f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(240), 0 }  // ��]�iX, Y, Z�j��Y����240�x��]
    );

    // 15�Ԗڂ̏�Q�����u�ہv��6�ԃe�N�X�`���ielephant_07.png�j�Ƃ��Ĕz�u�B�ʒu��(200, 90, 300)�A�X�P�[����(1.9, 1.9, 1.7)�A��]�Ȃ��B
    SetObstacle(
        15,             // ��Q���z��̃C���f�b�N�X�i15�ԁj
        2,              // ��ށF2 = �ہi0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        6,              // �ۂ�6�Ԗڂ̃e�N�X�`���ielephant_07.png�j
        { 200, 50, 300 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.9f, 1.9f, 1.7f }, // �X�P�[���iX, Y, Z�j
        { 0, 0, 0 }     // ��]�iX, Y, Z�j����]�Ȃ�
    );

    // 16�Ԗڂ̏�Q�����u�L�v��2�ԃe�N�X�`���icat_03.png�j�Ƃ��Ĕz�u�B�ʒu��(-1100, 50, -600)�A�X�P�[����(0.9, 0.9, 0.9)�AY����90�x��]�B
    SetObstacle(
        16,             // ��Q���z��̃C���f�b�N�X�i16�ԁj
        1,              // ��ށF1 = �L�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        2,              // �L��2�Ԗڂ̃e�N�X�`���icat_03.png�j
        { -1100, 30, -600 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 0.9f, 0.9f, 0.9f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(90), 0 }  // ��]�iX, Y, Z�j��Y����90�x��]
    );

    // 17�Ԗڂ̏�Q�����u���v��3�ԃe�N�X�`���idog_04.png�j�Ƃ��Ĕz�u�B�ʒu��(1200, -25, 100)�A�X�P�[����(1.2, 1.2, 1.1)�AY����165�x��]�B
    SetObstacle(
        17,             // ��Q���z��̃C���f�b�N�X�i17�ԁj
        0,              // ��ށF0 = ���i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        3,              // ����3�Ԗڂ̃e�N�X�`���idog_04.png�j
        { 1200, -25, 100 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.2f, 1.2f, 1.1f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(165), 0 }  // ��]�iX, Y, Z�j��Y����165�x��]
    );

    // 18�Ԗڂ̏�Q�����u�r�v��1�ԃe�N�X�`���isheep_02.png�j�Ƃ��Ĕz�u�B�ʒu��(-450, 70, -800)�A�X�P�[����(1.5, 1.5, 1.3)�AY����45�x��]�B
    SetObstacle(
        18,             // ��Q���z��̃C���f�b�N�X�i18�ԁj
        4,              // ��ށF4 = �r�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        1,              // �r��1�Ԗڂ̃e�N�X�`���isheep_02.png�j
        { -450, 70, -800 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.5f, 1.5f, 1.3f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(45), 0 }  // ��]�iX, Y, Z�j��Y����45�x��]
    );

    // 19�Ԗڂ̏�Q�����u�l�v��2�ԃe�N�X�`���imouse_03.png�j�Ƃ��Ĕz�u�B�ʒu��(950, 60, 600)�A�X�P�[����(1.1, 1.1, 1.0)�AY����315�x��]�B
    SetObstacle(
        19,             // ��Q���z��̃C���f�b�N�X�i19�ԁj
        3,              // ��ށF3 = �l�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        2,              // �l��2�Ԗڂ̃e�N�X�`���imouse_03.png�j
        { 950, 60, 600 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.1f, 1.1f, 1.0f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(315), 0 }  // ��]�iX, Y, Z�j��Y����315�x��]
    );
    // 20�Ԗڂ̏�Q�����u���v��0�ԃe�N�X�`���idog_01.png�j�Ƃ��Ĕz�u�B�ʒu��(-300, -20, 700)�A�X�P�[����(1.0, 1.0, 1.0)�AY����30�x��]�B
    SetObstacle(
        20,             // ��Q���z��̃C���f�b�N�X�i20�ԁj
        0,              // ��ށF0 = ���i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        0,              // ����0�Ԗڂ̃e�N�X�`���idog_01.png�j
        { -300, -20, 700 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.0f, 1.0f, 1.0f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(30), 0 }  // ��]�iX, Y, Z�j��Y����30�x��]
    );

    // 21�Ԗڂ̏�Q�����u�ہv��0�ԃe�N�X�`���ielephant_01.png�j�Ƃ��Ĕz�u�B�ʒu��(700, 15, -400)�A�X�P�[����(1.4, 1.4, 1.2)�AY����90�x��]�B
    SetObstacle(
        21,             // ��Q���z��̃C���f�b�N�X�i21�ԁj
        2,              // ��ށF2 = �ہi0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        0,              // �ۂ�0�Ԗڂ̃e�N�X�`���ielephant_01.png�j
        { 700, 15, -400 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.4f, 1.4f, 1.2f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(90), 0 }  // ��]�iX, Y, Z�j��Y����90�x��]
    );

    // 22�Ԗڂ̏�Q�����u�r�v��3�ԃe�N�X�`���isheep_04.png�j�Ƃ��Ĕz�u�B�ʒu��(-700, 25, 200)�A�X�P�[����(1.2, 1.2, 1.1)�AY����150�x��]�B
    SetObstacle(
        22,             // ��Q���z��̃C���f�b�N�X�i22�ԁj
        4,              // ��ށF4 = �r�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        3,              // �r��3�Ԗڂ̃e�N�X�`���isheep_04.png�j
        { -700, 25, 200 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.2f, 1.2f, 1.1f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(150), 0 }  // ��]�iX, Y, Z�j��Y����150�x��]
    );

    // 23�Ԗڂ̏�Q�����u�L�v��1�ԃe�N�X�`���icat_02.png�j�Ƃ��Ĕz�u�B�ʒu��(1000, 40, -300)�A�X�P�[����(0.8, 0.8, 0.8)�A��]�Ȃ��B
    SetObstacle(
        23,             // ��Q���z��̃C���f�b�N�X�i23�ԁj
        1,              // ��ށF1 = �L�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        1,              // �L��1�Ԗڂ̃e�N�X�`���icat_02.png�j
        { 1000, 40, -300 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 0.8f, 0.8f, 0.8f }, // �X�P�[���iX, Y, Z�j
        { 0, 0, 0 }     // ��]�iX, Y, Z�j����]�Ȃ�
    );

    // 24�Ԗڂ̏�Q�����u�l�v��3�ԃe�N�X�`���imouse_04.png�j�Ƃ��Ĕz�u�B�ʒu��(-150, 35, -600)�A�X�P�[����(1.3, 1.3, 1.2)�AY����180�x��]�B
    SetObstacle(
        24,             // ��Q���z��̃C���f�b�N�X�i24�ԁj
        3,              // ��ށF3 = �l�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        3,              // �l��3�Ԗڂ̃e�N�X�`���imouse_04.png�j
        { -150, 35, -600 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.3f, 1.3f, 1.2f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(180), 0 }  // ��]�iX, Y, Z�j��Y����180�x��]
    );

    // 25�Ԗڂ̏�Q�����u�ہv��4�ԃe�N�X�`���ielephant_05.png�j�Ƃ��Ĕz�u�B�ʒu��(1350, 80, 150)�A�X�P�[����(1.7, 1.7, 1.5)�AY����270�x��]�B
    SetObstacle(
        25,             // ��Q���z��̃C���f�b�N�X�i25�ԁj
        2,              // ��ށF2 = �ہi0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        4,              // �ۂ�4�Ԗڂ̃e�N�X�`���ielephant_05.png�j
        { 1350, 35, 150 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.7f, 1.7f, 1.5f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(270), 0 }  // ��]�iX, Y, Z�j��Y����270�x��]
    );

    // 26�Ԗڂ̏�Q�����u���v��2�ԃe�N�X�`���idog_03.png�j�Ƃ��Ĕz�u�B�ʒu��(-900, -30, -200)�A�X�P�[����(1.1, 1.1, 1.0)�AY����60�x��]�B
    SetObstacle(
        26,             // ��Q���z��̃C���f�b�N�X�i26�ԁj
        0,              // ��ށF0 = ���i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        2,              // ����2�Ԗڂ̃e�N�X�`���idog_03.png�j
        { -900, -30, -200 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.1f, 1.1f, 1.0f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(60), 0 }  // ��]�iX, Y, Z�j��Y����60�x��]
    );

    // 27�Ԗڂ̏�Q�����u�r�v��0�ԃe�N�X�`���isheep_01.png�j�Ƃ��Ĕz�u�B�ʒu��(400, -35, 800)�A�X�P�[����(1.5, 1.5, 1.3)�AY����225�x��]�B
    SetObstacle(
        27,             // ��Q���z��̃C���f�b�N�X�i27�ԁj
        4,              // ��ށF4 = �r�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        0,              // �r��0�Ԗڂ̃e�N�X�`���isheep_01.png�j
        { 620, 35, 800 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.5f, 1.5f, 1.3f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(225), 0 }  // ��]�iX, Y, Z�j��Y����225�x��]
    );

    // 28�Ԗڂ̏�Q�����u�L�v��0�ԃe�N�X�`���icat_01.png�j�Ƃ��Ĕz�u�B�ʒu��(-1250, 45, 350)�A�X�P�[����(1.2, 1.2, 1.1)�AY����315�x��]�B
    SetObstacle(
        28,             // ��Q���z��̃C���f�b�N�X�i28�ԁj
        1,              // ��ށF1 = �L�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        0,              // �L��0�Ԗڂ̃e�N�X�`���icat_01.png�j
        { -1250, 45, 350 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.2f, 1.2f, 1.1f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(315), 0 }  // ��]�iX, Y, Z�j��Y����315�x��]
    );

    // 29�Ԗڂ̏�Q�����u�l�v��1�ԃe�N�X�`���imouse_02.png�j�Ƃ��Ĕz�u�B�ʒu��(850, 55, 450)�A�X�P�[����(1.6, 1.6, 1.4)�AY����120�x��]�B
    SetObstacle(
        29,             // ��Q���z��̃C���f�b�N�X�i29�ԁj
        3,              // ��ށF3 = �l�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        1,              // �l��1�Ԗڂ̃e�N�X�`���imouse_02.png�j
        { 850, 55, 450 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.6f, 1.6f, 1.4f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(120), 0 }  // ��]�iX, Y, Z�j��Y����120�x��]
    );

    // 30�Ԗڂ̏�Q�����u�ہv��1�ԃe�N�X�`���ielephant_02.png�j�Ƃ��Ĕz�u�B�ʒu��(-500, 65, 650)�A�X�P�[����(1.8, 1.8, 1.6)�A��]�Ȃ��B
    SetObstacle(
        30,             // ��Q���z��̃C���f�b�N�X�i30�ԁj
        2,              // ��ށF2 = �ہi0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        1,              // �ۂ�1�Ԗڂ̃e�N�X�`���ielephant_02.png�j
        { -500, 65, 650 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.8f, 1.8f, 1.6f }, // �X�P�[���iX, Y, Z�j
        { 0, 0, 0 }     // ��]�iX, Y, Z�j����]�Ȃ�
    );

    // 31�Ԗڂ̏�Q�����u���v��4�ԃe�N�X�`���idog_05.png�j�Ƃ��Ĕz�u�B�ʒu��(1150, -10, -500)�A�X�P�[����(0.9, 0.9, 0.9)�AY����45�x��]�B
    SetObstacle(
        31,             // ��Q���z��̃C���f�b�N�X�i31�ԁj
        0,              // ��ށF0 = ���i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        4,              // ����4�Ԗڂ̃e�N�X�`���idog_05.png�j
        { 1150, -10, -500 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 0.9f, 0.9f, 0.9f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(45), 0 }  // ��]�iX, Y, Z�j��Y����45�x��]
    );

    // 32�Ԗڂ̏�Q�����u�r�v��2�ԃe�N�X�`���isheep_03.png�j�Ƃ��Ĕz�u�B�ʒu��(-800, 20, -750)�A�X�P�[����(1.3, 1.3, 1.2)�AY����135�x��]�B
    SetObstacle(
        32,             // ��Q���z��̃C���f�b�N�X�i32�ԁj
        4,              // ��ށF4 = �r�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        2,              // �r��2�Ԗڂ̃e�N�X�`���isheep_03.png�j
        { -800, 20, -750 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.3f, 1.3f, 1.2f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(135), 0 }  // ��]�iX, Y, Z�j��Y����135�x��]
    );

    // 33�Ԗڂ̏�Q�����u�L�v��3�ԃe�N�X�`���icat_04.png�j�Ƃ��Ĕz�u�B�ʒu��(250, 30, 50)�A�X�P�[����(1.4, 1.4, 1.3)�AY����240�x��]�B
    SetObstacle(
        33,             // ��Q���z��̃C���f�b�N�X�i33�ԁj
        1,              // ��ށF1 = �L�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        3,              // �L��3�Ԗڂ̃e�N�X�`���icat_04.png�j
        { 250, 30, 50 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.4f, 1.4f, 1.3f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(240), 0 }  // ��]�iX, Y, Z�j��Y����240�x��]
    );

    // 34�Ԗڂ̏�Q�����u�l�v��4�ԃe�N�X�`���imouse_05.png�j�Ƃ��Ĕz�u�B�ʒu��(-1150, -5, -100)�A�X�P�[����(1.1, 1.1, 1.0)�AY����165�x��]�B
    SetObstacle(
        34,             // ��Q���z��̃C���f�b�N�X�i34�ԁj
        3,              // ��ށF3 = �l�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        4,              // �l��4�Ԗڂ̃e�N�X�`���imouse_05.png�j
        { -1150, -5, -100 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.1f, 1.1f, 1.0f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(165), 0 }  // ��]�iX, Y, Z�j��Y����165�x��]
    );

    // 35�Ԗڂ̏�Q�����u�ہv��6�ԃe�N�X�`���ielephant_07.png�j�Ƃ��Ĕz�u�B�ʒu��(500, 75, 250)�A�X�P�[����(2.0, 2.0, 1.8)�AY����300�x��]�B
    SetObstacle(
        35,             // ��Q���z��̃C���f�b�N�X�i35�ԁj
        2,              // ��ށF2 = �ہi0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        6,              // �ۂ�6�Ԗڂ̃e�N�X�`���ielephant_07.png�j
        { 500, 25, 250 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 2.0f, 2.0f, 1.8f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(300), 0 }  // ��]�iX, Y, Z�j��Y����300�x��]
    );

    // 36�Ԗڂ̏�Q�����u���v��1�ԃe�N�X�`���idog_02.png�j�Ƃ��Ĕz�u�B�ʒu��(-400, -25, 500)�A�X�P�[����(1.2, 1.2, 1.1)�A��]�Ȃ��B
    SetObstacle(
        36,             // ��Q���z��̃C���f�b�N�X�i36�ԁj
        0,              // ��ށF0 = ���i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        1,              // ����1�Ԗڂ̃e�N�X�`���idog_02.png�j
        { -400, -25, 500 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.2f, 1.2f, 1.1f }, // �X�P�[���iX, Y, Z�j
        { 0, 0, 0 }     // ��]�iX, Y, Z�j����]�Ȃ�
    );

    // 37�Ԗڂ̏�Q�����u�r�v��1�ԃe�N�X�`���isheep_02.png�j�Ƃ��Ĕz�u�B�ʒu��(1250, 10, -650)�A�X�P�[����(0.7, 0.7, 0.7)�AY����75�x��]�B
    SetObstacle(
        37,             // ��Q���z��̃C���f�b�N�X�i37�ԁj
        4,              // ��ށF4 = �r�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        1,              // �r��1�Ԗڂ̃e�N�X�`���isheep_02.png�j
        { 1250, 10, -650 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 0.7f, 0.7f, 0.7f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(75), 0 }  // ��]�iX, Y, Z�j��Y����75�x��]
    );

    // 38�Ԗڂ̏�Q�����u�L�v��2�ԃe�N�X�`���icat_03.png�j�Ƃ��Ĕz�u�B�ʒu��(-650, 85, 100)�A�X�P�[����(1.5, 1.5, 1.4)�AY����195�x��]�B
    SetObstacle(
        38,             // ��Q���z��̃C���f�b�N�X�i38�ԁj
        1,              // ��ށF1 = �L�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        2,              // �L��2�Ԗڂ̃e�N�X�`���icat_03.png�j
        { -650, 45, 100 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.5f, 1.5f, 1.4f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(195), 0 }  // ��]�iX, Y, Z�j��Y����195�x��]
    );

    // 39�Ԗڂ̏�Q�����u�l�v��0�ԃe�N�X�`���imouse_01.png�j�Ƃ��Ĕz�u�B�ʒu��(750, 5, -800)�A�X�P�[����(1.7, 1.7, 1.5)�AY����270�x��]�B
    SetObstacle(
        39,             // ��Q���z��̃C���f�b�N�X�i39�ԁj
        3,              // ��ށF3 = �l�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        0,              // �l��0�Ԗڂ̃e�N�X�`���imouse_01.png�j
        { 250, 5, -800 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.7f, 1.7f, 1.5f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(270), 0 }  // ��]�iX, Y, Z�j��Y����270�x��]
    );

    // 40�Ԗڂ̏�Q�����u�ہv��3�ԃe�N�X�`���ielephant_04.png�j�Ƃ��Ĕz�u�B�ʒu��(-350, 95, -450)�A�X�P�[����(1.6, 1.6, 1.3)�AY����105�x��]�B
    SetObstacle(
        40,             // ��Q���z��̃C���f�b�N�X�i40�ԁj
        2,              // ��ށF2 = �ہi0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        3,              // �ۂ�3�Ԗڂ̃e�N�X�`���ielephant_04.png�j
        { -350, 15, -450 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.6f, 1.6f, 1.3f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(105), 0 }  // ��]�iX, Y, Z�j��Y����105�x��]
    );

    // 41�Ԗڂ̏�Q�����u���v��3�ԃe�N�X�`���idog_04.png�j�Ƃ��Ĕz�u�B�ʒu��(1000, -40, 350)�A�X�P�[����(1.0, 1.0, 1.0)�AY����210�x��]�B
    SetObstacle(
        41,             // ��Q���z��̃C���f�b�N�X�i41�ԁj
        0,              // ��ށF0 = ���i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        3,              // ����3�Ԗڂ̃e�N�X�`���idog_04.png�j
        { 1000, -40, 350 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.0f, 1.0f, 1.0f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(210), 0 }  // ��]�iX, Y, Z�j��Y����210�x��]
    );

    // 42�Ԗڂ̏�Q�����u�r�v��3�ԃe�N�X�`���isheep_04.png�j�Ƃ��Ĕz�u�B�ʒu��(-1000, 15, 250)�A�X�P�[����(1.8, 1.8, 1.6)�A��]�Ȃ��B
    SetObstacle(
        42,             // ��Q���z��̃C���f�b�N�X�i42�ԁj
        4,              // ��ށF4 = �r�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        3,              // �r��3�Ԗڂ̃e�N�X�`���isheep_04.png�j
        { -1000, 15, 250 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.8f, 1.8f, 1.6f }, // �X�P�[���iX, Y, Z�j
        { 0, 0, 0 }     // ��]�iX, Y, Z�j����]�Ȃ�
    );

    // 44�Ԗڂ̏�Q�����u�l�v��2�ԃe�N�X�`���imouse_03.png�j�Ƃ��Ĕz�u�B�ʒu��(-750, 70, 550)�A�X�P�[����(1.3, 1.3, 1.2)�AY����255�x��]�B
    SetObstacle(
        44,             // ��Q���z��̃C���f�b�N�X�i44�ԁj
        3,              // ��ށF3 = �l�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        2,              // �l��2�Ԗڂ̃e�N�X�`���imouse_03.png�j
        { -350, 30, -950 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.3f, 1.3f, 1.2f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(255), 0 }  // ��]�iX, Y, Z�j��Y����255�x��]
    );

    // 45�Ԗڂ̏�Q�����u�ہv��5�ԃe�N�X�`���ielephant_06.png�j�Ƃ��Ĕz�u�B�ʒu��(1350, 100, -100)�A�X�P�[����(1.9, 1.9, 1.7)�AY����15�x��]�B
    SetObstacle(
        45,             // ��Q���z��̃C���f�b�N�X�i45�ԁj
        2,              // ��ށF2 = �ہi0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        5,              // �ۂ�5�Ԗڂ̃e�N�X�`���ielephant_06.png�j
        { 1350, 20, -100 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.9f, 1.9f, 1.7f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(15), 0 }  // ��]�iX, Y, Z�j��Y����15�x��]
    );

    // 46�Ԗڂ̏�Q�����u���v��0�ԃe�N�X�`���idog_01.png�j�Ƃ��Ĕz�u�B�ʒu��(-250, 60, -350)�A�X�P�[����(1.4, 1.4, 1.3)�AY����120�x��]�B
    SetObstacle(
        46,             // ��Q���z��̃C���f�b�N�X�i46�ԁj
        0,              // ��ށF0 = ���i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        0,              // ����0�Ԗڂ̃e�N�X�`���idog_01.png�j
        { -250, 60, -350 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 1.4f, 1.4f, 1.3f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(120), 0 }  // ��]�iX, Y, Z�j��Y����120�x��]
    );

    // 47�Ԗڂ̏�Q�����u�r�v��0�ԃe�N�X�`���isheep_01.png�j�Ƃ��Ĕz�u�B�ʒu��(900, 35, 600)�A�X�P�[����(0.9, 0.9, 0.9)�AY����285�x��]�B
    SetObstacle(
        47,             // ��Q���z��̃C���f�b�N�X�i47�ԁj
        4,              // ��ށF4 = �r�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        0,              // �r��0�Ԗڂ̃e�N�X�`���isheep_01.png�j
        { 900, 35, 600 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 0.9f, 0.9f, 0.9f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(285), 0 }  // ��]�iX, Y, Z�j��Y����285�x��]
    );

    // 48�Ԗڂ̏�Q�����u�r�v��0�ԃe�N�X�`���isheep_01.png�j�Ƃ��Ĕz�u�B�ʒu��(900, 35, 600)�A�X�P�[����(0.9, 0.9, 0.9)�AY����285�x��]�B
    SetObstacle(
        48,             // ��Q���z��̃C���f�b�N�X�i47�ԁj
        4,              // ��ށF4 = �r�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        0,              // �r��0�Ԗڂ̃e�N�X�`���isheep_01.png�j
        { -110, 35, -1200 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 0.9f, 0.9f, 0.9f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(135), 0 }  // ��]�iX, Y, Z�j��Y����285�x��]
    );

    // 49�Ԗڂ̏�Q�����u�r�v��0�ԃe�N�X�`���isheep_01.png�j�Ƃ��Ĕz�u�B�ʒu��(900, 35, 600)�A�X�P�[����(0.9, 0.9, 0.9)�AY����285�x��]�B
    SetObstacle(
        49,             // ��Q���z��̃C���f�b�N�X�i47�ԁj
        2,              // ��ށF4 = �r�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        2,              // �r��0�Ԗڂ̃e�N�X�`���isheep_01.png�j
        { -520, 35, -1100 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 0.9f, 0.9f, 0.9f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(150), 0 }  // ��]�iX, Y, Z�j��Y����285�x��]
    );


    // 50�Ԗڂ̏�Q�����u�r�v��0�ԃe�N�X�`���isheep_01.png�j�Ƃ��Ĕz�u�B�ʒu��(900, 35, 600)�A�X�P�[����(0.9, 0.9, 0.9)�AY����285�x��]�B
    SetObstacle(
        50,             // ��Q���z��̃C���f�b�N�X�i47�ԁj
        3,              // ��ށF4 = �r�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        4,              // �r��0�Ԗڂ̃e�N�X�`���isheep_01.png�j
        { -220, 35, -1100 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 0.9f, 0.9f, 0.9f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(205), 0 }  // ��]�iX, Y, Z�j��Y����285�x��]
    );


    // 51�Ԗڂ̏�Q�����u�r�v��0�ԃe�N�X�`���isheep_01.png�j�Ƃ��Ĕz�u�B�ʒu��(900, 35, 600)�A�X�P�[����(0.9, 0.9, 0.9)�AY����285�x��]�B
    SetObstacle(
        51,             // ��Q���z��̃C���f�b�N�X�i47�ԁj
        0,              // ��ށF4 = �r�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        3,              // �r��0�Ԗڂ̃e�N�X�`���isheep_01.png�j
        { -20, 35, -1100 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 0.9f, 0.9f, 0.9f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(246), 0 }  // ��]�iX, Y, Z�j��Y����285�x��]
    );

    // 52�Ԗڂ̏�Q�����u�r�v��0�ԃe�N�X�`���isheep_01.png�j�Ƃ��Ĕz�u�B�ʒu��(900, 35, 600)�A�X�P�[����(0.9, 0.9, 0.9)�AY����285�x��]�B
    SetObstacle(
        52,             // ��Q���z��̃C���f�b�N�X�i47�ԁj
        3,              // ��ށF4 = �r�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        1,              // �r��0�Ԗڂ̃e�N�X�`���isheep_01.png�j
        { 120, 35, -900 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 0.9f, 0.9f, 0.9f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(300), 0 }  // ��]�iX, Y, Z�j��Y����285�x��]
    );

    // 53�Ԗڂ̏�Q�����u�r�v��0�ԃe�N�X�`���isheep_01.png�j�Ƃ��Ĕz�u�B�ʒu��(900, 35, 600)�A�X�P�[����(0.9, 0.9, 0.9)�AY����285�x��]�B
    SetObstacle(
        53,             // ��Q���z��̃C���f�b�N�X�i47�ԁj
        1,              // ��ށF4 = �r�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        3,              // �r��0�Ԗڂ̃e�N�X�`���isheep_01.png�j
        { 220, 35, -800 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 0.9f, 0.9f, 0.9f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(220), 0 }  // ��]�iX, Y, Z�j��Y����285�x��]
    );

    // 54�Ԗڂ̏�Q�����u�r�v��0�ԃe�N�X�`���isheep_01.png�j�Ƃ��Ĕz�u�B�ʒu��(900, 35, 600)�A�X�P�[����(0.9, 0.9, 0.9)�AY����285�x��]�B
    SetObstacle(
        54,             // ��Q���z��̃C���f�b�N�X�i47�ԁj
        4,              // ��ށF4 = �r�i0=��, 1=�L, 2=��, 3=�l, 4=�r�j
        2,              // �r��0�Ԗڂ̃e�N�X�`���isheep_01.png�j
        { 300, 35, -1200 }, // ���[���h��Ԃł̔z�u���W�iX, Y, Z�j
        { 0.9f, 0.9f, 0.9f }, // �X�P�[���iX, Y, Z�j
        { 0, XMConvertToRadians(271), 0 }  // ��]�iX, Y, Z�j��Y����285�x��]
    );

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

    SetCullingMode(CULL_MODE_NONE);
    SetAlphaTestEnable(BLEND_MODE_ALPHABLEND);
    SetLightEnable(FALSE);
    SetDepthEnable(TRUE);

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


