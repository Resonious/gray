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

	// ----------------------------------------------------------------------------------------------------------------------------
	// These events are to be called by the World to provide the Character with awareness lol

	void keyPressed(Key keyCode);
	void keyReleased(Key keyCode);

	void onCollide(cocos2d::PhysicsContact &contact, cocos2d::PhysicsShape &other);
	void onSeparate(cocos2d::PhysicsContact &contact, cocos2d::PhysicsShape &other);

	// ----------------------------------------------------------------------------------------------------------------------------
	// Shade related

	void setShade(const Shade shade);
	cocos2d::Color4F shadeColor() const;

	// ----------------------------------------------------------------------------------------------------------------------------
	// Returns a value from -1.0f to 1.0f indicating the analog value of the x axis input
	float getXAxis() const;

	bool isGrounded() const;

private:
	void setKeyAxisX(const Key key, const float value);
	void resetJump();

	// Left and right control axes read from input in keyPressed and keyReleased
	std::vector<float> xAxis;
	// Keep track of the input x axis for every possible key so that they can be summed up
	// to form the final axis.
	std::unordered_map<Key, size_t> xAxisKeys;

	// Whether the character's jump button is down
	bool wantJump;
	// Whether the current wantJump has been processed yet
	bool jumping;
	// Time (in seconds) until the character's jump is done
	float jumpTimer;
	// Time (in seconds) that jumpTimer is set to when a jump starts (more or less constant)
	float jumpTime;
	// Minimum time (in seconds) after which the jump gets canceled if the key is released
	float jumpCancelTime;
	// Strengh in velocity units of the initial jump (and somehow garbled into an impulse for linger)
	float jumpStrength;

	// Current move speed
	float moveSpeed;
	// Maximum move speed (more or less constant)
	float maxMoveSpeed;

	// Acceleration due to gravity
	float gravityAccel;
	// Current speed due to gravity
	float gravitySpeed;
	// Terminal velocity
	float maxGravitySpeed;

	// ----------------------------------------------------------------------------------------------------------------------------
	// If we setVelocity against the wall, we squeeze into the wall and it looks dumb,
	// so blockingLeft and blockingRight keep track of walls we're up against so that
	// we can know when to not setVelocity into the wall.

	cocos2d::PhysicsShape *blockingLeft;
	cocos2d::PhysicsShape *blockingRight;
	std::unordered_map<cocos2d::PhysicsShape*, cocos2d::Vec2> groundShapes;

	// ----------------------------------------------------------------------------------------------------------------------------
	// Appearance related things

	Shade shade;
	cocos2d::DrawNode *_idleFrame;
};

#endif __CHARACTER_H__
