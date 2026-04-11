#pragma once
#include "../CMUgraphicsLib/CMUgraphics.h"
#include "../UI/Toolbar.h"
#include "../UI/BudgetBar.h"

class Game
{
private:
	window* pWind;	//Pointer to the CMU graphics window
	Toolbar* gameToolbar;
	Budgetbar* gameBudgetbar;

public:
	int budget = 2000;
	int level = 1;
	int time = 150;
	int animalcount = 0;
	void gametimer(int level); // The function to calculate time
	Game();
	~Game();

	// FIX: Removed 'const' so this can update animal positions
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

	// FIX: Removed 'const' because the game loop modifies game states
	void go();

	window* getWind() const;		//returns a pointer to the graphics window
};
