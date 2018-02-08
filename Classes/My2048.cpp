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

	//添加点击事件
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(My2048::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(My2048::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener,this);
	
	//添加键盘事件
	auto keyboardEvent = EventListenerKeyboard::create();
	keyboardEvent->onKeyPressed = CC_CALLBACK_2(My2048::onKeyPressed,this);
	_eventDispatcher->addEventListenerWithFixedPriority(keyboardEvent, 1);

	//显示最高得分
	srand(time(0));
	_curScore = 0;
	_maxScore = CCUserDefault::getInstance()->getIntegerForKey("MaxScore",0); //读取用户数据
	auto label = CCLabelTTF::create("MaxScore: ", "fonts/arial.ttf", 28);//font size 24
	label->setAnchorPoint(ccp(0,0));
	label->setPosition(20, visibleSize.height - 40);
	this->addChild(label);
	_label_maxScore = CCLabelTTF::create("0", "fonts/arial.ttf", 28);
	_label_maxScore->setAnchorPoint(ccp(0, 0));
	_label_maxScore->setPosition(20 + label->getContentSize().width, label->getPositionY());
	_label_maxScore->setString(String::createWithFormat("%d", _maxScore)->getCString());
	this->addChild(_label_maxScore);
	
	//显示当前得分
	label = CCLabelTTF::create("Score: ", "fonts/arial.ttf", 25); //font size 20
	label->setAnchorPoint(ccp(0, 0));
	label->setPosition(20, _label_maxScore->getPositionY() - 40);
	this->addChild(label);
	_label_curScore = CCLabelTTF::create("0", "fonts/arial.ttf", 25);
	_label_curScore->setAnchorPoint(ccp(0, 0));
	_label_curScore->setPosition(20+label->getContentSize().width, label->getPositionY());
	this->addChild(_label_curScore);
	
	initTable(5, 4); //初始化背景 5行4列
	startGame();    //开始游戏

    return true;
}

