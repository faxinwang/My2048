#include "My2048.h"
#include "SimpleAudioEngine.h"
#include <math.h>

Scene* My2048::createScene()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto scene = My2048::create();
	auto layer = CCLayerColor::create(ccc4(155,110,87,255),visibleSize.width,visibleSize.height);
	scene->addChild(layer,-2);
	return scene;
}


// on "init" you need to initialize your instance
bool My2048::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
	auto visibleSize = Director::getInstance()->getVisibleSize();

	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(My2048::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(My2048::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener,this);

	auto keyboardEvent = EventListenerKeyboard::create();
	keyboardEvent->onKeyPressed = CC_CALLBACK_2(My2048::onKeyPressed,this);
	_eventDispatcher->addEventListenerWithFixedPriority(keyboardEvent, 1);

	srand(time(0));
	auto label = CCLabelTTF::create("MaxScore: ", "fonts/arial.ttf", 28);//font size 24
	label->setAnchorPoint(ccp(0,0));
	label->setPosition(20, visibleSize.height - 40);
	this->addChild(label);
	_maxScore = _curScore = 0;
	_label_maxScore = CCLabelTTF::create("0", "fonts/arial.ttf", 28);
	_label_maxScore->setAnchorPoint(ccp(0, 0));
	_label_maxScore->setPosition(20 + label->getContentSize().width, label->getPositionY());
	this->addChild(_label_maxScore);
	
	label = CCLabelTTF::create("Score: ", "fonts/arial.ttf", 25); //font size 20
	label->setAnchorPoint(ccp(0, 0));
	label->setPosition(20, _label_maxScore->getPositionY() - 40);
	this->addChild(label);
	_label_curScore = CCLabelTTF::create("0", "fonts/arial.ttf", 25);
	_label_curScore->setAnchorPoint(ccp(0, 0));
	_label_curScore->setPosition(20+label->getContentSize().width, label->getPositionY());
	this->addChild(_label_curScore);

	initTable(4, 4);
	startGame();

    return true;
}


void My2048::initTable(int row,int col){
	_row = row;
	_col = col;
	Color4B bgcolor = Color4B(187, 173, 160, 255);
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	cardSize = visibleSize.width / (col+1);
	const int gap = 10;
	int x, y=30;
	for (int i = 0; i < row; ++i) {
		x = 16;
		for (int j = 0; j < col; ++j) {
			auto card = Card::createCard(cardSize, cardSize);
			card->setAnchorPoint(ccp(0,0));
			card->setPosition(x, y);
			this->addChild(card, -1);
			_bgCards[i][j] = card;
			x += cardSize + gap;
			_cards[i][j] = 0;
		}
		y += cardSize + gap;
	}
}

void My2048::startGame() {
	int tx = rand() % _row;
	int ty = rand() % _col;
	_cards[tx][ty] = Card::createCard(cardSize, cardSize);
	_cards[tx][ty]->setNum(2);
	_cards[tx][ty]->setPosition(_bgCards[tx][ty]->getPosition());
	this->addChild(_cards[tx][ty]);

	int tx2, ty2;
	do {
		tx2 = rand() % _row;
		ty2 = rand() % _col;
	} while (tx == tx2 && ty == ty2);
	_cards[tx2][ty2] = Card::createCard(cardSize, cardSize);
	_cards[tx2][ty2]->setNum(4);
	_cards[tx2][ty2]->setPosition(_bgCards[tx2][ty2]->getPosition());
	this->addChild(_cards[tx2][ty2]);
}

bool My2048::onTouchBegan(Touch* touch, Event* event) {
	auto pos = touch->getLocation();
	_startx = pos.x;
	_starty = pos.y;
	return true;
}

void My2048::onTouchEnded(Touch* touch, Event* event) {
	auto pos = touch->getLocation();
	_diffx = _startx - pos.x;
	_diffy = _starty - pos.y;
	const int SLIDE_DIST = 10;
	if (abs(_diffx) > abs(_diffy)) {
		if (abs(_diffx) < SLIDE_DIST) return; //not leng enought to slide
		if (_diffx > 0 ) { // from right to left
			moveLeft();
		}
		else {
			moveRight();
		}
	}
	else {
		if (abs(_diffy) < SLIDE_DIST) return; //not leng enought to slide
		if (_diffy > 0) { //from high place to low place
			moveDown();
		}
		else {
			moveUp();
		}
	}
}

