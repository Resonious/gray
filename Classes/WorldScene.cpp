#include "WorldScene.h"
#include "SimpleAudioEngine.h"
#include "Character.h"

USING_NS_CC;

Scene* World::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    
    // 'layer' is an autorelease object
    auto layer = World::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // DPI stuff for windows
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	// TODO ...
	// ok turns out setting scale on the scene causes trouble with bounding boxes to some degree
#endif

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool World::init()
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
        CC_CALLBACK_1(World::menuCloseCallback, this));

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
	groundBody->setContactTestBitmask(PhysicsCategory::ALL);
	groundBody->setCategoryBitmask(PhysicsCategory::TERRAIN);

	ground->setName("Ground");
    addChild(ground);

    // ============== TEST CHARACTER =================
    Character *character = Character::create();

    auto eventListener = EventListenerKeyboard::create();
    eventListener->onKeyReleased = [](EventKeyboard::KeyCode keyCode, Event* event) {
        reinterpret_cast<Character*>(event->getCurrentTarget())->keyReleased(keyCode);
    };
    eventListener->onKeyPressed = [groundBody](EventKeyboard::KeyCode keyCode, Event* event) {
        reinterpret_cast<Character*>(event->getCurrentTarget())->keyPressed(keyCode);
    };
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener,character);

    character->setPosition(Vec2(visibleSize.width/10 + origin.x, visibleSize.height/1.1f + origin.y));
    addChild(character);

    // background color ... might wanna manage this elsewhere as well (a Level class or something)
    Director::getInstance()->setClearColor(Color4F::WHITE);

	// ============== COLLISION EVENTS ===============
	auto contactListener = EventListenerPhysicsContact::create();

	contactListener->onContactSeparate = [](PhysicsContact &contact) {
		auto shapeA = contact.getShapeA();
		auto shapeB = contact.getShapeB();

		// Run character separate callback if either of these shapes is a character
		if (shapeA->getBody()->getTag() & PhysicsTag::CHARACTER)
			reinterpret_cast<Character*>(shapeA->getBody()->getNode())
			    ->onSeparate(contact, *shapeB);

		else if (shapeB->getBody()->getTag() & PhysicsTag::CHARACTER)
			reinterpret_cast<Character*>(shapeB->getBody()->getNode())
			    ->onSeparate(contact, *shapeA);

		return true;
	};

	contactListener->onContactBegin = [](PhysicsContact &contact) {
		auto shapeA = contact.getShapeA();
		auto shapeB = contact.getShapeB();

		// Run character collision callback if either of these shapes is a character
		if (shapeA->getBody()->getTag() & PhysicsTag::CHARACTER)
			reinterpret_cast<Character*>(shapeA->getBody()->getNode())
			    ->onCollide(contact, *shapeB);

		else if (shapeB->getBody()->getTag() & PhysicsTag::CHARACTER)
			reinterpret_cast<Character*>(shapeB->getBody()->getNode())
			    ->onCollide(contact, *shapeA);

		return true;
	};
	getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);

    return true;
}


void World::menuCloseCallback(Ref* pSender)
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
