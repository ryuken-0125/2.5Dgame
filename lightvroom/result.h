/*------------------------------
*	リザルト処理[result.h]
*
* 制作者：山室飛龍		日付：2025/9/24
------------------------------*/
#ifndef RESULT_H
#define RESULT_H

#include "scene.h"

class Result : public Scene
{
public:
	void Initialize();
	void Finalize();
	void Update(double elapsedTime);
	void Draw();

private:

};


#endif // !RESULT_