void My2048::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {

	if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
		moveLeft();
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
		moveRight();
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW) {
		moveUp();
	}
	else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW) {
		moveDown();
	}
}

bool My2048::moveTo(int fx,int fy, int tx, int ty) {
	if (fx==tx && fy==ty) return false;
	auto from = _cards[fx][fy];
	auto to = _cards[tx][ty];

	if (from == 0) {
		log(" (%d, %d) ", fx, fy);
		return false;
	}

	if (to) {
		if (from->getNum() != to->getNum()) { log("two number are different");  return false; }
		this->addScore(from->getNum() * 2);
		auto move = MoveTo::create(0.2, to->getPosition());
		auto fade = FadeOut::create(0.2);
		auto scale = ScaleTo::create(0.2, 1.5);
		auto spawn = Spawn::create(
			fade,
			scale,
			NULL
		); 
	//	from->setAnchorPoint(ccp(0.5, 0.5));
		auto seq = Sequence::create(
				move,
			//	fade,
				spawn,
				CCCallFunc::create(CC_CALLBACK_0(Card::doubleNum,to)),
				CCCallFunc::create(CC_CALLBACK_0(Card::removeFromParent, from)),
				NULL);
		from->runAction(seq);
		_cards[fx][fy] = nullptr;
	}
	else {
		to = _bgCards[tx][ty];
		auto move = MoveTo::create(0.2, to->getPosition());
		from->runAction(move);
		_cards[tx][ty] = from;
		_cards[fx][fy] = nullptr;
	}
	return true;
}


void My2048::moveLeft() {
	log("\n move Left \n");
	for (int r = 0; r < _row; ++r) {
		for (int c = 0; c < _col; ++c) {
			for (int k = c + 1; k < _col; ++k) {
				if (_cards[r][k] == 0) continue;
				if (_cards[r][c]==0) {
					moveTo(r, k, r, c);
				}
				else if (_cards[r][c]->getNum() == _cards[r][k]->getNum()) {
					moveTo(r, k, r, c);
				}
				else {
					if (++c != k) {
						moveTo(r, k, r, c);
					}
				}
			//	break;
			}
		}
	}
	if (!checkGameOver()) {
		//在第_col - 1列随机生成新的方块
		std::vector<int> v;
		for (int k = 0; k < _row; ++k) if (_cards[k][_col - 1] == 0) v.push_back(k);
		if (v.size()) this->addCard(v[rand() % v.size()], _col - 1);
	}
}// move left


void My2048::moveRight() {
	log("\n move Right \n");
	for (int r = 0; r < _row; ++r) {
		for (int c = _col-1; c >=0; --c) {
			for (int k = c - 1; k >= 0; --k) {
				if (_cards[r][k] == 0) continue;
				if (_cards[r][c] == 0 ) {
					moveTo(r, k, r, c);
				}
				else if (_cards[r][c]->getNum() == _cards[r][k]->getNum()) {
					moveTo(r, k, r, c);
				}
				else {
					if (--c != k) {
						moveTo(r, k, r, c);
					}
				}
			//	break;
			}
		}
	}

	if (!checkGameOver()) {
		//在第0列随机生成新的方块
		std::vector<int> v;
		for (int k = 0; k < _row; ++k) if (_cards[k][0] == 0) v.push_back(k);
		if (v.size()) this->addCard(v[rand() % v.size()], 0);
	}
} // move right


void My2048::moveUp() {
	log("\n move Up \n");
	for (int c = 0; c<_col; ++c) {
		for (int r = _row - 1; r >= 0; --r) {
			for (int k = r - 1; k >= 0; --k) {
				if (_cards[k][c] == 0) continue;
				if (_cards[r][c]==0 ) {
					moveTo(k, c, r, c);
				}
				else if(_cards[k][c]->getNum() == _cards[r][c]->getNum()){
					moveTo(k, c, r, c);
				}
				else {
					if (--r != k) {
						moveTo(k, c, r, c);
					}
				}
			//	break;
			}
		}
	}

	if (!checkGameOver()) {
		//在第0行随机生成新的方块
		std::vector<int> v;
		for (int k = 0; k < _col; ++k) if (_cards[0][k] == 0) v.push_back(k);
		if (v.size()) this->addCard(0, v[rand() % v.size()]);
	}
} // move up

