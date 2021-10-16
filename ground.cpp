//=============================================================================
//
// ground処理 [ground.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#include "ground.h"
#include "Collider3D.h"
#include "player.h"
#include "input.h"
#include "debugproc.h"
#include "platform.h"
#include "enemy.h"

#include "camera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_MODEL		(11)					            // モデルの数
//#define	MODEL_GROUND_TEST       	"data/MODEL/TestMap.obj"		       
#define	MODEL_GROUND_TEST       	"data/MODEL/SHIKAKU_Stage_1.obj"		       
#define	MODEL_GROUND_TEST2       	"data/MODEL/SHIKAKU_Stage_2.obj"		       
#define	MODEL_GROUND_TEST3       	"data/MODEL/SHIKAKU_Stage_3.obj"		       
//#define	MODEL_GROUND_TEST       	"data/MODEL/stage_2.obj"		       

#define	MODEL_GROUND_HEARTWORLD  	"data/MODEL/HeartWorld.obj"		       
#define	MODEL_GROUND_HEARTWORLD_D  	"data/MODEL/HeartWorld_D.obj"		       
#define	MODEL_GROUND_HtoW        	"data/MODEL/HtoW.obj"		       
#define	MODEL_GROUND_HtoW_D        	"data/MODEL/HtoW_D.obj"		       
#define	MODEL_GROUND_WONDERLAND     "data/MODEL/WonderLand.obj"		
#define	MODEL_GROUND_WONDERLAND2    "data/MODEL/WonderLand2.obj"		
#define	MODEL_GROUND_WONDERLAND_D	"data/MODEL/WonderLand_D.obj"		
#define	MODEL_GROUND_WtoS           "data/MODEL/WtoS.obj"		
#define	MODEL_GROUND_SANDWORLD	    "data/MODEL/SandWorld.obj"		       
#define	MODEL_GROUND_SANDWORLD_D    "data/MODEL/SandWorld_D.obj"		       
#define	MODEL_GROUND_StoS        	"data/MODEL/StoS.obj"		       
#define	MODEL_GROUND_SNOWWORLD      "data/MODEL/SnowWorld.obj"		       
#define	MODEL_SKY    	"data/MODEL/sky.obj"		                       
#define	MODEL_SKY2    	"data/MODEL/sky2.obj"		                       
#define	MODEL_SKY3    	"data/MODEL/sky3.obj"		                       

#define SCALE_X         (10)
#define SCALE_Y         (10)
#define SCALE_Z         (-10)
#define Move_X          (0)
#define Move_Y          (0)
#define Move_Z          (0)

#define VIEW_ROTX_MAX   (D3DX_PI * 0.45f)
#define VIEW_ROTX_MIN   (-D3DX_PI * 0.45f)

#define WORLDROTATEMAG		(0.003f)
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static DX11_MODEL			g_Model[MAX_MODEL];						// モデル情報

//Rot system
D3DXVECTOR3 worldRot      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
D3DXVECTOR3 CurRot        = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
D3DXVECTOR3 deltaRot      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
static bool b_ResetGround = false;
static bool RotatetoDice = false;

static int WorldRotStatus = 1; // RotStatus in Dice


float skyRot = 0.0f;
float a = 0.0f;
int lvvvv = 0;

//GroundMesh Normal 
D3DXVECTOR3 *groundNormal_HeartWorld;
D3DXVECTOR3 *groundNormal_HtoW;
D3DXVECTOR3 *groundNormal_WonderLand;
D3DXVECTOR3 *groundNormal_WonderLand2;
D3DXVECTOR3 *groundNormal_WtoS;
D3DXVECTOR3 *groundNormal_SandWorld;
D3DXVECTOR3 *groundNormal_SandWorld_D;
D3DXVECTOR3 *groundNormal_StoS;
D3DXVECTOR3 *groundNormal_SnowWorld;

void InitNormals();
D3DXVECTOR3 RotWithWorld(D3DXVECTOR3 oriPos, D3DXVECTOR3 worldRot);

//FixDiceRotate
void FixDiceRotate();
void SetFixDiceRotate();

static float		r_time;
static float		r_speed;

static D3DXVECTOR3 cameraRotRecord      = D3DXVECTOR3(0.0f,0.0f,0.0f);
static D3DXVECTOR3 TempWrongRotRecord   = D3DXVECTOR3(0.0f,0.0f,0.0f);
static D3DXVECTOR3 CaculatedWorldRot    = D3DXVECTOR3(0.0f,0.0f,0.0f);
static D3DXVECTOR3* Rotroute;

void rotateOBJ();

