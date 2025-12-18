
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
	// 位置の初期化
	_vPos = VGet(0, 0, 0);

	// カメラの設定ヴューワーからコピペ
	_cam._vPos = VGet(73.636536f, 86.688026f, -140.440582f);
	_cam._vTarget = VGet(-12.230986f, 59.101776f, -15.002045f);
	_cam._clipNear = 2.376863f;
	_cam._clipFar = 594.215820f;

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


	if ((key & (PAD_INPUT_7 | PAD_INPUT_8)) == (PAD_INPUT_7 | PAD_INPUT_8)) {	// Q & W
		// 距離、ターゲットの高さ変更
		float sx = _cam._vPos.x - _cam._vTarget.x;
		float sz = _cam._vPos.z - _cam._vTarget.z;
		float rad = atan2(sz, sx);
		float length = sqrt(sz * sz + sx * sx);
		if (key & PAD_INPUT_LEFT) { length -= 1.f; }
		if (key & PAD_INPUT_RIGHT) { length += 1.f; }
		_cam._vPos.x = _cam._vTarget.x + cos(rad) * length;
		_cam._vPos.z = _cam._vTarget.z + sin(rad) * length;

		// Y位置
		if (key & PAD_INPUT_DOWN) { _cam._vTarget.y -= 1.f; }
		if (key & PAD_INPUT_UP) { _cam._vTarget.y += 1.f; }
	}
	else if (key & PAD_INPUT_7) {	// Q
		// 角度変更
		// Y軸回転
		float sx = _cam._vPos.x - _cam._vTarget.x;
		float sz = _cam._vPos.z - _cam._vTarget.z;
		float rad = atan2(sz, sx);
		float length = sqrt(sz * sz + sx * sx);
		if (key & PAD_INPUT_LEFT) { rad -= 0.05f; }
		if (key & PAD_INPUT_RIGHT) { rad += 0.05f; }
		_cam._vPos.x = _cam._vTarget.x + cos(rad) * length;
		_cam._vPos.z = _cam._vTarget.z + sin(rad) * length;

		// Y位置
		if (key & PAD_INPUT_DOWN) { _cam._vPos.y -= 5.f; }
		if (key & PAD_INPUT_UP) { _cam._vPos.y += 5.f; }
	}
	else if (key & PAD_INPUT_8) {	// W
		// カメラ位置（注目位置もXZスライド）
		float sx = _cam._vPos.x - _cam._vTarget.x;
		float sz = _cam._vPos.z - _cam._vTarget.z;
		float camrad = atan2(sz, sx);

		// 移動方向を決める
		VECTOR v = { 0,0,0 };
		float mvSpeed = 2.f;
		if (key & PAD_INPUT_DOWN) { v.x = 1; }
		if (key & PAD_INPUT_UP) { v.x = -1; }
		if (key & PAD_INPUT_LEFT) { v.z = -1; }
		if (key & PAD_INPUT_RIGHT) { v.z = 1; }

		// vをrad分回転させる
		float length = 0.f;
		if (VSize(v) > 0.f) { length = mvSpeed; }
		float rad = atan2(v.z, v.x);
		v.x = cos(rad + camrad) * length;
		v.z = sin(rad + camrad) * length;

		// vの分移動
		_cam._vPos = VAdd(_cam._vPos, v);
		_cam._vTarget = VAdd(_cam._vTarget, v);
	}
	else {
		// キャラ移動(カメラ設定に合わせて)

		// カメラの向いている角度を取得
		float sx = _cam._vPos.x - _cam._vTarget.x;
		float sz = _cam._vPos.z - _cam._vTarget.z;
		float camrad = atan2(sz, sx);

		// 移動方向を決める
		VECTOR v = { 0,0,0 };
		float mvSpeed = 6.f;
		if (key & PAD_INPUT_DOWN) { v.x = 1; }
		if (key & PAD_INPUT_UP) { v.x = -1; }
		if (key & PAD_INPUT_LEFT) { v.z = -1; }
		if (key & PAD_INPUT_RIGHT) { v.z = 1; }

		// vをrad分回転させる
		float length = 0.f;
		if (VSize(v) > 0.f) { length = mvSpeed; }
		float rad = atan2(v.z, v.x);
		v.x = cos(rad + camrad) * length;
		v.z = sin(rad + camrad) * length;

		// vの分移動
		_vPos = VAdd(_vPos, v);
		_cam._vPos = VAdd(_cam._vPos, v);
		_cam._vTarget = VAdd(_cam._vTarget, v);
	}

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

	// カメラ設定更新
	SetCameraPositionAndTarget_UpVecY(_cam._vPos, _cam._vTarget);
	SetCameraNearFar(_cam._clipNear, _cam._clipFar);

	// 0,0,0を中心に線を引く
	{
		float linelength = 1000.f;
		VECTOR v = { 0, 0, 0 };
		DrawLine3D(VAdd(v, VGet(-linelength, 0, 0)), VAdd(v, VGet(linelength, 0, 0)), GetColor(255, 0, 0));
		DrawLine3D(VAdd(v, VGet(0, -linelength, 0)), VAdd(v, VGet(0, linelength, 0)), GetColor(0, 255, 0));
		DrawLine3D(VAdd(v, VGet(0, 0, -linelength)), VAdd(v, VGet(0, 0, linelength)), GetColor(0, 0, 255));
	}

	// カメラターゲットを中心に短い線を引く
	{
		float linelength = 10.f;
		VECTOR v = _cam._vTarget;
		DrawLine3D(VAdd(v, VGet(-linelength, 0, 0)), VAdd(v, VGet(linelength, 0, 0)), GetColor(255, 0, 0));
		DrawLine3D(VAdd(v, VGet(0, -linelength, 0)), VAdd(v, VGet(0, linelength, 0)), GetColor(0, 255, 0));
		DrawLine3D(VAdd(v, VGet(0, 0, -linelength)), VAdd(v, VGet(0, 0, linelength)), GetColor(0, 0, 255));
	}

	// 再生時間をセットする
	MV1SetAttachAnimTime(_handle, _attach_index, _play_time);

	// モデルを描画する
	MV1SetPosition(_handle, _vPos);
	MV1DrawModel(_handle);

	// カメラ情報表示
	{
		int x = 0, y = 0, size = 16;
		SetFontSize(size);
		DrawFormatString(x, y, GetColor(255, 0, 0), "Camera:"); y += size;
		DrawFormatString(x, y, GetColor(255, 0, 0), "  target = (%5.2f, %5.2f, %5.2f)", _cam._vTarget.x, _cam._vTarget.y, _cam._vTarget.z); y += size;
		DrawFormatString(x, y, GetColor(255, 0, 0), "  pos    = (%5.2f, %5.2f, %5.2f)", _cam._vPos.x, _cam._vPos.y, _cam._vPos.z); y += size;
		float sx = _cam._vPos.x - _cam._vTarget.x;
		float sz = _cam._vPos.z - _cam._vTarget.z;
		float length = sqrt(sz * sz + sx * sx);
		float rad = atan2(sz, sx);
		float deg = RAD2DEG(rad);
		DrawFormatString(x, y, GetColor(255, 0, 0), "  len = %5.2f, rad = %5.2f, deg = %5.2f", length, rad, deg); y += size;
	}


	return true;
}

