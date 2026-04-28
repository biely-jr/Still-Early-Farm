#include <iostream>
#include "Game.h"
#include "../Config/GameConfig.h"
#include <thread>
#include <chrono>
#include "../Entities/Animal.h"
#include <random>

namespace
{
	const int chickCost = 100;
	const int cowCost = 200;
	const string wolfImagePath = "images\\Wolf.jpg";
	const string foodAreaImagePath = "images\\FoodArea.jpg";
	const string eggImagePath = "images\\Egg.jpg";
	const string milkImagePath = "images\\Milk.jpg";
	const string warehouseImagePath = "images\\warehouse.jpg";
}

Game::Game()
	: paused(false),
	eggCount(0),
	milkCount(0),
	warehouseEggCount(0),
	warehouseMilkCount(0)
{
	//1 - Create the main window
	pWind = CreateWind(config.windWidth, config.windHeight, config.wx, config.wy);

	//2 - create and draw the toolbar
	createToolbar();
	createBudgetbar();

	//3 - create and draw the backgroundPlayingArea

	//4- Create the Plane
	//TODO: Add code to create and draw the Plane

	//5- Create the Bullet
	//TODO: Add code to create and draw the Bullet

	//6- Create the enemies
	//TODO: Add code to create and draw enemies in random places
	wolfImagePath = "images\\Wolf.jpg"; // Adjust extension if it's .png
	wolfX = 835;
	wolfY = 205;

	//7- Create and clear the status bar
	gametimer(level);
	updatestatusbar();
	drawfieldboundary();
	clearStatusBar();
	spawnWolves();
	clearStatusBar();
	initializeFoodAreas();
	redrawField();

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
	// GetMouseClick is non-blocking. It checks if the mouse was clicked right now.
	// If yes, it updates x and y. If not, it returns NO_CLICK and lets the code continue.
	return pWind->GetMouseClick(x, y);
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

	pW->SetBuffering(true);

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
	pWind->DrawRectangle(config.windWidth - 500, config.toolBarHeight, config.windWidth, 2 * config.toolBarHeight);
}

void Game::printBudget(string msg) const
{
	clearBudget();	//First clear the status bar

	pWind->SetPen(config.penColor, 50);
	pWind->SetFont(24, BOLD, BY_NAME, "Arial");
	pWind->DrawString(config.windWidth - 575, config.toolBarHeight + 10, msg);
}

void Game::clearStatusBar() const
{
	//Clear Status bar by drawing a filled rectangle
	pWind->SetPen(config.statusBarColor, 1);
	pWind->SetBrush(config.statusBarColor);
	//DrawRectangle(X1,Y1,X2,Y2)
	pWind->DrawRectangle(0, config.windHeight - config.statusBarHeight, config.windWidth, config.windHeight);
}

void Game::updatestatusbar() const
{
	clearStatusBar();
	string status = "Level: " + to_string(level) + " | Timer: " + to_string(time) + " | Animals: " + to_string(animalcount);
	pWind->SetPen(config.penColor);
	pWind->SetFont(20, BOLD, BY_NAME, "Arial");
	pWind->DrawString(10, config.windHeight - (int)(0.85 * config.statusBarHeight), status);
}

void Game::clearPlayingArea() const
{
	pWind->SetPen(config.bkGrndColor, 1);
	pWind->SetBrush(config.bkGrndColor);
	pWind->DrawRectangle(0, 2 * config.toolBarHeight, config.windWidth, config.windHeight - config.statusBarHeight);
}

