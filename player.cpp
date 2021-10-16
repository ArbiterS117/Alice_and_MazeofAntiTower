//=============================================================================
//
// プレーヤ処理 [player.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#include "main.h"
#include "gameScene.h"
#include "input.h"
#include "camera.h"
#include "player.h"
#include "enemy.h"
#include "renderer.h"
#include "Collider3D.h"
#include "shadow.h"
#include "bullet.h"
#include "player_partsModelAnim.h"
#include "sound.h"
#include "particle.h"
#include "score.h"
#include "fade.h"
#include "platform.h"
#include "light.h"
#include "EditTool.h"
#include "debugproc.h"
#include "ground.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	VALUE_MOVE			   (1.5f)					// 移動量 (加速)
#define	VALUE_AIR_MOVE		   (0.6f)					// 空中移動量 (加速)
#define VALUE_DECELERATE       (0.8f)
#define VALUE_AIR_DECELERATE   (0.3f)
#define	VALUE_ROTATE		   (D3DX_PI * 0.02f)		// 回転量
#define	VALUE_GRAVITY		   (1.5f)		            // 重力
#define	VALUE_THROWINGGRAVITY  (0.5f)		            // カード投げる重力
#define	VALUE_CANSLOPECAL      (0.6f)		            // 1 => flat ; 0.99~0.6 => can walk on slope ; 0.6~ => like wall 

#define PLAYER_SHADOW_SIZE	   (3.0f)					// 影の大きさ

//COLLIDER
#define PLAYER_BODYCHECKC_XZ       (60.0f)	
#define PLAYER_BODYCHECKC_Y        (100.0f)	
#define PLAYER_GROUNDCHECKC_Y      (PLAYER_BODYCHECKC_Y * 0.5)		
#define PLAYER_GROUND_OFFSET_Y     (PLAYER_BODYCHECKC_Y * 0.5 + 30.0f)		
#define PLAYER_RANGE_COLLIDER_SIZE (500.0f)		

#define CAN_STAIR_HEIGHT                 (-20.0f)					
#define WALL_SLOPE_DEGREE                (0.7f) // 1 ~ 0  => normal Vector.y					
#define	VALUE_SLOPE_ADDSPD_DEGREE        (0.2f)		            
#define	PLAYER_MAX_SLOPEUP_MOVE_SPEED    (20.0f)		            
#define	PLAYER_MAX_SLOPEDOWN_MOVE_SPEED  (10.0f)		            

//SPEED
#define PLAYER_MAX_MOVE_SPEED           (15.0f)
#define PLAYER_MAX_MOVE_SPEED_SPRINT    (25.0f)
#define PLAYER_MAX_FALLEN_SPEED         (-30.0f)
#define PLAYER_JUMP_SPEED               (40.0f)
#define PLAYER_DASH_SPEED               (25.0f)
#define PLAYER_AIRDASH_SPEED            (25.0f)
#define PLAYER_DASH_JUMP_SPEED          (25.0f)
#define PLAYER_THROWING_JUMP_SPEED      (17.0f)
#define PLAYER_STEPCARD_JUMP_SPEED      (22.0f)
#define PLAYER_STEPCARD_DASHJUMP_SPEED  (30.0f)
#define PLAYER_WIND_MOVE_SPEED          (25.0f)
#define PLAYER_WIND_JUMP_SPEED          (42.0f)
#define PLAYER_BOB_JUMP_SPEED           (5.0f)
#define PLAYER_MAX_BOB_MOVE_SPEED       (10.0f)
#define PLAYER_MAX_SAND_MOVE_SPEED      (4.0f)
#define PLAYER_SAND_JUMP_SPEED          (30.0f)
#define PLAYER_MUSH_JUMP_SPEED          (60.0f)
#define	VALUE_SNADGRAVITY               (0.5f)		            
#define	VALUE_SNADJUMP_GRAVITY          (7.5f)		            
#define	VALUE_BOB_GRAVITY               (0.01f)		            

#define PLAYER_MAX_MOVE_SPEED_ICE       (25.0f)
#define	VALUE_ICE_MOVE			        (0.2f)
#define VALUE_ICE_DECELERATE            (0.1f)
#define VALUE_CAMERA_DIS_NOT_DRAW       (150.0f)

//TIMER
#define PLAYER_DASHTIME            (15)                      // ダッシュ時間
#define PLAYER_DASHCDTIME          (45)                      // ダッシュCD時間
#define PLAYER_THROWING_TIME       (15)                      // カード投げる時間
#define PLAYER_USEABILITYCD        (60)                      // カード能力を使うCD時間
#define PLAYER_WATERABILITYCD      (10)                      // カード能力アピールCD時間
#define PLAYER_BOBSTATETIME        (4200)                    // 泡能力時間
#define PLAYER_SANDSTATETIME       (1000)                    // 砂状態時間
#define PLAYER_MOVESMOKECDTIME     (6)                       // 歩いての煙
#define PLAYER_INAIR_TO_DEADTIME   (200)                     

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static void InputUpdate(void);
static void PhysicsUpdate(PLAYER &player);
static void CollisionUpdate(PLAYER &player);
static void PlayerStatusUpdate(PLAYER &player);
static void AnimStatusUpdate(PLAYER &player);

static void ChangeForceY(float force);
static void MovingPlatPhysicsUpdate(PLAYER &player, int InRangeOBJNum, int *OBJInRangeList);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER				g_Player;						// プレイヤー

static float                backToMAXMoveSpeed = 0.2f;

bool getFinalStar = false;

D3DXVECTOR3 temp_player_leave_plat_speed;

D3DXVECTOR3 playerRebornPos[12] = {
	{500.0f, -900.0f, -9650.0f},
	{-790.0f, 0.0f, -736.0f},
	{-955.0, -3000.0f, -200.0f},
	{-926.0f, 1950.0f, 5470.0f},
	{-560.0f, 3400.0f, 11300.0f},
	{ 3800.0f, 3718.0f, 22000.0f},
	{-800.0f, 2766.0f, 17530.0f},
	{-800.0f, 2766.0f, 17530.0f},
	{-800.0f, 2766.0f, 17530.0f},
	{-800.0f, 2766.0f, 17530.0f},
	{-800.0f, 2766.0f, 17530.0f},
	{-800.0f, 2766.0f, 17530.0f},

};
static int Max_Birth_Point = sizeof(playerRebornPos) / sizeof(playerRebornPos[0]);
int CurPlayerRebornPos = 0;
int RebornPosUsedID[12];
bool firstRebornPos;

