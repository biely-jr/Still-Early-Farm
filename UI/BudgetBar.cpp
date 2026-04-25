#include "Budgetbar.h"
#include "../Config/GameConfig.h"
#include "../Core/Game.h"


BudgetbarIcon::BudgetbarIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Drawable(r_pGame, r_point, r_width, r_height)
{
	image_path = img_path;
}

void BudgetbarIcon::draw() const
{
	//draw image of this object
	window* pWind = pGame->getWind();
	pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
}

ChickIcon::ChickIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{}

void ChickIcon::onClick()
{
	pGame->placeAnimal(ANIMAL_CHICK);
}

CowIcon::CowIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{}

void CowIcon::onClick()
{
	pGame->placeAnimal(ANIMAL_COW);
}


Budgetbar::Budgetbar(Game* r_pGame, point r_point, int r_width, int r_height) : Drawable(r_pGame, r_point, r_width, r_height)
{
	//First prepare List of images for each icon
	//To control the order of these images in the menu, reoder them in enum ICONS above	
	iconsImages[ICON_CHICK] = "images\\chick.jpg";
	iconsImages[ICON_COW] = "images\\cow.jpg";

	point p;
	p.x = 0;
	p.y = config.toolBarHeight;

	iconsList = new BudgetbarIcon * [ANIMAL_COUNT];

	//For each icon in the tool bar create an object 
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
	pWind->DrawLine(0, 2*config.toolBarHeight, pWind->GetWidth(), 2*config.toolBarHeight);
}

bool Budgetbar::handleClick(int x, int y)
{
	if (x > ANIMAL_COUNT * config.iconWidth)	//click outside toolbar boundaries
		return false;


	//Check whick icon was clicked
	//==> This assumes that menu icons are lined up horizontally <==
	//Divide x co-ord of the point clicked by the icon width (int division)
	//if division result is 0 ==> first icon is clicked, if 1 ==> 2nd icon and so on

	int clickedIconIndex = (x / config.iconWidth);
	iconsList[clickedIconIndex]->onClick();	//execute onClick action of clicled icon

	//if (clickedIconIndex == ICON_EXIT) return true;

	return false;

}
