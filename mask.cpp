//=============================================================================
//
// �}�X�N���� [mask.cpp]
// Author : GP11B132 14 �S�V���E�P��
//
//=============================================================================
#include "mask.h"
#include "Collider3D.h"
#include "player.h"
#include "input.h"
#include "debugproc.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_MODEL		(1)					            // ���f���̐�
#define	MODEL_GROUND_TEST       	"data/MODEL/cylindrical_mask.obj"		            

#define SCALE_X         (10)
#define SCALE_Y         (10)
#define SCALE_Z         (-10)
#define Move_X          (0)
#define Move_Y          (0)
#define Move_Z          (0)
//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static DX11_MODEL			g_Model[MAX_MODEL];						// ���f�����

HRESULT InitMask(void)
{
	LoadModel(MODEL_GROUND_TEST, &g_Model[0]);

	return S_OK;
}

void UninitMask(void)
{
	// ���f���̉������
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

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);
	//SetAlphaTestEnable(true);
	SetDepthEnable(false);
	SetLightEnable(false);
	//=================Ground
	
	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&mtxWorld);

	// �X�P�[���𔽉f
	D3DXMatrixScaling(&mtxScl, SCALE_X, SCALE_Y, SCALE_Z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScl);

	// ��]�𔽉f
	//D3DXMatrixRotationYawPitchRoll(&mtxRot, worldRot.x, worldRot.y, worldRot.z);
	//D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// �ړ��𔽉f
	//D3DXMatrixTranslation(&mtxTranslate, Move_X, Move_Y, Move_Z);
	//D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTranslate);

	// ���[���h�}�g���b�N�X�̐ݒ�
	SetWorldMatrix(&mtxWorld);

	// ���f���`��
	DrawModel(&g_Model[0]);

	SetFogEnable(true);
	SetLightEnable(true);
	//=================
	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
	//SetAlphaTestEnable(false);
	SetDepthEnable(true);

}


