#pragma once
#include "../CMUgraphicsLib/CMUgraphics.h"
#include "../UI/Toolbar.h"
#include "../UI/BudgetBar.h"
#include <vector>

class Animal;

enum AnimalType
{
	ANIMAL_CHICK,
	ANIMAL_COW
};

class Game
{
private:
	window* pWind;
	Toolbar* gameToolbar;
	Budgetbar* gameBudgetbar;
	std::vector<point> wolvesVel;
	std::vector<Animal*> animals;
	std::vector<point> wolves;
	std::vector<int> wolfHitCounts;

	struct GrassData
	{
		point pos;
		int timeRemaining;
	};

	std::vector<GrassData> grassPatches;

	struct ProductData
	{
		string label;
		point pos;
	};

	std::vector<ProductData> fieldProducts;
	
	bool paused;
	int eggCount;
	int milkCount;

	int warehouseEggCount;
	int warehouseMilkCount;
	int goalTarget;
	int goalProgress;
	bool mainWolfVisible;
	int consecutiveWolfClicks;
	mutable string statusMessage;
	mutable int statusMessageTimer;

	bool wolvesSpawned;
	int wolfSpawnCountdown;
	
	struct DeadWolf
	{
		bool isMain;
		int respawnCountdown;
	};
	std::vector<DeadWolf> deadWolves;
	void tickWolfRespawns();

	bool isAreaOccupiedByAnimal(int x, int y, int width, int height) const;
	void clearPlayingArea() const;
	void drawFieldBackground() const;
	void drawWolf() const;
	void drawWarehouse() const;
	void drawFieldProducts() const;
	void drawCollectAllButton() const;
	void addFieldProduct(const string& productLabel, point animalPosition, int animalWidth, int animalHeight);
	point getRandomAnimalPosition(int animalWidth, int animalHeight) const;
	bool collectFieldProductAt(int x, int y);
	bool isPointInsideCollectAllButton(int x, int y) const;
	void sellEggProducts(int amount);
	void sellMilkProducts(int amount);
	bool isPointInsidePrimaryWolf(int x, int y) const;
	bool isPointInsideExtraWolf(int index, int x, int y) const;
	bool isPointInsideWarehouse(int x, int y) const;
	bool isPointInsideEggSellButton(int x, int y) const;
	bool isPointInsideMilkSellButton(int x, int y) const;
	void showWarehouseWindow();
	void handlePlayAreaClick(int x, int y);
	void checkLevelGoal();
	void advanceLevel();
	void resetWolfHitCounters(int clickedWolfIndex);
	void updateStatusMessageTimer();

public:
	int budget = 2500;
	int level = 1;
	int time = 150;
	int animalcount = 0;

	mutable int wolfX;
	mutable int wolfY;
	mutable int wolfVelX;
	mutable int wolfVelY;
	void moveWolf();

	void addGrassPatch(point p);
	void drawGrass() const;
	bool isStandingOnGrass(point p, int w, int h) const;

	void collectEggs();
	void collectMilk();

	void gametimer(int level);
	Game();
	~Game();

	void updatePlayArea();
	void updateAnimalProduction(int elapsedSeconds);

	clicktype getMouseClick(int& x, int& y) const;
	string getSrting() const;
	window* CreateWind(int, int, int, int) const;
	void createToolbar();
	void createBudgetbar();
	void clearBudget() const;
	void printBudget(string msg) const;
	void clearStatusBar() const;
	void updatestatusbar() const;
	void drawfieldboundary() const;
	void Restart();
	void printMessage(string msg) const;
	void spawnWolves();
	void go();
	void redrawField() const;
	bool canAfford(int amount) const;
	bool spendBudget(int amount);
	void placeAnimal(AnimalType animalType);
	void placeFoodArea();
	void pauseGame();
	void resumeGame();
	void saveGame();
	void loadGame();
	void collectAllFieldProducts();
	void sellEggProducts();
	void sellMilkProducts();
	bool isPaused() const;
	void registerAnimalProduct(const string& productLabel);
	window* getWind() const;
};

