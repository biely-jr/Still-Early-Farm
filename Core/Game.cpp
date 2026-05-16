#include <iostream>
#include "Game.h"
#include "../Config/GameConfig.h"
#include <thread>
#include <chrono>
#include "../Entities/Animal.h"
#include <random>
#include <fstream>
#include <mmsystem.h>

namespace
{
	const int chickCost = 100;
	const int cowCost = 200;
	const string backgroundImagePath = "images\\BackGround.jpg";
	const string wolfImagePath = "images\\Wolf.jpg";
	const string eggImagePath = "images\\Egg.jpg";
	const string milkImagePath = "images\\Milk.jpg";
	const string warehouseImagePath = "images\\warehouse.jpg";
	const string collectAllImagePath = "images\\claim-all.jpg";
	const int collectAllButtonWidth = 70;
	const int collectAllButtonHeight = 50;
	const int eggPrice = 25;
	const int milkPrice = 50;
	const int sellButtonLeft = 1375;
	const int sellButtonRight = 1455;
	const int eggSellButtonTop = 352;
	const int eggSellButtonBottom = 382;
	const int milkSellButtonTop = 397;
	const int milkSellButtonBottom = 427;
	const string musicAlias = "farm_bgm";
	const string musicPathFromProject = "audio\\videoplayback.mp3";
	const string musicPathFromDebug = "..\\audio\\videoplayback.mp3";

	// -------------------------------------------------------
	// Warehouse boundary constants (must match drawWarehouse)
	// -------------------------------------------------------
	const int WAREHOUSE_X = 1250;
	const int WAREHOUSE_Y = 550;
	const int WAREHOUSE_W = 220;
	const int WAREHOUSE_H = 180;

	string getExistingMusicPath()
	{
		if (GetFileAttributesA(musicPathFromProject.c_str()) != INVALID_FILE_ATTRIBUTES)
			return musicPathFromProject;
		if (GetFileAttributesA(musicPathFromDebug.c_str()) != INVALID_FILE_ATTRIBUTES)
			return musicPathFromDebug;
		return "";
	}

	void startBackgroundMusic()
	{
		string musicPath = getExistingMusicPath();
		if (musicPath.empty())
			return;

		mciSendStringA(("close " + musicAlias).c_str(), NULL, 0, NULL);
		string openCommand = "open \"" + musicPath + "\" type mpegvideo alias " + musicAlias;
		if (mciSendStringA(openCommand.c_str(), NULL, 0, NULL) == 0)
			mciSendStringA(("play " + musicAlias + " repeat").c_str(), NULL, 0, NULL);
	}

	void stopBackgroundMusic()
	{
		mciSendStringA(("stop " + musicAlias).c_str(), NULL, 0, NULL);
		mciSendStringA(("close " + musicAlias).c_str(), NULL, 0, NULL);
	}
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
	startBackgroundMusic();

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
	stopBackgroundMusic();

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

	pWind->DrawImage(backgroundImagePath, 0, playTop, config.windWidth, playBottom - playTop);
}

