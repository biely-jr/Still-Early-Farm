#include "Budgetbar.h"
#include "../Config/GameConfig.h"
#include "../Core/Game.h"
#include <iostream>
using namespace std;

// Budgetbar inherits from Drawable (is a relationship)
// Reuse behavior / properties + improve / brush off / specialize
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

// Creation of a constructor which is called upon when object is created
ChickIcon::ChickIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path) // The second half builds the budget bar which contains the chich inon within
{
	// Creation of an array
	chickList = new Chick * [15];
	// (new) Creates a Chick somewhere in memory and its address is stored in chickList
	// We use dynamic memory because we do not know how many objects will be created, so when clicking the chick icon, the chick is created at runtime instead of manually setting the number of chicks
	for (int i = 0; i < 10; i++) {
		chickList[i] = nullptr;
	}
}

void ChickIcon::onClick()
{
	// Delegate animal placement to Game manager
	pGame->placeAnimal(ANIMAL_CHICK);
}

CowIcon::CowIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{
	count = 0;

	cowList = new Cow * [15];
	for (int i = 0; i < 15; i++)
	{
		cowList[i] = nullptr;
	}
}

void CowIcon::onClick()
{
	cout << "Icon Cow Clicked" << endl;

	// Delegate animal placement to Game manager
	pGame->placeAnimal(ANIMAL_COW);
}

Budgetbar::Budgetbar(Game* r_pGame, point r_point, int r_width, int r_height) : Drawable(r_pGame, r_point, r_width, r_height)
{
	//First prepare List of images for each icon
	//To control the order of these images in the menu, reoder them in enum ICONS above	
	iconsImages[ICON_WATER] = "images\\water.jpg";
	iconsImages[ICON_CHICK] = "images\\chick.jpg";
	iconsImages[ICON_COW] = "images\\cow.jpg";

	point p;
	p.x = 0;
	p.y = config.toolBarHeight;

	iconsList = new BudgetbarIcon * [ANIMAL_COUNT];

	iconsList[ICON_WATER] = new WaterIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_WATER]);
	p.x += config.iconWidth;

	//For each icon in the tool bar create an object 
	iconsList[ICON_CHICK] = new ChickIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_CHICK]);
	p.x += config.iconWidth;
	// The constructor is automatically called to impelement its setup function

	iconsList[ICON_COW] = new CowIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_COW]);
	p.x += config.iconWidth;
	//p.x += config.iconWidth;
	//iconsList[ICON_CHICK] = new ChickIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_CHICK]);

}

Budgetbar::~Budgetbar()
{
	for (int i = 0; i < ICON_COUNT; i++)
		delete iconsList[i];
	delete[] iconsList; // Adjusted to array delete from text version style
}

void Budgetbar::draw() const
{
	for (int i = 0; i < ANIMAL_COUNT; i++)
		// Encapsulation (Hides complexity) ==> The drawable function is just being called
		iconsList[i]->draw();
	window* pWind = pGame->getWind();
	pWind->SetPen(BLACK, 3);
	pWind->DrawLine(0, 2 * config.toolBarHeight, pWind->GetWidth(), 2 * config.toolBarHeight);
}

// Detect where the user clicked
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

void ChickIcon::moveAnimals()
{
	// Loop through all spawned chicks, move them, and redraw them
	for (int i = 0; i < count; i++) {
		if (chickList[i] != nullptr) {
			chickList[i]->moveStep();
			chickList[i]->draw();
		}
	}
}

void CowIcon::moveAnimals()
{
	// Loop through all spawned cows, move them, and redraw them
	for (int i = 0; i < count; i++) {
		if (cowList[i] != nullptr) {
			cowList[i]->moveStep();
			cowList[i]->draw();
		}
	}
}

WaterIcon::WaterIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path)
	: BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{
	count = 0;
	grassList = new point[50]; // Max 50 green areas
}

