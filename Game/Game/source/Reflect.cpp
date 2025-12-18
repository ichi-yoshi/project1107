#include "Reflect.h"
#include "ModeGame.h"


Reflect::Reflect()
{
	_isActive = false;          // 初期状態は非アクティブ
	_position = VGet(0, 0, 0);  // 初期位置
	_velocity = VGet(0, 0, 0);  // 初期速度
	_direction = VGet(0, 0, 0); // 初期移動方向
    _speed = 15.0f;             // 発射速度を少し下げる
	_modelHandle = -1;          // 球描画を使用
    _gravity = 0.3f;            // 重力を少し弱く
	_bounceCount = 0;           // 反射回数初期化
    _maxBounces = 100;          // 最大5回まで反射
    _scale = 0.5f;              // モデルを小さく表示
    _radius = 10.0f;            // 球体の半径を小さく
	bulletType = 0;             // 弾の種類初期化
}

Reflect::~Reflect()
{
    if (_modelHandle != -1)
    {
        MV1DeleteModel(_modelHandle);
    }
}

void Reflect::Initialize()
{
    _modelHandle = -1;  // 球描画を使用
	_isActive = false;  // 非アクティブ状態
	_bounceCount = 0;   // 反射回数初期化
}



void Reflect::Update(int mapHandle, int frameCollision)
{
    if (!_isActive) return;

    // 重力を適用
    _velocity.y -= _gravity;

    // 移動前の位置を保存
    VECTOR oldPos = _position;

    // 移動量を細かく分割してコリジョン判定を行う
    VECTOR moveStep = VScale(_velocity, 0.1f); // 移動を10分割
    int steps = 10;

    for (int i = 0; i < steps; i++)
    {
        VECTOR nextPos = VAdd(_position, moveStep);

        // コリジョン判定
        if (CheckCollisionAtPosition(nextPos, mapHandle, frameCollision))
        {
            // 衝突した場合は処理を終了
            break;
        }
        else
        {
            // 衝突しなかった場合は位置を更新
            _position = nextPos;
        }
    }

    // 地面に落ちたら無効化
    if (_position.y < -1000.0f)
    {
        _isActive = false;
        return;
    }

    // 最大反射回数に達したら無効化
    if (_bounceCount >= _maxBounces)
    {
        _isActive = false;
        return;
    }
}

void Reflect::Render()
{
    if (!_isActive) return;

    //// 球として描画
    //switch (bulletType)
    //{
    //case 0: // 通常弾
    //{
    //    DrawSphere3D(_position, _radius, 8, GetColor(255, 255, 0), GetColor(255, 255, 0), TRUE);
    //}
    //break;
    //case 1: // 硝子玉
    //{
    //    DrawSphere3D(_position, _radius, 16, GetColor(0, 255, 255), GetColor(0, 128, 255), TRUE);
    //}
    //break;
    //}
    DrawSphere3D(_position, _radius, 8, GetColor(255, 255, 0), GetColor(255, 255, 0), TRUE);
}

// 発射処理（カメラの向きと位置から）
void Reflect::Launch(VECTOR cameraPos, VECTOR cameraTarget)
{
    // 発射位置をカメラのターゲット位置に設定
    _position = cameraTarget;

    // カメラの向きベクトルを計算
    VECTOR cameraDirection = VSub(cameraTarget, cameraPos);
    cameraDirection = VNorm(cameraDirection);

    // 速度ベクトルを設定
    _velocity = VScale(cameraDirection, _speed);
    _direction = cameraDirection;

    // アクティブ化
    _isActive = true;
	_bounceCount = 0;// 反射回数初期化
}

void Reflect::ReflectVelocity(VECTOR normal)
{
    // 法線ベクトルを正規化
    normal = VNorm(normal);

    // 反射ベクトルの計算: R = V - 2(V・N)N
    float dot = VDot(_velocity, normal);
    VECTOR reflection = VScale(normal, 2.0f * dot);
    _velocity = VSub(_velocity, reflection);

    // 反射回数をカウント
    _bounceCount++;

    // 反射時に速度を少し減衰させる
    _velocity = VScale(_velocity, 0.9f);
}

// 指定位置でのコリジョン判定
bool Reflect::CheckCollisionAtPosition(VECTOR testPos, int mapHandle, int frameCollision)
{
    // 現在位置から次の位置へのレイキャスト
    MV1_COLL_RESULT_POLY hitPoly = MV1CollCheck_Line(
        mapHandle,
        frameCollision,
        _position,
        testPos
    );

	// 衝突判定
    if (hitPoly.HitFlag)
    {
        // 衝突点に位置を調整（法線方向に少し離す）
        VECTOR normal = VNorm(hitPoly.Normal);
        _position = VAdd(hitPoly.HitPosition, VScale(normal, _radius * 0.1f));

        // 法線ベクトルで反射
        ReflectVelocity(normal);

        return true;
    }
    return false;
}

// コリジョン判定（古い形式、互換性のため残す）
bool Reflect::CheckCollisionWithMovement(VECTOR oldPos, VECTOR newPos, int mapHandle, int frameCollision)
{
    return CheckCollisionAtPosition(newPos, mapHandle, frameCollision);
}

// コリジョン判定（古い形式、互換性のため残す）
bool Reflect::CheckCollision(int mapHandle, int frameCollision)
{
    return CheckCollisionAtPosition(_position, mapHandle, frameCollision);
}