void Game::drawfieldboundary() const {
	pWind->SetPen(BLUE, config.penWidth);
	pWind->DrawRectangle(0, 2 * config.toolBarHeight, config.windWidth, config.windHeight - config.statusBarHeight, FRAME);
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


void Game::spawnWolves()
{
	wolves.clear();

	int wolvesToSpawn = 1 + (level / 2);
	int minX = 50;
	int maxX = config.windWidth - 140 - 50;

	int minY = (2 * config.toolBarHeight) + 20;
	int maxY = config.windHeight - config.statusBarHeight - 140 - 20;

	for (int i = 0; i < wolvesToSpawn; i++)
	{
		int randX = minX + (rand() % (maxX - minX));
		int randY = minY + (rand() % (maxY - minY));

		point wolfPosition;
		wolfPosition.x = randX;
		wolfPosition.y = randY;
		wolves.push_back(wolfPosition);
	}

	updatestatusbar();
}

void Game::drawWolf() const
{
	// 1. Draw primary moving wolf
	pWind->DrawImage(Game::wolfImagePath, wolfX, wolfY, 140, 140);

	// 2. Start the loop at index 1 instead of 0 to skip the first vector wolf
	for (size_t i = 1; i < wolves.size(); i++)
	{
		pWind->DrawImage(::wolfImagePath, wolves[i].x, wolves[i].y, 100, 100);
	}
}

void Game::moveWolf() const
{
	// Generates a random number between -15 and +15 to shift X and Y
	wolfX += (rand() % 31 - 15);
	wolfY += (rand() % 31 - 15);

	// BOUNDARY CHECKING: Prevent the wolf from walking off the screen or over the UI
	// Right and Left bounds
	if (wolfX < 0) wolfX = 0;
	if (wolfX > config.windWidth - 140) wolfX = config.windWidth - 140;

	// Top and Bottom bounds (Respecting Toolbars and Status Bar)
	if (wolfY < config.toolBarHeight * 2) wolfY = config.toolBarHeight * 2;
	if (wolfY > config.windHeight - config.statusBarHeight - 140) wolfY = config.windHeight - config.statusBarHeight - 140;
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

	pWind->SetPen(BLACK, 2);
	pWind->SetBrush(WHITE);
	pWind->DrawRectangle(610, 130, 700, 156, FILLED, 10, 10);
	pWind->DrawRectangle(720, 130, 810, 156, FILLED, 10, 10);
	pWind->SetFont(16, BOLD, BY_NAME, "Arial");
	pWind->DrawString(620, 134, "Eggs: " + to_string(eggCount));
	pWind->DrawString(730, 134, "Milk: " + to_string(milkCount));
}

void Game::drawWarehouse() const
{
	pWind->DrawImage(warehouseImagePath, 900, 315, 220, 180);

	int textX = 50;
	int textY = config.windHeight - 80;

	pWind->SetPen(BLACK, 2);
	pWind->SetFont(18, BOLD, BY_NAME, "Arial");

	// Draw the text showing amounts
	string warehouseStatus = "Warehouse - Eggs: " + to_string(warehouseEggCount) + " | Milk: " + to_string(warehouseMilkCount);
	pWind->DrawString(textX, textY, warehouseStatus);
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

void Game::Restart()
{
	cout << "Restart button clicked" << endl;
	// 1. Reset budget
	budget = 5000;
	animalcount = 0;
	level = 1;               // Resets the level back to 1
	gametimer(level);		 // Resets the timer based on the level
	animalcount = 0;
	eggCount = 0;
	milkCount = 0;
	paused = false;

	for (Animal* animal : animals)
		delete animal;
	animals.clear();

	// 2. Clear the entire window, basically erases the set window and makes a new one
	// How it works : it is used in drawing a giant rectangle that covers the intial gameplay
	pWind->SetPen(config.bkGrndColor, 1);
	pWind->SetBrush(config.bkGrndColor);
	pWind->DrawRectangle(0, 0, config.windWidth, config.windHeight);

	// 3. Delete old UI elements, deletes their pointers to prevent memory leaks
	delete gameToolbar;
	delete gameBudgetbar;

	// 4. Recreate UI (toolbar + budget bar)
	createToolbar();
	createBudgetbar();

	// 5. Clear status bar
	clearStatusBar();

	initializeFoodAreas();
	spawnWolves();
	redrawField();
	updatestatusbar();
	printBudget("BUDGET: $" + to_string(budget) + " | Chick: $100 | Cow: $200 | water: $50 ");
	printMessage("Game restarted.");
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

void Game::gametimer(int level)
{
	time = 150 - (level - 1) * 10;
	if (time <= 60)
	{
		time = 60;
	}
}

void Game::redrawField() const
{
	clearPlayingArea();
	drawFieldBackground();
	drawfieldboundary();
	drawAllFoodAreas();
	drawEggsAndMilk();
	drawWolf();
	drawWarehouse();

	int chickCounter = 1;
	int cowCounter = 1;

	for (Animal* animal : animals)
	{
		animal->draw();

		pWind->SetPen(BLACK, 2);
		pWind->SetFont(16, BOLD, BY_NAME, "Arial");
		point p = animal->getRefPoint();

		int xShift = 15;
		int displayCounter = 1;

		// If width >= 70, it's a Cow. Otherwise, it's a Chick.
		if (animal->getWidth() >= 70)
		{
			xShift = 30;
			displayCounter = cowCounter++; // Use and increment cow counter
		}
		else
		{
			xShift = 15;
			displayCounter = chickCounter++; // Use and increment chick counter
		}

		pWind->DrawString(p.x + animal->getWidth() - xShift, p.y - 25, to_string(displayCounter));
	}
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
	if (paused)
	{
		printMessage("Resume the game before adding more animals.");
		return;
	}

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

	animalcount++;
	consumeFoodArea(foodAreaIndex);
	updatestatusbar();
	redrawField();
	printBudget("BUDGET = $" + to_string(budget));
	printMessage(animalName + " added. Cost = $" + to_string(animalCost));
}

void Game::pauseGame()
{
	paused = true;
	printMessage("Game paused.");
}

void Game::resumeGame()
{
	paused = false;
	printMessage("Game resumed.");
}

bool Game::isPaused() const
{
	return paused;
}

void Game::registerAnimalProduct(const string& productLabel)
{
	if (productLabel == "Egg")
		eggCount++;
	else if (productLabel == "Milk")
		milkCount++;
}

void Game::updateAnimalProduction(int elapsedSeconds)
{
	for (Animal* animal : animals)
		animal->advanceProduction(elapsedSeconds);
}

void Game::collectEggs()
{
	if (eggCount > 0) {
		warehouseEggCount += eggCount;
		eggCount = 0; // Remove from the icon bar
		updatestatusbar(); // Refresh UI if necessary
	}
}

void Game::collectMilk()
{
	if (milkCount > 0) {
		warehouseMilkCount += milkCount;
		milkCount = 0; // Remove from the icon bar
		updatestatusbar(); // Refresh UI if necessary
	}
}

void Game::go()
{
	//This function reads the position where the user clicks to determine the desired operation
	int x = -1, y = -1;
	bool isExit = false;

	//Change the title
	pWind->ChangeTitle("- - - - - - - - - - Farm Frenzy (CIE101-project) - - - - - - - - - -");
	pWind->SetBuffering(true);
	pWind->UpdateBuffer();
	pWind->ChangeTitle("Farm Frenzy");

	//Set up the clock tracker before the loop starts
	auto lastTime = std::chrono::steady_clock::now();
	auto lastProductionTick = lastTime;

	do
	{
		auto currentTime = std::chrono::steady_clock::now();

		// Check if 1 second has passed
		if (!paused && std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime).count() >= 1)
		{
			if (time > 0) {
				time--; // Decrease timer
			}
			lastTime = currentTime; // Reset the clock tracker
		}
		else if (paused)
		{
			lastTime = currentTime;
		}

		if (!paused)
		{
			int elapsedProductionSeconds = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastProductionTick).count());
			if (elapsedProductionSeconds >= 1)
			{
				updateAnimalProduction(elapsedProductionSeconds);
				lastProductionTick = currentTime;
			}
		}
		else
		{
			lastProductionTick = currentTime;
		}

		pWind->SetBuffering(true); // Prevent flickering

		string budget_string_code = "BUDGET = $" + to_string(budget);
		string budget_string = "BUDGET: $" + to_string(budget);
		string prices = " | Chick: $100 | Cow: $200 | water: $50 ";
		//printBudget("BUDGET = $1000");
		printBudget(budget_string + prices);

		gameToolbar->draw();
		gameBudgetbar->draw();
		drawfieldboundary();
		updatestatusbar();

		clicktype click = getMouseClick(x, y);	//Get the coordinates of the user click

		if (!paused)
			updatePlayArea();
		else if (paused)
			printMessage("Game paused");
		else
			redrawField();

		pWind->UpdateBuffer(); // Update the buffer after all drawing is finished

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		//if (gameMode == MODE_DSIGN)		//Game is in the Desgin mode
		//{
			//[1] If user clicks on the Toolbar
		if (click == LEFT_CLICK && y >= 0 && y < config.toolBarHeight)
		{
			isExit = gameToolbar->handleClick(x, y);
		}
		else if (click == LEFT_CLICK && y >= config.toolBarHeight && y < 2 * config.toolBarHeight)
		{
			isExit = gameBudgetbar->handleClick(x, y);
		}
		else if (click == LEFT_CLICK && y >= 2 * config.toolBarHeight)
		{
			// Check Egg Icon bounds (from drawEggsAndMilk: X: 610-700, Y: 130-250)
			if (x >= 610 && x <= 700 && y >= 130 && y <= 250)
			{
				collectEggs();
			}
			// Check Milk Icon bounds (from drawEggsAndMilk: X: 720-810, Y: 130-250)
			else if (x >= 720 && x <= 810 && y >= 130 && y <= 250)
			{
				collectMilk();
			}
		}
		//}

	} while (!isExit);
}

