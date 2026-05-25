#pragma once
#include "../CMUgraphicsLib/CMUgraphics.h"

class Game;

struct point
{
	int x;
	int y;
};

class Drawable
{
protected:
	Game* pGame;
	point RefPoint;
	int width;
	int height;

public:
	Drawable(Game* r_pGame, point r_point, int r_width, int r_height);
	virtual ~Drawable() {}
	virtual void draw() const = 0;
	point getRefPoint() const { return RefPoint; }
	int getWidth() const { return width; }
	int getHeight() const { return height; }
};
