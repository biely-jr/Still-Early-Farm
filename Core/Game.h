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
	FoodArea foodAreas[2];

	void initializeFoodAreas();
	void clearPlayingArea() const;
	void drawFieldBackground() const;
	void drawWolf() const;
	void drawFoodArea(const FoodArea& area) const;
	void drawAllFoodAreas() const;
	void drawEggsAndMilk() const;
	void drawWarehouse() const;
	point getRandomAnimalPosition(int animalWidth, int animalHeight) const;
	void consumeFoodArea(int areaIndex);

public:
	int budget = 5000;
	int level = 1;
	int time = 150;
	int animalcount = 0;
	void gametimer(int level);
	Game();
	~Game();

void updatePlayArea();

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

	void go() ;

	void redrawField() const;
	bool canAfford(int amount) const;
	bool spendBudget(int amount);
	void placeAnimal(AnimalType animalType);


	window* getWind() const;		//returns a pointer to the graphics window
};
