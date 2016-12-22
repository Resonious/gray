#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

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

	DrawNode *draw = DrawNode::create();
	// Triangles for body

	// Triangles for body
	draw->drawTriangle(Vec2(-1.7686300000000048, 7.07554), Vec2(0, 0), Vec2(3.55425, -19.03952), Color4F::BLUE);
	draw->drawTriangle(Vec2(-1.7686300000000048, 7.07554), Vec2(3.55425, -19.03952), Vec2(8.542110000000001, -44.90136), Color4F::BLUE);
	draw->drawTriangle(Vec2(-1.7686300000000048, 7.07554), Vec2(8.542110000000001, -44.90136), Vec2(-1.4331399999999999, -44.90136), Color4F::BLUE);
	draw->drawTriangle(Vec2(-1.7686300000000048, 7.07554), Vec2(-1.4331399999999999, -44.90136), Vec2(-3.0709299999999997, -20.18961), Color4F::BLUE);
	draw->drawTriangle(Vec2(-1.7686300000000048, 7.07554), Vec2(-3.0709299999999997, -20.18961), Vec2(-6.1051, -2.096149999999998), Color4F::BLUE);
	draw->drawTriangle(Vec2(-2.3278600000000047, 22.12135), Vec2(-1.7686300000000048, 7.07554), Vec2(-6.1051, -2.096149999999998), Color4F::BLUE);
	draw->drawTriangle(Vec2(0.6502999999999952, -1.7804800000000016), Vec2(-1.7036300000000049, 10.435119999999998), Vec2(-2.3278600000000047, 22.12135), Color4F::BLUE);
	draw->drawTriangle(Vec2(6.142999999999995, -0.39152000000000164), Vec2(0.6502999999999952, -1.7804800000000016), Vec2(-2.3278600000000047, 22.12135), Color4F::BLUE);
	draw->drawTriangle(Vec2(2.931349999999995, 9.498179999999998), Vec2(6.142999999999995, -0.39152000000000164), Vec2(-2.3278600000000047, 22.12135), Color4F::BLUE);
	draw->drawTriangle(Vec2(0.9659699999999951, 27.82962), Vec2(2.931349999999995, 9.498179999999998), Vec2(-2.3278600000000047, 22.12135), Color4F::BLUE);
	draw->drawTriangle(Vec2(-5.756930000000005, 33.2465), Vec2(0.9659699999999951, 27.82962), Vec2(-2.3278600000000047, 22.12135), Color4F::BLUE);
	draw->drawTriangle(Vec2(-10.080720000000005, 33.66863), Vec2(-5.756930000000005, 33.2465), Vec2(-2.3278600000000047, 22.12135), Color4F::BLUE);
	draw->drawTriangle(Vec2(-15.701550000000005, 28.20842), Vec2(-10.080720000000005, 33.66863), Vec2(-2.3278600000000047, 22.12135), Color4F::BLUE);
	draw->drawTriangle(Vec2(-21.320520000000002, -3.295699999999998), Vec2(-20.373500000000003, 11.604040000000001), Vec2(-15.701550000000005, 28.20842), Color4F::BLUE);
	draw->drawTriangle(Vec2(-15.385880000000002, -3.421979999999998), Vec2(-21.320520000000002, -3.295699999999998), Vec2(-15.701550000000005, 28.20842), Color4F::BLUE);
	draw->drawTriangle(Vec2(-15.259600000000002, 9.141800000000002), Vec2(-15.385880000000002, -3.421979999999998), Vec2(-15.701550000000005, 28.20842), Color4F::BLUE);
	draw->drawTriangle(Vec2(-13.491840000000002, 21.70557), Vec2(-15.259600000000002, 9.141800000000002), Vec2(-15.701550000000005, 28.20842), Color4F::BLUE);
	draw->drawTriangle(Vec2(-13.491840000000002, 21.70557), Vec2(-15.701550000000005, 28.20842), Vec2(-2.3278600000000047, 22.12135), Color4F::BLUE);
	draw->drawTriangle(Vec2(-13.491840000000002, 21.70557), Vec2(-2.3278600000000047, 22.12135), Vec2(-6.1051, -2.096149999999998), Color4F::BLUE);
	draw->drawTriangle(Vec2(-13.491840000000002, 21.70557), Vec2(-6.1051, -2.096149999999998), Vec2(-8.060410000000001, -19.47533), Color4F::BLUE);
	draw->drawTriangle(Vec2(-13.491840000000002, 21.70557), Vec2(-8.060410000000001, -19.47533), Vec2(-9.139270000000002, -44.470259999999996), Color4F::BLUE);
	draw->drawTriangle(Vec2(-9.139270000000002, -44.470259999999996), Vec2(-17.47303, -44.470259999999996), Vec2(-14.626410000000002, -18.970249999999997), Color4F::BLUE);
	draw->drawTriangle(Vec2(-9.139270000000002, -44.470259999999996), Vec2(-14.626410000000002, -18.970249999999997), Vec2(-14.393660000000002, 1.2589500000000022), Color4F::BLUE);
	draw->drawTriangle(Vec2(-9.139270000000002, -44.470259999999996), Vec2(-14.393660000000002, 1.2589500000000022), Vec2(-12.986760000000002, 11.667170000000002), Color4F::BLUE);
	draw->drawTriangle(Vec2(-13.491840000000002, 21.70557), Vec2(-9.139270000000002, -44.470259999999996), Vec2(-12.986760000000002, 11.667170000000002), Color4F::BLUE);

	draw->setScale(2.0f);
	draw->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height/2));
	addChild(draw);

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