HRESULT InitGround(void)
{
	LoadModel(MODEL_GROUND_TEST, &g_Model[GroundType_Test]);
	LoadModel(MODEL_GROUND_TEST2, &g_Model[GroundType_Test2]);
	LoadModel(MODEL_GROUND_TEST3, &g_Model[GroundType_Test3]);
	//LoadModel(MODEL_GROUND_HEARTWORLD, &g_Model[GroundType_HeartWorld]);
	//LoadModel(MODEL_GROUND_HEARTWORLD_D, &g_Model[GroundType_HeartWorld_D]);
	//LoadModel(MODEL_GROUND_HtoW, &g_Model[GroundType_HtoW]);
	//LoadModel(MODEL_GROUND_HtoW_D, &g_Model[GroundType_HtoW_D]);
	//LoadModel(MODEL_GROUND_WONDERLAND, &g_Model[GroundType_Wonderland]);
	//LoadModel(MODEL_GROUND_WONDERLAND2, &g_Model[GroundType_Wonderland2]);
	//LoadModel(MODEL_GROUND_WONDERLAND_D, &g_Model[GroundType_Wonderland_D]);
	//LoadModel(MODEL_GROUND_WtoS, &g_Model[GroundType_WtoS]);
	//LoadModel(MODEL_GROUND_SANDWORLD, &g_Model[GroundType_SandWorld]);
	//LoadModel(MODEL_GROUND_SANDWORLD_D, &g_Model[GroundType_SandWorld_D]);
	//LoadModel(MODEL_GROUND_StoS, &g_Model[GroundType_StoS]);
	//LoadModel(MODEL_GROUND_SNOWWORLD, &g_Model[GroundType_SnowWorld]);
	LoadModel(MODEL_SKY, &g_Model[GroundType_Sky1]);
	LoadModel(MODEL_SKY2, &g_Model[GroundType_Sky2]);
	LoadModel(MODEL_SKY3, &g_Model[GroundType_Sky3]);

	InitNormals();
	
	//fixDice change parameter
	Rotroute = new D3DXVECTOR3[CAMERA_ROUTE_SEGMENT + 1];
	r_time = 0.0f;
	r_speed = 0.45f;

	return S_OK;
}

void UninitGround(void)
{
	// モデルの解放処理
	for (int i = 0; i < MAX_MODEL; i++) {
		UnloadModel(&g_Model[i]);
	}

	if (groundNormal_HeartWorld)  { delete[] groundNormal_HeartWorld;  groundNormal_HeartWorld  = NULL; }
	if (groundNormal_HtoW)        { delete[] groundNormal_HtoW;        groundNormal_HtoW        = NULL; }
	if (groundNormal_WonderLand)  { delete[] groundNormal_WonderLand;  groundNormal_WonderLand  = NULL; }
	if (groundNormal_WonderLand2) { delete[] groundNormal_WonderLand2; groundNormal_WonderLand2 = NULL; }
	if (groundNormal_WtoS)        { delete[] groundNormal_WtoS;        groundNormal_WtoS        = NULL; }
	if (groundNormal_SandWorld)   { delete[] groundNormal_SandWorld;   groundNormal_SandWorld   = NULL; }
	if (groundNormal_SandWorld_D) { delete[] groundNormal_SandWorld_D; groundNormal_SandWorld_D = NULL; }
	if (groundNormal_StoS)        { delete[] groundNormal_StoS;        groundNormal_StoS        = NULL; }
	if (groundNormal_SnowWorld)   { delete[] groundNormal_SnowWorld;   groundNormal_SnowWorld   = NULL; }

}

void UpdateGround(void)
{
	PrintDebugProc("\n\n worldRotX:%f, worldRotY:%f, worldRotZ:%f\n\n", worldRot.x/ D3DX_PI, worldRot.y/ D3DX_PI, worldRot.z/ D3DX_PI);
	PrintDebugProc("\n\n WorldRotStatus:%d\n\n", WorldRotStatus);
	
	bool wrm = GetWorldRotateMode();

	     if (GetKeyboardTrigger(DIK_F1)) {worldRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f); worldRot.y =  0.0f * D3DX_PI; rotateOBJ(); WorldRotStatus = 1;}
	else if (GetKeyboardTrigger(DIK_F2)) {worldRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f); worldRot.y = -0.5f * D3DX_PI; rotateOBJ(); WorldRotStatus = 2;}
	else if (GetKeyboardTrigger(DIK_F3)) {worldRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f); worldRot.z = -0.5f * D3DX_PI; rotateOBJ(); WorldRotStatus = 3;}
	else if (GetKeyboardTrigger(DIK_F4)) {worldRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f); worldRot.z =  0.5f * D3DX_PI; rotateOBJ(); WorldRotStatus = 4;}
	else if (GetKeyboardTrigger(DIK_F5)) {worldRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f); worldRot.z =  1.0f * D3DX_PI; rotateOBJ(); WorldRotStatus = 5;}
	else if (GetKeyboardTrigger(DIK_F6)) {worldRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f); worldRot.y =  0.5f * D3DX_PI; rotateOBJ(); WorldRotStatus = 6;}
	else if (b_ResetGround)              {worldRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f); worldRot.y =  0.0f * D3DX_PI; rotateOBJ(); WorldRotStatus = 1;}

	if (wrm)
	{
		//if (IsMouseCenterPressed())
		{
			deltaRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			//ShowCursor(true);
			float dz = GetMousedX();
			float dy = GetMousedY();
			deltaRot.y -= dy * WORLDROTATEMAG;
			deltaRot.z -= dz * WORLDROTATEMAG;

			if (deltaRot.y < -D3DX_PI) deltaRot.y += D3DX_PI * 2.0f;
			if (deltaRot.y > D3DX_PI)  deltaRot.y -= D3DX_PI * 2.0f;
			if (deltaRot.z < -D3DX_PI) deltaRot.z += D3DX_PI * 2.0f;
			if (deltaRot.z > D3DX_PI)  deltaRot.z -= D3DX_PI * 2.0f;
			PrintDebugProc("\n\n deltaRot.x:%f, deltaRot.y:%f ,deltaRot.z:%f\n\n", deltaRot.x, deltaRot.y, deltaRot.z);

			//描画
			worldRot += deltaRot;
			if (worldRot.y < -D3DX_PI * 0.5f) worldRot.y = -D3DX_PI * 0.5f;
			if (worldRot.y >  D3DX_PI * 0.5f) worldRot.y =  D3DX_PI * 0.5f;
			if (worldRot.z < -D3DX_PI)  worldRot.z += D3DX_PI * 2.0f;
			if (worldRot.z >  D3DX_PI)  worldRot.z -= D3DX_PI * 2.0f;

			//============wrm POS
			//player
			D3DXVECTOR3 NewPos;
			NewPos = RotWithWorld(GetPlayer()->pos, worldRot);
			GetPlayer()->wrm_Pos = NewPos;

			//plat
			PLATFORM *plat = GetPlatform();
			for (int i = 0; i < MAX_PLATFORM; i++) {
				if (plat[i].Use && plat[i].RotWithStage) {
					if(plat[i].canMove) NewPos = RotWithWorld(plat[i].CaculatedPos, worldRot);
					else NewPos = RotWithWorld(plat[i].pos, worldRot);
					plat[i].wrm_Pos = NewPos;
					plat[i].wrm_Rot.z = worldRot.z;
					plat[i].wrm_Rot.x = worldRot.y;
					plat[i].wrm_Rot.y = worldRot.x;
				}
			}

			//plat
			ENEMY *enemy = GetEnemy();
			for (int i = 0; i < MAX_ENEMY; i++) {
				if (enemy[i].Use && enemy[i].RotWithStage) {
					if (enemy[i].CanMove) NewPos = RotWithWorld(enemy[i].CaculatedPos, worldRot);
					else NewPos = RotWithWorld(enemy[i].pos, worldRot);
					enemy[i].wrm_Pos = NewPos;
					enemy[i].wrm_Rot.z = -worldRot.z;
					enemy[i].wrm_Rot.x = -worldRot.y;
					enemy[i].wrm_Rot.y =  worldRot.x;
				}
			}

		}
	}

	if (RotatetoDice) {
		FixDiceRotate();

	}
}

