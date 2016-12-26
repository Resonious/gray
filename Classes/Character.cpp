#include "Character.h"

USING_NS_CC;

bool Character::init() {
	if (!Node::init()) return false;

	setCascadeColorEnabled(true);

	_idleFrame = DrawNode::create();
	setShade(WHITE);
	addChild(_idleFrame);

	return true;
}

void Character::setShade(const Shade shade) {
	this->shade = shade;
	auto color = shadeColor();
	_idleFrame->clear();
	#include "Generated/CharIdle.h"
}

Color4F Character::shadeColor() const {
	switch (shade) {
	case WHITE: return Color4F::WHITE;
	case BLACK: return Color4F::BLACK;
	default:    return Color4F::RED; // <-- SHOULD NOT HAPPEN
	}
}
