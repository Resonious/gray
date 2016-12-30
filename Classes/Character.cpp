#include "Character.h"

USING_NS_CC;

#define FEQ(f1, f2) (std::fabs((f1) - (f2)) < 0.0001)

bool Character::init() {
	if (!Node::init()) return false;

	setCascadeColorEnabled(true);

	_idleFrame = DrawNode::create();
	setShade(WHITE);
	addChild(_idleFrame);
	
	// Movement and jump
	moveSpeed = 0.0f;
	maxMoveSpeed = 200.0f;
	wantJump = false;
	jumping = false;
	jumpTimer = 0.0f;
	jumpTime = 0.5f;
	jumpCancelTime = 0.20f;
	jumpStrength = 550.0f;
	gravitySpeed = 0.0f;
	maxGravitySpeed = -510.0f;
	gravityAccel = maxGravitySpeed * 2.0f;

	// Physics
	auto cphys = PhysicsBody::createBox(Size(76.f, 190.f));
	cphys->setDynamic(true);
	cphys->setRotationEnable(false);
	cphys->setGravityEnable(false);
	cphys->setContactTestBitmask(PhysicsCategory::TERRAIN);
	cphys->setCategoryBitmask(PhysicsCategory::ENTITY);
	cphys->setTag(PhysicsTag::CHARACTER);
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

	Node::update(delta);

	auto body = getPhysicsBody();
	auto moveAmount = getXAxis();
	auto vel = body->getVelocity();

	// ======== Input movement =========

	// Don't accelerate into walls
	if (moveAmount > 0.0f && blockingRight)
		moveAmount = 0.0f;
	else if (moveAmount < 0.0f && blockingLeft)
		moveAmount = 0.0f;

	// ============ Gravity ============
	float gravityAmount = ground ?
		0.0f :
		gravityAccel * delta;

	// ============ Jumping ============
	if (jumping) {
		jumpTimer -= delta;
		if (jumpTimer <= 0.0f)
			jumping = false;
	}
	if (wantJump) {
		if (ground && !jumping) {
			// Initiate jump
			ground = nullptr;
			jumping = true;
			jumpTimer = jumpTime;
			gravitySpeed = jumpStrength;
		}
	}
	else if (jumping && !ground && jumpTimer > 0.0f && jumpTimer <= (jumpTime - jumpCancelTime)) {
		// Jump button has been released -- add extra gravity to shorten the jump
		gravityAmount += (gravityAccel * 10.0f) * delta;
		jumpTimer = 0.0f;
		jumping = false;
	}

	// =============== Apply movement ===============
	if (ground) {
		gravitySpeed = 0.0f;
	}
	else {
		gravitySpeed += gravityAmount;
		if (gravitySpeed < maxGravitySpeed)
			gravitySpeed = maxGravitySpeed;
	}

	moveSpeed = moveAmount * maxMoveSpeed; // TODO accelerate a little

	body->setVelocity(Vec2(moveSpeed, gravitySpeed));
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

void Character::resetJump() {
	wantJump = false;
	jumping = false;
	jumpTimer = 0.0f;
}

void Character::onCollide(PhysicsContact &contact, PhysicsShape &other) {
	const float pi = (float)std::_Pi;

	auto norm = contact.getContactData()->normal;
	if (FEQ(norm.x, -1.0f)) {
		blockingRight = &other;
	}
	else if (FEQ(norm.x, 1.0f)) {
		blockingLeft = &other;
	}
	else {
		auto angle = norm.getAngle();

		if (angle > 0.0f && angle < 2 * pi) {
			ground = &other;
			groundNormal = norm;
			resetJump();
		}
	}
}

void Character::onSeparate(PhysicsContact &contact, PhysicsShape &other) {
	if (&other == blockingLeft)
		blockingLeft = nullptr;

	if (&other == blockingRight)
		blockingRight = nullptr;

	if (&other == ground) {
		ground = nullptr;
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

	case Key::KEY_UP_ARROW: case Key::KEY_W:
		wantJump = true;
		break;
	}
}
void Character::keyReleased(Key keyCode) {
	switch (keyCode) {
	case Key::KEY_LEFT_ARROW: case Key::KEY_RIGHT_ARROW:
	case Key::KEY_A:          case Key::KEY_D:
		setKeyAxisX(keyCode, 0.0f);
		break;

	case Key::KEY_UP_ARROW: case Key::KEY_W:
		wantJump = false;
		break;
	}
}