void WaterIcon::draw() const
{
	window* pWind = pGame->getWind();
	pWind->SetPen(BLACK, 2);
	pWind->SetBrush(SKYBLUE);
	pWind->DrawRectangle(RefPoint.x, RefPoint.y, RefPoint.x + width, RefPoint.y + height, FILLED);
	pWind->SetPen(BLACK, 2);
	pWind->SetFont(18, BOLD, BY_NAME, "Arial");
	pWind->DrawString(RefPoint.x + 18, RefPoint.y + 14, "WATER");
}

void WaterIcon::onClick()
{
	if (pGame->budget >= 50)   // Price set to 50
	{
		pGame->budget -= 50;

		pGame->clearBudget();
		string budget_string = "BUDGET = $" + to_string(pGame->budget);
		pGame->printBudget(budget_string);

		point p;
		std::random_device rd;
		std::mt19937 gen(rd());

		std::uniform_int_distribution<int> dist1(range_min_x, range_max_x);
		p.x = dist1(gen);

		std::uniform_int_distribution<int> dist2(range_min_y, range_max_y);
		p.y = dist2(gen);

		if (count < 50)
		{
			grassList[count] = p;
			count++;
		}
	}
}

void WaterIcon::moveAnimals()
{
	// Because ICON_WATER is first in the enum, this draws the grass BEFORE the animals.
	// This ensures the grass sits safely in the background and stops the flickering.
	window* pWind = pGame->getWind();
	for (int i = 0; i < count; i++) {
		pWind->SetPen(GREEN, 1);
		pWind->SetBrush(GREEN);
		pWind->DrawRectangle(grassList[i].x, grassList[i].y, grassList[i].x + 50, grassList[i].y + 50);
	}
}

void Budgetbar::moveAllAnimals()
//moveAnimals function (child) BELONGS TO BudgetBar (Parent)
{
	// Tell every animal icon to move its respective animals where it iterates through iconsList
	// Which has pointers to all animals
	for (int i = 0; i < ANIMAL_COUNT; i++) {
		if (iconsList[i] != nullptr) { //Ensures the icon actually exists
			iconsList[i]->moveAnimals();
		}
	}
}

void Budgetbar::moveAnimals()
{
	// Included from the text version
}

void Budgetbar::catchAllAnimals(int wolfX, int wolfY, int wolfW, int wolfH)
{
	// Send the wolf's coordinates to all icons so they can check their animals
	for (int i = 0; i < ANIMAL_COUNT; i++) {
		if (iconsList[i] != nullptr) {
			iconsList[i]->catchAnimals(wolfX, wolfY, wolfW, wolfH);
		}
	}
}

void ChickIcon::catchAnimals(int wolfX, int wolfY, int wolfW, int wolfH)
{
	for (int i = 0; i < count; i++) {
		if (chickList[i] != nullptr) {
			point p = chickList[i]->getRefPoint();
			int w = chickList[i]->getWidth();
			int h = chickList[i]->getHeight();

			// Bounding Box Collision Detection
			if (p.x < wolfX + wolfW && p.x + w > wolfX &&
				p.y < wolfY + wolfH && p.y + h > wolfY)
			{
				// Collision happened! The wolf eats the chick
				delete chickList[i];      // Free the memory
				chickList[i] = nullptr;   // Prevent using dead pointers (making them disappear)

				if (pGame->animalcount > 0) pGame->animalcount--; // Optional: Update status bar
			}
		}
	}
}

void CowIcon::catchAnimals(int wolfX, int wolfY, int wolfW, int wolfH)
{
	for (int i = 0; i < count; i++) {
		if (cowList[i] != nullptr) {
			point p = cowList[i]->getRefPoint();
			int w = cowList[i]->getWidth();
			int h = cowList[i]->getHeight();

			// Bounding Box Collision Detection
			if (p.x < wolfX + wolfW && p.x + w > wolfX &&
				p.y < wolfY + wolfH && p.y + h > wolfY)
			{
				// Collision happened! The wolf eats the cow
				delete cowList[i];
				cowList[i] = nullptr;

				if (pGame->animalcount > 0) pGame->animalcount--; // Optional: Update status bar
			}
		}
	}
}
