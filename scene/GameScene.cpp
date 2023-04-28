#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include"MathUtilityForText.h"
#include<time.h>

//コンストラクタ
GameScene::GameScene() {}
//デストラクタ
GameScene::~GameScene() { delete spriteBG_;
	delete modelStage_;
	delete modelplayer_;
	delete modelBeam_;
	delete modelEnemy_;
}
//初期化
void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	//ビュープロジェクションの初期化
	viewProjection_.translation_.y = 1;
	viewProjection_.translation_.z = -6;
	viewProjection_.Initialize();

	textureHandleBG_ = TextureManager::Load("bg.jpg");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0, 0});
	textureHandleStage_ = TextureManager::Load("stage.jpg");
	modelStage_ = Model::Create();
	worldTransformStage_.Initialize();
	//ステージの位置を変更
	worldTransformStage_.translation_ = {0, -1.5f, 0};
	worldTransformStage_.scale_ = {4.5f, 1, 40};
	//変換行列を更新
	worldTransformStage_.matWorld_ = MakeAffineMatrix(
	    worldTransformStage_.scale_, worldTransformStage_.rotation_,
	    worldTransformStage_.translation_);
	//変換行列を定数バッファに転送
	worldTransformStage_.TransferMatrix();
	//プレイヤー
	textureHandleplayer_ = TextureManager::Load("player.png");
	modelplayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();
	//ビーム
	textureHandleBeam_ = TextureManager::Load("beam.png");
	modelBeam_ = Model::Create();
	worldTransformBeam_.scale_ = {0.3f, 0.3f, 0.3f};
	worldTransformBeam_.Initialize();
	//敵
	textureHandleEnemy_ = TextureManager::Load("enemy.png");
	modelEnemy_ = Model::Create();
	worldTransformEnemy_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformEnemy_.Initialize();
	srand((unsigned int)time(NULL));
	debugText_ = DebugText::GetInstance();
	debugText_->Initialize();
}
//更新
void GameScene::Update() {
	PlayerUpdate();
	BeamUpdate();
	EnemyUpdate();
	Collision();
	
}
//描画
void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);
	

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	spriteBG_->Draw();
	char str[100];
	sprintf_s(str, "SCORE %d", gameScore_);
	debugText_->Print(str, 200, 10, 2);
	char life[10];
	sprintf_s(life, "LIFE %d", playerLife);
	debugText_->Print(life, 640, 10, 2);

	debugText_->DrawAll();
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	modelStage_->Draw(worldTransformStage_, viewProjection_, textureHandleStage_);
	modelplayer_->Draw(worldTransformPlayer_, viewProjection_, textureHandleplayer_);
	if (beamFlag_) {
		modelBeam_->Draw(worldTransformBeam_, viewProjection_, textureHandleBeam_);
	}
	if (enemyFlag_) {
		modelEnemy_->Draw(worldTransformEnemy_, viewProjection_, textureHandleEnemy_);
	}
	/// </summary>

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
//---------------------------------------------------------------------------------------
//プレイヤー
//---------------------------------------------------------------------------------------

//プレイヤー更新
void GameScene::PlayerUpdate() {
	//変換行列を更新
	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(
		worldTransformPlayer_.scale_,
		worldTransformPlayer_.rotation_,
		worldTransformPlayer_.translation_
	);
	//変換行列を定数バッファに転送
	worldTransformPlayer_.TransferMatrix();
	//移動

	//右へ移動
	if (input_->PushKey(DIK_RIGHT)&&
		worldTransformPlayer_.translation_.x<4) {
		worldTransformPlayer_.translation_.x += 0.1f;
	}
	if (input_->PushKey(DIK_LEFT)&& 
		worldTransformPlayer_.translation_.x > -4) {
		worldTransformPlayer_.translation_.x -= 0.1f;
	}

}
//---------------------------------------------------------------------------------------
// ビーム
//---------------------------------------------------------------------------------------

//ビーム更新
void GameScene::BeamUpdate() {
	// 変換行列を更新
	worldTransformBeam_.matWorld_ = MakeAffineMatrix(
	    worldTransformBeam_.scale_, 
		worldTransformBeam_.rotation_,
	    worldTransformBeam_.translation_);
	// 変換行列を定数バッファに転送
	worldTransformBeam_.TransferMatrix();

	if (input_->PushKey(DIK_SPACE)&&!beamFlag_) {
		beamFlag_ = true;
		Beamborn();
	}
	if (beamFlag_) {
		BeamMove();
	}
}
//ビーム移動
void GameScene::BeamMove() { 
	worldTransformBeam_.translation_.z += 0.1f;
	worldTransformBeam_.rotation_.x += 0.1f;
	if (worldTransformBeam_.translation_.z > 40) {
	beamFlag_ = false;
	}
}
void GameScene::Beamborn() {
	worldTransformBeam_.translation_ = worldTransformPlayer_.translation_;
}
//---------------------------------------------------------------------------------------
// 敵
//---------------------------------------------------------------------------------------

// 敵更新
void GameScene::EnemyUpdate() {
	// 変換行列を更新
	worldTransformEnemy_.matWorld_ = MakeAffineMatrix(
	    worldTransformEnemy_.scale_, worldTransformEnemy_.rotation_,
	    worldTransformEnemy_.translation_);
	// 変換行列を定数バッファに転送
	worldTransformEnemy_.TransferMatrix();

	if (!enemyFlag_) {
	enemyFlag_ = true;
	Enemyborn();
	}
	if (enemyFlag_) {
	EnemyMove();
	
	}
}
void GameScene::EnemyMove() { 
	worldTransformEnemy_.translation_.z -= 0.1f;
	if (worldTransformEnemy_.translation_.z < 0) {
	enemyFlag_ = false;
	}
	worldTransformEnemy_.rotation_.x -= 0.1f;
}
void GameScene::Enemyborn() {
	worldTransformEnemy_.translation_.z = 40;
	int x = rand() % 80;
	float x2 = (float)x / 10 - 4;
	worldTransformEnemy_.translation_.x = x2;
}
//---------------------------------------------------------------------------------------
// 衝突判定
//---------------------------------------------------------------------------------------

// 衝突判定
void GameScene::Collision() { 
	CollisionPlayerEnemy();
	CollisionBeamEnemy();
}
void GameScene::CollisionPlayerEnemy() { 
	if (enemyFlag_) {
	float dx = abs(worldTransformPlayer_.translation_.x - worldTransformEnemy_.translation_.x);
	float dz = abs(worldTransformPlayer_.translation_.z - worldTransformEnemy_.translation_.z);

	//衝突したら
	if (dx < 1 && dz < 1) {
		enemyFlag_ = false;
		playerLife--;
	}

	}

}void GameScene::CollisionBeamEnemy() { 
	if (enemyFlag_&&beamFlag_) {
	float dx = abs(worldTransformBeam_.translation_.x - worldTransformEnemy_.translation_.x);
	float dz = abs(worldTransformBeam_.translation_.z - worldTransformEnemy_.translation_.z);

	//衝突したら
	if (dx < 1 && dz < 1) {
		enemyFlag_ = false;
		beamFlag_ = false;
		gameScore_ += 10;
	}

	}

}