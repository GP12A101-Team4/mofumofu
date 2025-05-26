//=============================================================================
//
// �^�C�g����ʏ��� [title.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "title.h"
#include "collision.h"
#include "debugproc.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// �w�i�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(10)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_LOGO			(480)			// ���S�T�C�Y
#define TEXTURE_HEIGHT_LOGO			(480)		

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

XMFLOAT3 MousePos;
XMFLOAT3 StartButtonPos;
XMFLOAT3 ExitButtonPos;

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/title.png",		//0
	"data/TEXTURE/title_logo.png",	//1
	"data/TEXTURE/effect000.jpg",	//2
	"data/TEXTURE/start.png",		//3
	"data/TEXTURE/start_selected.png",		//4
	"data/TEXTURE/exit.png",		//5
	"data/TEXTURE/exit_selected.png",		//6
	"data/TEXTURE/yuki001.png",		//7
	"data/TEXTURE/yuki002.png",		//8
	"data/TEXTURE/cursor.png"		//9

};



static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static BOOL						g_Load = FALSE;

int menu = 1;
int inputMode;

static SNOW	g_SNOW;



//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(void)
{
	ID3D11Device* pDevice = GetDevice();

	//�e�N�X�`������
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


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �ϐ��̏�����
	g_Use = TRUE;
	g_w = TEXTURE_WIDTH;
	g_h = TEXTURE_HEIGHT;
	g_Pos = XMFLOAT3(g_w / 2, g_h / 2, 0.0f);
	g_TexNo = 0;

	g_SNOW.scrl = 0.0f;			// TEX�X�N���[��
	g_SNOW.scrl2 = 0.0f;		// TEX�X�N���[��
	g_SNOW.scrl2 = 0.0f;		// TEX�X�N���[��

	StartButtonPos = { SCREEN_CENTER_X ,360.0f,0.0f };
	ExitButtonPos  = { SCREEN_CENTER_X ,450.0f,0.0f };

	//PlaySound(SOUND_LABEL_BGM_title);

	g_Load = TRUE;
	return S_OK;

}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
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

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateTitle(void)
{

	
	//// �Q�[���p�b�h�œ��͏���
	//else if (IsButtonTriggered(0, BUTTON_START))
	//{
	//	SetFade(FADE_OUT, MODE_TUTORIAL);
	//}
	//else if (IsButtonTriggered(0, BUTTON_B))
	//{
	//	SetFade(FADE_OUT, MODE_TUTORIAL);
	//}

	long MouseDeltaX = GetMouseX();
	long MouseDeltaY = GetMouseY();
	MousePos.x = float(GetMousePosX());
	MousePos.y = float(GetMousePosY());

	if (MouseDeltaX != 0 || MouseDeltaY != 0) {
		inputMode = 0;
		menu = -1;
	}

	if (CollisionBB(MousePos, 1.0f, 1.0f, StartButtonPos, 240.0f, 80.0f) && inputMode == 0) {
		menu = MENU_START;
	}
	if (CollisionBB(MousePos, 1.0f, 1.0f, ExitButtonPos , 240.0f, 80.0f) && inputMode == 0) {
		menu = MENU_EXIT;
	}
	

		if (GetKeyboardTrigger(DIK_UP))
		{
			if (inputMode == 0) {
				menu = 1;
				inputMode = 1;
			}
			else{ 
				menu--; 
				inputMode = 1;
			}
			
		}

		if (GetKeyboardTrigger(DIK_DOWN))
		{
			if (inputMode == 0) {
				menu = 1;
				inputMode = 1;
			}
			else{
				menu++;
				inputMode = 1;
			}
		}




	if (menu == MENU_MAX) {
		menu = MENU_START;
	}
	else if (menu == MENU_MIN) {
		menu = MENU_EXIT;
	}

	if (GetKeyboardTrigger(DIK_RETURN)|| IsMouseLeftPressed())
	{// Enter��������A�X�e�[�W��؂�ւ���
		if (menu == MENU_START)
		{
			SetFade(FADE_OUT, MODE_GAME);
		}
		else if (menu == MENU_EXIT)
		{
			PostQuitMessage(0);
		}
	}

	g_SNOW.old_pos = g_SNOW.pos;	// �P�t���O�̏���ۑ�


	g_SNOW.scrl -= 0.005f;		// 0.005f;		// �X�N���[��

	


#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	/*PrintDebugProc("menu:%d", menu);*/
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// �^�C�g���̔w�i��`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	// �X�^�[�g�{�^���`��
	{
		// �e�N�X�`���ݒ�
		if (menu == MENU_START) {
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);
		}
		else {
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);
		}

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, StartButtonPos.x, StartButtonPos.y, 240, 80, 0.0f, 0.0f, 1.0f, 1.0f);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �G�O�W�b�g�{�^����`��
	{
		// �e�N�X�`���ݒ�
		if (menu == MENU_EXIT) {
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);
		}
		else {
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);
		}

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, ExitButtonPos.x, ExitButtonPos.y, 240, 80, 0.0f, 0.0f, 1.0f, 1.0);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// ���`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);

		//�P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		g_SNOW.scrl3 -= 0.001f;

		SetSpriteLTColor(g_VertexBuffer,
			0.0f, 0.0f, SCREEN_WIDTH, 540.0f,
			0.0f, g_SNOW.scrl3, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// ���`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

		//�P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		g_SNOW.scrl += 0.001f;

		SetSpriteLTColor(g_VertexBuffer,
			0.0f, 0.0f, SCREEN_WIDTH, 540.0f,
			0.0f, g_SNOW.scrl, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	// �J�[�\��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

		SetSpriteLTColor(g_VertexBuffer,
			MousePos.x, MousePos.y, 40.0f, 40.0f,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}