void DrawGround(void)
{
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;
	D3DXMATRIX mtxWorld;
	int debugCtrlCount = 0; // reset SelectShineTimer

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	//=================Ground
	SetLightEnable(true);
	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScl, SCALE_X, SCALE_Y, SCALE_Z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScl);

	// 回転を反映
	//auto temp = worldRot + deltaRot;
	D3DXMatrixRotationYawPitchRoll(&mtxRot, worldRot.x, worldRot.y, worldRot.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 移動を反映
	//D3DXMatrixTranslation(&mtxTranslate, Move_X, Move_Y, Move_Z);
	//D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	//wrm
	for (int i = 0; i < 3; i++) {
		if (GetWorldRotateMode() || Getbezirmoveflag()) {
			for (unsigned short j = 0; j < g_Model[i].SubsetNum; j++)
			{
				g_Model[i].SubsetArray[j].Material.Material.Diffuse.a = 0.5f;
			}
		}
		else {
			for (unsigned short j = 0; j < g_Model[i].SubsetNum; j++)
			{
				g_Model[i].SubsetArray[j].Material.Material.Diffuse.a = 1.0f;
			}
		}
	}

	// モデル描画
	DrawModel(&g_Model[GroundType_Test]);
	DrawModel(&g_Model[GroundType_Test2]);
	DrawModel(&g_Model[GroundType_Test3]);
	//DrawModel(&g_Model[GroundType_HeartWorld]);
	//DrawModel(&g_Model[GroundType_HeartWorld_D]);
	//DrawModel(&g_Model[GroundType_HtoW]);
	//DrawModel(&g_Model[GroundType_HtoW_D]);
	//DrawModel(&g_Model[GroundType_Wonderland]);
	//DrawModel(&g_Model[GroundType_Wonderland2]);
	//DrawModel(&g_Model[GroundType_Wonderland_D]);
	//DrawModel(&g_Model[GroundType_WtoS]);
	//DrawModel(&g_Model[GroundType_SandWorld]);
	//DrawModel(&g_Model[GroundType_SandWorld_D]);
	//DrawModel(&g_Model[GroundType_StoS]);
	//DrawModel(&g_Model[GroundType_SnowWorld]);

	SetLightEnable(true);

	//=================SKY SPHERE
	SetLightEnable(false);
	SetFogEnable(false);
	skyRot += 0.0005f * D3DX_PI;
	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScl, 4000, 4000, 4000);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScl);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, skyRot, 0.0f, 0.0f);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 移動を反映
	PLAYER *player = GetPlayer();
	D3DXMatrixTranslation(&mtxTranslate, player->pos.x, 0.0f, player->pos.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	float p = 0.0f;
	float np = 0.0f;
	a += 1;
	if (a < 2000.0f) {
		if (lvvvv == 0) {
			p = 0.5f - a / 2000.0f;
			np = 0.0f + a / 2000.0f * 0.8f;
			g_Model[GroundType_Sky1].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, p);
			g_Model[GroundType_Sky2].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, np);
			g_Model[GroundType_Sky3].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
		}
		if (lvvvv == 1) {
			p = 0.8f - a / 2000.0f;
			np = 0.0f + a / 2000.0f * 0.7f;
			g_Model[GroundType_Sky1].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			g_Model[GroundType_Sky2].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, p);
			g_Model[GroundType_Sky3].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(np, np, np, np);
		}
		if (lvvvv == 2) {
			p = 0.7f - a / 2000.0f;
			np = 0.0f + a / 2000.0f * 0.5f;
			g_Model[GroundType_Sky1].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, np);
			g_Model[GroundType_Sky2].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			g_Model[GroundType_Sky3].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(p, p, p, p);
		}
	}
	else {
		a = 0.0f;
		lvvvv += 1;
		if (lvvvv == 3)lvvvv = 0;
	}
	//g_Model[2].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	//g_Model[3].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f);
	//g_Model[4].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(0.7f, 0.7f, 0.7f, 0.7f);
	// モデル描画
	DrawModel(&g_Model[GroundType_Sky1]);
	DrawModel(&g_Model[GroundType_Sky2]);
	DrawModel(&g_Model[GroundType_Sky3]);

	SetFogEnable(true);
	SetLightEnable(true);
	//=================
	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

