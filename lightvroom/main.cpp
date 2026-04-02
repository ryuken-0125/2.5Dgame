
#include "Application.h"

// メモリリークをチェックするためのライブラリ（デバッグ用）
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//山室

//りゅうけんです
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    //終了時にメモリの解放忘れ（リーク）がないかを出力ウィンドウに表示する設定
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    Application app;

    // ウィンドウサイズを 1280x720 に指定して初期化
    if (!app.Initialize(hInstance, nCmdShow, 1280, 720))
    {
        return -1;
    }

    // ゲームのメインループ開始
    app.Run();

    return 0;
}

/*
* 
*FBXモデルの頂点データをVertex構造体の配列に変換する例（擬似コード）
for (unsigned int i = 0; i < fbxModel->mNumVertices; i++)
{
    Vertex v;
    v.Pos.x = fbxModel->mVertices[i].x;
    v.Pos.y = fbxModel->mVertices[i].y;
    v.Pos.z = fbxModel->mVertices[i].z;
    // (法線やUVも同様にループで代入)

    vertices.push_back(v);
}


*/