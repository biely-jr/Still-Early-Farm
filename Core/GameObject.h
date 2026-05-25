#pragma once

#include "Drawable.h"

class GameObject : public Drawable
{
protected:
	color fillColor;
	color borderColor;

public:
	GameObject(Game* r_pGame, point ref, int r_width, int r_height, color fc, color bc);
	virtual ~GameObject() {}

	void setRefPoint(point p);
	bool CollisionDetection(const GameObject& gObj);
};
