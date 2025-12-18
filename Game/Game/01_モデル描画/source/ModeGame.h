
#include "appframe.h"

#include <string>


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


	// 3Dモデル描画用
	int _handle;
	int _attach_index;
	float _total_time;
	float _play_time;




}; 
