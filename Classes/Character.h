#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "cocos2d.h"

enum Shade {
	WHITE,
	BLACK
};

class Character : public cocos2d::Node {
public:
    CREATE_FUNC(Character);

	virtual bool init();

	void setShade(const Shade shade);
	cocos2d::Color4F shadeColor() const;

private:
	Shade shade;
	cocos2d::DrawNode *_idleFrame;
};

#endif __CHARACTER_H__
