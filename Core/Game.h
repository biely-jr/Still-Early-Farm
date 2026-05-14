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

struct FoodArea
{
	point topLeft;
	int width;
	int height;
	int counter;
	color fillColor;
	string label;
};

class Game
{
private:
	window* pWind;	//Pointer to the CMU graphics window
	Toolbar* gameToolbar;
	Budgetbar* gameBudgetbar;
	std::vector<Animal*> animals;
	std::vector<point> wolves;
	std::vector<int> wolfHitCounts; //Each extra wolf gets its own number, so every wolf can be killed separately after 5 clicks
	FoodArea foodAreas[2];
	bool paused;
	int eggCount;
	int milkCount;

	int warehouseEggCount;
	int warehouseMilkCount;
	int goalTarget; // stores how many total warehouse items are needed for the next level.
	int goalProgress; // stores progress for the current level only.
	bool mainWolfVisible; //remembers whether the main wolf should still be drawn.
	int consecutiveWolfClicks; //counts how many correct wolf clicks happened in a row.
	mutable string statusMessage; // string that stores the latest temporary message this is used for messages like Wolf hits and Level increased
	mutable int statusMessageTimer; //a timer that decides how long that temporary message stays visible

	bool wolvesSpawned; //acts as a switch ensuring we only trigger the spawn once per level
	int wolfSpawnCountdown; //tracks the remaining seconds until the spawn event
	
	bool isAreaOccupiedByAnimal(int x, int y, int width, int height) const;
	// Checks if any animal is currently inside the given rectangle coordinates to avoid wolves spawning on top of animals.

	void initializeFoodAreas();
	void clearPlayingArea() const;
	void drawFieldBackground() const;
	void drawWolf() const;
	void drawFoodArea(const FoodArea& area) const ;
	void drawAllFoodAreas() const;
	void drawWarehouse() const;
	point getRandomAnimalPosition(int animalWidth, int animalHeight) const;
	point getRandomFoodAreaPosition() const;
	bool isAnimalStandingOnFood(const Animal* animal) const;
	bool isPointInsidePrimaryWolf(int x, int y) const; //checks whether a click landed on the main wolf.
	bool isPointInsideExtraWolf(int index, int x, int y) const; //checks whether the player clicked one of the extra wolves
	bool isPointInsideWarehouse(int x, int y) const; //checks whether a click landed on the warehouse.
	bool isPointInsideEggSellButton(int x, int y) const;
	bool isPointInsideMilkSellButton(int x, int y) const;
	void showWarehouseWindow() ;//window for warehouse details.
	void handlePlayAreaClick(int x, int y);
	void checkLevelGoal();// checks whether the goal has been reached.
	void advanceLevel(); //increases the level and refreshes level-related state.
	void resetWolfHitCounters(int clickedWolfIndex); //resets the click counters of wolves you are not currently hitting
	void updateStatusMessageTimer(); //counts down the temporary message timer every game loop

public:
	int budget = 2500;
	int level = 2;
	int time = 150;
	int animalcount = 0;
	string wolfImagePath;
	mutable int wolfX;
	mutable int wolfY;
	void moveWolf();

	void collectEggs();
	void collectMilk();

	void gametimer(int level);
	Game();
	~Game();

	void updatePlayArea();
	void updateAnimalProduction(int elapsedSeconds);

	clicktype getMouseClick(int& x, int& y) const; //Get coordinate where user clicks and returns click type (left/right)
	string getSrting() const;	 //Returns a string entered by the user


	window* CreateWind(int, int, int, int) const; //creates the game window
	void createToolbar();
	void createBudgetbar();
	void clearBudget() const;
	void printBudget(string msg) const;
	void clearStatusBar() const;	//Clears the status bar
	void updatestatusbar() const; //refresh the status bar continously
	void drawfieldboundary() const; //to set the field boundary
	void Restart();
	void printMessage(string msg) const;	//Print a message on Status bar
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
	void sellEggProducts();
	void sellMilkProducts();
	bool isPaused() const;
	void registerAnimalProduct(const string& productLabel);


	window* getWind() const;		//returns a pointer to the graphics window
};

