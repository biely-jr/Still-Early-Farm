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
	const string eggImagePath = "images\\Egg.jpg";
	const string milkImagePath = "images\\Milk.jpg";
	const string warehouseImagePath = "images\\warehouse.jpg";
	const int eggPrice = 25;
	const int milkPrice = 50;
	const int sellButtonLeft = 1375;
	const int sellButtonRight = 1455;
	const int eggSellButtonTop = 352;
	const int eggSellButtonBottom = 382;
	const int milkSellButtonTop = 397;
	const int milkSellButtonBottom = 427;
}

Game::Game()
	: paused(false),
	eggCount(0),
	milkCount(0),
	warehouseEggCount(0),
	warehouseMilkCount(0),
	goalTarget(500),
	goalProgress(0),
	mainWolfVisible(false),
	wolvesSpawned(false),
	wolfSpawnCountdown(10),
	consecutiveWolfClicks(0),
	statusMessage(""),
	statusMessageTimer(0)
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
	wolfImagePath = "images\\Wolf.jpg";
	wolfX = 835;
	wolfY = 205;
	wolfVelX = 1;
	wolfVelY = 1;

	//7- Create and clear the status bar
	gametimer(level);
	updatestatusbar();
	drawfieldboundary();
	clearStatusBar();
	//spawnWolves();
	clearStatusBar();
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
	string status = "Level: " + to_string(level) + " | Timer: " + to_string(time) + " | Animals: " + to_string(animalcount) + " | Goal: " + to_string(goalProgress) + "/" + to_string(goalTarget);
	if (statusMessageTimer > 0 && !statusMessage.empty()) // checks whether a temporary message should still be shown
		status += " | " + statusMessage;
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

// Adds a new grass patch to the list
void Game::addGrassPatch(point p)
{
    GrassData newGrass;
    newGrass.pos = p;
    newGrass.timeRemaining = 10; // Set lifespan to 10 seconds
    
    grassPatches.push_back(newGrass);
}

// Draws all grass patches on the screen
void Game::drawGrass() const
{
	for (const GrassData& grass : grassPatches)
	{
		pWind->SetPen(DARKGREEN);
		pWind->SetBrush(GREEN);
		pWind->DrawRectangle(grass.pos.x, grass.pos.y, grass.pos.x + 40, grass.pos.y + 40);
	}
}

// Checks if an animal's bounding box intersects with any grass patch
bool Game::isStandingOnGrass(point p, int w, int h) const
{
	for (const GrassData& grass : grassPatches)
	{
		if (p.x < grass.pos.x + 40 && p.x + w > grass.pos.x &&
			p.y < grass.pos.y + 40 && p.y + h > grass.pos.y)
		{
			return true;
		}
	}
	return false;
}

void Game::spawnWolves()
{
	wolves.clear();
	wolfHitCounts.clear(); //When new wolves are spawned, it clears the old hit counters too, so the vector stays in sync with the wolves vector
	wolvesVel.clear(); //Clear the velocity vector

	int wolvesToSpawn = level - 1;
	int minX = 50;
	int maxX = config.windWidth - 140 - 50;

	int minY = (2 * config.toolBarHeight) + 20;
	int maxY = config.windHeight - config.statusBarHeight - 140 - 20;

	for (int i = 0; i < wolvesToSpawn; i++)
	{
		int randX, randY;
		bool safe = false;

		// Keep trying new positions until a safe one is found
		while (!safe)
		{
			randX = minX + (rand() % (maxX - minX));
			randY = minY + (rand() % (maxY - minY));
			safe = !isAreaOccupiedByAnimal(randX, randY, 100, 100);
		}

		point wolfPosition;
		wolfPosition.x = randX;
		wolfPosition.y = randY;
		wolves.push_back(wolfPosition);
		wolfHitCounts.push_back(0);

		// Give the new wolf a random initial velocity direction
		point vel;
		vel.x = (rand() % 3) - 1;
		vel.y = (rand() % 3) - 1;
		if (vel.x == 0 && vel.y == 0) vel.x = 1; // Ensure it's not standing still
		wolvesVel.push_back(vel);
	}

	updatestatusbar();
}

