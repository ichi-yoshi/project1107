#pragma once
#include "DxLib.h"

class Reflect
{
public:
    Reflect();
    ~Reflect();

    // 初期化
    void Initialize();

    // 更新処理（コリジョン判定も含む）
    void Update(int mapHandle, int frameCollision);

    // 描画処理
    void Render();

    // 発射処理（カメラの向きと位置から）
    void Launch(VECTOR cameraPos, VECTOR cameraTarget);

    // アクティブ状態の取得
    bool IsActive() const { return _isActive; }


    // 反射処理
    void ReflectVelocity(VECTOR normal);

    // 指定位置でのコリジョン判定
    bool CheckCollisionAtPosition(VECTOR testPos, int mapHandle, int frameCollision);

    // コリジョン判定（古い形式、互換性のため残す）
    bool CheckCollisionWithMovement(VECTOR oldPos, VECTOR newPos, int mapHandle, int frameCollision);
    bool CheckCollision(int mapHandle, int frameCollision);

    void RenderTrajectory(VECTOR cameraPos, VECTOR cameraTarget);

	void BulletTypeSet(int type) { bulletType = type; }

    bool _isActive;         // アクティブ状態
    VECTOR _position;       // 現在位置
    VECTOR _velocity;       // 速度ベクトル
    VECTOR _direction;      // 移動方向
    float _speed;           // 移動速度
    int _modelHandle;       // モデルハンドル
    float _gravity;         // 重力
    int _bounceCount;       // 反射回数
    int _maxBounces;        // 最大反射回数
    float _scale;           // モデルのスケール
    float _radius;          // 球体の半径

	int bulletType;	  // 弾の種類

};