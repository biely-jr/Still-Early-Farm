#pragma once
#include "../Core/Drawable.h"
#include "../Entities/Animal.h"
#include "../Config/GameConfig.h"

const int range_min_x = 50;
const int range_max_x = config.windWidth - 50;
const int range_min_y = (config.toolBarHeight * 2) + 50;
const int range_max_y = config.windHeight - config.statusBarHeight - 50;

class BudgetbarIcon : public Drawable
{
private:
public:
	string image_path;
	BudgetbarIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void draw() const override;
	virtual void onClick() = 0;
};

class ChickIcon : public BudgetbarIcon
{
public:
	ChickIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick() override;
};

class CowIcon : public BudgetbarIcon
{
public:
	CowIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick() override;
};

class FoodAreaIcon : public BudgetbarIcon
{
public:
	FoodAreaIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void draw() const override;
	virtual void onClick() override;
};

enum ANIMAL_ICONS
{
	ICON_FOOD_AREA,
	ICON_CHICK,
	ICON_COW,
	ANIMAL_COUNT
};

class Budgetbar : public Drawable
{
private:
	BudgetbarIcon** iconsList;
	string iconsImages[ANIMAL_COUNT];

public:
	Budgetbar(Game* r_pGame, point r_point, int r_width, int r_height);
	~Budgetbar();
	void draw() const override;
	bool handleClick(int x, int y);
};