//根据给定参数创建游戏界面
void My2048::initTable(int row,int col){
	_row = row;
	_col = col;
	Color4B bgcolor = Color4B(187, 173, 160, 255); //背景方格的颜色
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	cardSize = visibleSize.width / (col+1); //根据列数计算合适大小的方块尺寸
	const int gap = 10; //方块与方块之间的间隔
	int x, y=30;

	//生成tabel,并计算table中各个方块的位置
	for (int i = 0; i < row; ++i) {
		x = 16;//左边空16个像素
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

//在table上随机找两个位置添加两个卡片，数字分别为2或4
void My2048::startGame() {
	int tx = rand() % _row;
	int ty = rand() % _col;
	_cards[tx][ty] = Card::createCard(cardSize, cardSize);
	_cards[tx][ty]->setNum(2);
	_cards[tx][ty]->setPosition(_bgCards[tx][ty]->getPosition());
	this->addChild(_cards[tx][ty]);

	int tx2, ty2;
	//用循环是为了避免随机找到的两个位置是一个位置
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
	//X轴上的分量大于Y轴上的分量，则为左右滑动，否则为上下滑动
	if (abs(_diffx) > abs(_diffy)) {
		if (abs(_diffx) < SLIDE_DIST) return; //滑动的距离不够长的话，视为没有滑动
		if (_diffx > 0 ) { // from right to left
			moveLeft();
		}
		else {
			moveRight();
		}
	}
	else {
		if (abs(_diffy) < SLIDE_DIST) return; //滑动距离不够长，视为未滑动
		if (_diffy > 0) { //from high place to low place
			moveDown();
		}
		else {
			moveUp();
		}
	}
}

//用键盘的上下左右箭头控制滑动
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


//将table上[fx,fy]位置上的卡片移动到 [tx,ty]位置上去，如果目标位置上有卡片，说明两卡片数字相等
bool My2048::moveTo(int fx,int fy, int tx, int ty) {
	if (fx==tx && fy==ty) return false;
	auto from = _cards[fx][fy];
	auto to = _cards[tx][ty];

/* for debug
	if (from == 0) {
		log(" (%d, %d) ", fx, fy);
		return false;
	}
*/
	//如果目标位置上有卡票，说明两卡片可以合并
	if (to) {
	//	if (from->getNum() != to->getNum()) { log("two number are different");  return false; } //for debug
		this->addScore(from->getNum() * 2); //增加得分，分值为两个卡片上的数字之和
		auto move = MoveTo::create(0.2, to->getPosition());
		auto fade = FadeOut::create(0.2);
		auto scale = ScaleTo::create(0.2, 1.5);

		//fade动作和scale动作同时进行
		auto spawn = Spawn::create(
			fade,
			scale,
			NULL
		); 
	//	from->setAnchorPoint(ccp(0.5, 0.5));

		auto seq = Sequence::create(
				move, //卡票先移动到目标位置
				spawn,  //然后边放大边淡出
				CCCallFunc::create(CC_CALLBACK_0(Card::doubleNum,to)), //然后让目标位置的卡片分数翻倍
				CCCallFunc::create(CC_CALLBACK_0(Card::removeFromParent, from)), //然后移动的卡片从游戏界面中删除
				NULL);
		from->runAction(seq);
		_cards[fx][fy] = nullptr; //将from位置置空值
	}
	//如果目标位置上没有卡片，说明只需要将from位置的卡片移动到to位置
	else {
		to = _bgCards[tx][ty];
		auto move = MoveTo::create(0.2, to->getPosition());
		from->runAction(move);
		_cards[tx][ty] = from;
		_cards[fx][fy] = nullptr;  //将from位置置空值
	}
	return true;
}


/*
方块的滑动逻辑:
* 由于四个方向上的滑动逻辑都是相同的，所以这里就只以往左滑动为例说一下滑动的逻辑实现。
* 由于每一行的滑动逻辑和第一行是完全一样的，所以只以第一行的滑动逻辑为例进行讲解。
* 往左移动的时候，首先处理[0,0]位置，处理方法如下：
*	1.在该方块右边找一个数字不为0的方块，如果找到了，就进行下面的操作。否则不进行任何操作
*   2.判断当前要处理的位置[0,0]上是否有方块。
*		2.1如果没有方块，则将找到的方块移动到当前待处理的位置[0,0].
*		2.2如果有方块，则判断找到的方块的数字和当前待处理位置[0,0]上的数字是否相同。
*			2.2.1如果两个位置上方块的数字相同，则调用上面的moveTo函数将找到的方块移动到当前待处理的位置[0,0]上,
*				 并修改相应的循环变量，使待处理位置变为下一个要处理的位置[0,1]，避免连续的三个2变成8等类似的情况发生。
*			2.2.2如果两个位置上的方块的数字不同，则将找到的方块移动到当前待处理的方块[0,0]右边的一个位置[0,1]上
*				(也可以不做任何处理,因为循环继续进行的时候自然会处理到后面的每一个位置)
*	3.处理完第一个位置后，循环变量自增指向后面一个待处理的位置，处理逻辑与第一个一样。
* 
*  总结：不管往哪个方向移动，逻辑都是一样的，都是首先考虑移动方向上最前面的方块，然后在后面找方块来跟它合并或仅仅移动到当前位置。
*/
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
					++c;
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
					--c;
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
					--r;
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
					++r;
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


//检查游戏是否结束，当且仅当所有位置都填满了数字并且没有两个相邻位置上的数字相等时游戏结束。
//也就是再也无法移动的时候游戏结束
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


//游戏结束
void My2048::gameOver() {
	if (_maxScore < _curScore) {
		_maxScore = _curScore; //更新最高得分
	}
	_curScore = 0;		//更新当前得分
	My2048 * game = this;
	log("\nGameOver!\n");
	
	//让所有卡片都移出到窗体外面去，然后从游戏中删除
	for (int r = 0; r < _row; ++r) {
		for (int c = 0; c < _col; ++c) {
			if (_cards[r][c]) {
				_cards[r][c]->runAction(
					Sequence::create(
//						EaseOut::create(
							//目标位置是根据下标随意计算出来的，目的是移出游戏界面
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

/*
1.累计得分。
2.将新的得分更新显示到label上
3.执行一个显示加分的动画
*/
void My2048::addScore(int add) {
	_curScore += add;
	_label_curScore->setString(String::createWithFormat("%d", _curScore)->getCString());
	if (_curScore > _maxScore) {
		_maxScore = _curScore;
		_label_maxScore->setString(String::createWithFormat("%d",_maxScore)->getCString());
		CCUserDefault::getInstance()->setIntegerForKey("MaxScore", _maxScore);
		CCUserDefault::getInstance()->flush();
	}
	auto label = LabelTTF::create();
	label->setString(String::createWithFormat("+%d", add)->getCString());
	label->setFontSize(25);
	label->setPosition(_label_curScore->getPositionX()+20, _label_curScore->getPositionY()+30);
	label->setColor(Color3B(125,250,146));
	this->addChild(label);

	//同时执行移动，放大，淡出得动作。
	auto spawn = Spawn::create(
		MoveBy::create(2, ccp(10, 35)),
		ScaleBy::create(2, 2),
		FadeOut::create(2),
		NULL);
	label->runAction(
		Sequence::create(
			spawn, //执行动画动作，
			CCCallFuncN::create(CC_CALLBACK_0(LabelTTF::removeFromParent, label)), //然后从游戏中删除。
			NULL)
	);
}

//在给定位置添加卡片，以3:7的比例随机分配数字2或者4
void My2048::addCard(int x,int y) {
	auto card = Card::createCard(cardSize, cardSize);
	if (rand() % 10 >= 7) {
		card->setNum(4);
	}
	else {
		card->setNum(2);
	}
	card->setPosition(_bgCards[x][y]->getPosition());
	_cards[x][y] = card;
	this->addChild(card);
}