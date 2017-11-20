#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Card.h"
#include <vector>
using namespace cocos2d;


class My2048 : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
   
    
	virtual bool onTouchBegan(Touch* touch, Event* event);
	virtual void onTouchEnded(Touch* touch, Event* event);

    // implement the "static create()" method manually
    CREATE_FUNC(My2048);
	
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();

	bool moveTo(int fx,int fy, int tx, int ty);
	void addScore(int add);
	void addCard(int x,int y);
	
	bool checkGameOver();
	void startGame();
	void gameOver();
private:
	int _startx, _starty, _diffx, _diffy;
	Card *_cards[10][10];
	Card *_bgCards[10][10];
	int _row, _col;
	int cardSize;

	int _curScore,_maxScore;
	CCLabelTTF *_label_curScore;
	CCLabelTTF *_label_maxScore;
	
	void initTable(int row, int col);
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
};

#endif // __HELLOWORLD_SCENE_H__
