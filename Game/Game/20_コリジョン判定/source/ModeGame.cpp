
#include "AppFrame.h"
#include "ApplicationMain.h"
#include "ModeGame.h"


bool ModeGame::Initialize() {
	if (!base::Initialize()) { return false; }

	// モデルデータのロード（テクスチャも読み込まれる）
	_handle = MV1LoadModel("res/SDChar/SDChar.mv1");
	_attach_index = -1;		// アニメーションアタッチはされていない
	// ステータスを「無し」に設定
	_status = STATUS::NONE;
	// 再生時間の初期化
	_total_time = 0.f;
	_play_time = 0.0f;
	// 位置,向きの初期化
	_vPos = VGet(0, 0, 0);
	_vDir = VGet(0, 0, -1);		// キャラモデルはデフォルトで-Z方向を向いている

	// マップ
	_handleMap = MV1LoadModel("res/Dungeon/Dungeon.mv1");
	_handleSkySphere = MV1LoadModel("res/SkySphere/skysphere.mv1");

	// コリジョン情報の生成
	_frameMapCollision = MV1SearchFrame(_handleMap, "dungeon_collision");
	MV1SetupCollInfo(_handleMap, _frameMapCollision, 16, 16, 16);
	// コリジョンのフレームを描画しない設定
	MV1SetFrameVisible(_handleMap, _frameMapCollision, FALSE);
	// 腰位置の設定
	_colSubY = 40.f;


	// カメラの設定（わかりやすい位置に）
	_cam._vPos = VGet(0, 90.f, -300.f);
	_cam._vTarget = VGet(0, 60, 0);
	_cam._clipNear = 2.f;
	_cam._clipFar = 10000.f;

	// その他初期化
	_bViewCollision = TRUE;


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

	// 処理前のステータスを保存しておく
	STATUS oldStatus = _status;


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

		// 移動前の位置を保存
		VECTOR oldvPos = _vPos;

		// vの分移動
		_vPos = VAdd(_vPos, v);

		// 移動した先でコリジョン判定
		MV1_COLL_RESULT_POLY hitPoly;
		// 主人公の腰位置から下方向への直線
		hitPoly = MV1CollCheck_Line(_handleMap, _frameMapCollision,
			VAdd(_vPos, VGet(0, _colSubY, 0)), VAdd(_vPos, VGet(0, -99999.f, 0)));
		if (hitPoly.HitFlag) {
			// 当たった
			// 当たったY位置をキャラ座標にする
			_vPos.y = hitPoly.HitPosition.y;

			// カメラも移動する
			_cam._vPos = VAdd(_cam._vPos, v);
			_cam._vTarget = VAdd(_cam._vTarget, v);
		}
		else {
			// 当たらなかった。元の座標に戻す
			_vPos = oldvPos;
		}

		// 移動量をそのままキャラの向きにする
		if (VSize(v) > 0.f) {		// 移動していない時は無視するため
			_vDir = v;
			_status = STATUS::WALK;
		}
		else {
			_status = STATUS::WAIT;
		}


		// デバッグ機能
		if (trg & PAD_INPUT_6) {
			_bViewCollision = !_bViewCollision;
		}
		if (_bViewCollision) {
			MV1SetFrameVisible(_handleMap, _frameMapCollision, TRUE);
		}
		else {
			MV1SetFrameVisible(_handleMap, _frameMapCollision, FALSE);
		}
	}

	// ステータスが変わっていないか？
	if (oldStatus == _status) {
		// 再生時間を進める
		_play_time += 0.5f;
	}
	else {
		// アニメーションがアタッチされていたら、デタッチする
		if (_attach_index != -1) {
			MV1DetachAnim(_handle, _attach_index);
			_attach_index = -1;
		}
		// ステータスに合わせてアニメーションのアタッチ
		switch (_status) {
		case STATUS::WAIT:
			_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle,"idle"), -1, FALSE);
			break;
		case STATUS::WALK:
			_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "run"), -1, FALSE);
			break;
		}
		// アタッチしたアニメーションの総再生時間を取得する
		_total_time = MV1GetAttachAnimTotalTime(_handle, _attach_index);
		// 再生時間を初期化
		_play_time = 0.0f;
	}

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

	// ライト設定
	SetUseLighting(TRUE);
#if 0	// 平行ライト
	SetGlobalAmbientLight(GetColorF(0.5f, 0.f, 0.f, 0.f));
	ChangeLightTypeDir(VGet(-1, -1, 0));
#endif
#if 1	// ポイントライト
	SetGlobalAmbientLight(GetColorF(0.f, 0.f, 0.f, 0.f));
	ChangeLightTypePoint(VAdd(_vPos,VGet(0,50.f,0)), 1000.f, 0.f, 0.005f, 0.f);
#endif

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
	{
		// 位置
		MV1SetPosition(_handle, _vPos);
		// 向きからY軸回転を算出
		VECTOR vRot = { 0,0,0 };
		vRot.y = atan2(_vDir.x * -1, _vDir.z * -1);		// モデルが標準でどちらを向いているかで式が変わる(これは-zを向いている場合)
		MV1SetRotationXYZ(_handle, vRot);
		// 描画
		MV1DrawModel(_handle);

		// コリジョン判定用ラインの描画
		if (_bViewCollision) {
			DrawLine3D(VAdd(_vPos, VGet(0, _colSubY, 0)), VAdd(_vPos, VGet(0, -99999.f, 0)), GetColor(255, 0, 0));
		}

	}

	// マップモデルを描画する
	{
		MV1DrawModel(_handleMap);
		MV1DrawModel(_handleSkySphere);
	}

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

