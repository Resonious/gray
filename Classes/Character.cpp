#include "Character.h"

USING_NS_CC;

bool Character::init() {
	if (!Node::init()) return false;

	setCascadeColorEnabled(true);

	_idleFrame = DrawNode::create();
	setShade(WHITE);
	addChild(_idleFrame);
	
	// Movement
	stopped = true;
	maxMoveSpeed = 200.0f;

	// Physics
	auto cphys = PhysicsBody::createBox(Size(76.f, 190.f));
	cphys->setDynamic(true);
	cphys->setRotationEnable(false);
	cphys->setLinearDamping(0.5f);
	cphys->setContactTestBitmask(PhysicsCategory::TERRAIN);
	cphys->setCategoryBitmask(PhysicsCategory::ENTITY);
	setPhysicsBody(cphys);

	return true;
}

float Character::getXAxis() const {
	float pos = 0.0f;
	float neg = 0.0f;

	for (auto &a : xAxis) {
		if (a < 0.0f) { if (a < neg) neg = a; }
		else          { if (a > pos) pos = a; }
	}

	return pos + neg;
}

void Character::update(float delta) {
	using std::signbit;
	const float moveImpulse = maxMoveSpeed * 100.0f;

	Node::update(delta);

	auto body = getPhysicsBody();
	auto xDir = getXAxis();
	auto vel  = body->getVelocity();
	auto move = [body,xDir,moveImpulse]() { body->applyImpulse(Vec2(xDir * moveImpulse, 0)); };

	if (xDir != 0.0f) {
		stopped = false;

		if (signbit(vel.x) == signbit(xDir)) {
			if (std::fabs(vel.x) < maxMoveSpeed)
				move();
		}
		else
			move();
	}
	else {
		if (!stopped && std::fabs(vel.x) < 7.0f) {
			body->setVelocity(Vec2(0.0f, vel.y));
			stopped = true;
		}
	}

	if (xDir == 0.0f && !stopped) {
		// Apply "stopping" force if needed
		const float opposingDirection = vel.x < 0.0f ? 1.0f : -1.0f;
		body->applyImpulse(Vec2(opposingDirection * moveImpulse, 0));
	}
}

void Character::setShade(const Shade shade) {
	this->shade = shade;
	auto color = shadeColor();
	_idleFrame->clear();
	#include "Generated/CharIdle.h"
	_idleFrame->drawSolidCircle(Vec2::ZERO, 10.0f, 0.0f, 10, Color4F::RED);
}

Color4F Character::shadeColor() const {
	switch (shade) {
	case WHITE: return Color4F::WHITE;
	case BLACK: return Color4F::BLACK;
	default:    return Color4F::RED; // <-- SHOULD NOT HAPPEN
	}
}

void Character::setKeyAxisX(const Key key, const float value) {
	if (xAxisKeys.count(key) <= 0) {
		xAxisKeys.insert({ key, xAxis.size() });
		xAxis.push_back(value);
	}
	else {
		xAxis[xAxisKeys[key]] = value;
	}
}

void Character::keyPressed(Key keyCode) {
	switch (keyCode) {
	case Key::KEY_LEFT_ARROW: case Key::KEY_A:
		setKeyAxisX(keyCode, -1.0f);
		break;

	case Key::KEY_RIGHT_ARROW: case Key::KEY_D:
		setKeyAxisX(keyCode, 1.0f);
		break;
	}
}
void Character::keyReleased(Key keyCode) {
	switch (keyCode) {
	case Key::KEY_LEFT_ARROW: case Key::KEY_RIGHT_ARROW:
	case Key::KEY_A:          case Key::KEY_D:
		setKeyAxisX(keyCode, 0.0f);
		break;
	}
}
