//=============================================================================
//
// プレーヤ処理 [player.h]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#pragma once

#include "model.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_PLAYER		(1)					// プレイヤーの数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct PLAYER
{
	bool              Use;
	bool              isGround;
	bool              isGroundPrev;
	bool              canDash;
	bool              isDash;
	bool              isFloat;                // Floating after Dash
	bool              backToMAXMove;          // Slowly back to normal MaxSpeed
	bool              cardInHand;             // カードを持っています
	bool              canThrowCard;           // カードを投げるか
	bool              isThrowing;             // Set speed while Throwing
	bool              canUseCard;
	bool              canUseCardTrigger;      // if touch ground : triggered 
	bool              isInBobStatus;          // 泡の能力
	bool              isInSandStatus;         // 砂の状態
	bool 			  isInIceStatus;		  // 氷の状態
	bool			  isOnSlope;
	bool              On_Moving_Plat;
	bool              On_Moving_PlatPre;
	bool              Exit_Moving_Plat;
	bool              TouchedCeiling;

	//Anim
	bool              Anim_isJumping;
	bool              Anim_isWindBoost;
	bool              Anim_isCatchedCard;
	bool              Anim_isStepOnMushroomSpring;
	bool              Anim_GetStar;

	//Collision detect Area
	bool              InStage1Area;
	bool              InStage2Area;
	bool              InStage3Area;
	bool              InWonderLandArea;
	bool              InWonderLand2Area;
	bool              InWtoSArea;
	bool              InSandWorldArea;
	bool              InStoSArea;
	bool              InSnowWorldArea;

	int               On_Moving_PlatID;
	int               dashCDTimer;
	int               dashTimer;
	int               cardBulletID;           // -1 represent card in hand
	int               throwingTimer;
	int               playerAbility;          // same in bullet.h => enumCardAbility
	int               canUseCardTimer;        // カード能力を使うCD時間
	int               bobStateTimer;
	int               SandStateTimer;
	int               Anim_Status;
	int               bodyColliderIdx;        // colliderのインデックス番号
	int               groundCheckColliderIdx; // colliderのインデックス番号
	int               CircleRangeColliderIdx; // colliderのインデックス番号
	int				  shadowIdx;			  // 影のインデックス番号
	int               moveSmokeTimer;
	int               touchingCeilingTimer;
	int               InAirTime;

	float             speedXZLengh;
	float             maxMoveSpeed;
	float             decelerate;
	float             gravityScale;
	float             jumpSpeed;
	float             dashSpeed;

	D3DXMATRIX		  mtxWorld;			      // ワールドマトリックス
					  					      
	D3DXVECTOR3		  pos;				      // モデルの位置
	D3DXVECTOR3		  rot;				      // モデルの向き(回転)
	D3DXVECTOR3		  scl;				      // モデルの大きさ(スケール)
	D3DXVECTOR3		  prePos;				 								      
	D3DXVECTOR3       speed;                  // 移動スピード
	D3DXVECTOR3       dirXZ;                  // 向き      
	D3DXVECTOR3       dashDir;
	D3DXVECTOR3       WallPushVec1;
	D3DXVECTOR3       WallPushVec2;
	
	D3DXVECTOR3       On_Moving_Plat_Pos;
	D3DXVECTOR3       Moving_Plat_PrePos;
	D3DXVECTOR3       Moving_Plat_Speed;

	D3DXVECTOR3       wrm_Pos;
	
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);

PLAYER *GetPlayer(void);

void setPlayerPosition(D3DXVECTOR3 pos);
void playerDead();

void setwrm(bool b);
