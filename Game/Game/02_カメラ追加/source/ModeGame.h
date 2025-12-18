
#include "appframe.h"

#include <string>

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




}; 
