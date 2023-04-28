#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include"DebugText.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	//プレイヤー更新
	void PlayerUpdate();

	//ビーム更新
	void BeamUpdate();
	void BeamMove();
	void Beamborn();

	//敵
	void EnemyUpdate();
	void EnemyMove();
	void Enemyborn();

	//衝突判定
	void Collision();
	void CollisionPlayerEnemy();
	void CollisionBeamEnemy();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	uint32_t textureHandleBG_ = 0;
	Sprite* spriteBG_ = nullptr;
	ViewProjection viewProjection_;
	uint32_t textureHandleStage_ = 0;
	Model* modelStage_ = nullptr;
	WorldTransform worldTransformStage_;
	//プレイヤー
	uint32_t textureHandleplayer_ = 0;
	Model* modelplayer_ = nullptr;
	WorldTransform worldTransformPlayer_;
	//ビーム
	uint32_t textureHandleBeam_ = 0;
	Model* modelBeam_ = nullptr;
	WorldTransform worldTransformBeam_;
	bool beamFlag_=false;
	//敵
	uint32_t textureHandleEnemy_ = 0;
	Model* modelEnemy_ = nullptr;
	WorldTransform worldTransformEnemy_;
	bool enemyFlag_ = false;
	DebugText* debugText_ = nullptr;
	int gameScore_ = 0;
	int playerLife=3;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
