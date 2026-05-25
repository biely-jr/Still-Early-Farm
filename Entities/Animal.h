#pragma once
#include "../Core/Drawable.h"
#include <string>

class Animal :public Drawable
{
private:
	string image_path;
	int productionElapsedSeconds;
	int productionIntervalSeconds;
	string productLabel;
public:
	point curr_vel;
	Animal(Game* r_pGame, point r_point, int r_width, int r_height, string img_path, int productIntervalSeconds, const string& productName);
	virtual void draw() const override;
	virtual void moveStep() = 0;
	bool advanceProduction(int elapsedSeconds);
	int getProductionCounter() const;
	int getProductionInterval() const;
	string getProductLabel() const;
	void addProductionTime(int seconds);
};

class Chick : public Animal
{
public:
	Chick(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void moveStep();
};

class Cow : public Animal
{
public:
	Cow(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void moveStep();
};
