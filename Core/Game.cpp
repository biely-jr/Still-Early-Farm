#include "Game.h"
#include "../Config/GameConfig.h"
#include "../Entities/Animal.h"
#include <random>

namespace
{
	const int chickCost = 100;
	const int cowCost = 400;
	const string wolfImagePath = "images\\Wolf.jpg";
	const string foodAreaImagePath = "images\\FoodArea.jpg";
	const string eggImagePath = "images\\Egg.jpg";
	const string milkImagePath = "images\\Milk.jpg";
	const string warehouseImagePath = "images\\warehouse.jpg";
}

Game::Game()
{
	//1 - Create the main window
	pWind = CreateWind(config.windWidth, config.windHeight, config.wx, config.wy);

	//2 - create and draw the toolbar
	createToolbar();
	createBudgetbar();
	//3 - create and draw the backgroundPlayingArea
	initializeFoodAreas();
	redrawField();


	//4- Create the Plane
	//TODO: Add code to create and draw the Plane

	//5- Create the Bullet
	//TODO: Add code to create and draw the Bullet

	//6- Create the enemies
	//TODO: Add code to create and draw enemies in random places

	//7- Create and clear the status bar
	clearStatusBar();
}

Game::~Game()
{
	for (Animal* animal : animals)
		delete animal;

	delete gameToolbar;
	delete gameBudgetbar;
	delete pWind;
}

clicktype Game::getMouseClick(int& x, int& y) const
{
	return pWind->WaitMouseClick(x, y);	//Wait for mouse click

}

string Game::getSrting() const
{
	string Label;
	char Key;
	keytype ktype;
	pWind->FlushKeyQueue();
	while (1)
	{
		ktype = pWind->WaitKeyPress(Key);
		if (ktype == ESCAPE)	//ESCAPE key is pressed
			return "";	//returns nothing as user has cancelled label
		if (Key == 13)	//ENTER key is pressed
			return Label;
		if (Key == 8)	//BackSpace is pressed
			if (Label.size() > 0)
				Label.resize(Label.size() - 1);
			else
				Key = '\0';
		else
			Label += Key;
		printMessage(Label);
	}
}

window* Game::CreateWind(int w, int h, int x, int y) const
{
	window* pW = new window(w, h, x, y);
	pW->SetBrush(config.bkGrndColor);
	pW->SetPen(config.bkGrndColor, 1);
	pW->DrawRectangle(0, 0, w, h);
	return pW;
}

void Game::createToolbar() 
{
	point toolbarUpperleft;
	toolbarUpperleft.x = 0;
	toolbarUpperleft.y = 0;

	gameToolbar = new Toolbar(this, toolbarUpperleft, 0, config.toolBarHeight);
	gameToolbar->draw();
}

void Game::createBudgetbar()
{
	point budgetbarUpperleft;
	budgetbarUpperleft.x = 0;
	budgetbarUpperleft.y = config.toolBarHeight;

	gameBudgetbar = new Budgetbar(this, budgetbarUpperleft, 0, config.toolBarHeight);
	gameBudgetbar->draw();
}

void Game::initializeFoodAreas()
{
	foodAreas[0] = { {120, 150}, 180, 90, 5, LIGHTGREEN, "CHICK FOOD" }; // 4 & 5 is how many food in the foodarea
	foodAreas[1] = { {350, 150}, 180, 90, 7, KHAKI, "COW FOOD" };
}


void Game::clearBudget() const
{
	//Clear Status bar by drawing a filled rectangle
	pWind->SetPen(config.bkGrndColor, 1);
	pWind->SetBrush(config.bkGrndColor);
	pWind->DrawRectangle(config.windWidth - 500, config.toolBarHeight, config.windWidth, 2*config.toolBarHeight);
}

void Game::printBudget(string msg) const
{
	clearBudget();	//First clear the status bar

	pWind->SetPen(config.penColor, 50);
	pWind->SetFont(24, BOLD, BY_NAME, "Arial");
	pWind->DrawString(config.windWidth-200, config.toolBarHeight + 10, msg);

}

void Game::clearStatusBar() const
{
	//Clear Status bar by drawing a filled rectangle
	pWind->SetPen(config.statusBarColor, 1);
	pWind->SetBrush(config.statusBarColor);
	pWind->DrawRectangle(0, config.windHeight - config.statusBarHeight, config.windWidth, config.windHeight);
}

void Game::clearPlayingArea() const
{
	pWind->SetPen(config.bkGrndColor, 1);
	pWind->SetBrush(config.bkGrndColor);
	pWind->DrawRectangle(0, 2 * config.toolBarHeight, config.windWidth, config.windHeight - config.statusBarHeight);
}

void Game::drawFieldBackground() const
{
	const int playTop = 2 * config.toolBarHeight;
	const int playBottom = config.windHeight - config.statusBarHeight;

	pWind->SetPen(SKYBLUE, 1);
	pWind->SetBrush(SKYBLUE);
	pWind->DrawRectangle(0, playTop, config.windWidth, playTop + 140);

	pWind->SetPen(LIGHTGREEN, 1);
	pWind->SetBrush(LIGHTGREEN);
	pWind->DrawRectangle(0, playTop + 140, config.windWidth, playBottom);

	pWind->SetPen(SANDYBROWN, 4);
	pWind->DrawLine(0, playTop + 140, config.windWidth, playTop + 140);
}

