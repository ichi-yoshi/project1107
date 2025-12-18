/*
** リソースサーバ
*/

#include "DxLib.h"
#include "ResourceServer.h"

// 静的メンバ実体
std::unordered_map<std::string, int>	ResourceServer::_mapGraph;
std::unordered_map<std::string, ResourceServer::DIVGRAPH>	ResourceServer::_mapDivGraph;
std::unordered_map<std::string, int>	ResourceServer::_mapSound;


void    ResourceServer::Init()
{
	_mapGraph.clear();
    _mapDivGraph.clear();
    _mapSound.clear();
}

void    ResourceServer::Release()
{
    ClearGraph();
}

void	ResourceServer::ClearGraph()
{
    // すべてのデータの削除をする
    for (auto itr = _mapGraph.begin(); itr != _mapGraph.end(); itr++)
    {
        DeleteGraph(itr->second);
    }
    _mapGraph.clear();

    for (auto itr = _mapDivGraph.begin(); itr != _mapDivGraph.end(); itr++)
    {
        for (int i = 0; i < itr->second.AllNum; i++) {
            DeleteGraph(itr->second.handle[i]);
        }
        delete[] itr->second.handle;
    }
    _mapDivGraph.clear();

    for (auto itr = _mapSound.begin(); itr != _mapSound.end(); itr++)
    {
        DeleteSoundMem(itr->second);
    }
    _mapSound.clear();

}


int		ResourceServer::LoadGraph(const TCHAR* FileName)
{
    // キーの検索
    auto itr = _mapGraph.find(FileName);
    if (itr != _mapGraph.end())
    {
        // キーがあった
        return itr->second;
    }
    // キーが無かった
    int cg = ::LoadGraph(FileName);     // DXLIBのAPIを呼ぶので、::を先頭に付け、このクラスの同じ名前の関数と区別する
    // キーとデータをmapに登録
    _mapGraph[FileName] = cg;

    return cg;
}

int		ResourceServer::LoadDivGraph(const TCHAR* FileName, int AllNum,
    int XNum, int YNum,
    int XSize, int YSize, int* HandleBuf)
{
    // キーの検索
    auto itr = _mapDivGraph.find(FileName);
    if (itr != _mapDivGraph.end())
    {
        // キーがあった
        // データをコピー
        for (int i = 0; i < itr->second.AllNum; i++) {
            HandleBuf[i] = itr->second.handle[i];
        }
        return 0;
    }
    // キーが無かった
    // まずはメモリを作成する
    int* hbuf = new int[AllNum];
    int err = ::LoadDivGraph(FileName,AllNum,XNum,YNum,XSize,YSize, hbuf);     // DXLIBのAPIを呼ぶので、::を先頭に付け、このクラスの同じ名前の関数と区別する
    if (err == 0) {
        // 成功
        // キーとデータをmapに登録
        _mapDivGraph[FileName].AllNum = AllNum;
        _mapDivGraph[FileName].handle = hbuf;
        // データをコピー
        for (int i = 0; i < AllNum; i++) {
            HandleBuf[i] = hbuf[i];
        }
    }

    return err;

}


int		ResourceServer::LoadSoundMem(const TCHAR* FileName) {
    // キーの検索
    auto itr = _mapSound.find(FileName);
    if (itr != _mapSound.end())
    {
        // キーがあった
        return itr->second;
    }
    // キーが無かった
    int snd = ::LoadSoundMem(FileName);     // DXLIBのAPIを呼ぶので、::を先頭に付け、このクラスの同じ名前の関数と区別する
    // キーとデータをmapに登録
    _mapSound[FileName] = snd;

    return snd;
}