DX11_MODEL getGroundModel(int id)
{
	return g_Model[id];
}

D3DXVECTOR3 *getGroundNormal(int id)
{
	if (id == GroundType_HeartWorld)    return &groundNormal_HeartWorld[0];
	if (id == GroundType_HtoW)	        return &groundNormal_HtoW[0];
	if(id == GroundType_Wonderland)	    return &groundNormal_WonderLand[0];
	if(id == GroundType_Wonderland2)	return &groundNormal_WonderLand2[0];
	if(id == GroundType_WtoS)	        return &groundNormal_WtoS[0];
	if(id == GroundType_SandWorld)	    return &groundNormal_SandWorld[0];
	if(id == GroundType_SandWorld_D)	return &groundNormal_SandWorld_D[0];
	if(id == GroundType_StoS)	        return &groundNormal_StoS[0];
	if(id == GroundType_SnowWorld)	    return &groundNormal_SnowWorld[0];
	
}

float returnWorldRot(int i)
{
	if (i == 1)	return worldRot.x;
	if (i == 2)	return worldRot.y;
	if (i == 3)	return worldRot.z;
}

D3DXVECTOR3 GetGroungModelSCL()
{
	return D3DXVECTOR3(SCALE_X, SCALE_Y, SCALE_Z);
}

void resetGround()
{
	b_ResetGround = true;
}

