//=============================================================================
//
// タイマー処理 [score.h]
// Author : 
//
//=============================================================================
#pragma once

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitScore(void);
void UninitScore(void);
void UpdateScore(void);
void DrawScore(void);

// タイマー制御
void StartTimer();     // ゲーム開始で呼ぶ
void StopTimer();      // ゲーム終了で呼ぶ

// 経過秒（整数秒）を返す（必要なら利用）
int GetElapsedTime(void);
int GetElapsedTimeMs();
// 位置 & 大小（缩放）设置
void Score_SetPosition(float x, float y);
void Score_SetScale(float scale);