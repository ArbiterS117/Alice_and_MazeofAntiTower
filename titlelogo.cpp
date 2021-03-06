//=============================================================================
//
// titlelogo画面処理 [titlelogo.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#include "titlelogo.h"
#include "input.h"
#include "fade.h"
#include "UISprite.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_UISPRITE                (1)

#define INTRO_TEXTURE_WIDTH         (1920.0f)
#define INTRO_TEXTURE_HEIGHT        (1080.0f)
#define UISPRITE_INTROA             "data/TEXTURE/result/present.png"
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static DX11_UISPRITE		        g_UISprite[MAX_UISPRITE]; // UIスプライト情報

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitlelogo(void)
{
	LoadUISprite(UISPRITE_INTROA, &g_UISprite[0], INTRO_TEXTURE_WIDTH, INTRO_TEXTURE_HEIGHT, SCREEN_CENTER_X, SCREEN_CENTER_Y);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitlelogo(void)
{
	// スプライトの解放処理
	for (int i = 0; i < MAX_UISPRITE; i++) {
		UnloadUISprite(&g_UISprite[i]);
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTitlelogo(void)
{
	// ボタン押したら、ステージを切り替える
	
	if (GetKeyboardTrigger(DIK_RETURN) || IsMouseLeftTriggered()) SetFade(FADE_OUT, MODE_TITLE);
	

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitlelogo(void)
{
	// 2D描画なので深度無効
	SetDepthEnable(false);
	SetLightEnable(false);

	DrawUISprite(&g_UISprite[0]);

	SetDepthEnable(true);
	SetLightEnable(true);

}

