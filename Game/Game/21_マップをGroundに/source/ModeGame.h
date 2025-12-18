
#include "appframe.h"

#include <string>

// 計算用マクロ
#define	PI	(3.1415926535897932386f)
#define	DEG2RAD(x)			( ((x) / 180.0f ) * PI )
#define	RAD2DEG(x)			( ((x) * 180.0f ) / PI )

// カメラ
class Camera {
public:
	VECTOR	_vPos;					// 位置
	VECTOR	_vTarget;				// 距離
	float	_clipNear, _clipFar;	// クリップ
};

// モード
class ModeGame : public ModeBase
{
	typedef ModeBase base;
public:
	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();
	virtual bool Render();

protected:

	// カメラ
	Camera	_cam;

	// 3Dモデル描画用
	int _handle;
	int _attach_index;
	float _total_time;
	float _play_time;
	VECTOR _vPos;	// 位置
	VECTOR _vDir;	// 向き
	float _colSubY;	// コリジョン判定時のY補正(腰位置）
	enum class STATUS {
		NONE,
		WAIT,
		WALK,
		_EOT_
	};
	STATUS _status;

	// マップ用
	int _handleMap;
	int _handleSkySphere;
	int _frameMapCollision;


	// デバッグ用
	bool	_bViewCollision;


}; 
