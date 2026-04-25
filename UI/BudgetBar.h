#pragma once
#include "../Core/Drawable.h"
#include "../Entities/Animal.h"
#include "../Config/GameConfig.h"

//Base class for all toolbar icons 
class BudgetbarIcon :public Drawable
{
private:
	//string image_path;
public:
	string image_path;
	BudgetbarIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void draw() const override;
	virtual void onClick() = 0;   //The action that should be taken when this icon is clicked
};

class ChickIcon : public BudgetbarIcon
{
public:
	ChickIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick();
};

class CowIcon : public BudgetbarIcon
{
public:
	CowIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void onClick();
};


// TO DO: The rest of icons in the toolbar

enum ANIMAL_ICONS //The icons of the toolbar (you should add more icons)
{
	//Note: Icons are ordered here as they appear in menu
	//If you want to change the menu icons order, change the order here
	ICON_CHICK,
	ICON_COW,

	ANIMAL_COUNT		//no. of menu icons ==> This should be the last line in this enum
};

class Budgetbar : public Drawable
{
private:
	BudgetbarIcon** iconsList; //an array of toolbarIcon pointers
	string iconsImages[ANIMAL_COUNT];

public:
	Budgetbar(Game* r_pGame, point r_point, int r_width, int r_height);
	~Budgetbar();
	void draw() const override;
	bool handleClick(int x, int y);	//handles clicks on toolbar icons, returns true if exit is clicked

};

