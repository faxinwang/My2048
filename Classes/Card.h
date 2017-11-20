#pragma once
#include "cocos2d.h"
using namespace cocos2d;

class Card : public Sprite {
public:

	static Card* createCard(int width, int height);
	virtual bool init();
	int getNum() { return num; }
	void setNum(int n);
	void doubleNum();
	Color3B getColor() { return layerBG->getColor(); }

private:
	void initCard(int width ,int height);
private:
	CCLabelTTF * labelNum;
	CCLayerColor* layerBG;
	int num;
};