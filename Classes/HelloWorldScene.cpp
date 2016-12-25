#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "Character.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	/////////////////////////////
	// 2. add a menu item with "X" image, which is clicked to quit the program
	//    you may modify it.

	// add a "close" icon to exit the progress. it's an autorelease object
	auto closeItem = MenuItemImage::create(
		"CloseNormal.png",
		"CloseSelected.png",
		CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width / 2,
		origin.y + closeItem->getContentSize().height / 2));

	// create menu, it's an autorelease object
	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	/////////////////////////////
	// 3. add your codes below...

	// add a label shows "Hello World"
	// create and initialize a label

	auto label = Label::createWithTTF("Hello World", "fonts/Marker Felt.ttf", 24);

	// position the label on the center of the screen
	label->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - label->getContentSize().height));

	// add the label as a child to this layer
	this->addChild(label, 1);

	// add "HelloWorld" splash screen"
	// auto sprite = Sprite::create("HelloWorld.png");

	// position the sprite on the center of the screen
	// sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

	// add the sprite as a child to this layer
	// this->addChild(sprite, 0);

	/*
	DrawNode *draw = DrawNode::create();

	// Triangles for body-walk1
	draw->drawTriangle(Vec2(9.1792500000f, -17.8788100000f), Vec2(-20.8324300000f, -29.4415900000f), Vec2(-24.2005400000f, -22.2094500000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(9.1792500000f, -17.8788100000f), Vec2(-24.2005400000f, -22.2094500000f), Vec2(-2.9816400000f, -17.1539000000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(9.1792500000f, -17.8788100000f), Vec2(-2.9816400000f, -17.1539000000f), Vec2(-6.1051000000f, -2.0961500000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(9.1792500000f, -17.8788100000f), Vec2(-6.1051000000f, -2.0961500000f), Vec2(2.1618100000f, -17.5353400000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(9.1792500000f, -17.8788100000f), Vec2(2.1618100000f, -17.5353400000f), Vec2(-13.8743600000f, -44.7227900000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(9.1792500000f, -17.8788100000f), Vec2(-13.8743600000f, -44.7227900000f), Vec2(-20.5666200000f, -40.6190400000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(9.1792500000f, -17.8788100000f), Vec2(-20.5666200000f, -40.6190400000f), Vec2(-7.6184800000f, -17.8338300000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(9.1792500000f, -17.8788100000f), Vec2(-7.6184800000f, -17.8338300000f), Vec2(-14.3936600000f, 1.2589500000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(9.1792500000f, -17.8788100000f), Vec2(-14.3936600000f, 1.2589500000f), Vec2(-14.4388600000f, 10.6570200000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(-14.4388600000f, 10.6570200000f), Vec2(-12.7342400000f, 21.7687000000f), Vec2(-12.6079600000f, 11.9828500000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(-14.4388600000f, 10.6570200000f), Vec2(-12.6079600000f, 11.9828500000f), Vec2(-7.4940700000f, -0.0758500000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(-14.4388600000f, 10.6570200000f), Vec2(-7.4940700000f, -0.0758500000f), Vec2(-13.6812500000f, -1.8436100000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(-14.4388600000f, 10.6570200000f), Vec2(-13.6812500000f, -1.8436100000f), Vec2(-17.2167800000f, 12.5510600000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(-14.4388600000f, 10.6570200000f), Vec2(-17.2167800000f, 12.5510600000f), Vec2(-13.8075200000f, 28.8397700000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(-14.4388600000f, 10.6570200000f), Vec2(-13.8075200000f, 28.8397700000f), Vec2(-10.0176000000f, 34.2368400000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(-14.4388600000f, 10.6570200000f), Vec2(-10.0176000000f, 34.2368400000f), Vec2(-5.7569400000f, 33.2465000000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(-14.4388600000f, 10.6570200000f), Vec2(-5.7569400000f, 33.2465000000f), Vec2(-0.9280800000f, 28.3978300000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(-14.4388600000f, 10.6570200000f), Vec2(-0.9280800000f, 28.3978300000f), Vec2(1.6055100000f, 9.9401200000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(-3.0294700000f, 10.8770600000f), Vec2(-3.0224700000f, 22.6895600000f), Vec2(-2.5263700000f, 8.2751000000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(-0.6755400000f, -1.3385400000f), Vec2(-3.0294700000f, 10.8770600000f), Vec2(-2.5263700000f, 8.2751000000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(0.0000000000f, 0.0000000000f), Vec2(9.1792500000f, -17.8788100000f), Vec2(-14.4388600000f, 10.6570200000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(-2.5263700000f, 8.2751000000f), Vec2(0.0000000000f, 0.0000000000f), Vec2(-14.4388600000f, 10.6570200000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(-0.6755400000f, -1.3385400000f), Vec2(-2.5263700000f, 8.2751000000f), Vec2(-14.4388600000f, 10.6570200000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(4.8171600000f, 0.0504200000f), Vec2(-0.6755400000f, -1.3385400000f), Vec2(-14.4388600000f, 10.6570200000f), Color4F::WHITE);
	draw->drawTriangle(Vec2(4.8171600000f, 0.0504200000f), Vec2(-14.4388600000f, 10.6570200000f), Vec2(1.6055100000f, 9.9401200000f), Color4F::WHITE);
	// Circle for head-walk1
	draw->drawSolidCircle(Vec2(-3.1522400000f, 45.4130000000f), 13.709299f, 0.0f, 20, Color4F::WHITE);

	draw->setScale(0.7f);
	draw->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height/2));
	addChild(draw);
	*/

	Character *character = Character::create();

	auto eventListener = EventListenerKeyboard::create();
	eventListener->onKeyPressed = [character](EventKeyboard::KeyCode keyCode, Event* event) {

		Vec2 loc = event->getCurrentTarget()->getPosition();
		switch (keyCode) {
		case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		case EventKeyboard::KeyCode::KEY_A:
			event->getCurrentTarget()->setPosition(--loc.x, loc.y);
			break;
		case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		case EventKeyboard::KeyCode::KEY_D:
			event->getCurrentTarget()->setPosition(++loc.x, loc.y);
			break;
		case EventKeyboard::KeyCode::KEY_UP_ARROW:
		case EventKeyboard::KeyCode::KEY_W:
			event->getCurrentTarget()->setPosition(loc.x, ++loc.y);
			break;
		case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		case EventKeyboard::KeyCode::KEY_S:
			event->getCurrentTarget()->setPosition(loc.x, --loc.y);
			break;

		case EventKeyboard::KeyCode::KEY_0: {
			auto rot = event->getCurrentTarget()->getRotation();
			event->getCurrentTarget()->setRotation(rot + 1.0f);
		} break;
		case EventKeyboard::KeyCode::KEY_9: {
			auto rot = event->getCurrentTarget()->getRotation();
			event->getCurrentTarget()->setRotation(rot - 1.0f);
		} break;

		case EventKeyboard::KeyCode::KEY_1:
			character->setShade(WHITE);
			break;
		case EventKeyboard::KeyCode::KEY_2:
			character->setShade(BLACK);
			break;
		}
	};
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener,character);

	character->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
	addChild(character);

    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
    
    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/
    
    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
    
    
}