void InitNormals()
{
	//Record GroundMesh Normal
	D3DXMATRIX  mtxScl, mtxRot, mtxTranslate, mtxWorld;
	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);
	// スケールを反映
	D3DXMatrixScaling(&mtxScl, SCALE_X, SCALE_Y, SCALE_Z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScl);

	// 回転を反映
	//D3DXMatrixRotationYawPitchRoll(&mtxRot, 0.0f, 0.0f, 0.0f);
	//D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 移動を反映
	//D3DXMatrixTranslation(&mtxTranslate, Move_X, Move_Y, Move_Z);
	//D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTranslate);

	//====================GetNormal
	// HeartWorld
	groundNormal_HeartWorld = new D3DXVECTOR3[g_Model[GroundType_HeartWorld].IndexNum];
	for (int i = 0; i < g_Model[GroundType_HeartWorld].IndexNum; i += 3) {
		D3DXVECTOR3 p0, p1, p2;
		p0 = g_Model[GroundType_HeartWorld].Vertexlist[g_Model[GroundType_HeartWorld].Indexlist[i]];
		p1 = g_Model[GroundType_HeartWorld].Vertexlist[g_Model[GroundType_HeartWorld].Indexlist[i + 1]];
		p2 = g_Model[GroundType_HeartWorld].Vertexlist[g_Model[GroundType_HeartWorld].Indexlist[i + 2]];

		D3DXVec3TransformCoord(&p0, &p0, &mtxWorld);
		D3DXVec3TransformCoord(&p1, &p1, &mtxWorld);
		D3DXVec3TransformCoord(&p2, &p2, &mtxWorld);
		D3DXVECTOR3		nor;		// ポリゴンの法線
		D3DXVECTOR3		vec1;
		D3DXVECTOR3		vec2;

		{	// ポリゴンの外積をとって法線を求める(固定物なら予めInit()で行っておくと良い)
			vec1 = p1 - p0;
			vec2 = p2 - p0;
			crossProduct(&nor, &vec2, &vec1);
			D3DXVec3Normalize(&nor, &nor);		// 計算しやすいように法線をノーマルイズしておく
			groundNormal_HeartWorld[i] = nor;				// 求めた法線を入れておく
			groundNormal_HeartWorld[i + 1] = nor;			// 求めた法線を入れておく
			groundNormal_HeartWorld[i + 2] = nor;			// 求めた法線を入れておく
		}
	}

	// HtoW
	groundNormal_HtoW = new D3DXVECTOR3[g_Model[GroundType_HtoW].IndexNum];
	for (int i = 0; i < g_Model[GroundType_HtoW].IndexNum; i += 3) {
		D3DXVECTOR3 p0, p1, p2;
		p0 = g_Model[GroundType_HtoW].Vertexlist[g_Model[GroundType_HtoW].Indexlist[i]];
		p1 = g_Model[GroundType_HtoW].Vertexlist[g_Model[GroundType_HtoW].Indexlist[i + 1]];
		p2 = g_Model[GroundType_HtoW].Vertexlist[g_Model[GroundType_HtoW].Indexlist[i + 2]];

		D3DXVec3TransformCoord(&p0, &p0, &mtxWorld);
		D3DXVec3TransformCoord(&p1, &p1, &mtxWorld);
		D3DXVec3TransformCoord(&p2, &p2, &mtxWorld);
		D3DXVECTOR3		nor;		// ポリゴンの法線
		D3DXVECTOR3		vec1;
		D3DXVECTOR3		vec2;

		{	// ポリゴンの外積をとって法線を求める(固定物なら予めInit()で行っておくと良い)
			vec1 = p1 - p0;
			vec2 = p2 - p0;
			crossProduct(&nor, &vec2, &vec1);
			D3DXVec3Normalize(&nor, &nor);		// 計算しやすいように法線をノーマルイズしておく
			groundNormal_HtoW[i] = nor;				// 求めた法線を入れておく
			groundNormal_HtoW[i + 1] = nor;			// 求めた法線を入れておく
			groundNormal_HtoW[i + 2] = nor;			// 求めた法線を入れておく
		}
	}

	// WonderLand
	groundNormal_WonderLand = new D3DXVECTOR3[g_Model[GroundType_Wonderland].IndexNum];
	for (int i = 0; i < g_Model[GroundType_Wonderland].IndexNum; i += 3) {
		D3DXVECTOR3 p0, p1, p2;
		p0 = g_Model[GroundType_Wonderland].Vertexlist[g_Model[GroundType_Wonderland].Indexlist[i]];
		p1 = g_Model[GroundType_Wonderland].Vertexlist[g_Model[GroundType_Wonderland].Indexlist[i + 1]];
		p2 = g_Model[GroundType_Wonderland].Vertexlist[g_Model[GroundType_Wonderland].Indexlist[i + 2]];

		D3DXVec3TransformCoord(&p0, &p0, &mtxWorld);
		D3DXVec3TransformCoord(&p1, &p1, &mtxWorld);
		D3DXVec3TransformCoord(&p2, &p2, &mtxWorld);
		D3DXVECTOR3		nor;		// ポリゴンの法線
		D3DXVECTOR3		vec1;
		D3DXVECTOR3		vec2;

		{	// ポリゴンの外積をとって法線を求める(固定物なら予めInit()で行っておくと良い)
			vec1 = p1 - p0;
			vec2 = p2 - p0;
			crossProduct(&nor, &vec2, &vec1);
			D3DXVec3Normalize(&nor, &nor);		// 計算しやすいように法線をノーマルイズしておく
			groundNormal_WonderLand[i] = nor;				// 求めた法線を入れておく
			groundNormal_WonderLand[i + 1] = nor;			// 求めた法線を入れておく
			groundNormal_WonderLand[i + 2] = nor;			// 求めた法線を入れておく
		}
	}

	// WonderLand2
	groundNormal_WonderLand2 = new D3DXVECTOR3[g_Model[GroundType_Wonderland2].IndexNum];
	for (int i = 0; i < g_Model[GroundType_Wonderland2].IndexNum; i += 3) {
		D3DXVECTOR3 p0, p1, p2;
		p0 = g_Model[GroundType_Wonderland2].Vertexlist[g_Model[GroundType_Wonderland2].Indexlist[i]];
		p1 = g_Model[GroundType_Wonderland2].Vertexlist[g_Model[GroundType_Wonderland2].Indexlist[i + 1]];
		p2 = g_Model[GroundType_Wonderland2].Vertexlist[g_Model[GroundType_Wonderland2].Indexlist[i + 2]];

		D3DXVec3TransformCoord(&p0, &p0, &mtxWorld);
		D3DXVec3TransformCoord(&p1, &p1, &mtxWorld);
		D3DXVec3TransformCoord(&p2, &p2, &mtxWorld);
		D3DXVECTOR3		nor;		// ポリゴンの法線
		D3DXVECTOR3		vec1;
		D3DXVECTOR3		vec2;

		{	// ポリゴンの外積をとって法線を求める(固定物なら予めInit()で行っておくと良い)
			vec1 = p1 - p0;
			vec2 = p2 - p0;
			crossProduct(&nor, &vec2, &vec1);
			D3DXVec3Normalize(&nor, &nor);		// 計算しやすいように法線をノーマルイズしておく
			groundNormal_WonderLand2[i] = nor;				// 求めた法線を入れておく
			groundNormal_WonderLand2[i + 1] = nor;			// 求めた法線を入れておく
			groundNormal_WonderLand2[i + 2] = nor;			// 求めた法線を入れておく
		}
	}

	// WtoS
	groundNormal_WtoS = new D3DXVECTOR3[g_Model[GroundType_WtoS].IndexNum];
	for (int i = 0; i < g_Model[GroundType_WtoS].IndexNum; i += 3) {
		D3DXVECTOR3 p0, p1, p2;
		p0 = g_Model[GroundType_WtoS].Vertexlist[g_Model[GroundType_WtoS].Indexlist[i]];
		p1 = g_Model[GroundType_WtoS].Vertexlist[g_Model[GroundType_WtoS].Indexlist[i + 1]];
		p2 = g_Model[GroundType_WtoS].Vertexlist[g_Model[GroundType_WtoS].Indexlist[i + 2]];

		D3DXVec3TransformCoord(&p0, &p0, &mtxWorld);
		D3DXVec3TransformCoord(&p1, &p1, &mtxWorld);
		D3DXVec3TransformCoord(&p2, &p2, &mtxWorld);
		D3DXVECTOR3		nor;		// ポリゴンの法線
		D3DXVECTOR3		vec1;
		D3DXVECTOR3		vec2;

		{	// ポリゴンの外積をとって法線を求める(固定物なら予めInit()で行っておくと良い)
			vec1 = p1 - p0;
			vec2 = p2 - p0;
			crossProduct(&nor, &vec2, &vec1);
			D3DXVec3Normalize(&nor, &nor);		// 計算しやすいように法線をノーマルイズしておく
			groundNormal_WtoS[i] = nor;				// 求めた法線を入れておく
			groundNormal_WtoS[i + 1] = nor;			// 求めた法線を入れておく
			groundNormal_WtoS[i + 2] = nor;			// 求めた法線を入れておく
		}
	}

	// SandWorld
	groundNormal_SandWorld = new D3DXVECTOR3[g_Model[GroundType_SandWorld].IndexNum];
	for (int i = 0; i < g_Model[GroundType_SandWorld].IndexNum; i += 3) {
		D3DXVECTOR3 p0, p1, p2;
		p0 = g_Model[GroundType_SandWorld].Vertexlist[g_Model[GroundType_SandWorld].Indexlist[i]];
		p1 = g_Model[GroundType_SandWorld].Vertexlist[g_Model[GroundType_SandWorld].Indexlist[i + 1]];
		p2 = g_Model[GroundType_SandWorld].Vertexlist[g_Model[GroundType_SandWorld].Indexlist[i + 2]];

		D3DXVec3TransformCoord(&p0, &p0, &mtxWorld);
		D3DXVec3TransformCoord(&p1, &p1, &mtxWorld);
		D3DXVec3TransformCoord(&p2, &p2, &mtxWorld);
		D3DXVECTOR3		nor;		// ポリゴンの法線
		D3DXVECTOR3		vec1;
		D3DXVECTOR3		vec2;

		{	// ポリゴンの外積をとって法線を求める(固定物なら予めInit()で行っておくと良い)
			vec1 = p1 - p0;
			vec2 = p2 - p0;
			crossProduct(&nor, &vec2, &vec1);
			D3DXVec3Normalize(&nor, &nor);		// 計算しやすいように法線をノーマルイズしておく
			groundNormal_SandWorld[i] = nor;				// 求めた法線を入れておく
			groundNormal_SandWorld[i + 1] = nor;			// 求めた法線を入れておく
			groundNormal_SandWorld[i + 2] = nor;			// 求めた法線を入れておく
		}
	}

	// StoS
	groundNormal_StoS = new D3DXVECTOR3[g_Model[GroundType_StoS].IndexNum];
	for (int i = 0; i < g_Model[GroundType_StoS].IndexNum; i += 3) {
		D3DXVECTOR3 p0, p1, p2;
		p0 = g_Model[GroundType_StoS].Vertexlist[g_Model[GroundType_StoS].Indexlist[i]];
		p1 = g_Model[GroundType_StoS].Vertexlist[g_Model[GroundType_StoS].Indexlist[i + 1]];
		p2 = g_Model[GroundType_StoS].Vertexlist[g_Model[GroundType_StoS].Indexlist[i + 2]];

		D3DXVec3TransformCoord(&p0, &p0, &mtxWorld);
		D3DXVec3TransformCoord(&p1, &p1, &mtxWorld);
		D3DXVec3TransformCoord(&p2, &p2, &mtxWorld);
		D3DXVECTOR3		nor;		// ポリゴンの法線
		D3DXVECTOR3		vec1;
		D3DXVECTOR3		vec2;

		{	// ポリゴンの外積をとって法線を求める(固定物なら予めInit()で行っておくと良い)
			vec1 = p1 - p0;
			vec2 = p2 - p0;
			crossProduct(&nor, &vec2, &vec1);
			D3DXVec3Normalize(&nor, &nor);		// 計算しやすいように法線をノーマルイズしておく
			groundNormal_StoS[i] = nor;				// 求めた法線を入れておく
			groundNormal_StoS[i + 1] = nor;			// 求めた法線を入れておく
			groundNormal_StoS[i + 2] = nor;			// 求めた法線を入れておく
		}
	}

	// SnowWorld
	//groundNormal_SnowWorld = new D3DXVECTOR3[g_Model[GroundType_SnowWorld].IndexNum];
	//for (int i = 0; i < g_Model[GroundType_SnowWorld].IndexNum; i += 3) {
	//	D3DXVECTOR3 p0, p1, p2;
	//	p0 = g_Model[GroundType_SnowWorld].Vertexlist[g_Model[GroundType_SnowWorld].Indexlist[i]];
	//	p1 = g_Model[GroundType_SnowWorld].Vertexlist[g_Model[GroundType_SnowWorld].Indexlist[i + 1]];
	//	p2 = g_Model[GroundType_SnowWorld].Vertexlist[g_Model[GroundType_SnowWorld].Indexlist[i + 2]];
	//
	//	D3DXVec3TransformCoord(&p0, &p0, &mtxWorld);
	//	D3DXVec3TransformCoord(&p1, &p1, &mtxWorld);
	//	D3DXVec3TransformCoord(&p2, &p2, &mtxWorld);
	//	D3DXVECTOR3		nor;		// ポリゴンの法線
	//	D3DXVECTOR3		vec1;
	//	D3DXVECTOR3		vec2;
	//
	//	{	// ポリゴンの外積をとって法線を求める(固定物なら予めInit()で行っておくと良い)
	//		vec1 = p1 - p0;
	//		vec2 = p2 - p0;
	//		crossProduct(&nor, &vec2, &vec1);
	//		D3DXVec3Normalize(&nor, &nor);		// 計算しやすいように法線をノーマルイズしておく
	//		groundNormal_SnowWorld[i] = nor;				// 求めた法線を入れておく
	//		groundNormal_SnowWorld[i + 1] = nor;			// 求めた法線を入れておく
	//		groundNormal_SnowWorld[i + 2] = nor;			// 求めた法線を入れておく
	//	}
	//}

}