// Constantly erases the old frame and draws the new one.
void Game::updatePlayArea()
{
	// 1. Check collisions with the vector of animals
	for (auto it = animals.begin(); it != animals.end(); )
	{
		Animal* animal = *it;
		point p = animal->getRefPoint();
		int w = animal->getWidth();
		int h = animal->getHeight();

		bool isEaten = false;

		// A. Check collision with the primary moving wolf
		if (p.x < wolfX + 140 && p.x + w > wolfX &&
			p.y < wolfY + 140 && p.y + h > wolfY)
		{
			isEaten = true;
		}

		// B. Check collision with any multiplied/cloned wolves
		if (!isEaten) {
			for (size_t i = 1; i < wolves.size(); i++) {
				if (p.x < wolves[i].x + 140 && p.x + w > wolves[i].x &&
					p.y < wolves[i].y + 140 && p.y + h > wolves[i].y)
				{
					isEaten = true;
					break; // Stop checking other wolves if already eaten
				}
			}
		}

		// C. If the animal was eaten, remove it from the game
		if (isEaten)
		{
			delete animal;              // Free the memory
			it = animals.erase(it);     // Remove safely from the vector

			if (animalcount > 0) {
				animalcount--;          // Decrease the counter
				updatestatusbar();      // Refresh the UI to show the new count
			}
		}
		else
		{
			++it; // Move to the next animal only if this one wasn't eaten
		}
	}

	// 2. Move wolves and animals
	moveWolf();
	for (Animal* animal : animals)
		animal->moveStep();

	// 3. Clear the playing area and draw static background elements
	clearPlayingArea();
	drawFieldBackground();
	drawfieldboundary();
	drawAllFoodAreas();
	drawEggsAndMilk();
	drawWarehouse();

	// 4. Draw the grass (and any old array animals) so it sits on the ground
	gameBudgetbar->moveAllAnimals();

	// 5. Draw the moving wolf and new vector animals ON TOP of the grass
	drawWolf();

	int chickCounter = 1;
	int cowCounter = 1;

	for (Animal* animal : animals)
	{
		animal->draw();

		pWind->SetPen(BLACK, 2);
		pWind->SetFont(16, BOLD, BY_NAME, "Arial");
		point p = animal->getRefPoint();

		int xShift = 15;
		int displayCounter = 1;

		// If width >= 70, it's a Cow. Otherwise, it's a Chick.
		if (animal->getWidth() >= 70)
		{
			xShift = 30;
			displayCounter = cowCounter++; // Use and increment cow counter
		}
		else
		{
			xShift = 15;
			displayCounter = chickCounter++; // Use and increment chick counter
		}

		pWind->DrawString(p.x + animal->getWidth() - xShift, p.y - 25, to_string(displayCounter));
	}
}
