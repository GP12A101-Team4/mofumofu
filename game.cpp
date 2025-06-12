//=============================================================================
//
// �Q�[����ʏ��� [game.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "sound.h"
#include "player.h"
#include "meshfield.h"
#include "shadow.h"
#include "score.h"
#include "ui.h"
#include "collision.h"
#include "debugproc.h"
#include "meshwall.h"
#include "fragment.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************





//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;


//static BOOL	g_bPause = FALSE;	// �|�[�YON/OFF

RANDOM	g_random;
UI		g_Ui;


float yaw;
float pitch;



//=============================================================================
// ����������
//=============================================================================
HRESULT InitGame(void)
{
	g_Ui.pause = FALSE;

	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// �t�B�[���h�̏�����
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 100, 13.0f, 13.0f);

	// ���C�g��L����	// �e�̏���������
	InitShadow();

	// ���Ђ̏�������
	InitFragment();

	// �v���C���[�̏�����
	InitPlayer();


	// �X�R�A�̏�����
	InitScore();

	// �X�R�A�̏�����
	InitUI();

	// BGM�Đ�
	//PlaySound(SOUND_LABEL_BGM_game);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGame(void)
{
	// �X�R�A�̏I������
	UninitScore();

	UninitUI();

	// �n�ʂ̏I������
	UninitMeshField();

	// �v���C���[�̏I������
	UninitPlayer();

	// �e�̏I������
	UninitShadow();
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateGame(void)
{
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Game);
	}
#endif
	if (GetKeyboardTrigger(DIK_P))
	{
		g_Ui.pause = g_Ui.pause ? FALSE : TRUE;
	}

	if (GetKeyboardTrigger(DIK_RETURN)) {
		SetFade(FADE_OUT, MODE_RESULT);
	}
	

	/*if (g_Ui.pause == TRUE) {
		UpdateUi();
		return;
	}*/
	
	

	// �n�ʏ����̍X�V
	UpdateMeshField();

	// �v���C���[�̍X�V����
	UpdatePlayer();

	//���Ђ̍X�V����
	UpdateFragment();

	// �e�̍X�V����
	//UpdateShadow();

	// �X�R�A�̍X�V����
	UpdateScore();

	UpdateUI();

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGame0(void)
{
	// 3D�̕���`�悷�鏈��
	// �n�ʂ̕`�揈��
	DrawMeshField();

	// �e�̕`�揈��
	//DrawShadow();

	// �v���C���[�̕`�揈��
	DrawPlayer();

	
	// �ǂ̕`�揈��
	DrawMeshWall();

	//���Ђ̕`�揈��
	DrawFragment();
	
	// 2D�̕���`�悷�鏈��
	// Z��r�Ȃ�
	SetDepthEnable(FALSE);

	// ���C�e�B���O�𖳌�
	SetLightEnable(FALSE);

	// �X�R�A�̕`�揈��
	DrawScore();

	DrawUI();

	DrawGaugeBars();

	
	/*if(g_Ui.pause == TRUE)
	DrawUi();*/

	// ���C�e�B���O��L����
	SetLightEnable(TRUE);

	// Z��r����
	SetDepthEnable(TRUE);
}


void DrawGame(void)
{

#ifdef _DEBUG
	// �f�o�b�O�\��
	/*PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);*/

#endif
	CAMERA* camera = GetCamera();
	PLAYER* player = GetPlayer();

	{
		float dist = 10.0f;
		XMFLOAT3 dir = GetCameraDir();
		
		//�x�N�g�����g�傷��@Camera.at��Camera.pos�̒l�������ɂȂ�o�O��h������ 
		dir.x *= dist;
		dir.y *= dist;
		dir.z *= dist;

		camera->dir = { camera->pos.x + dir.x,
						camera->pos.y + dir.y,
						camera->pos.z + dir.z };

		SetCameraAT(camera->dir);
		SetCamera();
		


		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();	//OBJ�`�揈��
	}

	

}

//��]������
float turning(float target, float current) {
	target = NormalizeAngle(target);
	current = NormalizeAngle(current);

	float diff = target - current;
	diff = NormalizeAngle(diff);

	return current + diff / 10.0f;
}

float NormalizeAngle(float angle) {
	while (angle > XM_PI) angle -= 2 * XM_PI;
	while (angle < -XM_PI) angle += 2 * XM_PI;
	return angle;
}

//�J�����p�x����
XMFLOAT3 GetCameraDir() {

	int deltaX = GetMouseX();
	/*int deltaZ = GetMouseY();*/

	XMFLOAT3 dir;

	yaw += deltaX * 0.4f;
	/*pitch -= deltaZ * 0.4f;*/

	if (pitch > 89.0f) { pitch = 89.0f; }
	if (pitch < -89.0f) { pitch = -89.0f; }

	float yawRad = DirectX::XMConvertToRadians(yaw);
	float pitchRad = DirectX::XMConvertToRadians(pitch);

	dir = { sinf(yawRad) * cosf((pitchRad)),
		sinf(pitchRad), 
		cosf(yawRad) * cosf(pitchRad) };

	//���K��
	XMVECTOR v = XMVector3Normalize(XMLoadFloat3(&dir));
	XMStoreFloat3(&dir, v);

	return dir;
}

//�J�����㉺�p�x�v�Z
float GetCameraYaw(XMFLOAT3 dir, XMFLOAT3 pos) {
	float deltaX = dir.x - pos.x;
	float deltaZ = dir.z - pos.z;

	float yaw = atan2f(deltaX, deltaZ);

	return yaw;
}

////�J�������E�p�x�v�Z
float GetCameraPitch(XMFLOAT3 dir, XMFLOAT3 pos) {
	float deltaY = dir.y - pos.y;
	float deltaX = dir.x - pos.x;
	float deltaZ = dir.z - pos.z;

	float XY = (float)sqrt(deltaX * deltaX + deltaZ * deltaZ);

	float pitch = atan2f(deltaY, XY);



	return pitch;
}


RANDOM* GetRandom() {
	return &g_random;
}

UI* GetUi() {
	return &g_Ui;
}