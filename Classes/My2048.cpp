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

	//��ӵ���¼�
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(My2048::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(My2048::onTouchEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener,this);
	
	//��Ӽ����¼�
	auto keyboardEvent = EventListenerKeyboard::create();
	keyboardEvent->onKeyPressed = CC_CALLBACK_2(My2048::onKeyPressed,this);
	_eventDispatcher->addEventListenerWithFixedPriority(keyboardEvent, 1);

	//��ʾ��ߵ÷�
	srand(time(0));
	_curScore = 0;
	_maxScore = CCUserDefault::getInstance()->getIntegerForKey("MaxScore",0); //��ȡ�û�����
	auto label = CCLabelTTF::create("MaxScore: ", "fonts/arial.ttf", 28);//font size 24
	label->setAnchorPoint(ccp(0,0));
	label->setPosition(20, visibleSize.height - 40);
	this->addChild(label);
	_label_maxScore = CCLabelTTF::create("0", "fonts/arial.ttf", 28);
	_label_maxScore->setAnchorPoint(ccp(0, 0));
	_label_maxScore->setPosition(20 + label->getContentSize().width, label->getPositionY());
	_label_maxScore->setString(String::createWithFormat("%d", _maxScore)->getCString());
	this->addChild(_label_maxScore);
	
	//��ʾ��ǰ�÷�
	label = CCLabelTTF::create("Score: ", "fonts/arial.ttf", 25); //font size 20
	label->setAnchorPoint(ccp(0, 0));
	label->setPosition(20, _label_maxScore->getPositionY() - 40);
	this->addChild(label);
	_label_curScore = CCLabelTTF::create("0", "fonts/arial.ttf", 25);
	_label_curScore->setAnchorPoint(ccp(0, 0));
	_label_curScore->setPosition(20+label->getContentSize().width, label->getPositionY());
	this->addChild(_label_curScore);
	
	initTable(5, 4); //��ʼ������ 5��4��
	startGame();    //��ʼ��Ϸ

    return true;
}

