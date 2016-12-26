#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "Character.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    
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

	// ============== TEST FLOOR =====================
	DrawNode *ground = DrawNode::create();
	ground->drawSolidCircle(Vec2::ZERO, 10.0f, 0.0f, 10, Color4F::BLUE);
	ground->setPosition(Vec2(50, 300));

	// floor physics
	auto groundBody = PhysicsBody::create();
	groundBody->setDynamic(false);
	ground->setPhysicsBody(groundBody);
	#include "Generated/TestLevel.h"

	addChild(ground);

	// ============== TEST CHARACTER =================
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

	character->setPosition(Vec2(visibleSize.width/10 + origin.x, visibleSize.height/2 + origin.y));
	addChild(character);
	
	// character physics
	auto cphys = PhysicsBody::createBox(Size(76.f, 170.f));
	cphys->setDynamic(true);
	character->setPhysicsBody(cphys);

	// background color ... might wanna manage this elsewhere as well (a Level class or something)
	Director::getInstance()->setClearColor(Color4F::BLACK);

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
