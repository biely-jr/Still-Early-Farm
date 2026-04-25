#include "Budgetbar.h"
#include "../Config/GameConfig.h"
#include "../Core/Game.h"
#include <iostream>
using namespace std;

BudgetbarIcon::BudgetbarIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: Drawable(r_pGame, r_point, r_width, r_height)
{
	image_path = img_path;
}

void BudgetbarIcon::draw() const
{
	window* pWind = pGame->getWind();
	pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
}

ChickIcon::ChickIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{}

void ChickIcon::onClick()
{
	pGame->placeAnimal(ANIMAL_CHICK);
}

void ChickIcon::moveAnimals()
{
}

CowIcon::CowIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{}

void CowIcon::onClick()
{
	pGame->placeAnimal(ANIMAL_COW);
}

void CowIcon::moveAnimals()
{
}

Budgetbar::Budgetbar(Game* r_pGame, point r_point, int r_width, int r_height)
	: Drawable(r_pGame, r_point, r_width, r_height)
{
	iconsImages[ICON_CHICK] = "images\\chick.jpg";
	iconsImages[ICON_COW] = "images\\cow.jpg";

	point p;
	p.x = 0;
	p.y = config.toolBarHeight;

	iconsList = new BudgetbarIcon * [ANIMAL_COUNT];

	iconsList[ICON_CHICK] = new ChickIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_CHICK]);
	p.x += config.iconWidth;
	iconsList[ICON_COW] = new CowIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_COW]);
}

Budgetbar::~Budgetbar()
{
	for (int i = 0; i < ANIMAL_COUNT; i++)
		delete iconsList[i];

	delete[] iconsList;
}

void Budgetbar::draw() const
{
	for (int i = 0; i < ANIMAL_COUNT; i++)
		iconsList[i]->draw();

	window* pWind = pGame->getWind();
	pWind->SetPen(BLACK, 3);
	pWind->DrawLine(0, 2 * config.toolBarHeight, pWind->GetWidth(), 2 * config.toolBarHeight);
}

bool Budgetbar::handleClick(int x, int y)
{
	if (x > ANIMAL_COUNT * config.iconWidth)
		return false;

	int clickedIconIndex = (x / config.iconWidth);
	iconsList[clickedIconIndex]->onClick();

	return false;
}

void Budgetbar::moveAnimals()
{
}