bool Game::isAreaOccupiedByAnimal(int x, int y, int width, int height) const
{
	for (Animal* animal : animals)
	{
		point p = animal->getRefPoint();
		int w = animal->getWidth();
		int h = animal->getHeight();

		// Check if the wolf's rectangle overlaps with the animal's rectangle
		if (x < p.x + w && x + width > p.x &&
			y < p.y + h && y + height > p.y)
		{
			return true; // Collision found
		}
	}
	return false; // Area is clear
}

void Game::drawWolf() const
{
	// 1. Draw primary moving wolf
	if (mainWolfVisible)
		pWind->DrawImage(Game::wolfImagePath, wolfX, wolfY, 80, 80);

	// 2. Draw every spawned wolf from the vector
	for (size_t i = 0; i < wolves.size(); i++)
	{
		pWind->DrawImage(::wolfImagePath, wolves[i].x, wolves[i].y, 70, 70);
	}
}

void Game::moveWolf()
{
	int speed = 4 + 2 * sqrt(level);

	// 1. Move extra spawned wolves
	for (size_t i = 0; i < wolves.size(); i++)
	{
		// 5% chance to change direction naturally (like the cow)
		if (rand() % 100 < 5) {
			wolvesVel[i].x = (rand() % 3) - 1;
			wolvesVel[i].y = (rand() % 3) - 1;
		}

		// Apply smooth continuous movement using the velocity multiplied by level speed
		wolves[i].x += wolvesVel[i].x * speed;
		wolves[i].y += wolvesVel[i].y * speed;

		// Boundary checks and bouncing for extra wolves
		if (wolves[i].x < 0) { wolves[i].x = 0; wolvesVel[i].x = 1; }
		if (wolves[i].x > config.windWidth - 70) { wolves[i].x = config.windWidth - 70; wolvesVel[i].x = -1; }
		if (wolves[i].y < config.toolBarHeight * 2) { wolves[i].y = config.toolBarHeight * 2; wolvesVel[i].y = 1; }
		if (wolves[i].y > config.windHeight - config.statusBarHeight - 70) { wolves[i].y = config.windHeight - config.statusBarHeight - 70; wolvesVel[i].y = -1; }
	}

	// 2. Move primary wolf if visible
	if (mainWolfVisible)
	{
		// 10% chance to change direction naturally
		if (rand() % 100 < 10) {
			wolfVelX = (rand() % 3) - 1;
			wolfVelY = (rand() % 3) - 1;
		}

		// Apply smooth continuous movement
		wolfX += wolfVelX * speed;
		wolfY += wolfVelY * speed;

		// Boundary checks and bouncing for primary wolf
		if (wolfX < 0) { wolfX = 0; wolfVelX = 1; }
		if (wolfX > config.windWidth - 80) { wolfX = config.windWidth - 80; wolfVelX = -1; }
		if (wolfY < config.toolBarHeight * 2) { wolfY = config.toolBarHeight * 2; wolfVelY = 1; }
		if (wolfY > config.windHeight - config.statusBarHeight - 80) { wolfY = config.windHeight - config.statusBarHeight - 80; wolfVelY = -1; }
	}
}

