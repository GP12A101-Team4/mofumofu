//=============================================================================
//
// �X�R�A���� [ui.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "fragment.h"
#include "fragment_dog.h"
#include "ui.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(70)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(70)	// 
#define TEXTURE_MAX					(5)		// �e�N�X�`���̐�


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/UI_black.png",
	"data/TEXTURE/UI_normal.png",
	"data/TEXTURE/successbarbottom.png",
	"data/TEXTURE/successbarcenter.png",
	"data/TEXTURE/successbartop.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

//=============================================================================
// ����������
//=============================================================================
HRESULT InitUI(void)
{
	ID3D11Device *pDevice = GetDevice();

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


	// �v���C���[�̏�����
	g_Use   = TRUE;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = { 925.0f, 35.0f, 0.0f };
	g_TexNo = 0;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitUI(void)
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
// �X�V����
//=============================================================================
void UpdateUI(void)
{
	if (g_ShowFullImage)
	{
		g_TexNo = 1;
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawUI(void)
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

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	// ��������������
	
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer,
			g_Pos.x, g_Pos.y, g_w, g_h,     // position siz
			0.0f, 0.0f, 1.0f, 1.0f,         // texture coord
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
}

void DrawGaugeBars()
{
	float screenX   = 480.0f;
	float screenY   = 475.0f;
	float barWidth  = 250.0f;
	float barHeight = 50.0f;
	float ratio = GetPuzzleAlignmentRatio();

	// bottom layer (background)
	SetSpriteColor(g_VertexBuffer, screenX, screenY, barWidth, barHeight,
		0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1, 1, 1, 1));
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);
	GetDeviceContext()->Draw(4, 0);

	// center layer (ratio-filled bar)
	float filledWidth = barWidth * ratio;
	SetSpriteColor(g_VertexBuffer, screenX - (barWidth - filledWidth) / 2.0f, screenY, filledWidth, barHeight,
		0.0f, 0.0f, ratio, 1.0f, XMFLOAT4(1, 1, 1, 1));
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);
	GetDeviceContext()->Draw(4, 0);

	// top layer (outline)
	SetSpriteColor(g_VertexBuffer, screenX, screenY, barWidth, barHeight,
		0.0f, 0.0f, 1.0f, 1.0f, XMFLOAT4(1, 1, 1, 1));
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);
	GetDeviceContext()->Draw(4, 0);
}