//���ݸ�������������Ϸ����
void My2048::initTable(int row,int col){
	_row = row;
	_col = col;
	Color4B bgcolor = Color4B(187, 173, 160, 255); //�����������ɫ
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	cardSize = visibleSize.width / (col+1); //��������������ʴ�С�ķ���ߴ�
	const int gap = 10; //�����뷽��֮��ļ��
	int x, y=30;

	//����tabel,������table�и��������λ��
	for (int i = 0; i < row; ++i) {
		x = 16;//��߿�16������
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

//��table�����������λ�����������Ƭ�����ֱַ�Ϊ2��4
void My2048::startGame() {
	int tx = rand() % _row;
	int ty = rand() % _col;
	_cards[tx][ty] = Card::createCard(cardSize, cardSize);
	_cards[tx][ty]->setNum(2);
	_cards[tx][ty]->setPosition(_bgCards[tx][ty]->getPosition());
	this->addChild(_cards[tx][ty]);

	int tx2, ty2;
	//��ѭ����Ϊ�˱�������ҵ�������λ����һ��λ��
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
	//X���ϵķ�������Y���ϵķ�������Ϊ���һ���������Ϊ���»���
	if (abs(_diffx) > abs(_diffy)) {
		if (abs(_diffx) < SLIDE_DIST) return; //�����ľ��벻�����Ļ�����Ϊû�л���
		if (_diffx > 0 ) { // from right to left
			moveLeft();
		}
		else {
			moveRight();
		}
	}
	else {
		if (abs(_diffy) < SLIDE_DIST) return; //�������벻��������Ϊδ����
		if (_diffy > 0) { //from high place to low place
			moveDown();
		}
		else {
			moveUp();
		}
	}
}

//�ü��̵��������Ҽ�ͷ���ƻ���
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


//��table��[fx,fy]λ���ϵĿ�Ƭ�ƶ��� [tx,ty]λ����ȥ�����Ŀ��λ�����п�Ƭ��˵������Ƭ�������
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
	//���Ŀ��λ�����п�Ʊ��˵������Ƭ���Ժϲ�
	if (to) {
	//	if (from->getNum() != to->getNum()) { log("two number are different");  return false; } //for debug
		this->addScore(from->getNum() * 2); //���ӵ÷֣���ֵΪ������Ƭ�ϵ�����֮��
		auto move = MoveTo::create(0.2, to->getPosition());
		auto fade = FadeOut::create(0.2);
		auto scale = ScaleTo::create(0.2, 1.5);

		//fade������scale����ͬʱ����
		auto spawn = Spawn::create(
			fade,
			scale,
			NULL
		); 
	//	from->setAnchorPoint(ccp(0.5, 0.5));

		auto seq = Sequence::create(
				move, //��Ʊ���ƶ���Ŀ��λ��
				spawn,  //Ȼ��߷Ŵ�ߵ���
				CCCallFunc::create(CC_CALLBACK_0(Card::doubleNum,to)), //Ȼ����Ŀ��λ�õĿ�Ƭ��������
				CCCallFunc::create(CC_CALLBACK_0(Card::removeFromParent, from)), //Ȼ���ƶ��Ŀ�Ƭ����Ϸ������ɾ��
				NULL);
		from->runAction(seq);
		_cards[fx][fy] = nullptr; //��fromλ���ÿ�ֵ
	}
	//���Ŀ��λ����û�п�Ƭ��˵��ֻ��Ҫ��fromλ�õĿ�Ƭ�ƶ���toλ��
	else {
		to = _bgCards[tx][ty];
		auto move = MoveTo::create(0.2, to->getPosition());
		from->runAction(move);
		_cards[tx][ty] = from;
		_cards[fx][fy] = nullptr;  //��fromλ���ÿ�ֵ
	}
	return true;
}


/*
����Ļ����߼�:
* �����ĸ������ϵĻ����߼�������ͬ�ģ����������ֻ�����󻬶�Ϊ��˵һ�»������߼�ʵ�֡�
* ����ÿһ�еĻ����߼��͵�һ������ȫһ���ģ�����ֻ�Ե�һ�еĻ����߼�Ϊ�����н��⡣
* �����ƶ���ʱ�����ȴ���[0,0]λ�ã����������£�
*	1.�ڸ÷����ұ���һ�����ֲ�Ϊ0�ķ��飬����ҵ��ˣ��ͽ�������Ĳ��������򲻽����κβ���
*   2.�жϵ�ǰҪ�����λ��[0,0]���Ƿ��з��顣
*		2.1���û�з��飬���ҵ��ķ����ƶ�����ǰ�������λ��[0,0].
*		2.2����з��飬���ж��ҵ��ķ�������ֺ͵�ǰ������λ��[0,0]�ϵ������Ƿ���ͬ��
*			2.2.1�������λ���Ϸ����������ͬ������������moveTo�������ҵ��ķ����ƶ�����ǰ�������λ��[0,0]��,
*				 ���޸���Ӧ��ѭ��������ʹ������λ�ñ�Ϊ��һ��Ҫ�����λ��[0,1]����������������2���8�����Ƶ����������
*			2.2.2�������λ���ϵķ�������ֲ�ͬ�����ҵ��ķ����ƶ�����ǰ������ķ���[0,0]�ұߵ�һ��λ��[0,1]��
*				(Ҳ���Բ����κδ���,��Ϊѭ���������е�ʱ����Ȼ�ᴦ�������ÿһ��λ��)
*	3.�������һ��λ�ú�ѭ����������ָ�����һ���������λ�ã������߼����һ��һ����
* 
*  �ܽ᣺�������ĸ������ƶ����߼�����һ���ģ��������ȿ����ƶ���������ǰ��ķ��飬Ȼ���ں����ҷ����������ϲ�������ƶ�����ǰλ�á�
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
		//�ڵ�_col - 1����������µķ���
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
		//�ڵ�0����������µķ���
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
		//�ڵ�0����������µķ���
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
		//�ڵ�_row-1����������µķ���
		std::vector<int> v;
		for (int k = 0; k < _col; ++k) if (_cards[_row - 1][k] == 0) v.push_back(k);
		if (v.size()) this->addCard(_row - 1, v[rand() % v.size()]);
	}
} // move down


//�����Ϸ�Ƿ���������ҽ�������λ�ö����������ֲ���û����������λ���ϵ��������ʱ��Ϸ������
//Ҳ������Ҳ�޷��ƶ���ʱ����Ϸ����
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


//��Ϸ����
void My2048::gameOver() {
	if (_maxScore < _curScore) {
		_maxScore = _curScore; //������ߵ÷�
	}
	_curScore = 0;		//���µ�ǰ�÷�
	My2048 * game = this;
	log("\nGameOver!\n");
	
	//�����п�Ƭ���Ƴ�����������ȥ��Ȼ�����Ϸ��ɾ��
	for (int r = 0; r < _row; ++r) {
		for (int c = 0; c < _col; ++c) {
			if (_cards[r][c]) {
				_cards[r][c]->runAction(
					Sequence::create(
//						EaseOut::create(
							//Ŀ��λ���Ǹ����±������������ģ�Ŀ�����Ƴ���Ϸ����
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
1.�ۼƵ÷֡�
2.���µĵ÷ָ�����ʾ��label��
3.ִ��һ����ʾ�ӷֵĶ���
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

	//ͬʱִ���ƶ����Ŵ󣬵����ö�����
	auto spawn = Spawn::create(
		MoveBy::create(2, ccp(10, 35)),
		ScaleBy::create(2, 2),
		FadeOut::create(2),
		NULL);
	label->runAction(
		Sequence::create(
			spawn, //ִ�ж���������
			CCCallFuncN::create(CC_CALLBACK_0(LabelTTF::removeFromParent, label)), //Ȼ�����Ϸ��ɾ����
			NULL)
	);
}

//�ڸ���λ����ӿ�Ƭ����3:7�ı��������������2����4
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