void Game::drawWarehouse() const
{
	pWind->DrawImage(warehouseImagePath, 1250, 550, 220, 180);
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

void Game::Restart()
{
	cout << "Restart button clicked" << endl;
	// 1. Reset budget
	paused = false; //Prevents pausing when restarting from a "Game Over"
	budget = 2500;
	animalcount = 0;
	wolvesVel.clear();
	mainWolfVisible = false;
	wolves.clear();
	consecutiveWolfClicks = 0;
	wolvesSpawned = false;
	wolfSpawnCountdown = 10;
	grassPatches.clear();

	if (level == 1)
		goalTarget = 500;
	else {
		goalTarget = 500;
		for (int i = 1; i < level; i++)
		{
			goalTarget += 700;
		}
	}
	goalProgress = 0;
	statusMessage = "";
	statusMessageTimer = 0;
	gametimer(level);		 // Resets the timer based on the level
	eggCount = 0;
	milkCount = 0;
	warehouseEggCount = 0;
	warehouseMilkCount = 0;

	for (Animal* animal : animals)
		delete animal;
	animals.clear();

	// 2. Clear the entire window, basically erases the set window and makes a new one
	// How it works : it is used in drawing a giant rectangle that covers the initial gameplay
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

	//spawnWolves();
	redrawField();
	updatestatusbar();
	printBudget("BUDGET: $" + to_string(budget) + " | Chick: $100 | Cow: $200 | water: $50 ");
	printMessage("Game restarted.");

}

void Game::printMessage(string msg) const
{
	statusMessage = msg;
	statusMessageTimer = 25;
	updatestatusbar();
}

window* Game::getWind() const
{
	return pWind;
}

void Game::gametimer(int level)
{
	time = 150 - (level - 1) * 10;
	if (time <= 90)
	{
		time = 90;
	}
}

void Game::redrawField() const
{
	clearPlayingArea();
	drawFieldBackground();
	drawfieldboundary();
	drawGrass();

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

	drawWarehouse();
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
	string animalName = "Chicken";

	if (animalType == ANIMAL_COW)
	{
		animalCost = cowCost;
		animalWidth = 70;
		animalHeight = 70;
		imagePath = "images\\cow.jpg";
		animalName = "Cow";
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
	updatestatusbar();
	redrawField();
	printBudget("BUDGET = $" + to_string(budget));
	printMessage(animalName + " added. Cost = $" + to_string(animalCost));
}

void Game::placeFoodArea()
{
	if (paused)
	{
		printMessage("Resume the game before watering.");
		return;
	}

	if (!spendBudget(50))
	{
		printMessage("Not enough budget for water.");
		return;
	}

	redrawField();
	printBudget("BUDGET = $" + to_string(budget));
	printMessage("Food area added. Cost = $50.");
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

void Game::saveGame()
{
	printMessage("Save clicked.");
}

void Game::loadGame()
{
	printMessage("Load clicked.");
}

void Game::sellEggProducts()
{
    int earnings = warehouseEggCount * eggPrice;

    if (earnings <= 0)
    {
        printMessage("No eggs to sell.");
        return;
    }

    budget += earnings;
    goalProgress += earnings;
    warehouseEggCount = 0;
    checkLevelGoal();

    redrawField();
    printBudget("BUDGET = $" + to_string(budget));
    printMessage("Eggs sold for $" + to_string(earnings) + ".");
}

void Game::sellMilkProducts()
{
    int earnings = warehouseMilkCount * milkPrice;

    if (earnings <= 0)
    {
        printMessage("No milk to sell.");
        return;
    }

    budget += earnings;
    goalProgress += earnings;
    warehouseMilkCount = 0;
    checkLevelGoal();

    redrawField();
    printBudget("BUDGET = $" + to_string(budget));
    printMessage("Milk sold for $" + to_string(earnings) + ".");
}

bool Game::isPaused() const
{
	return paused;
}

void Game::registerAnimalProduct(const string& productLabel)
{
	if (productLabel == "Egg")
	{
		warehouseEggCount++;
		checkLevelGoal();
	}
	else if (productLabel == "Milk")
	{
		warehouseMilkCount++;
		checkLevelGoal();
	}
}

void Game::updateAnimalProduction(int elapsedSeconds)
{
	for (Animal* animal : animals)
	{
		// Tell the animal time has passed. If it returns true, it produced something.
		if (animal->advanceProduction(elapsedSeconds))
		{
			// Check which animal it is and add the product to the Game's inventory
			if (animal->getProductLabel() == "Egg")
			{
				warehouseEggCount++;
				// Optional: updatestatusbar();
			}
			else if (animal->getProductLabel() == "Milk")
			{
				warehouseMilkCount++;
				// Optional: updatestatusbar();
			}
		}
	}
}

void Game::collectEggs()
{
	if (eggCount > 0) {
		warehouseEggCount += eggCount;
		eggCount = 0; // Remove from the icon bar
		checkLevelGoal(); //hecks for a level up right after eggs are moved into the warehouse
		updatestatusbar(); // Refresh UI if necessary
	}
}

void Game::collectMilk()
{
	if (milkCount > 0) {
		warehouseMilkCount += milkCount;
		milkCount = 0; // Remove from the icon bar
		checkLevelGoal(); //checks for a level-up right after milk is moved into the warehouse
		updatestatusbar(); // Refresh UI if necessary
	}
}

bool Game::isPointInsidePrimaryWolf(int x, int y) const //tests wolf click bounds
{
	return mainWolfVisible && //requires the wolf to be visible before it can be clicked
		x >= wolfX && x <= wolfX + 80 && // checks the click’s horizontal range against the wolf image
		y >= wolfY && y <= wolfY + 80; //checks the click’s vertical range against the wolf image
}

bool Game::isPointInsideExtraWolf(int index, int x, int y) const
{
	if (index < 0 || index >= static_cast<int>(wolves.size()))
		return false;

	return x >= wolves[index].x && x <= wolves[index].x + 80 &&
		y >= wolves[index].y && y <= wolves[index].y + 80;
}

bool Game::isPointInsideWarehouse(int x, int y) const
{
	return (x >= 1250 && x <= 1470) && //Left = 1250, Right = 1250 + 220 = 1470
		(y >= 550 && y <= 730); // Top = 550, Bottom = 550 + 180  = 730
}

bool Game::isPointInsideEggSellButton(int x, int y) const
{
	return x >= sellButtonLeft && x <= sellButtonRight &&
		y >= eggSellButtonTop && y <= eggSellButtonBottom;
}

bool Game::isPointInsideMilkSellButton(int x, int y) const
{
	return x >= sellButtonLeft && x <= sellButtonRight &&
		y >= milkSellButtonTop && y <= milkSellButtonBottom;
}

void Game::showWarehouseWindow() //starts the function that opens a new warehouse window
{
    window* warehouseWindow = new window(430, 240, config.wx + 120, config.wy + 120);
    warehouseWindow->SetWaitClose(false);
    warehouseWindow->SetBuffering(true);
    warehouseWindow->ChangeTitle("Warehouse Details");

    while (warehouseWindow->IsOpen())
    {
		//draw bkgrnd
        warehouseWindow->SetPen(WHITE, 1);
        warehouseWindow->SetBrush(WHITE);
        warehouseWindow->DrawRectangle(0, 0, 430, 240);
		//warehouse header
        warehouseWindow->SetPen(BLACK);
        warehouseWindow->SetFont(22, BOLD, BY_NAME, "Arial");
        warehouseWindow->DrawString(20, 20, "Warehouse Summary");
		//warehouse txt
        warehouseWindow->SetFont(18, BOLD, BY_NAME, "Arial");
        warehouseWindow->DrawString(20, 70, "Eggs: " + to_string(warehouseEggCount) + " ($" + to_string(warehouseEggCount * eggPrice) + ")");
        warehouseWindow->DrawString(20, 110, "Milk: " + to_string(warehouseMilkCount) + " ($" + to_string(warehouseMilkCount * milkPrice) + ")");
		
        int btnL = 300, btnR = 400;
        warehouseWindow->SetPen(DARKRED, 2);
        warehouseWindow->SetBrush(WHITE);
        warehouseWindow->DrawRectangle(btnL, 65, btnR, 95, FILLED, 8, 8);
        warehouseWindow->DrawRectangle(btnL, 105, btnR, 135, FILLED, 8, 8);
		//sell button
        warehouseWindow->SetFont(16, BOLD, BY_NAME, "Arial");
        warehouseWindow->DrawString(btnL + 15, 70, "SELL");
        warehouseWindow->DrawString(btnL + 15, 110, "SELL");
        warehouseWindow->UpdateBuffer();
		//get click area
        int popupX = 0, popupY = 0;
        clicktype popupClick = warehouseWindow->GetMouseClick(popupX, popupY);

        if (popupClick == LEFT_CLICK && popupX >= btnL && popupX <= btnR)
        {
            if (popupY >= 65 && popupY <= 95)
            {
                sellEggProducts();
            }
            else if (popupY >= 105 && popupY <= 135)
            {
                sellMilkProducts();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    delete warehouseWindow;
}

void Game::advanceLevel() // starts the level up function
{
	level++;
	gametimer(level); //recalculates the timer for the new level
	goalProgress = 0; //resets the current level goal progress
	goalTarget += 700; //increases the next goal target after leveling up
	
	mainWolfVisible = false; //makes the main wolf visible again for the new level
	wolvesSpawned = false;
	wolfSpawnCountdown = 10;
	consecutiveWolfClicks = 0;//resets the wolf click combo counter
	wolves.clear();
	wolvesVel.clear();

	//spawnWolves();
	redrawField();
	printMessage("Level increased to " + to_string(level) + "."); // shows a status-bar message telling the player they leveled up
}

void Game::checkLevelGoal() //goal check function

{
	while (goalProgress >= goalTarget)
		advanceLevel();
}

void Game::resetWolfHitCounters(int clickedWolfIndex)
{
	if (clickedWolfIndex == -1)
	{
		for (size_t i = 0; i < wolfHitCounts.size(); i++)
			wolfHitCounts[i] = 0;
		return;
	}

	consecutiveWolfClicks = 0;

	for (size_t i = 0; i < wolfHitCounts.size(); i++)
	{
		if (static_cast<int>(i) != clickedWolfIndex)
			wolfHitCounts[i] = 0;
	}
}

void Game::updateStatusMessageTimer()
{
	if (statusMessageTimer > 0)
	{
		statusMessageTimer--;
		if (statusMessageTimer == 0)
			statusMessage.clear();
	}
}

void Game::handlePlayAreaClick(int x, int y)
{
	if (isPointInsideEggSellButton(x, y))
	{
		sellEggProducts();
		return;
	}

	if (isPointInsideMilkSellButton(x, y))
	{
		sellMilkProducts();
		return;
	}

	if (isPointInsideWarehouse(x, y)) //checks whether the warehouse itself was clicked
	{
		consecutiveWolfClicks = 0;//resets the wolf combo because the click was not on the wolf
		resetWolfHitCounters(-2);
		showWarehouseWindow();//opens the popup warehouse window
		return;
	}

	if (isPointInsidePrimaryWolf(x, y)) //checks whether the main wolf was clicked
	{
		resetWolfHitCounters(-1);
		consecutiveWolfClicks++;

		if (consecutiveWolfClicks >= 5)
		{
			mainWolfVisible = false;
			consecutiveWolfClicks = 0;
			printMessage("The wolf disappeared.");
		}
		else
		{
			printMessage("Wolf hits: " + to_string(consecutiveWolfClicks) + "/5");
		}

		return;
	}

	for (size_t i = 0; i < wolves.size(); i++)
	{
		if (isPointInsideExtraWolf(static_cast<int>(i), x, y))
		{
			resetWolfHitCounters(static_cast<int>(i));
			wolfHitCounts[i]++;

			if (wolfHitCounts[i] >= 5)
			{
				wolves.erase(wolves.begin() + i);
				wolfHitCounts.erase(wolfHitCounts.begin() + i);
				wolvesVel.erase(wolvesVel.begin() + i);
				printMessage("A wolf disappeared.");
			}
			else
			{
				printMessage("Wolf hits: " + to_string(wolfHitCounts[i]) + "/5");
			}

			return;
		}
	}

	consecutiveWolfClicks = 0;
	resetWolfHitCounters(-2);
}

void Game::go()
{
	//This function reads the position where the user clicks to determine the desired operation
	int x = -1, y = -1;
	bool isExit = false;

	//Change the title
	pWind->SetBuffering(true);
	pWind->UpdateBuffer();
	pWind->ChangeTitle("Farm Frenzy");

	//Set up the clock tracker before the loop starts
	auto lastTime = std::chrono::steady_clock::now();
	auto lastProductionTick = lastTime;

	do
	{
		updateStatusMessageTimer();
		auto currentTime = std::chrono::steady_clock::now();

		// Check if 1 second has passed
		if (!paused && std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime).count() >= 1)
		{
			if (time > 0) {
				time--; // Decrease timer
			}

			if (!wolvesSpawned) {
				if (wolfSpawnCountdown > 0) {
					wolfSpawnCountdown--;
				} // Holds the first 10 seconds of gameplay

				else {
					wolvesSpawned = true;
					bool safe = false;
					while (!safe) 
					{
						wolfX = 50 + (rand() % (config.windWidth - 80 - 100));
						wolfY = (2 * config.toolBarHeight + 20) + (rand() % (config.windHeight - config.statusBarHeight - 80 - 40));
						// Main wolf uses a size of 80x80
						safe = !isAreaOccupiedByAnimal(wolfX, wolfY, 80, 80);
					}
					mainWolfVisible = true;
					spawnWolves();
					printMessage("Watch out! The wolves have arrived!");
				}
			}

			if (time <= 0) {
				paused = true; // Stop the game when time runs out
			}

			lastTime = currentTime; // Reset the clock tracker
		}
		else if (paused)
		{
			lastTime = currentTime;
		}
		else
		{
			lastProductionTick = currentTime;
		}

		pWind->SetBuffering(true); // Prevent flickering

		string budget_string_code = "BUDGET = $" + to_string(budget);
		string budget_string = "BUDGET: $" + to_string(budget);
		string prices = " | Chick: $100 | Cow: $200 | water: $50 ";
		printBudget(budget_string + prices); //printBudget bar

		gameToolbar->draw();
		gameBudgetbar->draw();
		drawfieldboundary();
		updatestatusbar();

		clicktype click = getMouseClick(x, y);	//Get the coordinates of the user click

		if (!paused)
			updatePlayArea();
		else if (paused && time > 0)
			printMessage("Game paused"); // Only say paused if there is still time left
		else if (time <= 0) 
		{
			pWind->SetPen(RED, 5);
			pWind->SetFont(90, BOLD, BY_NAME, "Arial");

			string msg = "GAME OVER";
			int textWidth, textHeight;

			// Calculate center of the screen instead of hardcoding the text position
			pWind->GetStringSize(textWidth, textHeight, msg);
			int x = (config.windWidth - textWidth) / 2;
			int y = (config.windHeight - textHeight) / 2;

			pWind->DrawString(x, y, msg);

			// Shows the final budget below the Game Over message
			pWind->SetFont(30, BOLD, BY_NAME, "Arial");
			string subMsg = "Final Budget: $" + to_string(budget);
			pWind->GetStringSize(textWidth, textHeight, subMsg);
			pWind->DrawString((config.windWidth - textWidth) / 2, y + 100, subMsg);
		}
		else
			redrawField();

		pWind->UpdateBuffer(); // Update the buffer after all drawing is finished

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		//if (gameMode == MODE_DSIGN)		//Game is in the Design mode
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
			handlePlayAreaClick(x, y);
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

		// makes collision with the main wolf happen only while that wolf is visible
		if (mainWolfVisible &&
			p.x < wolfX + 80 && p.x + w > wolfX &&
			p.y < wolfY + 80 && p.y + h > wolfY)
		{
			isEaten = true;
		}

		// B. Check collision with any multiplied/cloned wolves
		if (!isEaten) {
			for (size_t i = 0; i < wolves.size(); i++) {
				if (p.x < wolves[i].x + 80 && p.x + w > wolves[i].x &&
					p.y < wolves[i].y + 80 && p.y + h > wolves[i].y)
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

	static auto lastTime = std::chrono::steady_clock::now();
	auto currentTime = std::chrono::steady_clock::now();

	int realSecondsPassed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime).count();

	// 1. Check if the game is paused (from clicking Pause or opening Warehouse)
	if (paused || realSecondsPassed > 1) // large gap means we just resumed from a pause
	{
		lastTime = currentTime; // discard accumulated pause time
		realSecondsPassed = 0;  // don't advance the production counter
	}
	else if (realSecondsPassed >= 1)
	{
		updateAnimalProduction(realSecondsPassed);
		lastTime = currentTime;
	}

	// 2. If NOT paused, check if a second has passed
	else if (realSecondsPassed >= 1)
	{
		updateAnimalProduction(realSecondsPassed); // Tell animals to produce
		lastTime = currentTime; // Reset the timer for the next second
	}

	// 3. Clear the playing area and draw static background elements
	clearPlayingArea();
	drawFieldBackground();
	drawfieldboundary();
	drawGrass();

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

	drawWarehouse();
}