void My2048::moveDown() {
	log("\n move Down \n");
	for (int c = 0; c < _col; ++c) {
		for (int r = 0; r < _row; ++r) {
			for (int k = r + 1; k < _row; ++k) {
				if (_cards[k][c] == 0) continue;
				if (_cards[r][c] == 0) {
					moveTo(k, c, r, c);
				}
				else if (_cards[r][c]->getNum() == _cards[k][c]->getNum()) {
					moveTo(k, c, r, c);
				}
				else {
					if (++r != k) {
						moveTo(k, c, r, c);
					}
				}
			//	break;
			}
		}
	}

	if (! checkGameOver()) {
		//在第_row-1行随机生成新的方块
		std::vector<int> v;
		for (int k = 0; k < _col; ++k) if (_cards[_row - 1][k] == 0) v.push_back(k);
		if (v.size()) this->addCard(_row - 1, v[rand() % v.size()]);
	}
} // move down

bool My2048::checkGameOver() {
	bool isGameOver = true;
	for (int r = 0; r < _row; ++r) {
		for (int c = 0; c < _col; ++c) { 
			if (_cards[r][c] == 0) return false;
			int num = _cards[r][c]->getNum();
			if (r -1>=0 && _cards[r - 1][c] && _cards[r - 1][c]->getNum() == num) return false;
			if (r + 1 < _row && _cards[r + 1][c] && _cards[r + 1][c]->getNum() == num) return false;
			if (c - 1 >= 0 && _cards[r][c - 1] && _cards[r][c - 1]->getNum() == num) return false;
			if (c + 1 < _col && _cards[r][c + 1] && _cards[r][c + 1]->getNum() == num) return false;
		}
	}
	if (isGameOver) {
		this->runAction(
			Sequence::create(
				CCCallFunc::create(CC_CALLBACK_0(My2048::gameOver, this)),
				DelayTime::create(2.0),
				CCCallFunc::create(CC_CALLBACK_0(My2048::startGame, this)),
				NULL)
		);
	}
	return isGameOver;
}

void My2048::gameOver() {
	if (_maxScore < _curScore) _maxScore = _curScore;
	_curScore = 0;
	My2048 * game = this;
	log("\nGameOver!\n");
	
	for (int r = 0; r < _row; ++r) {
		for (int c = 0; c < _col; ++c) {
			if (_cards[r][c]) {
				_cards[r][c]->runAction(
					Sequence::create(
//						EaseOut::create(
							MoveTo::create(1.5, ccp(((r+1) * 15)*((c+2) * 10), ((c+2)* 15)*((1	+2) * 10))),
//						2),
						CCCallFuncN::create(CC_CALLBACK_0(Card::removeFromParent, _cards[r][c])),
						NULL)
				);
				_cards[r][c] = 0;
			}
		}
	}
	//Director::getInstance()->replaceScene(TransitionJumpZoom::create(2, My2048::createScene()));
}

void My2048::addScore(int add) {
	_curScore += add;
	_label_curScore->setString(String::createWithFormat("%d", _curScore)->getCString());
	if (_curScore > _maxScore) {
		_maxScore = _curScore;
		_label_maxScore->setString(String::createWithFormat("%d",_maxScore)->getCString());
	}
	auto label = LabelTTF::create();
	label->setString(String::createWithFormat("+%d", add)->getCString());
	label->setFontSize(25);
	label->setPosition(_label_curScore->getPositionX()+20, _label_curScore->getPositionY()+30);
	label->setColor(Color3B(125,250,146));
	this->addChild(label);
	auto spawn = Spawn::create(
		MoveBy::create(2, ccp(10, 35)),
		ScaleBy::create(2, 2),
		FadeOut::create(2),
		NULL);
	label->runAction(
		Sequence::create(
			spawn,
			CCCallFuncN::create(CC_CALLBACK_0(LabelTTF::removeFromParent, label)),
			NULL)
	);
}


void My2048::addCard(int x,int y) {
	auto card = Card::createCard(cardSize, cardSize);
	if (rand() % 10 > 7) {
		card->setNum(4);
	//	this->addScore(4);
	}
	else {
		card->setNum(2);
	//	this->addScore(2);
	}
	card->setPosition(_bgCards[x][y]->getPosition());
	_cards[x][y] = card;
	this->addChild(card);
}