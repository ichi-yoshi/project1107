
#include "AppFrame.h"
#include "ApplicationMain.h"
#include "ModeGame.h"


bool ModeGame::Initialize() {
	if (!base::Initialize()) { return false; }

	// モデルデータのロード（テクスチャも読み込まれる）
	_handle = MV1LoadModel("res/SDChar/SDChar.mv1");
	// 3Dモデルの1番目のアニメーションをアタッチする
	_attach_index = MV1AttachAnim(_handle, 1, -1, FALSE);
	// アタッチしたアニメーションの総再生時間を取得する
	_total_time = MV1GetAttachAnimTotalTime(_handle, _attach_index);
	// 再生時間の初期化
	_play_time = 0.0f;


	return true;
}

bool ModeGame::Terminate() {
	base::Terminate();
	return true;
}

bool ModeGame::Process() {
	base::Process();
	int key = ApplicationMain::GetInstance()->GetKey();
	int trg = ApplicationMain::GetInstance()->GetTrg();


	// 再生時間を進める
	_play_time += 0.5f;
	// 再生時間がアニメーションの総再生時間に達したら再生時間を０に戻す
	if (_play_time >= _total_time) {
		_play_time = 0.0f;
	}


	return true;
}

bool ModeGame::Render() {
	base::Render();

	// 3D基本設定
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetUseBackCulling(TRUE);

	// カメラの設定ヴューワーからコピペ
	SetCameraPositionAndTarget_UpVecY(VGet(73.636536f, 86.688026f, -140.440582f), VGet(-12.230986f, 59.101776f, -15.002045f));
	SetCameraNearFar(2.376863f, 594.215820f);

	// 再生時間をセットする
	MV1SetAttachAnimTime(_handle, _attach_index, _play_time);

	// モデルを描画する
	MV1DrawModel(_handle);


	return true;
}

