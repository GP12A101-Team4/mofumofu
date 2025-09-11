//=============================================================================
//
// メイン処理 [main.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "debugproc.h"
#include "model.h"
#include "player.h"
#include "shadow.h"
#include "light.h"
#include "meshfield.h"
#include "fragment.h"
#include "fragment_dog.h"
#include "fragment_elephant.h"
#include "fragment_mouse.h"
#include "fragment_sheep.h"
#include "fragment_obstacle.h"
#include "meshwall.h"
#include "sound.h"
#include "score.h"
#include "ui.h"
#include "sprite.h"
#include "fade.h"
#include "game.h"
#include "title.h"
#include "result.h"
#include "cursor.h"
#include "bg.h"
#include "menu.h"
#include "object.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
	


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"			// ウインドウのクラス名
#define WINDOW_NAME		"メッシュ表示"		// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

//*****************************************************************************
// グローバル変数:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;

HWND g_hWnd = nullptr;

#ifdef _DEBUG
int		g_CountFPS;							// FPSカウンタ
char	g_DebugStr[2048] = WINDOW_NAME;		// デバッグ文字表示用

#endif

int	g_Mode = MODE_TITLE	;					// 起動時の画面を設定



//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	// 時間計測用
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// ウィンドウの左座標
		CW_USEDEFAULT,																		// ウィンドウの上座標
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// ウィンドウ横幅
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// ウィンドウ縦幅
		NULL,
		NULL,
		hInstance,
		NULL);

	// 初期化処理(ウィンドウを作成してから行う)
	if(FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}

	// ✅ 加在這裡開始初始化 ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ID3D11Device* g_pd3dDevice = GetD3DDevice();
	ID3D11DeviceContext* g_d3dDeviceContext = GetD3DDeviceContext();

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_d3dDeviceContext);




	// フレームカウント初期化
	timeBeginPeriod(1);	// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = 0;

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// メッセージループ
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳と送出
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
        }
		else
		{
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1秒ごとに実行
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
				dwFrameCount = 0;							// カウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60秒ごとに実行
			{
				dwExecLastTime = dwCurrentTime;	// 処理した時刻を保存

#ifdef _DEBUG	// デバッグ版の時だけFPSを表示する
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// 更新処理
				Draw();				// 描画処理

#ifdef _DEBUG	// デバッグ版の時だけ表示する
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// 分解能を戻す

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// 先讓 ImGui 處理訊息，如果它回傳 true 就不繼續處理
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;
	UINT dataSize = 0; 
	switch (message)
	{
	case WM_CREATE:
		g_hWnd = hWnd;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;
	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);

		/*POINT point = GetClientCenter(g_hWnd);
		SetCursorPos(point.x, point.y);*/


		break;

	case WM_ACTIVATE:
		if (wParam != WA_INACTIVE) {

			ShowCursor(FALSE);
			RECT rect;
			GetClientRect(hWnd, &rect);
			MapWindowPoints(hWnd, nullptr, (POINT*)&rect, 2);
			ClipCursor(&rect);

		}
		else {

			ClipCursor(nullptr);
			ShowCursor(TRUE);
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// レンダラーの初期化
	InitRenderer(hInstance, hWnd, bWindow);

	// ライトの初期化
	InitLight();

	// カメラの初期化
	InitCamera();

	////サウンドの初期化
	InitSound(hWnd);

	InitScore();

	InitUI();

	InitMenu();

	InitCursor();

	// 入力処理の初期化
	InitInput(hInstance, hWnd);

	// フィールドの初期化
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 1, 1, 1500.0f, 1500.0f);
	InitBG();

	// 壁の初期化
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	//// 壁(裏側用の半透明)
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, XM_PI, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	//InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
	//	XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	// 影の初期化処理
	InitShadow();

	// 欠片の初期処理
	InitFragment();
	InitFragment_Dog();
	InitFragment_Elph();
	InitFragment_Mouse();
	InitFragment_Sheep();
	InitObstacleFragment();

	// プレイヤーの初期化
	InitPlayer();

	// ライトを有効化
	SetLightEnable(FALSE);

	// 背面ポリゴンをカリング
	SetCullingMode(CULL_MODE_BACK);

	// フェードの初期化
	InitFade();

	SetMasterVolume(0.3f);


	// 最初のモードをセット
	SetMode(g_Mode);	// ここはSetModeのままで！

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{

	// プレイヤーの終了処理
	UninitPlayer();

	// 影の終了処理
	UninitShadow();

	// 壁の終了処理
	UninitMeshWall();

	//欠片の終了処理
	UninitFragment();
	UninitFragment_Dog();
	UninitFragment_Elph();
	UninitFragment_Mouse();
	UninitFragment_Sheep();
	UninitObstacleFragment();

	UninitBG();
	// 地面の終了処理
	UninitMeshField();
	UninitBG();
	// カメラの終了処理
	UninitCamera();

	////サウンドの終了処理
	UninitSound();

	UninitScore();

	UninitUI();

	UninitMenu();

	UninitCursor();
	//入力の終了処理
	UninitInput();

	// レンダラーの終了処理
	UninitRenderer();
}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	// 入力の更新処理
	UpdateInput();
	UpdateScore();

	MENU* menu = GetMenu();
	PrintDebugProc("MENU use : %d", menu->use);

	switch (g_Mode)
	{
	case MODE_TITLE:		// タイトル画面の更新
		
		UpdateCursor();
		UpdateMenu();
		UpdateTitle();
		break;

	case MODE_GAME:			// ゲーム画面の更新
		UpdateGame();
		//UpdateBG();
		break;

	case MODE_RESULT:		// リザルト画面の更新
		UpdateResult();
		break;
	}

	// カメラ更新
	UpdateCamera();

	// フェード処理の更新
	UpdateFade();

}

//=============================================================================
// 描画処理
//=============================================================================
void Draw0(void) 
{
	// 地面の描画処理
	DrawMeshField();

	// 影の描画処理
	//DrawShadow();

	// プレイヤーの描画処理
	/*DrawPlayer();*/

	// 壁の描画処理
	DrawMeshWall();
	//DrawBG();
	
}

void Draw(void)
{
	// バックバッファクリア
	Clear();

	SetCamera();


	PLAYER* player = GetPlayer();
	MENU* menu = GetMenu();

	switch (g_Mode)
	{
	case MODE_TITLE:
		SetViewPort(TYPE_FULL_SCREEN);

		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		DrawTitle();

		if (menu->use == TRUE) {
			DrawMenu();
		}

		DrawCursor();

		// ライティングを有効に
		SetLightEnable(TRUE);
      
		// Z比較あり
		SetDepthEnable(TRUE);
		break;
      
	case MODE_GAME:
		DrawGame();

		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		DrawCursor();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
		break;
	case MODE_RESULT:
		SetViewPort(TYPE_FULL_SCREEN);

		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);

		// ライティングを無効
		SetLightEnable(FALSE);

		DrawResult();

		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
		break;

	default:
		break;
	}
	
	{	// フェード処理
		SetViewPort(TYPE_FULL_SCREEN);

		// 2Dの物を描画する処理
		// Z比較なし
		SetDepthEnable(FALSE);


		// ライティングを無効
		SetLightEnable(FALSE);

		// フェード描画
		DrawFade();

		
		
		// ライティングを有効に
		SetLightEnable(TRUE);

		// Z比較あり
		SetDepthEnable(TRUE);
	}


#ifdef _DEBUG

	/*g_ImmediateContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);*/

	// デバッグ表示
	DrawDebugProc();

	// 新幀開始
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug Info");
	ImGui::Text("FPS: %d", g_CountFPS); // 你已有的 FPS 計算變數
	ImGui::Text("Player Pos: %.2f, %.2f, %.2f", player->pos.x, player->pos.y, player->pos.z);
	
	ImGui::End();

	/*DrawPartDebugUI();
	DrawPartDebugUI_Dog();
	DrawPartDebugUI_Elph();
	DrawPartDebugUI_Mouse();
	DrawPartDebugUI_Sheep();*/

	DrawDebugMenu();
  

	//ImGui::ShowDemoWindow();

	DrawFadeDebugUI();
	//ShowObjectDebugWindow();

	// 結束新幀
	ImGui::Render();

	// 渲染 ImGui 主視窗
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// 如果啟用了多視窗（Docking / Viewports），要呼叫這兩個函式
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();

	
#endif


	ID3D11RenderTargetView* g_RenderTargetView = GetRenderTargetView();
	ID3D11DepthStencilView* g_DepthStencilView = GetDepthStencilView();
	ID3D11DeviceContext* g_D3DContext = GetD3DDeviceContext();


	g_D3DContext->OMSetRenderTargets(1, &g_RenderTargetView, g_DepthStencilView);
	// バックバッファ、フロントバッファ入れ替え
	Present();
}

