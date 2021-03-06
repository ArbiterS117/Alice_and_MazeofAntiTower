//=============================================================================
//
// カメラ処理 [camera.h]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#pragma once


//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <d3dx9.h>
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//階層アニメーション or else
#define CAMERA_ROUTE_SEGMENT		(10)

enum MOIVE_LOOKAT_TYPE
{
	onePoint,
	twoPointMove,
	twoPointLookAt,
	twoPointMoveandLookAt,
	oneObject, // D3DXVECTOR3 at => no use
	twoObject, // D3DXVECTOR3 at => no use
};

struct MOIVE_TRANSFORM
{
	float frame;    // 実行フレーム数
	D3DXVECTOR3 pos;
	D3DXVECTOR3 at;
	MOIVE_LOOKAT_TYPE type;//look at direction but not "Point" when moving
};

struct CAMERA
{
	bool                TPMode;
	bool                canMove;
	bool                MovieMode;
	//Collision detect Area
	bool 				InStage1Area;
	bool 				InStage2Area;
	bool 				InStage3Area;

	int                 TDN;            //階層アニメーション
	int                 CircleRangeColliderIdx; 

	float               maxMoveSpeed;
	float               decelerate;
	float               move_time;      //階層アニメーション
	float				len;			// カメラの視点と注視点の距離

	D3DXMATRIX			mtxView;		// ビューマトリックス
	D3DXMATRIX			mtxInvView;		// ビューマトリックス
	D3DXMATRIX			mtxProjection;	// プロジェクションマトリックス

	D3DXVECTOR3			pos;			// カメラの視点(位置)
	D3DXVECTOR3			at;				// カメラの注視点
	D3DXVECTOR3			up;				// カメラの上方向ベクトル
	D3DXVECTOR3			rot;			// カメラの回転
	D3DXVECTOR3         speed;          // 移動スピード

	MOIVE_TRANSFORM *tbl_adr;

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);
void SetCamera(void);

CAMERA *GetCamera(void);

void SetCameraWorldRotateView(bool b_goback);
D3DXVECTOR3* GetThreePowerBeizerList(D3DXVECTOR3 startPoint, D3DXVECTOR3 controlPoint1, D3DXVECTOR3 controlPoint2, D3DXVECTOR3 endPoint, int segmentNum);
D3DXVECTOR3 CalculateThreePowerBezierPoint(float t, D3DXVECTOR3 p0, D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3);

bool GetWorldRotateMode();

bool Getbezirmoveflag();
void SetWorldRotateMode(bool wrm);
void SetCameraPosRecord();
void SetCameraAtRecord();
