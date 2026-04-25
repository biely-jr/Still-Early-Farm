#pragma once
#include "../Core/Drawable.h"
#include "../Config/GameConfig.h"

class BudgetbarIcon : public Drawable
{
public:
	string image_path;
	BudgetbarIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void draw() const override;
	virtual void onClick() = 0;

	virtual void moveAnimals() {} 
};

class ChickIcon : public BudgetbarIcon
{
public:
	ChickIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick() override;
	virtual void moveAnimals() override; 
};

class CowIcon : public BudgetbarIcon
{
public:
	CowIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick() override;
	virtual void moveAnimals() override;
};

enum ANIMAL_ICONS
{
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

	void moveAnimals();
};