static bool g_canCreateSPParticle = true;
static bool g_canCtrl = true;
static bool g_wrm = false;
static int  g_getStarAnimTimer = 0;
static int  waterAbilityTimer = 0;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	g_Player.Use = true;

	// 位置・回転・スケールの初期設定
	g_Player.pos = playerRebornPos[0];
	g_Player.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Player.scl = D3DXVECTOR3(3.0f, 3.0f, 3.0f);
	g_Player.prePos = g_Player.pos;

	//physics
	g_Player.speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Player.dirXZ = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	g_Player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
	g_Player.decelerate   = VALUE_DECELERATE;
	g_Player.gravityScale = VALUE_GRAVITY;
	g_Player.dashSpeed    = PLAYER_DASH_SPEED;
	g_Player.dashDir      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Player.WallPushVec1 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Player.WallPushVec2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	////===system
	g_Player.isGround            = false;
	g_Player.isGroundPrev        = true;
	g_Player.canDash             = true;
	g_Player.dashCDTimer         = 0;
	g_Player.isDash              = false;
	g_Player.dashTimer           = 0;
	g_Player.isFloat             = false;
	g_Player.cardBulletID        = -1;
	g_Player.cardInHand          = true;
	g_Player.canThrowCard        = true;
	g_Player.isThrowing          = false;
	g_Player.throwingTimer       = 0;
	g_Player.moveSmokeTimer      = 0;
	g_Player.playerAbility       = CardAbility_none;
	g_Player.canUseCard          = true;
	g_Player.canUseCardTrigger   = false;
	g_Player.canUseCardTimer     = 0;
	g_Player.isInBobStatus       = false;
	g_Player.bobStateTimer       = 0;
	g_Player.isInSandStatus		 = false;
	g_Player.SandStateTimer		 = 0;
	g_Player.Anim_Status         = AnimStatus_Idel;
	g_Player.backToMAXMove       = false;
	g_Player.Anim_isJumping      = false;
	g_Player.Anim_isWindBoost    = false;
	g_Player.Anim_isCatchedCard  = false;
	g_Player.Anim_isStepOnMushroomSpring = false;
	g_Player.Anim_GetStar        = false;
	g_Player.isOnSlope           = false;
	g_Player.isInIceStatus       = false;
	g_Player.TouchedCeiling      = false;
	g_Player.touchingCeilingTimer = 0;

	////====moving plat
	g_Player.Exit_Moving_Plat  = false;

	////====階層アニメーション parts init
	InitPlayerPartAnim();

	////====collider
	g_Player.bodyColliderIdx         = CreateCollider3DBox(collider3DTag_player, g_Player.pos, PLAYER_BODYCHECKC_XZ, PLAYER_BODYCHECKC_Y, PLAYER_BODYCHECKC_XZ, 0);
	g_Player.CircleRangeColliderIdx  = CreateCollider3DCircle(collider3DTag_CircleRange, g_Player.pos, PLAYER_RANGE_COLLIDER_SIZE, 0);
	g_Player.groundCheckColliderIdx  = CreateCollider3DBox(collider3DTag_groundCheck, D3DXVECTOR3(g_Player.pos.x, g_Player.pos.y - PLAYER_GROUNDCHECKC_Y - PLAYER_BODYCHECKC_XZ * 0.5f, g_Player.pos.z), 10.0f, 10.0f, 10.0f, 0);
	
	//shadow
	D3DXVECTOR3 pos = g_Player.pos;
	pos.y = 0.0f;
	//g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);

	//other
	for (int i = 0; i < 12; i++) {
		RebornPosUsedID[i] = -1;
	}
	firstRebornPos = false;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	UninitPlayerPartAnim();

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	//================Input
	InputUpdate();
	if (GetWorldRotateMode() || g_wrm)return;
	//================Physics
	PhysicsUpdate(g_Player);

	//================collider detect
	CollisionUpdate(g_Player);

	//================PlayerStatus & other
	PlayerStatusUpdate(g_Player);

	// 影もプレイヤーの位置に合わせる
	D3DXVECTOR3 pos = g_Player.pos;
	pos.y = 0.0f;
	//SetPositionShadow(g_Player.shadowIdx, pos);

	////================階層アニメーション更新
	AnimStatusUpdate(g_Player);
	UpdatePlayerPartAnim();

	// ポイントライトのテスト
	{
		LIGHT *light = GetLightData(2);
		D3DXVECTOR3 pos = g_Player.pos;
		pos.y += 20.0f;

		light->Position = pos;
		light->Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light->Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light->Direction = D3DXVECTOR3(0.2f, 1.0f, 0.2f);
		light->Type = LIGHT_TYPE_POINT;
		light->Attenuation = 40.0f;	// 減衰距離
		light->Enable = true;
		SetLightData(1, light);
	}

	//================== debug
#ifdef _DEBUG	// デバッグ情報を表示する
	char* str = GetDebugStr();
	//sprintf(&str[strlen(str)], " posX : %.2f   posY : %.2f   posZ : %.2f ", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	//sprintf(&str[strlen(str)], " MAXSPD : %.2f ", g_Player.maxMoveSpeed);
	//sprintf(&str[strlen(str)], " spdX : %.2f   spdY : %.2f   spdZ : %.2f ", g_Player.speed.x, g_Player.speed.y, g_Player.speed.z);
	//PrintDebugProc("\n wallVec1: X:%f ,Y:%f, Z:%f\n", g_Player.WallPushVec1.x, g_Player.WallPushVec1.y, g_Player.WallPushVec1.z);
	//PrintDebugProc("\n wallVec2: X:%f ,Y:%f, Z:%f\n", g_Player.WallPushVec2.x, g_Player.WallPushVec2.y, g_Player.WallPushVec2.z);
	//PrintDebugProc("\n dot:%f\n", D3DXVec3Dot(&g_Player.WallPushVec1, &g_Player.WallPushVec2));
	PrintDebugProc("\n Cur RebornPos: %d\n", CurPlayerRebornPos);
#endif

	//=====debug
	if (GetKeyboardTrigger(DIK_0)) {
		if (GetGameSceneEditMode() == false) {
			playerDead();
		}
	}

	if (fabs(g_Player.pos.x) > 120000.0f || fabs(g_Player.pos.y) > 120000.0f || fabs(g_Player.pos.z) > 120000.0f) {
		g_Player.pos = playerRebornPos[CurPlayerRebornPos];
		g_Player.speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
	
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	D3DXVECTOR3 dis = g_Player.pos - GetCamera()->pos;
	float camera_dis_sq = D3DXVec3LengthSq(&dis);

	PrintDebugProc("\n\ncamera_dis_sq: %f\n\n", camera_dis_sq);

	if (camera_dis_sq < VALUE_CAMERA_DIS_NOT_DRAW * VALUE_CAMERA_DIS_NOT_DRAW) return;

	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	//SetAlphaTestEnable(true);

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_Player.mtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScl, g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxScl);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Player.rot.y, g_Player.rot.x, g_Player.rot.z);
	//D3DXQUATERNION q;
	//D3DXMatrixRotationQuaternion(&mtxRot, &q);
	D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxRot);

	// 移動を反映
	if (GetWorldRotateMode() || g_wrm) D3DXMatrixTranslation(&mtxTranslate, g_Player.wrm_Pos.x, g_Player.wrm_Pos.y, g_Player.wrm_Pos.z);
	else D3DXMatrixTranslation(&mtxTranslate, g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&g_Player.mtxWorld);

	// モデル描画
	//DrawModel(&g_Player.model);

	//階層アニメーション 親を描画してから、PARTS(子)の描画
	DrawPlayerPartAnim();

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);

	//SetAlphaTestEnable(false);
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

