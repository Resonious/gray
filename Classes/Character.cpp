#include "Character.h"

USING_NS_CC;

#define FEQ(f1, f2) (std::fabsf((f1) - (f2)) < 0.0001)
#define SIGNUM(f) (((f) < 0.0f) ? -1.0f : 1.0f)

const static float pi = (float)std::_Pi;

bool Character::init() {
	if (!Node::init()) return false;

	setCascadeColorEnabled(true);

	_idleFrame = DrawNode::create();
	setShade(BLACK);
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

	blockingRight = nullptr;
	blockingLeft = nullptr;

	// Physics
	auto cphys = PhysicsBody::createBox(Size(41.f, 105.0f), PHYSICSBODY_MATERIAL_DEFAULT, Vec2(0.0f, -37.0f));
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
	auto moveDir = getXAxis();
	auto vel = body->getVelocity();

	// ======== Input movement =========

	// Don't accelerate into walls
	if (moveDir > 0.0f && blockingRight)
		moveDir = 0.0f;
	else if (moveDir < 0.0f && blockingLeft)
		moveDir = 0.0f;

	// ============ Gravity ============
	float gravityAmount = isGrounded() ?
		0.0f :
		gravityAccel * delta;

	// ============ Jumping ============
	if (jumping) {
		jumpTimer -= delta;
		if (jumpTimer <= 0.0f)
			jumping = false;
	}
	if (wantJump) {
		if (isGrounded() && !jumping) {
			// Initiate jump
			groundShapes.clear();
			jumping = true;
			jumpTimer = jumpTime;
			gravitySpeed = jumpStrength;
		}
	}
	else if (jumping && !isGrounded() && jumpTimer > 0.0f && jumpTimer <= (jumpTime - jumpCancelTime)) {
		// Jump button has been released -- add extra gravity to shorten the jump
		gravityAmount += (gravityAccel * 10.0f) * delta;
		jumpTimer = 0.0f;
		jumping = false;
	}

	// =============== Apply movement ===============
	if (isGrounded()) {
		gravitySpeed = 0.0f;
	}
	else {
		gravitySpeed += gravityAmount;
		if (gravitySpeed < maxGravitySpeed)
			gravitySpeed = maxGravitySpeed;
	}

	moveSpeed = std::fabsf(moveDir) * maxMoveSpeed;

	Vec2 forward = Vec2::ZERO;
	if (isGrounded()) {
		// When grounded, we move in the direction of the steepest ground
		for (auto &shapenorm : groundShapes) {
			Vec2 candidate = shapenorm.second.rotateByAngle(Vec2::ZERO, -SIGNUM(moveDir) * (pi / 2.0f));
			if (forward.isZero() || candidate.y > forward.y)
				forward = candidate;
		}
	}
	else {
		// When airborne, we just move left or right
		forward = Vec2(SIGNUM(moveDir), 0.0f);
	}
	Vec2 up = Vec2::UNIT_Y;

	body->setVelocity(moveSpeed * forward  +  gravitySpeed * up);
}

bool Character::isGrounded() const {
	return !groundShapes.empty();
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
			groundShapes[&other] = norm;
			resetJump();
		}
	}
}

void Character::onSeparate(PhysicsContact &contact, PhysicsShape &other) {
	if (&other == blockingLeft)
		blockingLeft = nullptr;

	if (&other == blockingRight)
		blockingRight = nullptr;

	if (groundShapes.count(&other) > 0) {
		groundShapes.erase(&other);
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
