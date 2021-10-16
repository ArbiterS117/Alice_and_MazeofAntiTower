﻿/*******************************************************************************
* タイトル:		ファイルヘッダー
* プログラム名:	saveFile.h
* 作成者:		GP11B132 14 ゴショウケン
* 作成日:		2020/08/21
*******************************************************************************/
#ifndef FILE_H
#define FILE_H

/*******************************************************************************
* インクルードファイル
*******************************************************************************/
#include "main.h"
#include "player.h"
#include "enemy.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/



/*******************************************************************************
* 構造体定義
*******************************************************************************/
struct	SAVEDATA				// セーブデータの構造体
{
	// プレイヤーデータを保存
	PLAYER player[MAX_PLAYER];	// プレイヤーのMAX人数分セーブする

	// エネミーデータを保存
	ENEMY   enemy[MAX_ENEMY];	// エネミーのMAX人数分セーブする

	// 何かセーブしたい物があればここに追加してみよう


};



/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/
void SaveData(void);
void LoadData(void);


#endif