// Adds a new grass patch to the list
void Game::addGrassPatch(point p)
{
	GrassData newGrass;
	newGrass.pos = p;
	newGrass.timeRemaining = 30; // Set lifespan to 30 seconds

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

void Game::tickWolfRespawns()
{
	for (auto it = deadWolves.begin(); it != deadWolves.end(); )
	{
		it->respawnCountdown--;

		if (it->respawnCountdown <= 0)
		{
			if (it->isMain)
			{
				bool safe = false;
				while (!safe)
				{
					wolfX = 50 + (rand() % (config.windWidth - 80 - 100));
					wolfY = (2 * config.toolBarHeight + 20) + (rand() % (config.windHeight - config.statusBarHeight - 80 - 40));
					safe = !isAreaOccupiedByAnimal(wolfX, wolfY, 80, 80);
				}
				mainWolfVisible = true;
				printMessage("The main wolf is back!");
			}
			else
			{
				int randX, randY;
				bool safe = false;
				while (!safe)
				{
					randX = 50 + (rand() % (config.windWidth - 140 - 50));
					randY = (2 * config.toolBarHeight) + 20 + (rand() % (config.windHeight - config.statusBarHeight - 140 - 20));
					safe = !isAreaOccupiedByAnimal(randX, randY, 70, 70);
				}
				point wolfPos = { randX, randY };
				wolves.push_back(wolfPos);
				wolfHitCounts.push_back(0);
				point vel;
				vel.x = (rand() % 3) - 1;
				vel.y = (rand() % 3) - 1;
				if (vel.x == 0 && vel.y == 0) vel.x = 1;
				wolvesVel.push_back(vel);
				printMessage("A wolf is back!");
			}

			it = deadWolves.erase(it);
		}
		else
			++it;
	}
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

	// -------------------------------------------------------
	// Helper: bounce a rectangle off the warehouse
	// wolfSz = the wolf's width/height (square wolves)
	// posX/posY = wolf position (top-left corner)
	// velX/velY = wolf velocity components
	// -------------------------------------------------------
	auto bounceOffWarehouse = [](int wolfSz, int& posX, int& posY, int& velX, int& velY)
		{
			bool overlapX = (posX + wolfSz > WAREHOUSE_X) && (posX < WAREHOUSE_X + WAREHOUSE_W);
			bool overlapY = (posY + wolfSz > WAREHOUSE_Y) && (posY < WAREHOUSE_Y + WAREHOUSE_H);

			if (!overlapX || !overlapY)
				return; // no collision

			int penLeft = (posX + wolfSz) - WAREHOUSE_X;
			int penRight = (WAREHOUSE_X + WAREHOUSE_W) - posX;
			int penTop = (posY + wolfSz) - WAREHOUSE_Y;
			int penBottom = (WAREHOUSE_Y + WAREHOUSE_H) - posY;

			int minPenX = min(penLeft, penRight);
			int minPenY = min(penTop, penBottom);

			if (minPenX < minPenY)
			{
				// Push out horizontally
				if (penLeft < penRight) posX = WAREHOUSE_X - wolfSz;
				else                    posX = WAREHOUSE_X + WAREHOUSE_W;
				velX = -velX;
			}
			else
			{
				// Push out vertically
				if (penTop < penBottom) posY = WAREHOUSE_Y - wolfSz;
				else                    posY = WAREHOUSE_Y + WAREHOUSE_H;
				velY = -velY;
			}
		};

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

		// Field boundary checks and bouncing for extra wolves
		if (wolves[i].x < 0) { wolves[i].x = 0; wolvesVel[i].x = 1; }
		if (wolves[i].x > config.windWidth - 70) { wolves[i].x = config.windWidth - 70; wolvesVel[i].x = -1; }
		if (wolves[i].y < config.toolBarHeight * 2) { wolves[i].y = config.toolBarHeight * 2; wolvesVel[i].y = 1; }
		if (wolves[i].y > config.windHeight - config.statusBarHeight - 70) { wolves[i].y = config.windHeight - config.statusBarHeight - 70; wolvesVel[i].y = -1; }

		// Warehouse boundary bounce for extra wolves (size 70x70)
		bounceOffWarehouse(70, wolves[i].x, wolves[i].y, wolvesVel[i].x, wolvesVel[i].y);
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

		// Field boundary checks and bouncing for primary wolf
		if (wolfX < 0) { wolfX = 0; wolfVelX = 1; }
		if (wolfX > config.windWidth - 80) { wolfX = config.windWidth - 80; wolfVelX = -1; }
		if (wolfY < config.toolBarHeight * 2) { wolfY = config.toolBarHeight * 2; wolfVelY = 1; }
		if (wolfY > config.windHeight - config.statusBarHeight - 80) { wolfY = config.windHeight - config.statusBarHeight - 80; wolfVelY = -1; }

		// Warehouse boundary bounce for primary wolf (size 80x80)
		bounceOffWarehouse(80, wolfX, wolfY, wolfVelX, wolfVelY);
	}
}

