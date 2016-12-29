#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "cocos2d.h"
#include <vector>
#include <unordered_map>

// TODO some of these typedefs and things should be moved to a more
// central location

namespace PhysicsCategory {
	const int NONE    = 0;
	const int ENTITY  = (1 << 0);
	const int TERRAIN = (1 << 1);
	const int ALL     = 0xFFFFFFFF;
}

namespace PhysicsTag {
	const int NONE = 0;

	// Any shape/body with this tag MUST have a node catstable to Character
	const int CHARACTER = (1 << 0);
}

enum Shade {
	WHITE,
	BLACK
};

typedef cocos2d::EventKeyboard::KeyCode Key;

class Character : public cocos2d::Node {
public:
    CREATE_FUNC(Character);

	virtual bool init();
	virtual void update(float delta);

	void keyPressed(Key keyCode);
	void keyReleased(Key keyCode);

	void setShade(const Shade shade);
	cocos2d::Color4F shadeColor() const;

	void onCollide(cocos2d::PhysicsContact &contact, cocos2d::PhysicsShape &other);
	void onSeparate(cocos2d::PhysicsContact &contact, cocos2d::PhysicsShape &other);

	float getXAxis() const;

private:
	void setKeyAxisX(const Key key, const float value);

	std::vector<float> xAxis;
	std::unordered_map<Key, size_t> xAxisKeys;
	// Current move speed
	float moveSpeed;
	// Maximum move speed (more or less constant)
	float maxMoveSpeed;

	cocos2d::PhysicsShape *blockingLeft;
	cocos2d::PhysicsShape *blockingRight;

	Shade shade;
	cocos2d::DrawNode *_idleFrame;
};

#endif __CHARACTER_H__
