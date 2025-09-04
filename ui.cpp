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
#include "fragment_elephant.h"
#include "fragment_mouse.h"
#include "fragment_sheep.h"
#include "ui.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(70)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(70)	// 
#define TEXTURE_MAX					(5)		// �e�N�X�`���̐�
#define ANIMAL_TEX_MAX				(5)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static ID3D11ShaderResourceView	*g_AnimalTex[ANIMAL_TEX_MAX] = { NULL };

static char *g_TexturName[] = {
	"data/TEXTURE/UI_black.png",
	"data/TEXTURE/UI_normal.png",
	"data/TEXTURE/successbarbottom.png",
	"data/TEXTURE/successbarcenter.png",
	"data/TEXTURE/successbartop.png",
};

static const char* g_AnimalTexName[ANIMAL_TEX_MAX] = {
	"data/TEXTURE/cat.png",
	"data/TEXTURE/dog.png",
	"data/TEXTURE/elephant.png",
	"data/TEXTURE/mouse.png",
	"data/TEXTURE/sheep.png",
};


static BOOL						g_Use;						// TRUE:�g���Ă���  FALSE:���g�p
static float					g_w, g_h;					// ���ƍ���
static XMFLOAT3					g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�
static int g_ActiveAnimalIdx = -1;

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

	for (int i = 0; i < ANIMAL_TEX_MAX; ++i) {
		g_AnimalTex[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_AnimalTexName[i],
			NULL,
			NULL, 
			&g_AnimalTex[i],
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

	for (int i = 0; i < ANIMAL_TEX_MAX; ++i) {
		if (g_AnimalTex[i]) 
		{
			g_AnimalTex[i]->Release();
			g_AnimalTex[i] = NULL;
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
// BAR����
//===========================================================================
static inline float Safe01(float v) {
	if (!(v == v) || !isfinite(v)) return 0.0f; // NaN/Inf -> 0
	if (v < 0.0f) v = 0.0f;
	if (v > 1.0f) v = 1.0f;
	return v;
}

// ���݃A�N�e�B�u�ȓ����̃p�Y�������x���擾����
float GetActivePuzzleAlignmentRatio()
{
	float ratios[5] = {
		GetPuzzleAlignmentRatio(),        // �L
		GetPuzzleAlignmentRatio_Dog(),    // ��
		GetPuzzleAlignmentRatio_Elph(),   // ��
		GetPuzzleAlignmentRatio_Mouse(),  // �l
		GetPuzzleAlignmentRatio_Sheep()   // �r
	};

	// �擾�����l�� [0,1] �Ɏ��߂�
	for (int i = 0; i < 5; ++i) {
		ratios[i] = Safe01(ratios[i]);
	}

	// �ő�l����������T��
	int   bestIdx = -1;
	float bestVal = 0.0f;
	for (int i = 0; i < 5; ++i) {
		if (ratios[i] > bestVal) {
			bestVal = ratios[i];
			bestIdx = i;
		}
	}

	// �Q�[�W�\�����J�n����ŏ��������l
	const float ENTER_EPS = 0.005f;
#ifdef _DEBUG
	{
		// �f�o�b�O�p�Ɋe�����̔䗦�ƍő�l���o��
		char* str = GetDebugStr();
		sprintf(&str[strlen(str)], " [GB] cat=%.3f dog=%.3f elph=%.3f mou=%.3f sheep=%.3f | best=%d val=%.3f",
			ratios[0], ratios[1], ratios[2], ratios[3], ratios[4], bestIdx, bestVal);
	}
#endif

	// ���ȏ�̔䗦������ꍇ�̓A�N�e�B�u�����Ƃ��ēo�^
	if (bestIdx >= 0 && bestVal > ENTER_EPS) {
		g_ActiveAnimalIdx = bestIdx;  
		return bestVal;
	}

	// ������̓������ΏۊO�̏ꍇ
	g_ActiveAnimalIdx = -1;

	return 0.0f;
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
	// ���݂̃A�N�e�B�u�����̊����x���擾
	float ratio = GetActivePuzzleAlignmentRatio();
	
	// �Q�[�W�o�[�̕\���ʒu�ƃT�C�Y
	float screenX   = 480.0f;
	float screenY   = 475.0f;
	float barWidth  = 250.0f;
	float barHeight = 50.0f;

	// ===== �A�N�e�B�u�����̃A�C�R���`�� =====
	if (g_ActiveAnimalIdx >= 0 && g_ActiveAnimalIdx < ANIMAL_TEX_MAX) {
		
		float iconW = 64.0f;
		float iconH = 64.0f;

		float padding = 24.0f; 
		
		// �A�C�R�����Q�[�W���ɔz�u���邽�߂̍��W
		float iconX = (screenX - barWidth * 0.5f) - padding - iconW * 0.5f;
		float iconY = screenY; 

		// �����A�C�R���̃e�N�X�`����ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_AnimalTex[g_ActiveAnimalIdx]);

		// �A�C�R���̃X�v���C�g�`��
		SetSpriteColor(g_VertexBuffer,
			iconX, iconY, iconW, iconH,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1, 1, 1, 1));
		GetDeviceContext()->Draw(4, 0);
	}

	// ===== Bar =====
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