void Game::drawWarehouse() const
{
	pWind->DrawImage(warehouseImagePath, 1250, 550, 220, 180);
}

void Game::drawFieldProducts() const
{
	for (const ProductData& product : fieldProducts)
	{
		const string imagePath = (product.label == "Egg") ? eggImagePath : milkImagePath;
		pWind->DrawImage(imagePath, product.pos.x, product.pos.y, 30, 30);
	}
}

void Game::drawCollectAllButton() const
{
	pWind->DrawImage(collectAllImagePath, config.windWidth - collectAllButtonWidth, 0, collectAllButtonWidth, collectAllButtonHeight);
}

void Game::addFieldProduct(const string& productLabel, point animalPosition, int animalWidth, int animalHeight)
{
	ProductData product;
	product.label = productLabel;
	product.pos.x = animalPosition.x + (animalWidth / 2) - 15;
	product.pos.y = animalPosition.y + (animalHeight / 2) - 15;

	if (product.pos.x < 0)
		product.pos.x = 0;
	if (product.pos.x > config.windWidth - 30)
		product.pos.x = config.windWidth - 30;
	if (product.pos.y < config.toolBarHeight * 2)
		product.pos.y = config.toolBarHeight * 2;
	if (product.pos.y > config.windHeight - config.statusBarHeight - 30)
		product.pos.y = config.windHeight - config.statusBarHeight - 30;

	fieldProducts.push_back(product);
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
	fieldProducts.clear();

	if (level == 1)
		goalTarget = 500;
	else {
		goalTarget = 500;
		for (int i = 1; i < level; i++)
		{
			goalTarget += 450;
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
	printBudget("BUDGET: $" + to_string(budget) + " | Chick: $100 | Cow: $200 | Food Area: $50 ");
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

	drawFieldProducts();
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
		printMessage("Resume the game before adding food area.");
		return;
	}

	if (!spendBudget(50))
	{
		printMessage("Not enough budget for food area.");
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
	ofstream saveFile("savegame.txt");

	if (!saveFile)
	{
		printMessage("Could not save game.");
		return;
	}

	saveFile << "StillEarlyFarmSave 1\n";
	saveFile << "budget " << budget << "\n";
	saveFile << "level " << level << "\n";
	saveFile << "time " << time << "\n";
	saveFile << "animalcount " << animalcount << "\n";
	saveFile << "paused " << paused << "\n";
	saveFile << "eggCount " << eggCount << "\n";
	saveFile << "milkCount " << milkCount << "\n";
	saveFile << "warehouseEggCount " << warehouseEggCount << "\n";
	saveFile << "warehouseMilkCount " << warehouseMilkCount << "\n";
	saveFile << "goalTarget " << goalTarget << "\n";
	saveFile << "goalProgress " << goalProgress << "\n";
	saveFile << "mainWolfVisible " << mainWolfVisible << "\n";
	saveFile << "wolf " << wolfX << " " << wolfY << " " << wolfVelX << " " << wolfVelY << "\n";

	saveFile << "animals " << animals.size() << "\n";
	for (Animal* animal : animals)
	{
		const string animalType = (animal->getWidth() >= 70) ? "Cow" : "Chick";
		point animalPosition = animal->getRefPoint();
		saveFile << animalType << " "
			<< animalPosition.x << " " << animalPosition.y << " "
			<< animal->curr_vel.x << " " << animal->curr_vel.y << " "
			<< animal->getProductionCounter() << "\n";
	}

	saveFile << "grass " << grassPatches.size() << "\n";
	for (const GrassData& grass : grassPatches)
		saveFile << grass.pos.x << " " << grass.pos.y << " " << grass.timeRemaining << "\n";

	saveFile << "fieldProducts " << fieldProducts.size() << "\n";
	for (const ProductData& product : fieldProducts)
		saveFile << product.label << " " << product.pos.x << " " << product.pos.y << "\n";

	saveFile << "wolves " << wolves.size() << "\n";
	for (size_t i = 0; i < wolves.size(); i++)
	{
		saveFile << wolves[i].x << " " << wolves[i].y << " "
			<< wolvesVel[i].x << " " << wolvesVel[i].y << " "
			<< wolfHitCounts[i] << "\n";
	}

	printMessage("Game saved to savegame.txt.");
}

void Game::loadGame()
{
	ifstream saveFile("savegame.txt");

	if (!saveFile)
	{
		printMessage("No save file found.");
		return;
	}

	// ---- Step 1: Validate header ----
	string header, version;
	saveFile >> header >> version;
	if (header != "StillEarlyFarmSave")
	{
		printMessage("Save file is corrupt.");
		return;
	}

	// ---- Step 2: Read all scalar fields ----
	string key;
	int  tempBudget, tempLevel, tempTime, tempAnimalcount;
	bool tempPaused;
	int  tempEggCount, tempMilkCount;
	int  tempWarehouseEggCount, tempWarehouseMilkCount;
	int  tempGoalTarget, tempGoalProgress;
	bool tempMainWolfVisible;
	int  tempWolfX, tempWolfY, tempWolfVelX, tempWolfVelY;

	saveFile >> key >> tempBudget;
	saveFile >> key >> tempLevel;
	saveFile >> key >> tempTime;
	saveFile >> key >> tempAnimalcount;
	saveFile >> key >> tempPaused;
	saveFile >> key >> tempEggCount;
	saveFile >> key >> tempMilkCount;
	saveFile >> key >> tempWarehouseEggCount;
	saveFile >> key >> tempWarehouseMilkCount;
	saveFile >> key >> tempGoalTarget;
	saveFile >> key >> tempGoalProgress;
	saveFile >> key >> tempMainWolfVisible;
	saveFile >> key >> tempWolfX >> tempWolfY >> tempWolfVelX >> tempWolfVelY;

	// ---- Step 3: Read animals ----
	int animalCount;
	saveFile >> key >> animalCount; // reads "animals <N>"

	vector<Animal*> loadedAnimals;
	for (int i = 0; i < animalCount; i++)
	{
		string type;
		int ax, ay, avx, avy, aprod;
		saveFile >> type >> ax >> ay >> avx >> avy >> aprod;

		point pos = { ax, ay };
		Animal* a = nullptr;

		if (type == "Chick")
			a = new Chick(this, pos, 50, 50, "images\\chick.jpg");
		else
			a = new Cow(this, pos, 70, 70, "images\\cow.jpg");

		a->curr_vel.x = avx;
		a->curr_vel.y = avy;
		a->addProductionTime(aprod); // restores the saved production timer
		loadedAnimals.push_back(a);
	}

	// ---- Step 4: Read grass patches ----
	int grassCount;
	saveFile >> key >> grassCount; // reads "grass <N>"

	vector<GrassData> loadedGrass;
	for (int i = 0; i < grassCount; i++)
	{
		GrassData g;
		saveFile >> g.pos.x >> g.pos.y >> g.timeRemaining;
		loadedGrass.push_back(g);
	}

	// ---- Step 5: Read field products ----
	int productCount;
	saveFile >> key >> productCount; // reads "fieldProducts <N>"

	vector<ProductData> loadedProducts;
	for (int i = 0; i < productCount; i++)
	{
		ProductData p;
		saveFile >> p.label >> p.pos.x >> p.pos.y;
		loadedProducts.push_back(p);
	}

	// ---- Step 6: Read wolves ----
	int wolfCount;
	saveFile >> key >> wolfCount; // reads "wolves <N>"

	vector<point> loadedWolves;
	vector<point> loadedWolvesVel;
	vector<int>   loadedWolfHitCounts;

	for (int i = 0; i < wolfCount; i++)
	{
		point wp, wv;
		int hits;
		saveFile >> wp.x >> wp.y >> wv.x >> wv.y >> hits;
		loadedWolves.push_back(wp);
		loadedWolvesVel.push_back(wv);
		loadedWolfHitCounts.push_back(hits);
	}

	// ---- All data read. Now apply everything atomically. ----

	// Step 7: Destroy current animals
	for (Animal* a : animals)
		delete a;
	animals.clear();

	// Step 8: Apply scalars
	budget = tempBudget;
	level = tempLevel;
	time = tempTime;
	animalcount = tempAnimalcount;
	paused = false;              // always resume on load
	eggCount = tempEggCount;
	milkCount = tempMilkCount;
	warehouseEggCount = tempWarehouseEggCount;
	warehouseMilkCount = tempWarehouseMilkCount;
	goalTarget = tempGoalTarget;
	goalProgress = tempGoalProgress;
	mainWolfVisible = tempMainWolfVisible;
	wolfX = tempWolfX;
	wolfY = tempWolfY;
	wolfVelX = tempWolfVelX;
	wolfVelY = tempWolfVelY;

	// Step 9: Apply collections
	animals = loadedAnimals;
	grassPatches = loadedGrass;
	fieldProducts = loadedProducts;
	wolves = loadedWolves;
	wolvesVel = loadedWolvesVel;
	wolfHitCounts = loadedWolfHitCounts;

	// Step 10: Reset transient state that is never saved
	consecutiveWolfClicks = 0;
	statusMessage = "";
	statusMessageTimer = 0;
	deadWolves.clear();

	// If the save had wolves on screen, they had already spawned
	wolvesSpawned = (mainWolfVisible || !wolves.empty());
	wolfSpawnCountdown = wolvesSpawned ? 0 : 10;

	// Step 11: Rebuild UI bars
	delete gameToolbar;
	delete gameBudgetbar;
	createToolbar();
	createBudgetbar();

	// Step 12: Redraw everything from scratch
	pWind->SetPen(config.bkGrndColor, 1);
	pWind->SetBrush(config.bkGrndColor);
	pWind->DrawRectangle(0, 0, config.windWidth, config.windHeight);

	redrawField();
	updatestatusbar();
	printBudget("BUDGET: $" + to_string(budget) + " | Chick: $100 | Cow: $200 | Food Area: $50 ");
	printMessage("Game loaded successfully.");
}

void Game::sellEggProducts()
{
	sellEggProducts(warehouseEggCount);
}

void Game::sellEggProducts(int amount)
{
	if (amount > warehouseEggCount)
		amount = warehouseEggCount;

	int earnings = amount * eggPrice;

	if (earnings <= 0)
	{
		printMessage("No eggs to sell.");
		return;
	}

	budget += earnings;
	goalProgress += earnings;
	warehouseEggCount -= amount;
	checkLevelGoal();

	redrawField();
	printBudget("BUDGET = $" + to_string(budget));
	printMessage(to_string(amount) + " eggs sold for $" + to_string(earnings) + ".");
}

void Game::sellMilkProducts()
{
	sellMilkProducts(warehouseMilkCount);
}

void Game::sellMilkProducts(int amount)
{
	if (amount > warehouseMilkCount)
		amount = warehouseMilkCount;

	int earnings = amount * milkPrice;

	if (earnings <= 0)
	{
		printMessage("No milk to sell.");
		return;
	}

	budget += earnings;
	goalProgress += earnings;
	warehouseMilkCount -= amount;
	checkLevelGoal();

	redrawField();
	printBudget("BUDGET = $" + to_string(budget));
	printMessage(to_string(amount) + " milk sold for $" + to_string(earnings) + ".");
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

bool Game::collectFieldProductAt(int x, int y)
{
	for (auto it = fieldProducts.begin(); it != fieldProducts.end(); ++it)
	{
		if (x >= it->pos.x && x <= it->pos.x + 30 &&
			y >= it->pos.y && y <= it->pos.y + 30)
		{
			registerAnimalProduct(it->label);
			printMessage(it->label + " collected.");
			fieldProducts.erase(it);
			return true;
		}
	}

	return false;
}

bool Game::isPointInsideCollectAllButton(int x, int y) const
{
	return x >= config.windWidth - collectAllButtonWidth && x <= config.windWidth &&
		y >= 0 && y <= collectAllButtonHeight;
}

void Game::collectAllFieldProducts()
{
	if (fieldProducts.empty())
	{
		printMessage("No products to collect.");
		return;
	}

	int collectedEggs = 0;
	int collectedMilk = 0;

	for (const ProductData& product : fieldProducts)
	{
		if (product.label == "Egg")
			collectedEggs++;
		else if (product.label == "Milk")
			collectedMilk++;

		registerAnimalProduct(product.label);
	}

	fieldProducts.clear();
	printMessage("Collected " + to_string(collectedEggs) + " eggs and " + to_string(collectedMilk) + " milk.");
}

void Game::updateAnimalProduction(int elapsedSeconds)
{
	for (Animal* animal : animals)
	{
		// Tell the animal time has passed. If it returns true, it produced something.
		if (animal->advanceProduction(elapsedSeconds))
		{
			addFieldProduct(animal->getProductLabel(), animal->getRefPoint(), animal->getWidth(), animal->getHeight());
		}
	}

	// Base countdown: tick every grass patch down by 1 each second
	for (GrassData& grass : grassPatches)
		grass.timeRemaining--;

	// Grass-animal interaction: animal on grass gets +1, that patch loses 1 extra
	for (Animal* animal : animals)
	{
		point p = animal->getRefPoint();
		int w = animal->getWidth();
		int h = animal->getHeight();

		for (GrassData& grass : grassPatches)
		{
			if (p.x < grass.pos.x + 40 && p.x + w > grass.pos.x &&
				p.y < grass.pos.y + 40 && p.y + h > grass.pos.y)
			{
				animal->addProductionTime(2); // animal earns +1 extra production second
				grass.timeRemaining -= 2;        // this patch loses 1 extra second
			}
		}
	}

	// Remove expired patches
	for (auto it = grassPatches.begin(); it != grassPatches.end(); )
	{
		if (it->timeRemaining <= 0)
			it = grassPatches.erase(it);
		else
			++it;
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
		x >= wolfX && x <= wolfX + 80 && // checks the click's horizontal range against the wolf image
		y >= wolfY && y <= wolfY + 80; //checks the click's vertical range against the wolf image
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
	window* warehouseWindow = new window(520, 240, config.wx + 120, config.wy + 120);
	warehouseWindow->SetWaitClose(false);
	warehouseWindow->SetBuffering(true);
	warehouseWindow->ChangeTitle("Warehouse Details");

	int selectedEggs = (warehouseEggCount > 0) ? 1 : 0;
	int selectedMilk = (warehouseMilkCount > 0) ? 1 : 0;

	while (warehouseWindow->IsOpen())
	{
		if (selectedEggs > warehouseEggCount)
			selectedEggs = warehouseEggCount;
		if (selectedMilk > warehouseMilkCount)
			selectedMilk = warehouseMilkCount;

		//draw bkgrnd
		warehouseWindow->SetPen(WHITE, 1);
		warehouseWindow->SetBrush(WHITE);
		warehouseWindow->DrawRectangle(0, 0, 520, 240);
		//warehouse header
		warehouseWindow->SetPen(BLACK);
		warehouseWindow->SetFont(22, BOLD, BY_NAME, "Arial");
		warehouseWindow->DrawString(20, 20, "Warehouse Summary");
		//warehouse txt
		warehouseWindow->SetFont(18, BOLD, BY_NAME, "Arial");
		warehouseWindow->DrawString(20, 70, "Eggs: " + to_string(warehouseEggCount) + " ($" + to_string(warehouseEggCount * eggPrice) + ")");
		warehouseWindow->DrawString(20, 110, "Milk: " + to_string(warehouseMilkCount) + " ($" + to_string(warehouseMilkCount * milkPrice) + ")");

		const int minusL = 190, minusR = 220;
		const int cashL = 230;
		const int numberL = 325;
		const int plusL = 355, plusR = 385;
		const int btnL = 410, btnR = 495;
		const int eggTop = 65, eggBottom = 95;
		const int milkTop = 105, milkBottom = 135;

		warehouseWindow->SetPen(DARKRED, 2);
		warehouseWindow->SetBrush(WHITE);
		warehouseWindow->DrawRectangle(minusL, eggTop, minusR, eggBottom, FILLED, 8, 8);
		warehouseWindow->DrawRectangle(plusL, eggTop, plusR, eggBottom, FILLED, 8, 8);
		warehouseWindow->DrawRectangle(btnL, eggTop, btnR, eggBottom, FILLED, 8, 8);

		warehouseWindow->DrawRectangle(minusL, milkTop, minusR, milkBottom, FILLED, 8, 8);
		warehouseWindow->DrawRectangle(plusL, milkTop, plusR, milkBottom, FILLED, 8, 8);
		warehouseWindow->DrawRectangle(btnL, milkTop, btnR, milkBottom, FILLED, 8, 8);

		//quantity controls and sell button
		warehouseWindow->SetFont(16, BOLD, BY_NAME, "Arial");
		warehouseWindow->DrawString(minusL + 10, eggTop + 5, "-");
		warehouseWindow->DrawString(cashL, eggTop + 5, "$" + to_string(selectedEggs * eggPrice));
		warehouseWindow->DrawString(numberL, eggTop + 5, to_string(selectedEggs));
		warehouseWindow->DrawString(plusL + 9, eggTop + 5, "+");
		warehouseWindow->DrawString(btnL + 15, eggTop + 5, "SELL");

		warehouseWindow->DrawString(minusL + 10, milkTop + 5, "-");
		warehouseWindow->DrawString(cashL, milkTop + 5, "$" + to_string(selectedMilk * milkPrice));
		warehouseWindow->DrawString(numberL, milkTop + 5, to_string(selectedMilk));
		warehouseWindow->DrawString(plusL + 9, milkTop + 5, "+");
		warehouseWindow->DrawString(btnL + 15, milkTop + 5, "SELL");
		warehouseWindow->UpdateBuffer();
		//get click area
		int popupX = 0, popupY = 0;
		clicktype popupClick = warehouseWindow->GetMouseClick(popupX, popupY);

		if (popupClick == LEFT_CLICK)
		{
			if (popupY >= eggTop && popupY <= eggBottom)
			{
				if (popupX >= minusL && popupX <= minusR && selectedEggs > 0)
					selectedEggs--;
				else if (popupX >= plusL && popupX <= plusR && selectedEggs < warehouseEggCount)
					selectedEggs++;
				else if (popupX >= btnL && popupX <= btnR)
					sellEggProducts(selectedEggs);
			}
			else if (popupY >= milkTop && popupY <= milkBottom)
			{
				if (popupX >= minusL && popupX <= minusR && selectedMilk > 0)
					selectedMilk--;
				else if (popupX >= plusL && popupX <= plusR && selectedMilk < warehouseMilkCount)
					selectedMilk++;
				else if (popupX >= btnL && popupX <= btnR)
					sellMilkProducts(selectedMilk);
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
	goalTarget += 450; //increases the next goal target after leveling up

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
	if (collectFieldProductAt(x, y))
		return;

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
			deadWolves.push_back({ true, 15 });
			printMessage("The Main wolf disappeared.");
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
				deadWolves.push_back({ false, 15 });
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
					if (level == 1) printMessage("Watch out! The wolves have arrived!");
				}
			}

			if (time <= 0) {
				paused = true; // Stop the game when time runs out
			}

			lastTime = currentTime; // Reset the clock tracker
			tickWolfRespawns();
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
		string prices = " | Chick: $100 | Cow: $200 | Food Area: $50 ";
		printBudget(budget_string + prices); //printBudget bar

		gameToolbar->draw();
		drawCollectAllButton();
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
			if (isPointInsideCollectAllButton(x, y))
				collectAllFieldProducts();
			else
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

	drawFieldProducts();
	drawWarehouse();
}
