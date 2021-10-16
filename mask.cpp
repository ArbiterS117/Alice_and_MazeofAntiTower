//=============================================================================
//
// マスク処理 [mask.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#include "mask.h"
#include "Collider3D.h"
#include "player.h"
#include "input.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_MODEL		(1)					            // モデルの数
#define	MODEL_GROUND_TEST       	"data/MODEL/cylindrical_mask.obj"		            

#define SCALE_X         (10)
#define SCALE_Y         (10)
#define SCALE_Z         (-10)
#define Move_X          (0)
#define Move_Y          (0)
#define Move_Z          (0)
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static DX11_MODEL			g_Model[MAX_MODEL];						// モデル情報

HRESULT InitMask(void)
{
	LoadModel(MODEL_GROUND_TEST, &g_Model[0]);

	return S_OK;
}

void UninitMask(void)
{
	// モデルの解放処理
	for (int i = 0; i < MAX_MODEL; i++) {
		UnloadModel(&g_Model[i]);
	}

}

void UpdateMask(void)
{
	//PrintDebugProc("\n\n worldRotX:%f, worldRotX:%f, worldRotX:%f\n\n", worldRot.x / D3DX_PI, worldRot.y / D3DX_PI, worldRot.z / D3DX_PI);

}

void DrawMask(void)
{
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;
	D3DXMATRIX mtxWorld;
	int debugCtrlCount = 0; // reset SelectShineTimer

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);
	//SetAlphaTestEnable(true);
	SetDepthEnable(false);
	SetLightEnable(false);
	//=================Ground
	
	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScl, SCALE_X, SCALE_Y, SCALE_Z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScl);

	// 回転を反映
	//D3DXMatrixRotationYawPitchRoll(&mtxRot, worldRot.x, worldRot.y, worldRot.z);
	//D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 移動を反映
	//D3DXMatrixTranslation(&mtxTranslate, Move_X, Move_Y, Move_Z);
	//D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	// モデル描画
	DrawModel(&g_Model[0]);

	SetFogEnable(true);
	SetLightEnable(true);
	//=================
	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
	//SetAlphaTestEnable(false);
	SetDepthEnable(true);

}


