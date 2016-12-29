#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "cocos2d.h"
#include <vector>
#include <unordered_map>

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

	float getXAxis() const;

private:
	void setKeyAxisX(const Key key, const float value);

	std::vector<float> xAxis;
	std::unordered_map<Key, size_t> xAxisKeys;
	bool stopped;

	Shade shade;
	cocos2d::DrawNode *_idleFrame;
};

#endif __CHARACTER_H__