void InputUpdate(void) {

	if (GetKeyboardTrigger(DIK_NUMPAD2))g_canCtrl = !g_canCtrl;
	if (GetKeyboardTrigger(DIK_TAB))g_canCtrl = !g_canCtrl;
	if (GetKeyboardTrigger(DIK_F9) || IsMouseCenterTriggered()) g_Player.pos = GetCamera()->at;

	//Mouse
	if (IsMouseLeftTriggered()) {
		if(GetWorldRotateMode)AdjustDeltaRot();
	}

	CAMERA *cam = GetCamera();
	// 移動させちゃう
	D3DXVECTOR3 dir = g_Player.pos - GetCamera()->pos;
	dir = D3DXVECTOR3{ dir.x, 0.0f, dir.z }; // don't want to fly (move according to dir.y)
	D3DXVECTOR3 Ndir = { dir.z, 0.0f, -dir.x };
	D3DXVec3Normalize(&dir, &dir);
	D3DXVec3Normalize(&Ndir, &Ndir);
	D3DXVECTOR3 InputDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//if (!cam->TPMode) return;
	if (!g_canCtrl) return;
	if (g_wrm) return;

	float moveSpeed;
	moveSpeed = VALUE_MOVE;
	if (g_Player.isInIceStatus)moveSpeed = VALUE_ICE_MOVE;
	if (!g_Player.isGround) moveSpeed = VALUE_AIR_MOVE;
	if (!g_Player.isGround && g_Player.Exit_Moving_Plat) moveSpeed = VALUE_AIR_MOVE * 0.5f;

	if (GetKeyboardPress(DIK_W))
	{
		g_Player.speed += dir * moveSpeed;
		g_Player.dirXZ += dir * moveSpeed;
		D3DXVec3Normalize(&g_Player.dirXZ, &g_Player.dirXZ);
		InputDir += dir;
		D3DXVec3Normalize(&InputDir, &InputDir);
	}
	if (GetKeyboardPress(DIK_S))
	{
		g_Player.speed -= dir * moveSpeed;
		g_Player.dirXZ -= dir * moveSpeed;
		D3DXVec3Normalize(&g_Player.dirXZ, &g_Player.dirXZ);
		InputDir -= dir;
		D3DXVec3Normalize(&InputDir, &InputDir);
	}
	if (GetKeyboardPress(DIK_A))
	{
		g_Player.speed -= Ndir * moveSpeed;
		g_Player.dirXZ -= Ndir * moveSpeed;
		D3DXVec3Normalize(&g_Player.dirXZ, &g_Player.dirXZ);
		InputDir -= Ndir;
		D3DXVec3Normalize(&InputDir, &InputDir);
	}
	if (GetKeyboardPress(DIK_D))
	{
		g_Player.speed += Ndir * moveSpeed;
		g_Player.dirXZ += Ndir * moveSpeed;
		D3DXVec3Normalize(&g_Player.dirXZ, &g_Player.dirXZ);
		InputDir += Ndir;
		D3DXVec3Normalize(&InputDir, &InputDir);
	}


	//gravity
	if (GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_A)) {
		if (g_Player.isGround && !g_Player.isInSandStatus) {
			COLLIDER3D *Collider = GetCollider();
			ChangeForceY(PLAYER_JUMP_SPEED); // jump
			SetParticle(D3DXVECTOR3(g_Player.pos.x, g_Player.pos.y - Collider[g_Player.bodyColliderIdx].hy * 0.5f, g_Player.pos.z), D3DXVECTOR3(0.0f, 1.0f, 0.0f), Particletype_Smoke, 10);
			PlaySound(SOUND_LABEL_SE_jump);
		}
		if (g_Player.isInBobStatus) {
			ChangeForceY(PLAYER_BOB_JUMP_SPEED);
			PlaySound(SOUND_LABEL_SE_jump);
		}
		if (g_Player.isInSandStatus) {
			ChangeForceY(PLAYER_SAND_JUMP_SPEED);
			PlaySound(SOUND_LABEL_SE_jump);
		}
	}

	//Sprint
	if (GetKeyboardPress(DIK_LSHIFT))
	{
		g_Player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED_SPRINT;
	}

	else {
		g_Player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
	}

	if (IsMouseRightPressed()) {
		if (g_Player.isGround) {
			SetCameraWorldRotateView(true);
			g_wrm = true;
			g_Player.wrm_Pos = g_Player.pos;
		}
	}
	
	//===========debug
	//if (GetKeyboardTrigger(DIK_CAPSLOCK)) {
	//	g_canCreateSPParticle = !g_canCreateSPParticle;
	//}
	//if (GetKeyboardTrigger(DIK_2)) {
	//	g_Player.playerAbility = CardAbility_water;
	//}
	if (GetKeyboardTrigger(DIK_F7)) {
		CurPlayerRebornPos += 1;
	}

}

