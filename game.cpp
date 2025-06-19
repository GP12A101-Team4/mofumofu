//=============================================================================
//
// 繧ｲ繝ｼ繝逕ｻ髱｢蜃ｦ逅 [game.cpp]
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
#include "bg.h"


//*****************************************************************************
// 繝槭け繝ｭ螳夂ｾｩ
//*****************************************************************************



//*****************************************************************************
// 繝励Ο繝医ち繧､繝怜ｮ｣險
//*****************************************************************************





//*****************************************************************************
// 繧ｰ繝ｭ繝ｼ繝舌Ν螟画焚
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;


//static BOOL	g_bPause = FALSE;	// 繝昴�繧ｺON/OFF

RANDOM	g_random;
UI		g_Ui;


float yaw;
float pitch;



//=============================================================================
// 蛻晄悄蛹門�逅
//=============================================================================
HRESULT InitGame(void)
{
	g_Ui.pause = FALSE;

	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// 繝輔ぅ繝ｼ繝ｫ繝峨�蛻晄悄蛹
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 100, 13.0f, 13.0f);
	InitBG();

	InitShadow();

	// 谺迚��蛻晄悄蜃ｦ逅
	InitFragment();

	// 繝励Ξ繧､繝､繝ｼ縺ｮ蛻晄悄蛹
	InitPlayer();


	// 繧ｹ繧ｳ繧｢縺ｮ蛻晄悄蛹
	InitScore();

	// 繧ｹ繧ｳ繧｢縺ｮ蛻晄悄蛹
	InitUI();

	// BGM蜀咲函
	//PlaySound(SOUND_LABEL_BGM_GAME);

	return S_OK;
}

//=============================================================================
// 邨ゆｺ��逅
//=============================================================================
void UninitGame(void)
{
	// 繧ｹ繧ｳ繧｢縺ｮ邨ゆｺ��逅
	UninitScore();

	UninitUI();

	// 蝨ｰ髱｢縺ｮ邨ゆｺ��逅
	UninitMeshField();
	UninitBG();

	// プレイヤーの終了処理
	UninitPlayer();

	// 蠖ｱ縺ｮ邨ゆｺ��逅
	UninitShadow();
}

//=============================================================================
// 譖ｴ譁ｰ蜃ｦ逅
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
	
	

	// 蝨ｰ髱｢蜃ｦ逅��譖ｴ譁ｰ
	UpdateMeshField();
	UpdateBG();

	// プレイヤーの更新処理
	UpdatePlayer();

	//谺迚��譖ｴ譁ｰ蜃ｦ逅
	UpdateFragment();

	// 蠖ｱ縺ｮ譖ｴ譁ｰ蜃ｦ逅
	//UpdateShadow();

	// 繧ｹ繧ｳ繧｢縺ｮ譖ｴ譁ｰ蜃ｦ逅
	UpdateScore();

	UpdateUI();

}

//=============================================================================
// 謠冗判蜃ｦ逅
//=============================================================================
void DrawGame0(void)
{
	// 3D縺ｮ迚ｩ繧呈緒逕ｻ縺吶ｋ蜃ｦ逅
	// 蝨ｰ髱｢縺ｮ謠冗判蜃ｦ逅
	DrawMeshField();

	// 蠖ｱ縺ｮ謠冗判蜃ｦ逅
	//DrawShadow();

	// 繝励Ξ繧､繝､繝ｼ縺ｮ謠冗判蜃ｦ逅
	DrawPlayer();

	
	// 螢√�謠冗判蜃ｦ逅
	DrawMeshWall();
	DrawBG();

	//欠片の描画処理
	DrawFragment();
	
	// 2D縺ｮ迚ｩ繧呈緒逕ｻ縺吶ｋ蜃ｦ逅
	// Z豈碑ｼ�↑縺
	SetDepthEnable(FALSE);

	// 繝ｩ繧､繝�ぅ繝ｳ繧ｰ繧堤┌蜉ｹ
	SetLightEnable(FALSE);

	// 繧ｹ繧ｳ繧｢縺ｮ謠冗判蜃ｦ逅
	DrawScore();

	DrawUI();

	DrawGaugeBars();

	
	/*if(g_Ui.pause == TRUE)
	DrawUi();*/

	// 繝ｩ繧､繝�ぅ繝ｳ繧ｰ繧呈怏蜉ｹ縺ｫ
	SetLightEnable(TRUE);

	// Z豈碑ｼ�≠繧
	SetDepthEnable(TRUE);
}


void DrawGame(void)
{

#ifdef _DEBUG
	// 繝�ヰ繝�げ陦ｨ遉ｺ
	/*PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);*/

#endif
	CAMERA* camera = GetCamera();
	PLAYER* player = GetPlayer();

	{
		float dist = 10.0f;
		XMFLOAT3 dir = GetCameraDir();
		
		//繝吶け繝医Ν繧呈僑螟ｧ縺吶ｋ縲Camera.at縺ｨCamera.pos縺ｮ蛟､縺悟酔縺倥↓縺ｪ繧九ヰ繧ｰ繧帝亟縺舌◆繧 
		dir.x *= dist;
		dir.y *= dist;
		dir.z *= dist;

		camera->dir = { camera->pos.x + dir.x,
						camera->pos.y + dir.y,
						camera->pos.z + dir.z };

		SetCameraAT(camera->dir);
		SetCamera();
		


		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();	//OBJ謠冗判蜃ｦ逅
	}

	

}

//蝗櫁ｻ｢蟷ｳ貊大喧
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

//繧ｫ繝｡繝ｩ隗貞ｺｦ蛻ｶ蠕｡
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

	//豁｣隕丞喧
	XMVECTOR v = XMVector3Normalize(XMLoadFloat3(&dir));
	XMStoreFloat3(&dir, v);

	return dir;
}

//繧ｫ繝｡繝ｩ荳贋ｸ玖ｧ貞ｺｦ險育ｮ
float GetCameraYaw(XMFLOAT3 dir, XMFLOAT3 pos) {
	float deltaX = dir.x - pos.x;
	float deltaZ = dir.z - pos.z;

	float yaw = atan2f(deltaX, deltaZ);

	return yaw;
}

////繧ｫ繝｡繝ｩ蟾ｦ蜿ｳ隗貞ｺｦ險育ｮ
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