void Game::drawWolf() const
{
	const int x = 835;
	const int y = 205;
	pWind->DrawImage(wolfImagePath, x, y, 140, 140);
}

void Game::drawFoodArea(const FoodArea& area) const
{
	if (area.counter <= 0)
		return;

	pWind->DrawImage(foodAreaImagePath, area.topLeft.x, area.topLeft.y, area.width, area.height);

	pWind->SetPen(BLACK, 2);
	pWind->SetBrush(WHITE);
	pWind->DrawRectangle(area.topLeft.x + 55, area.topLeft.y - 34, area.topLeft.x + 125, area.topLeft.y - 6, FILLED, 10, 10);
	pWind->SetFont(18, BOLD, BY_NAME, "Arial");
	pWind->DrawString(area.topLeft.x + 84, area.topLeft.y - 30, to_string(area.counter));
	pWind->SetFont(16, BOLD, BY_NAME, "Arial");
	pWind->DrawString(area.topLeft.x + 22, area.topLeft.y + 32, area.label);
}

void Game::drawAllFoodAreas() const
{
	for (const FoodArea& area : foodAreas)
		drawFoodArea(area);
}

void Game::drawEggsAndMilk() const
{
	pWind->DrawImage(eggImagePath, 610, 160, 90, 90);
	pWind->DrawImage(milkImagePath, 720, 160, 80, 90);
}

void Game::drawWarehouse() const
{
	pWind->DrawImage(warehouseImagePath, 900, 315, 220, 180);
}

point Game::getRandomAnimalPosition(int animalWidth, int animalHeight) const
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	const int minX = 40;
	const int maxX = config.windWidth - animalWidth - 40;
	const int minY = (2 * config.toolBarHeight) + 190;
	const int maxY = config.windHeight - config.statusBarHeight - animalHeight - 20;

	std::uniform_int_distribution<int> distX(minX, maxX);
	std::uniform_int_distribution<int> distY(minY, maxY);

	point p;
	p.x = distX(gen);
	p.y = distY(gen);
	return p;
}

void Game::consumeFoodArea(int areaIndex)
{
	if (areaIndex < 0 || areaIndex >= 2)
		return;

	if (foodAreas[areaIndex].counter > 0)
		foodAreas[areaIndex].counter--;
}

void Game::printMessage(string msg) const
{
	clearStatusBar();	//First clear the status bar

	pWind->SetPen(config.penColor, 50);
	pWind->SetFont(24, BOLD, BY_NAME, "Arial");
	pWind->DrawString(10, config.windHeight - (int)(0.85 * config.statusBarHeight), msg);

}

window* Game::getWind() const
{
	return pWind;
}

void Game::redrawField() const
{
	clearPlayingArea();
	drawFieldBackground();
	drawAllFoodAreas();
	drawEggsAndMilk();
	drawWolf();
	drawWarehouse();

	for (Animal* animal : animals)
		animal->draw();
}

bool Game::canAfford(int amount) const
{
	return budget >= amount;
}

bool Game::spendBudget(int amount)
{
	if (!canAfford(amount))
		return false;

	budget -= amount;
	return true;
}

void Game::placeAnimal(AnimalType animalType)
{
	int animalCost = chickCost;
	int animalWidth = 50;
	int animalHeight = 50;
	string imagePath = "images\\chick.jpg";
	int foodAreaIndex = 0;
	string animalName = "Chicken";

	if (animalType == ANIMAL_COW)
	{
		animalCost = cowCost;
		animalWidth = 70;
		animalHeight = 70;
		imagePath = "images\\cow.jpg";
		foodAreaIndex = 1;
		animalName = "Cow";
	}

	if (foodAreas[foodAreaIndex].counter <= 0)
	{
		printMessage(animalName + " cannot be added because its food is finished.");
		return;
	}

	if (!spendBudget(animalCost))
	{
		printMessage("Not enough budget for " + animalName + ".");
		return;
	}

	point animalPosition = getRandomAnimalPosition(animalWidth, animalHeight);

	if (animalType == ANIMAL_CHICK)
		animals.push_back(new Chick(this, animalPosition, animalWidth, animalHeight, imagePath));
	else
		animals.push_back(new Cow(this, animalPosition, animalWidth, animalHeight, imagePath));

	consumeFoodArea(foodAreaIndex);
	redrawField();
	printBudget("BUDGET = $" + to_string(budget));
	printMessage(animalName + " added. Cost = $" + to_string(animalCost));
}

void Game::go() const
{
	//This function reads the position where the user clicks to determine the desired operation
	int x, y;
	bool isExit = false;

	//Change the title
	pWind->ChangeTitle("- - - - - - - - - - Farm Frenzy (CIE101-project) - - - - - - - - - -");

	do
	{
		printMessage("Ready...");
		string budget_string = "BUDGET = $" + to_string(budget);
		printBudget(budget_string);
		//printBudget("BUDGET = $1000");
		getMouseClick(x, y);	//Get the coordinates of the user click
		//if (gameMode == MODE_DSIGN)		//Game is in the Desgin mode
		//{
			//[1] If user clicks on the Toolbar
		if (y >= 0 && y < config.toolBarHeight)
		{
			isExit = gameToolbar->handleClick(x, y);
		}
		else if (y >= config.toolBarHeight && y < 2*config.toolBarHeight)
		{
			isExit = gameBudgetbar->handleClick(x, y);
		}
		//}

	} while (!isExit);
}


