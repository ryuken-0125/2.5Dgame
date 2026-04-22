/*------------------------------
*	タイトル[title.h]
*
* 制作者：山室飛龍		日付：2025/9/22
------------------------------*/

#ifndef TITLE_H
#define TITLE_H
#include "scene.h"

class Title :public Scene
{
private:

public:
	void Initialize();
	void Finalize();
	void Update(double elapsedTime);
	void Draw();
};

#endif // !TITLE_H