//=============================================================================
// モードの設定
//=============================================================================
void SetMode(int mode)
{
	// モードを変える前に全部メモリを解放しちゃう
	StopSound();			// まず曲を止める

	// モードを変える前に全部メモリを解放しちゃう

	// プレイヤーの終了処理
	UninitPlayer();

	//// スコアの終了処理
	//UninitScore();

	g_Mode = mode;	// 次のモードをセットしている

	switch (g_Mode)
	{
	case MODE_TITLE:
		//タイトル画面の初期化
		InitTitle();
		InitCursor();
		InitMenu();
		PlaySound(SOUND_LABEL_BGM_TITLE);


		break;

	case MODE_GAME:
		// ゲーム画面の初期化
		/*InitPlayer();
		InitScore();*/
		InitGame();
		StartTimer();

		PlaySound(SOUND_LABEL_BGM_GAME);
		break;

	case MODE_RESULT:
		InitResult();
		StopTimer();
		PlaySound(SOUND_LABEL_BGM_GAME);
		break;

	case MODE_MAX:
		break;
	}
}

//=============================================================================
// モードの取得
//=============================================================================
int GetMode(void)
{
	return g_Mode;
}


POINT GetClientCenter(HWND hWnd){
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	int centerX = (clientRect.left + clientRect.right) / 2;
	int centerY = (clientRect.top + clientRect.bottom) / 2;

	POINT Center = { centerX, centerY };
	ClientToScreen(hWnd, &Center);

	return Center;
}

long GetMousePosX(void)
{
	return g_MouseX;
}


long GetMousePosY(void)
{
	return g_MouseY;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}
#endif

HWND GetHWND() {
	return g_hWnd;
}