D3DXVECTOR3 RotWithWorld(D3DXVECTOR3 oriPos, D3DXVECTOR3 worldRot) {
	D3DXMATRIX mtxWorld;
	D3DXMATRIX mtxRot, mtxTranslate;
	D3DXVECTOR3 NewPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 OutPos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 OriPos = oriPos;
	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);
	//===========Back to 000
	// 移動を反映
	D3DXMatrixTranslation(&mtxTranslate, OriPos.x, OriPos.y, OriPos.z); // have to put the model at (0,0,0)
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTranslate);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, -CurRot.x, -CurRot.y, -CurRot.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	D3DXVec3TransformCoord(&NewPos, &NewPos, &mtxWorld);

	//===========to New
	D3DXMatrixIdentity(&mtxWorld);
	// 移動を反映
	D3DXMatrixTranslation(&mtxTranslate, NewPos.x, NewPos.y, NewPos.z); // have to put the model at (0,0,0)
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTranslate);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, worldRot.x, worldRot.y, worldRot.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	D3DXVec3TransformCoord(&OutPos, &OutPos, &mtxWorld);

	return OutPos;
}

int GetWorldRotStatus() {
	return WorldRotStatus;
}

void AdjustDeltaRot() {

	CaculatedWorldRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	if (-0.25 * D3DX_PI < worldRot.z && worldRot.z <= 0.25 * D3DX_PI)
	{
		CaculatedWorldRot.z = 0.0f;
	}
	else if (-0.75 * D3DX_PI < worldRot.z && worldRot.z <= -0.25 * D3DX_PI)
	{
		CaculatedWorldRot.z = -0.5 * D3DX_PI;
	}
	else if (0.25 * D3DX_PI < worldRot.z && worldRot.z <= 0.75 * D3DX_PI)
	{
		CaculatedWorldRot.z = 0.5 * D3DX_PI;
	}
	else if ((0.75 * D3DX_PI < worldRot.z && worldRot.z <= D3DX_PI) || (-0.75 * D3DX_PI > worldRot.z && worldRot.z >= -D3DX_PI))
	{
		CaculatedWorldRot.z = D3DX_PI;
	}

	if ((-0.75 * D3DX_PI > worldRot.z && worldRot.z >= -D3DX_PI)) {
		worldRot.z = D3DX_PI;
	}


	if ((-0.25 * D3DX_PI < worldRot.y && worldRot.y <= 0.25 * D3DX_PI))
	{
		CaculatedWorldRot.y = 0.0f;
	}
	else if (-0.75 * D3DX_PI < worldRot.y && worldRot.y <= -0.25 * D3DX_PI)
	{
		CaculatedWorldRot.y = -0.5 * D3DX_PI;
	}
	else if (0.25 * D3DX_PI < worldRot.y && worldRot.y <= 0.75 * D3DX_PI)
	{
		CaculatedWorldRot.y = 0.5 * D3DX_PI;
	}
	else if ((0.75 * D3DX_PI < worldRot.y && worldRot.y <= D3DX_PI) || (-0.75 * D3DX_PI > worldRot.y && worldRot.y >= -D3DX_PI))
	{
		CaculatedWorldRot.y = D3DX_PI;
	}

	if (CaculatedWorldRot.y == -1.0f * D3DX_PI) CaculatedWorldRot.y = 1.0f * D3DX_PI;
	if (CaculatedWorldRot.z == -1.0f * D3DX_PI) CaculatedWorldRot.z = 1.0f * D3DX_PI;

	if(CaculatedWorldRot.y ==  0.0f * D3DX_PI && CaculatedWorldRot.z ==  0.0f * D3DX_PI) {CaculatedWorldRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f); CaculatedWorldRot.y =  0.0f * D3DX_PI; WorldRotStatus = 1; }
	if(CaculatedWorldRot.y ==  0.0f * D3DX_PI && CaculatedWorldRot.z ==  0.5f * D3DX_PI) {CaculatedWorldRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f); CaculatedWorldRot.z =  0.5f * D3DX_PI; WorldRotStatus = 4; }
	if(CaculatedWorldRot.y ==  0.0f * D3DX_PI && CaculatedWorldRot.z ==  1.0f * D3DX_PI) {CaculatedWorldRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f); CaculatedWorldRot.z =  1.0f * D3DX_PI; WorldRotStatus = 5; }
	if(CaculatedWorldRot.y ==  0.0f * D3DX_PI && CaculatedWorldRot.z == -0.5f * D3DX_PI) {CaculatedWorldRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f); CaculatedWorldRot.z = -0.5f * D3DX_PI; WorldRotStatus = 3; }
																							 
	if(CaculatedWorldRot.y ==  1.0f * D3DX_PI && CaculatedWorldRot.z ==  0.0f * D3DX_PI) {CaculatedWorldRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f); CaculatedWorldRot.z =  1.0f * D3DX_PI; WorldRotStatus = 5; }
	if(CaculatedWorldRot.y ==  1.0f * D3DX_PI && CaculatedWorldRot.z ==  0.5f * D3DX_PI) {CaculatedWorldRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f); CaculatedWorldRot.z = -0.5f * D3DX_PI; WorldRotStatus = 3; }
	if(CaculatedWorldRot.y ==  1.0f * D3DX_PI && CaculatedWorldRot.z ==  1.0f * D3DX_PI) {CaculatedWorldRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f); CaculatedWorldRot.y =  0.0f * D3DX_PI; WorldRotStatus = 1; }
	if(CaculatedWorldRot.y ==  1.0f * D3DX_PI && CaculatedWorldRot.z == -0.5f * D3DX_PI) {CaculatedWorldRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f); CaculatedWorldRot.z =  0.5f * D3DX_PI; WorldRotStatus = 4; }
								
	if(CaculatedWorldRot.y ==  0.5f * D3DX_PI)  {CaculatedWorldRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f); CaculatedWorldRot.y =  0.5f * D3DX_PI; WorldRotStatus = 6; }
	if(CaculatedWorldRot.y == -0.5f * D3DX_PI ) {CaculatedWorldRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f); CaculatedWorldRot.y = -0.5f * D3DX_PI; WorldRotStatus = 2; }
																				
	TempWrongRotRecord = worldRot;
	RotatetoDice = true;
	SetFixDiceRotate();

	// after RotedtoDice
	//SetCameraPosRecord();
	//SetCameraAtRecord();
}

