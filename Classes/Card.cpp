#include "Card.h"

Card* Card::createCard(int width, int height) {
	Card *card = new Card();
	if (card && card->init()) {
		card->autorelease();
		card->initCard(width ,height);
		return card;
	}
	else 
	{
		CC_SAFE_DELETE(card);
		return nullptr;
	}
}

bool Card::init() {
	if (!Sprite::init()) return false;
	return true;
}

void Card::initCard(int width, int height) {
	
	labelNum = CCLabelTTF::create("", "fonts/arial.ttf", 40);
	layerBG = CCLayerColor::create(Color4B(187, 173, 160,255), width, height); 
	layerBG->addChild(labelNum);
	//将label的位置设置在卡片的中心
	labelNum->setPosition(layerBG->getContentSize().width / 2, layerBG->getContentSize().height / 2);
//	labelNum->setColor(Color3B::BLUE);
	this->addChild(layerBG);
}

//设置卡片数字的时候动态修改卡片上数字字体的大小和卡片的背景颜色。
void Card::setNum(int n) {
	num = n;
	if (n > 1000) labelNum->setFontSize(20);
	else if (n > 100) labelNum->setFontSize(30);
	Color3B  color;
	switch (num) {
	case 2:		color = Color3B(240, 220, 210); break;
	case 4:		color = Color3B(240, 220, 200); break;
	case 8:		color = Color3B(240, 180, 120); break;
	case 16:	color = Color3B(240, 140, 90);	break;
	case 32:	color = Color3B(240, 120, 90);	break;
	case 64:	color = Color3B(240, 90, 60);	break;
	case 128:	color = Color3B(240, 90, 60);	break;
	case 256:	color = Color3B(240, 200, 70);	break;
	case 512:	color = Color3B(240, 200, 70);	break;
	case 1024:	color = Color3B(240, 140, 90);	break;
	case 2048:	color = Color3B(0, 130, 0);		break;
	default: 	color = Color3B(0, 180, 0);		break;
	}
	layerBG->setColor(color);
	if (num != 0) {
		labelNum->setString(String::createWithFormat("%d",num)->getCString());
	}
	else {
		labelNum->setString(""); //数字为0是不显示
	}
}

//将卡片上的数字设为当前数字的两倍
void Card::doubleNum() {
	this->setNum(num * 2);
}