void PhysicsUpdate(PLAYER & player) {
	COLLIDER3D *Collider = GetCollider();
	PLATFORM *plat = GetPlatform();

	//==========Caculate Speed
	D3DXVECTOR3 speedXZ = D3DXVECTOR3(player.speed.x, 0.0f, player.speed.z);
	D3DXVECTOR3 dir = speedXZ;
	D3DXVec3Normalize(&dir, &dir);
	//if(D3DXVec3Length(&dir) != 0.0f)player.dirXZ = dir; //プレーヤの向き
	player.speedXZLengh = D3DXVec3Length(&speedXZ);
	if (player.speedXZLengh >= player.maxMoveSpeed) {
		player.speed.x = dir.x * player.maxMoveSpeed;
		player.speed.z = dir.z * player.maxMoveSpeed;
	}
	if (player.speedXZLengh > 0) {
		player.speed -= dir * player.decelerate;
	}
	if (player.speedXZLengh <= player.decelerate) {
		player.speed.x = 0.0f; 
		player.speed.z = 0.0f;
	}

	//Gravity
	if (player.gravityScale > 0.0f) {
		if (player.isGround == false)player.speed.y -= player.gravityScale;

	}
	if (player.speed.y < PLAYER_MAX_FALLEN_SPEED) {
		player.speed.y = PLAYER_MAX_FALLEN_SPEED;
	}

	//==========Caculate Rot
	player.rot.y = (atan2f(player.dirXZ.z, player.dirXZ.x) - D3DX_PI * 0.5f) * -1.0f;   //coordinate(counter clockwise), obj rotation(clockwise) : different way => -1

	//==========Dash => maxMove & gravity 加算の後 maxMoveを変更する必要がある
	if (player.isDash) {
		player.speed.x = player.dashDir.x * player.dashSpeed;
		player.speed.z = player.dashDir.z * player.dashSpeed;
		//player.speed.y = 0.0f;
	}

	//==========Ground Detect
	if (player.On_Moving_Plat) {
		player.pos.y = Collider[player.bodyColliderIdx].hy * 0.5f + Collider[player.On_Moving_PlatID].pos.y + Collider[player.On_Moving_PlatID].hy * 0.5f;
	}
	//==========ground Detect
	//==============groundTest Raycast
	D3DXVECTOR3 hit, normal;
	bool rayHit = false;
	if (player.InStage1Area)rayHit += RayHitGround(player.pos, &hit, &normal, GroundType_Test);
	if (player.InStage2Area)rayHit += RayHitGround(player.pos, &hit, &normal, GroundType_Test2);
	if (player.InStage3Area)rayHit += RayHitGround(player.pos, &hit, &normal, GroundType_Test3);
	//if(player.InHeartWorldArea) rayHit += RayHitGround(player.pos, &hit, &normal, GroundType_HeartWorld);
	//if(player.InHtoWArea)       rayHit += RayHitGround(player.pos, &hit, &normal, GroundType_HtoW);
	//if(player.InWonderLandArea) rayHit += RayHitGround(player.pos, &hit, &normal, GroundType_Wonderland);
	//if(player.InWonderLand2Area)rayHit += RayHitGround(player.pos, &hit, &normal, GroundType_Wonderland2);
	//if(player.InWtoSArea)       rayHit += RayHitGround(player.pos, &hit, &normal, GroundType_WtoS);
	//if(player.InSandWorldArea)  rayHit += RayHitGround(player.pos, &hit, &normal, GroundType_SandWorld);
	//if(player.InStoSArea)       rayHit += RayHitGround(player.pos, &hit, &normal, GroundType_StoS);
	//if(player.InSnowWorldArea)  rayHit += RayHitGround(player.pos, &hit, &normal, GroundType_SnowWorld);
	
	if (rayHit) {
		if (player.speed.y <= 0.0f) {
			bool slopeANS = false;
			if (player.isOnSlope && player.pos.y - hit.y <= PLAYER_GROUND_OFFSET_Y * 1.5f)slopeANS = true;
			if (player.pos.y - hit.y <= PLAYER_GROUND_OFFSET_Y * 1.2f || slopeANS) {
				player.isGround = true;
				player.pos = hit;
				player.pos.y += PLAYER_GROUND_OFFSET_Y;
				if(player.InSnowWorldArea || player.InStoSArea)player.isInIceStatus = true;
				else player.isInIceStatus = false;
				// SLOPE
				float slope = dotProduct(&player.dirXZ, &normal);
				if (slope >= VALUE_SLOPE_ADDSPD_DEGREE) {
					player.isOnSlope = true;
					g_Player.maxMoveSpeed = PLAYER_MAX_SLOPEUP_MOVE_SPEED;
				}
				else if (slope <= -VALUE_SLOPE_ADDSPD_DEGREE) {
					player.isOnSlope = true;
					if (g_Player.maxMoveSpeed > PLAYER_MAX_MOVE_SPEED - ((PLAYER_MAX_MOVE_SPEED -PLAYER_MAX_SLOPEDOWN_MOVE_SPEED) * (-slope/0.5f))) {
						g_Player.maxMoveSpeed -= backToMAXMoveSpeed;					
					}
					else {
						g_Player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED - ((PLAYER_MAX_MOVE_SPEED - PLAYER_MAX_SLOPEDOWN_MOVE_SPEED) * (-slope / 0.5f));
					}
				}
				else {
					player.isOnSlope = false;
				}
			}
			else {
				player.isGround = false;
			}
		}
	}
	else {
		player.isGround = false;
	}

	//天井
	//if (player.speed.y > 0.0f) { // 天井
    player.TouchedCeiling = false; // init
	D3DXVECTOR3 cellingY = player.pos;
	cellingY.y += PLAYER_GROUNDCHECKC_Y + 100.0f;
	rayHit = false;
	if (player.InStage1Area)rayHit += RayHitGround(cellingY, &hit, &normal, GroundType_Test, 100.0f);
	if (player.InStage2Area)rayHit += RayHitGround(cellingY, &hit, &normal, GroundType_Test2, 100.0f);
	if (player.InStage3Area)rayHit += RayHitGround(cellingY, &hit, &normal, GroundType_Test3, 100.0f);
	if (rayHit) {
		//if (normal.y < 0.0f) {
			player.TouchedCeiling = true;
			if (player.speed.y > 0.0f && hit.y > player.pos.y) {
				player.speed.y = 0.0f;
			}
		//}
	}
	//}

	
	//==========OBJ in PlatCircleRange => OBJInRange = {inRangeOBJ1, inRangeOBJ2 .... OBJNum}
	int OBJNum = 0;
	for (int i = 0; i < MAX_COLLIDER3D; i++) {
		if (Collider[i].tag == collider3DTag_PlatCircleRange) {
			if (CheckHitByID(player.CircleRangeColliderIdx, i)) {
				OBJNum++;
			}
		}
	}
	int *OBJInRange = NULL; 
	if (OBJNum > 0) OBJInRange = new int[OBJNum];
	int OBJPutCnt = 0;
	for (int i = 0; i < MAX_COLLIDER3D; i++) {
		if (Collider[i].tag == collider3DTag_PlatCircleRange) {
			if (CheckHitByID(player.CircleRangeColliderIdx, i)) {
				OBJInRange[OBJPutCnt] = i;
				OBJPutCnt++;
			}
		}
	}
	PrintDebugProc("~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	PrintDebugProc("OBJPutCnt: %d \n", OBJPutCnt);
	for (int i = 0; i < OBJPutCnt; i++) {
		PrintDebugProc("Number :%d OBJInRangeID: %d  MasterID: %d\n", i, OBJInRange[i], Collider[OBJInRange[i]].masterID);
	}

	PrintDebugProc("~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	//==========Slope plat ground
	SetPositionCollider3D(player.bodyColliderIdx, player.pos, player.rot);
	SetPositionCollider3D(player.groundCheckColliderIdx, D3DXVECTOR3(player.pos.x, player.pos.y - PLAYER_GROUNDCHECKC_Y - PLAYER_BODYCHECKC_XZ * 0.5f, player.pos.z));
	SetPositionCollider3D(player.CircleRangeColliderIdx, player.pos);
	D3DXVECTOR3 cantactPVec;
	for (int i = 0; i < OBJNum; i++) {
		int targetCID = OBJInRange[i];
		if (CheckHitByID(player.CircleRangeColliderIdx, targetCID)) {
			
			if (player.speed.y <= 0.0f) {
				if (RayHitPlat(player.pos, targetCID, &hit, &normal)) {
					bool slopeANS = false;
					if (player.isOnSlope && player.pos.y - hit.y <= PLAYER_GROUND_OFFSET_Y * 1.5f)slopeANS = true;
					if (player.pos.y - hit.y <= PLAYER_GROUND_OFFSET_Y * 1.2f || slopeANS) {
						if (normal.y > WALL_SLOPE_DEGREE) {
							PrintDebugProc("///////////////////////////////\n");
							PrintDebugProc("hit Plat ID: %d\n", targetCID);
							player.isGround = true;
							player.pos = hit;
							player.pos.y += PLAYER_GROUND_OFFSET_Y;
							// SLOPE
							float slope = dotProduct(&player.dirXZ, &normal);
							if (slope >= VALUE_SLOPE_ADDSPD_DEGREE) {
								player.isOnSlope = true;
								g_Player.maxMoveSpeed = PLAYER_MAX_SLOPEUP_MOVE_SPEED;
							}
							else if (slope <= -VALUE_SLOPE_ADDSPD_DEGREE) {
								player.isOnSlope = true;
								if (g_Player.maxMoveSpeed > PLAYER_MAX_MOVE_SPEED - ((PLAYER_MAX_MOVE_SPEED - PLAYER_MAX_SLOPEDOWN_MOVE_SPEED) * (-slope / 0.5f))) {
									g_Player.maxMoveSpeed -= backToMAXMoveSpeed;
								}
								else {
									g_Player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED - ((PLAYER_MAX_MOVE_SPEED - PLAYER_MAX_SLOPEDOWN_MOVE_SPEED) * (-slope / 0.5f));
								}
							}
							else {
								player.isOnSlope = false;
							}
							//===================================ICE
							if (plat[Collider[targetCID].masterID].platformType == PlatformType_ice) {
								player.isInIceStatus = true;
							}
							else {
								player.isInIceStatus = false;
							}
						}
						else if (normal.y <= WALL_SLOPE_DEGREE && normal.y > 0.0f) { // slide
							PrintDebugProc("///////////////////////////////\n");
							PrintDebugProc("hit Plat ID: %d\n", targetCID);
							PrintDebugProc("///////Slide///////////\n");
							player.pos = hit;
							player.pos.y += PLAYER_GROUND_OFFSET_Y;
							D3DXVECTOR3 HITtoP = hit - player.pos; // push out
							float dis = fabsf(D3DXVec3Dot(&HITtoP, &normal));
							for (int j = 0; j <= 100; j++) {
								if (dis <= 50.0f) { // distance from wall : 100

									player.pos += normal * 0.1f;
									player.dashTimer = 0;
									player.isDash = false;
									player.isFloat = false;

									HITtoP = hit - player.pos;
									dis = fabsf(D3DXVec3Dot(&HITtoP, &normal));
									//player.speed = player.pos - player.prePos;
								}
								else {
									break;
								}
							}
						}
					}
				}
			}
			
			//if (player.speed.y > 0.0f) { // 天井
			D3DXVECTOR3 cellingY = player.pos;
			cellingY.y += PLAYER_GROUNDCHECKC_Y;
			if (RayHitPlat(cellingY, targetCID, &hit, &normal, 100.0f)) {
				if (normal.y < 0.0f) {
					player.TouchedCeiling = true;
					if (player.speed.y > 0.0f && hit.y > player.pos.y) {
						player.speed.y = 0.0f;
					}
				}

			}
		}
		

	}
	
	//========= MovingPlat
	MovingPlatPhysicsUpdate(player, OBJNum, OBJInRange);

	if (player.speed.y > 0.0f) player.isGround = false;

	player.isGroundPrev = player.isGround;

	if (player.isGround) {
		player.speed.y = 0.0f;
		player.Exit_Moving_Plat = false;
	}

	//==========Update Speed
	player.pos += player.speed;

	// Push out wall
	// 推出
	rayHit = false;

	D3DXVECTOR3 dirXZ;
	if (player.speed.x == 0.0f && player.speed.z == 0.0f) dirXZ =  player.dirXZ;
	else  dirXZ = player.speed;
	if (player.InStage1Area)rayHit += RayHitGroundWall(player.pos, dirXZ, &hit, &normal, GroundType_Test);
	if (player.InStage2Area)rayHit += RayHitGroundWall(player.pos, dirXZ, &hit, &normal, GroundType_Test2);
	if (player.InStage3Area)rayHit += RayHitGroundWall(player.pos, dirXZ, &hit, &normal, GroundType_Test3);
	//if(player.InHeartWorldArea) rayHit += RayHitGroundWall(player.pos , dirXZ, &hit, &normal, GroundType_HeartWorld);
	//if(player.InHtoWArea)       rayHit += RayHitGroundWall(player.pos , dirXZ, &hit, &normal, GroundType_HtoW);
	//if(player.InWonderLandArea) rayHit += RayHitGroundWall(player.pos , dirXZ, &hit, &normal, GroundType_Wonderland);
	//if(player.InWonderLand2Area)rayHit += RayHitGroundWall(player.pos , dirXZ, &hit, &normal, GroundType_Wonderland2);
	//if(player.InWtoSArea)       rayHit += RayHitGroundWall(player.pos , dirXZ, &hit, &normal, GroundType_WtoS);
	//if(player.InSandWorldArea)  rayHit += RayHitGroundWall(player.pos , dirXZ, &hit, &normal, GroundType_SandWorld);
	//if(player.InStoSArea)       rayHit += RayHitGroundWall(player.pos , dirXZ, &hit, &normal, GroundType_StoS);
	//if(player.InSnowWorldArea)  rayHit += RayHitGroundWall(player.pos , dirXZ, &hit, &normal, GroundType_SnowWorld);

	player.WallPushVec1 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	player.WallPushVec2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 Vec0 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	if (rayHit) {
		if (fabsf(normal.y) <= WALL_SLOPE_DEGREE) {
			D3DXVECTOR3 HITtoP = hit - player.pos;
			float dis = fabsf(D3DXVec3Dot(&HITtoP, &normal));
			for (int i = 0; i <= 100; i++) {
				if (dis <= 50.0f) { // distance from wall : 100
					float dir = D3DXVec3Dot(&player.speed, &normal);

					if (dir > 0.0f) player.pos -= normal * 0.1f * i;
					else player.pos += normal * 0.1f * i;

					player.dashTimer = 0;
					player.isDash = false;
					player.isFloat = false;

					HITtoP = hit - player.pos;
					dis = fabsf(D3DXVec3Dot(&HITtoP, &normal));
					player.WallPushVec1 = HITtoP;
				}
				else {
					break;
				}
			}
		}
	}

	// Push out plat
	// 推出
	for (int i = 0; i < OBJNum; i++) {
		int targetCID = OBJInRange[i];
		if (CheckHitByID(player.CircleRangeColliderIdx, targetCID)) {
			dirXZ = Collider[targetCID].pos - player.pos;
			if (RayHitPlatWall(player.pos, targetCID, dirXZ, &hit, &normal)) {
				if (fabsf(normal.y) <= WALL_SLOPE_DEGREE) {
					D3DXVECTOR3 HITtoP = hit - player.pos;
					float dis = fabsf(D3DXVec3Dot(&HITtoP, &normal));
					bool groundWallDetectedSwitch = false;
					for (int j = 0; j <= 100; j++) {
						if (dis <= 50.0f) { // distance from wall : 100
							player.pos += normal * 0.1f * j;
							if (player.On_Moving_Plat) {
								int targetBoxCID = plat[Collider[targetCID].masterID].colliderIdx;
								player.On_Moving_Plat_Pos += normal * 0.1f * j;
							}
							player.dashTimer = 0;
							player.isDash = false;
							player.isFloat = false;

							HITtoP = hit - player.pos;
							dis = fabsf(D3DXVec3Dot(&HITtoP, &normal));

							if (player.WallPushVec1 == Vec0) { player.WallPushVec1 = HITtoP; groundWallDetectedSwitch = true; }
							if (groundWallDetectedSwitch == false && player.WallPushVec1 != Vec0) player.WallPushVec2 = HITtoP;
						}
						else {
							break;
						}
					}
				}
			}
		}
	}
	player.prePos = player.pos;
	
	//==========Anim
	D3DXVECTOR3 AnimspeedXZ = D3DXVECTOR3(player.speed.x, 0.0f, player.speed.z);
	player.speedXZLengh = D3DXVec3Length(&AnimspeedXZ);
	////////////////////////////////////////////////////////////////////////////
	if(OBJInRange) { delete[] OBJInRange; OBJInRange = NULL; }

	//=========== other
	if (player.TouchedCeiling && player.isGround) {
		// dead
		player.touchingCeilingTimer += 1;
	}
	else {
		player.touchingCeilingTimer = 0;
	}
	if (player.touchingCeilingTimer >= 10) {
		playerDead();
	}

	float wallDot = D3DXVec3Dot(&player.WallPushVec1, &player.WallPushVec2);
	if (wallDot < 0.0f ) playerDead();

	if (!player.isGround) {
		player.InAirTime += 1;
	}
	else {
		player.InAirTime = 0;
	}
	if (player.Anim_GetStar) player.InAirTime = 0;

	if (player.InAirTime >= PLAYER_INAIR_TO_DEADTIME) {
		playerDead();
	}
}

void CollisionUpdate(PLAYER & player)
{
	//========collider position
	SetPositionCollider3D(player.bodyColliderIdx, player.pos, player.rot);
	SetPositionCollider3D(player.groundCheckColliderIdx, D3DXVECTOR3(player.pos.x, player.pos.y - PLAYER_GROUNDCHECKC_Y - PLAYER_BODYCHECKC_XZ * 0.5f, player.pos.z));
	SetPositionCollider3D(player.CircleRangeColliderIdx, player.pos);

	//========collision
	COLLIDER3D *collider = GetCollider();
	//===enemy
	// 一次性
	showSign(-1);

	ENEMY *Enemy = GetEnemy();
	int targetID;
	if (CheckHitByTagReturnMasterID(player.bodyColliderIdx, collider3DTag_enemy, &targetID)) {
		if (Enemy[targetID].enemyType == EnemyType_gem) {
			CurPlayerRebornPos = 1;
			resetGround();
			g_Player.pos = playerRebornPos[CurPlayerRebornPos];
		}
		if(Enemy[targetID].enemyType == EnemyType_star) {
			
			ReleaseEnemy(targetID);
			g_Player.Anim_GetStar = true;
			g_Player.isInBobStatus = false;
			PlaySound(SOUND_LABEL_SE_getStar);
			AddScore(1);
			if (Enemy[targetID].CanChase) { // ending
				getFinalStar = true;
			}
		}
		if (Enemy[targetID].enemyType == EnemyType_coin) {
			ReleaseEnemy(targetID);
			PlaySound(SOUND_LABEL_SE_start);
			AddCoin(1);
		}
		if (Enemy[targetID].enemyType == EnemyType_quickSand) {
			g_Player.isInSandStatus = true;
			g_Player.isInBobStatus = false;
			PlaySound(SOUND_LABEL_SE_aqua);
			player.gravityScale = VALUE_GRAVITY;
			player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
		}
		if (Enemy[targetID].enemyType == EnemyType_mushroomSpring) {
			ChangeForceY(PLAYER_MUSH_JUMP_SPEED);
			g_Player.Anim_isStepOnMushroomSpring = true;
			player.canThrowCard = true;
			player.canDash = true;
			Enemy[targetID].Status = EnemyStatus_Hit;
			PlaySound(SOUND_LABEL_SE_hit);
			PlaySound(SOUND_LABEL_SE_jump);
			SetParticle(D3DXVECTOR3(player.pos.x, player.pos.y - PLAYER_GROUND_OFFSET_Y, player.pos.z), g_Player.dirXZ, Particletype_BumpSoftThings, 25);
		}
		if (Enemy[targetID].enemyType == EnemyType_mushroom) {
			g_Player.isInBobStatus = false;
			player.gravityScale = VALUE_GRAVITY;
			player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
			ChangeForceY(PLAYER_SAND_JUMP_SPEED);
			player.speed.x = -player.dirXZ.x * player.dashSpeed;
			player.speed.z = -player.dirXZ.z * player.dashSpeed;
			PlaySound(SOUND_LABEL_SE_hit2);
		}

		if (Enemy[targetID].enemyType == EnemyType_Sign) {
			//int min = D3DXVec3LengthSq(&(player.pos - playerRebornPos[0]));
			//int ans = 0;
			//for (int i = 0; i < Max_Birth_Point; i++) {
			//	float c = D3DXVec3LengthSq(&(player.pos - playerRebornPos[i]));
			//	if (c <= min) {
			//		min = c;
			//		ans = i;
			//	}
			//}
			//CurPlayerRebornPos = ans;
			bool ans = true;
			for (int i = 0; i <= 12; i++) {
				if (RebornPosUsedID[i] == targetID) {
					ans = false;
				}
			}
			if (ans) {
				RebornPosUsedID[CurPlayerRebornPos] = targetID;
				if(firstRebornPos)CurPlayerRebornPos += 1;
				else firstRebornPos = true;
			}

			//overflow
			//if (CurPlayerRebornPos == 0) {
			//	if (player.pos.x <= -45000.0) CurPlayerRebornPos = 9;
			//	if (player.pos.x <= -50000.0) CurPlayerRebornPos = 10;
			//	if (player.pos.y >= 6000.0f) CurPlayerRebornPos = 11;
			//}

			//drawSign
			if (CurPlayerRebornPos == 0) { showSign(0); }
			else if (CurPlayerRebornPos == 1)showSign(6);
			else if (CurPlayerRebornPos == 2)showSign(6);
			else if (CurPlayerRebornPos == 6)showSign(6);
			else if (CurPlayerRebornPos == 11)showSign(6);
			else showSign(6); // SP
			
		}

		if (Enemy[targetID].enemyType == EnemyType_Spike) {
			//dead
			playerDead();
			
		}
		
	}
	else {
		if (player.isInSandStatus) {
			player.SandStateTimer = 3;
			player.isInSandStatus = false;
			player.gravityScale = VALUE_GRAVITY;
			player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
			player.canDash = true;
			player.canThrowCard = true;
			player.canUseCardTrigger = true;
		}
	}

	//===card
	BULLET *Bullet = GetBullet();
	if (CheckHitByTagReturnMasterID(player.bodyColliderIdx, collider3DTag_card, &targetID)) {
		if (Bullet[targetID].cardIsForward == false) {
			if (!player.isGround && !player.isDash && !player.isFloat && !player.isInBobStatus && !g_Player.isInSandStatus) {
				ChangeForceY(PLAYER_STEPCARD_JUMP_SPEED);
				PlaySound(SOUND_LABEL_SE_sword);
			}
			else if (player.isDash || player.isFloat) {
				ChangeForceY(PLAYER_STEPCARD_DASHJUMP_SPEED);
				PlaySound(SOUND_LABEL_SE_sword);
			}
			g_Player.cardInHand = true;
			g_Player.Anim_isCatchedCard = true;
			g_Player.playerAbility = Bullet[targetID].cardAbility;
			ReleaseBullet(targetID);

		}
	}
	//===ability
	if (CheckHitByTagReturnMasterID(player.bodyColliderIdx, collider3DTag_bullet, &targetID)) {
		
		if (Bullet[targetID].bulletType == BulletType_windStorm) {
			D3DXVECTOR3 dir = player.pos - Bullet[targetID].pos;
			D3DXVec3Normalize(&dir, &dir);
			player.speed.x = dir.x * PLAYER_WIND_MOVE_SPEED;
			ChangeForceY(PLAYER_WIND_JUMP_SPEED);
			player.speed.z = dir.z * PLAYER_WIND_MOVE_SPEED;
			player.maxMoveSpeed = PLAYER_WIND_MOVE_SPEED;
			if(player.speedXZLengh > PLAYER_MAX_MOVE_SPEED * 0.5f) player.Anim_isWindBoost = true;
			g_Player.backToMAXMove = false;
		}
		else if (Bullet[targetID].bulletType == BulletType_waterBob) {
			//泡泡狀態
			if (!player.isInBobStatus) {
				ChangeForceY(PLAYER_MUSH_JUMP_SPEED);
				g_Player.Anim_isStepOnMushroomSpring = true;
				//player.canThrowCard = true;
				//player.canDash = true;
				Bullet[targetID].Status = BulletStatus_Hit;
				//SetParticle(D3DXVECTOR3(player.pos.x, player.pos.y - PLAYER_GROUND_OFFSET_Y, player.pos.z), g_Player.dirXZ, Particletype_BumpSoftThings, 25);

				//ReleaseBullet(targetID);
			}
		}
		
	}
	//===fall Down Area
	if (CheckHitByTag(player.bodyColliderIdx, collider3DTag_fallDownArea)) {
		if (!GetGameSceneEditMode()) {
			GetCamera()->rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			GetCamera()->rot.x -= D3DX_PI * 0.1f;
		}
		playerDead();
	}
	//===ground Check Area
	if (CheckHitByTag(player.bodyColliderIdx, collider3DTag_GroundArea_Stage1)) g_Player.InStage1Area = true;
	else g_Player.InStage1Area = false;
	if (CheckHitByTag(player.bodyColliderIdx, collider3DTag_GroundArea_Stage2)) g_Player.InStage2Area = true;
	else g_Player.InStage2Area = false;
	if (CheckHitByTag(player.bodyColliderIdx, collider3DTag_GroundArea_Stage3)) g_Player.InStage3Area = true;
	else g_Player.InStage3Area = false;
	
}

void PlayerStatusUpdate(PLAYER & player) // & other
{
	// InAir
	if (player.isGround) {
		player.decelerate = VALUE_DECELERATE;
		if (player.isInIceStatus)player.decelerate = VALUE_ICE_DECELERATE;
	}
	else {
		player.decelerate = VALUE_AIR_DECELERATE;
	}
	if (player.Exit_Moving_Plat) {
		player.decelerate = 0.1f;
	}

	// Dash Sys
	if (player.isDash) {
		if(player.isGround)player.maxMoveSpeed = PLAYER_DASH_SPEED;
		else player.maxMoveSpeed = PLAYER_AIRDASH_SPEED;
		player.dashTimer += 1; player.isFloat = true;
		if (player.dashTimer >= PLAYER_DASHTIME) {
			player.dashTimer = 0;
			player.isDash = false;
			player.isFloat = true;
		}
	}

	if (player.isFloat) {
		if(!player.isGround)player.maxMoveSpeed = PLAYER_AIRDASH_SPEED;
		else { 
			g_Player.backToMAXMove = true;
			player.isFloat = false;
		}
	}

	//脫離平台後掉落地面重置
	if (player.isGround) {
		g_Player.backToMAXMove = true;
		if(!g_Player.Anim_isStepOnMushroomSpring) g_Player.Anim_isJumping = false;
	}
	else {
		player.isOnSlope = false;
	}
	player.On_Moving_PlatPre = player.On_Moving_Plat;
	if (g_Player.backToMAXMove && !player.isOnSlope && player.isGround) {
		player.maxMoveSpeed -= backToMAXMoveSpeed;
		if (player.maxMoveSpeed <= PLAYER_MAX_MOVE_SPEED || player.speedXZLengh <= PLAYER_MAX_MOVE_SPEED) {
			player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
			g_Player.backToMAXMove = false;
		}
	}

	// Dash CD
	if (!player.canDash) {
		player.dashCDTimer += 1;
		if (player.dashCDTimer >= PLAYER_DASHCDTIME) {
			if (player.isGround) {
				player.dashCDTimer = 0;
				player.canDash = true;
			}
		}
	}

	//Throwing
	if (player.isThrowing) {
		player.gravityScale = VALUE_THROWINGGRAVITY;
		player.throwingTimer += 1;
		if (player.throwingTimer >= PLAYER_THROWING_TIME) {
			player.throwingTimer = 0;
			player.isThrowing = false;
			player.gravityScale = VALUE_GRAVITY;
		}
	}
	if (player.isGround)player.canThrowCard = true; //掉落地面重置

	//UseCardCD
	if (player.canUseCard == false) {
		player.canUseCardTimer += 1;
		if (player.isGround) {
			if(!player.isInBobStatus)player.canUseCardTrigger = true;
		}
		if (player.canUseCardTimer >= PLAYER_USEABILITYCD) {
			if (player.canUseCardTrigger) {
				player.canUseCardTimer = 0;
				player.canUseCard = true;
				player.canUseCardTrigger = false;
			}
		}
	}

	//Card Ability Effect
	if (player.cardInHand && player.playerAbility == CardAbility_water) {
		waterAbilityTimer += 1;
		if (waterAbilityTimer >= PLAYER_WATERABILITYCD) {
			waterAbilityTimer = 0;
			SetParticle(player.pos, D3DXVECTOR3(0.0f, 1.0f, 0.0f), Particletype_WaterAbilityEffect, 3);
		}
	}
	else {
		waterAbilityTimer = 0;
	}

	// BobState
	if (player.isInBobStatus == true) {
		player.bobStateTimer += 1;
		player.gravityScale = VALUE_BOB_GRAVITY;
		player.maxMoveSpeed = PLAYER_MAX_BOB_MOVE_SPEED;
		if (player.bobStateTimer >= PLAYER_BOBSTATETIME) {
			player.bobStateTimer = 0;
			player.isInBobStatus = false;
			PlaySound(SOUND_LABEL_SE_aqua);
			player.gravityScale = VALUE_GRAVITY;
			player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
		}
	}

	//canCreateMoveSmoke
	if (player.speedXZLengh > 0.0f && player.isGround) {
		int movecdTime = 35.5f - (player.speedXZLengh / player.maxMoveSpeed) * 24.0f;
		player.moveSmokeTimer += 1;
		if (player.moveSmokeTimer >= movecdTime) {
			COLLIDER3D *Collider = GetCollider();
			player.moveSmokeTimer = 0;
			SetParticle(D3DXVECTOR3(g_Player.pos.x, g_Player.pos.y - Collider[g_Player.bodyColliderIdx].hy * 1.0f, g_Player.pos.z), D3DXVECTOR3(0.0f, 1.0f, 0.0f), Particletype_Smoke, 2);
			if (player.InWonderLandArea || player.InWonderLand2Area) {
				if(g_canCreateSPParticle)SetParticle(D3DXVECTOR3(g_Player.pos.x, g_Player.pos.y - Collider[g_Player.bodyColliderIdx].hy * 1.0f, g_Player.pos.z), D3DXVECTOR3(0.0f, 1.0f, 0.0f), Particletype_WalkOnGrass, 2);
			}
			if (player.InSandWorldArea) {
				if (g_canCreateSPParticle)SetParticle(D3DXVECTOR3(g_Player.pos.x, g_Player.pos.y - Collider[g_Player.bodyColliderIdx].hy * 1.0f, g_Player.pos.z), D3DXVECTOR3(0.0f, 1.0f, 0.0f), Particletype_WalkOnSand, 6);
			}
		}
		//else player.moveSmokeTimer = 0;
	}

	//InSand
	if (player.isInSandStatus == true) {
		if (g_Player.speed.y <= 0.0f) {
			g_Player.speed.y = 0.0f;
			player.gravityScale = VALUE_SNADGRAVITY;
		}
		else {
			player.gravityScale = VALUE_SNADJUMP_GRAVITY;
		}
		player.SandStateTimer += 1;
		player.maxMoveSpeed = PLAYER_MAX_SAND_MOVE_SPEED;
		if (player.SandStateTimer >= PLAYER_SANDSTATETIME && player.isGround == false) {
			player.SandStateTimer = 3;
			player.isInSandStatus = false;
			player.gravityScale = VALUE_GRAVITY;
			player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
		}
	}
	else {
		if (player.SandStateTimer > 0)player.SandStateTimer -= 1;
	}
}

void AnimStatusUpdate(PLAYER & player)
{
	if (GetAnimPlayType() != APT_CBR) { // normal
		if (player.speed.x != 0.0f || player.speed.z != 0.0f) {
			player.Anim_Status = AnimStatus_Move;
		}
		else {
			player.Anim_Status = AnimStatus_Idel;
			//player.Anim_Status = AnimStatus_Move;
		}
		if (player.backToMAXMove && player.speedXZLengh > PLAYER_MAX_MOVE_SPEED) {
			player.Anim_Status = AnimStatus_FastMove;
		}
		if (!player.isGround && player.Anim_isJumping) {
			player.Anim_Status = AnimStatus_Jump;
		}
		if (!player.isGround && player.speedXZLengh > PLAYER_MAX_MOVE_SPEED) {
			player.Anim_Status = AnimStatus_BigJump;
		}
		else if (!player.isGround && !player.Anim_isJumping) {
			player.Anim_Status = AnimStatus_Air;
		}
		//WaterBobStatus
		if (player.isInBobStatus) {
			player.Anim_Status = AnimStatus_BobStatus;
		}
		//sand
		if (player.SandStateTimer > 0) {
			if (player.speed.x != 0.0f || player.speed.z != 0.0f) {
				player.Anim_Status = AnimStatus_Move;
			}
			else {
				player.Anim_Status = AnimStatus_Idel;
			}
		}
	}
	// CBR
	if (player.isDash) {
		player.Anim_Status = AnimStatus_Dash;
	}
	if (player.Anim_isWindBoost) {
		player.Anim_Status = AnimStatus_Dash;
		player.Anim_isWindBoost = false;
	}
	if (player.isThrowing) {
		player.Anim_Status = AnimStatus_ThrowCard;
	}
	if (player.Anim_isCatchedCard) {
		if(!player.isGround)player.Anim_Status = AnimStatus_CatchCardJump;
		player.Anim_isCatchedCard = false;
	}
	if (player.Anim_isStepOnMushroomSpring) {
		player.Anim_Status = AnimStatus_CatchCardJump;
		player.Anim_isStepOnMushroomSpring = false;
	}
	if (player.Anim_GetStar) {
		player.Anim_Status = AnimStatus_GetStar;
		player.maxMoveSpeed = 0.0f;
		player.gravityScale = 0.0f;
		player.speed = D3DXVECTOR3(0.0f,0.0f,0.0f);

		//face to camera
		CAMERA *cam = GetCamera();
		D3DXVECTOR3 cdir = g_Player.pos - GetCamera()->pos;
		cdir = D3DXVECTOR3{ cdir.x, 0.0f, cdir.z }; // don't want to fly (move according to dir.y)
		D3DXVec3Normalize(&cdir, &cdir);
		g_Player.dirXZ = -cdir;

		g_getStarAnimTimer += 1;
		if (g_getStarAnimTimer <= 60) {
			SetParticle(player.pos, D3DXVECTOR3(1.0f, 0.0f, 1.0f) , Particletype_CardMove, 2);
		}
		else if (g_getStarAnimTimer == 80) {
			SetParticle(player.pos, g_Player.dirXZ, Particletype_BumpSoftThings, 45);
		}

		if (GetKeyboardTrigger(DIK_SPACE) || IsMouseLeftTriggered()) {
			if (!getFinalStar) {
				player.Anim_Status = AnimStatus_Air;
				player.Anim_GetStar = false;
				player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
				player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
				player.gravityScale = VALUE_GRAVITY;
				g_getStarAnimTimer = 0.0f;
			}
			else {
				SetFade(FADE_OUT, MODE_RESULT);
			}
		}
	}
}

void ChangeForceY(float force)
{
	g_Player.speed.y = force;
	
	g_Player.On_Moving_Plat = false;
	g_Player.Anim_isJumping = true;
}

void MovingPlatPhysicsUpdate(PLAYER &player, int OBJNum, int *OBJInRange)
{
	COLLIDER3D *Collider = GetCollider();
	PLATFORM *plat = GetPlatform();
	//==============plat
	SetPositionCollider3D(player.bodyColliderIdx, player.pos, player.rot);
	SetPositionCollider3D(player.groundCheckColliderIdx, D3DXVECTOR3(player.pos.x, player.pos.y - PLAYER_GROUNDCHECKC_Y - PLAYER_BODYCHECKC_XZ * 0.5f, player.pos.z));
	SetPositionCollider3D(player.CircleRangeColliderIdx, player.pos);
	int ansCnt = 0; // how many collision detected
	for (int i = 0; i < OBJNum; i++) { // iterate every posible collision
		int targetCID = plat[Collider[OBJInRange[i]].masterID].colliderIdx;
		int platID = Collider[targetCID].masterID;
		if (CheckHitByID(player.groundCheckColliderIdx, targetCID)) { // collision detection
			if (player.speed.y <= 0.0f && player.pos.y + Collider[player.bodyColliderIdx].hy*0.5f >= (Collider[targetCID].pos.y + Collider[targetCID].hy*0.5f)) { // player's head higher than target's head
				if (plat[platID].canMove) {
					player.pos.y = PLAYER_GROUND_OFFSET_Y + Collider[targetCID].pos.y + Collider[targetCID].hy * 0.5f;
					ansCnt++;
					//====== NOT On moving platform
					if (player.On_Moving_Plat == false) {
						if (plat[platID].canMove) {
							player.Moving_Plat_PrePos = Collider[targetCID].pos; // put the first Moving_Plat_PrePos in
							player.On_Moving_Plat_Pos.x = player.pos.x - Collider[targetCID].pos.x;
							player.On_Moving_Plat_Pos.z = player.pos.z - Collider[targetCID].pos.z;
							player.On_Moving_Plat = true;
							player.On_Moving_PlatID = targetCID;
							if (player.Exit_Moving_Plat) {
								player.speed.x = 0.0f;
								player.speed.z = 0.0f;
							}
						}
					}
					//====== On moving platform
					if (player.On_Moving_Plat == true) {
						player.Moving_Plat_Speed = Collider[targetCID].pos - player.Moving_Plat_PrePos;
					}
					//===================================
					if (plat[platID].canMove) {
						player.Moving_Plat_PrePos = Collider[targetCID].pos;
						player.pos.x = Collider[targetCID].pos.x + player.On_Moving_Plat_Pos.x;
						player.pos.z = Collider[targetCID].pos.z + player.On_Moving_Plat_Pos.z;
						player.On_Moving_Plat_Pos.x += player.speed.x;
						player.On_Moving_Plat_Pos.z += player.speed.z;
					}
					//===================================ICE
					if (plat[platID].platformType == PlatformType_ice) {
						player.isInIceStatus = true;
					}
					else {
						player.isInIceStatus = false;
					}
					player.isGround = true;
				}
			}

		}
	}
	if(ansCnt == 0) player.On_Moving_Plat = false;

	//プラットフォームから出る瞬間
	if (player.On_Moving_PlatPre == true && player.On_Moving_Plat == false) {
		player.speed.x += player.Moving_Plat_Speed.x;
		player.speed.y += player.Moving_Plat_Speed.y;
		player.speed.z += player.Moving_Plat_Speed.z;

		temp_player_leave_plat_speed = player.Moving_Plat_Speed;

		player.maxMoveSpeed += D3DXVec3Length(&player.Moving_Plat_Speed); // 暫時放大最高速度 => 在平台上跳躍使speed.xz正常變動
		g_Player.backToMAXMove = false;
		player.Moving_Plat_PrePos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		if (player.Moving_Plat_Speed.x != 0.0f || player.Moving_Plat_Speed.z != 0.0f)player.Exit_Moving_Plat = true;
	}

}

void setPlayerPosition(D3DXVECTOR3 pos) {
	g_Player.pos = pos;
}

void playerDead() {
	g_Player.pos = playerRebornPos[CurPlayerRebornPos];
	g_Player.speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Player.InAirTime = 0;
	resetGround();
	AddMiss(1);
}

void setwrm(bool b)
{
	g_wrm = b;
}