void SetFixDiceRotate() {

	r_time = 0.0f;
	cameraRotRecord = TempWrongRotRecord;
	D3DXVECTOR3 ep = CaculatedWorldRot;
	D3DXVECTOR3 cp1 = D3DXVECTOR3(
		(CaculatedWorldRot.x - cameraRotRecord.x) / 3 + cameraRotRecord.x,
		(CaculatedWorldRot.y - cameraRotRecord.y) / 3 + cameraRotRecord.y,
		(CaculatedWorldRot.z - cameraRotRecord.z) / 3 + cameraRotRecord.z);
	D3DXVECTOR3 cp2 = D3DXVECTOR3(
		(CaculatedWorldRot.x - cameraRotRecord.x) * 2 / 3 + cameraRotRecord.x,
		(CaculatedWorldRot.y - cameraRotRecord.y) * 2 / 3 + cameraRotRecord.y,
		(CaculatedWorldRot.z - cameraRotRecord.z) * 2 / 3 + cameraRotRecord.z);


	ZeroMemory(&Rotroute, sizeof(Rotroute));
	Rotroute = new D3DXVECTOR3[CAMERA_ROUTE_SEGMENT + 1];
	Rotroute = GetThreePowerBeizerList(cameraRotRecord, cp1, cp2, ep, CAMERA_ROUTE_SEGMENT);
	Rotroute[CAMERA_ROUTE_SEGMENT] = ep;
}

void FixDiceRotate() {

	// エネミー０番だけテーブルに従って座標移動（線形補間）
	int nowNo = r_time;		                              // 整数分であるテーブル番号を取り出している
	int maxNo = CAMERA_ROUTE_SEGMENT + 1;			      // 登録テーブル数を数えている
	int nextNo = (nowNo + 1) % maxNo;			          // 移動先テーブルの番号を求めている
	D3DXVECTOR3	rot = Rotroute[nextNo] - Rotroute[nowNo]; // XYZ移動量を計算している
	float nowTime = r_time - nowNo;	                      // 時間部分である少数を取り出している
	rot *= nowTime;							     	      // 現在の移動量を計算している
	r_time += r_speed;	                                  // 時間を進めている
	// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
	worldRot = Rotroute[nowNo] + rot;

	if ((int)r_time >= CAMERA_ROUTE_SEGMENT)		// 登録テーブル最後まで移動したか？
	{
		worldRot = Rotroute[CAMERA_ROUTE_SEGMENT];
		r_time = CAMERA_ROUTE_SEGMENT;
		RotatetoDice = false;
		rotateOBJ();
		SetCameraPosRecord();
		SetCameraAtRecord();
		SetCameraWorldRotateView(false);
		setwrm(false);
	}
	
}

void rotateOBJ() {
	D3DXVECTOR3 NewPos;
	//Player rot
	if (!b_ResetGround) {
		NewPos = RotWithWorld(GetPlayer()->pos, worldRot);
		GetPlayer()->On_Moving_Plat = false;
		setPlayerPosition(NewPos);
	}
	else {
		b_ResetGround = false;
	}
	//Plat rot
	PLATFORM *plat = GetPlatform();
	for (int i = 0; i < MAX_PLATFORM; i++) {
		if (plat[i].Use && plat[i].RotWithStage) {
			NewPos = RotWithWorld(plat[i].pos, worldRot);
			plat[i].pos = NewPos;
			D3DXVECTOR3 dRot = worldRot - CurRot;
			plat[i].WorldRot.z += dRot.z;
			plat[i].WorldRot.x += dRot.y;
		}
	}

	//Enemy rot
	ENEMY *enemy = GetEnemy();
	for (int i = 0; i < MAX_ENEMY; i++) {
		if (enemy[i].Use && enemy[i].RotWithStage) {
			NewPos = RotWithWorld(enemy[i].pos, worldRot);
			enemy[i].pos = NewPos;
			if (enemy[i].RotWithStage_Self) {
				D3DXVECTOR3 dRot = worldRot - CurRot;
				enemy[i].WorldRot.z -= dRot.z;
				enemy[i].WorldRot.x -= dRot.y;
			}

		}
	}

	//World Collider Rot
	COLLIDER3D *collider = GetCollider();
	for (int i = 0; i < MAX_COLLIDER3D; i++) {
		if (collider[i].tag == collider3DTag_GroundArea_Stage1 ||
			collider[i].tag == collider3DTag_GroundArea_Stage2 ||
			collider[i].tag == collider3DTag_GroundArea_Stage3 ) {
			NewPos = RotWithWorld(collider[i].pos, worldRot);
			collider[i].pos = NewPos;
		}
	}
	CurRot = worldRot;

}