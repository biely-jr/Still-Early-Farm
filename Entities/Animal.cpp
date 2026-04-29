#include "Animal.h"
#include "../Config/GameConfig.h"
#include "../Core/Game.h"
#include <iostream>
#include <utility>
#include <cstdlib>
using namespace std;

Animal::Animal(Game* r_pGame, point r_point, int r_width, int r_height, string img_path, int productIntervalSeconds, const string& productName)
	: Drawable(r_pGame, r_point, r_width, r_height),
	productionElapsedSeconds(0),
	productionIntervalSeconds(productIntervalSeconds),
	productLabel(productName)
{
	image_path = img_path;
	curr_pos = r_point;
	curr_vel.x = 1;
	curr_vel.y = 1;

}

void Animal::draw() const
{
	//draw image of this object
	//Both the chick and cow inherit the properties from this function in order to be drawn on the screen
	window* pWind = pGame->getWind();
	pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height); // Passing the animal's unique properties as parameters

	pWind->SetPen(BLACK, 2);
	pWind->SetBrush(WHITE);
	pWind->DrawRectangle(RefPoint.x - 6, RefPoint.y - 28, RefPoint.x + 56, RefPoint.y - 4, FILLED, 10, 10);
	pWind->SetFont(14, BOLD, BY_NAME, "Arial");
	//Draws a small white rectangle above it and prints its production time
	pWind->DrawString(RefPoint.x + 4, RefPoint.y - 24, to_string(productionElapsedSeconds) + "/" + to_string(productionIntervalSeconds));
}

void Animal::advanceProduction(int elapsedSeconds)
{
	if (elapsedSeconds <= 0)
		return;

	productionElapsedSeconds += elapsedSeconds;

	while (productionElapsedSeconds >= productionIntervalSeconds)
	{
		productionElapsedSeconds -= productionIntervalSeconds;
		pGame->registerAnimalProduct(productLabel);
	}
}

int Animal::getProductionCounter() const
{
	return productionElapsedSeconds;
}

int Animal::getProductionInterval() const
{
	return productionIntervalSeconds;
}

string Animal::getProductLabel() const
{
	return productLabel;
}

Chick::Chick(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Animal(r_pGame, r_point, r_width, r_height, img_path, 10, "Egg")
{
}

void Chick::moveStep()
{
	//TO DO: add code for cleanup and game exit here
	/*
	//draw image of this object in the field
	window* pWind = pGame->getWind();
	pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
	*/
	cout << "Icon Chick Clicked" << endl;

	// Update position based on velocity
	RefPoint.x += curr_vel.x * 3;
	RefPoint.y += curr_vel.y * 3;

	int min_x = 50;
	int max_x = config.windWidth - 50;
	int min_y = (config.toolBarHeight * 2) + 50;
	int max_y = config.windHeight - config.statusBarHeight - 50;

	// Bounce off walls if they go out of bounds
	if (RefPoint.x < min_x) { RefPoint.x = min_x; curr_vel.x = 1; }
	if (RefPoint.x > max_x) { RefPoint.x = max_x; curr_vel.x = -1; }
	if (RefPoint.y < min_y) { RefPoint.y = min_y; curr_vel.y = 1; }
	if (RefPoint.y > max_y) { RefPoint.y = max_y; curr_vel.y = -1; }
}

Cow::Cow(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Animal(r_pGame, r_point, r_width, r_height, img_path, 15, "Milk")
{
}

void Cow::moveStep()
{
	//TO DO: add code for cleanup and game exit here
	cout << "Icon Cow Clicked" << endl;

	// Cows change direction slightly less often than chicks
	if (rand() % 100 < 5) {
		curr_vel.x = (rand() % 3) - 1;
		curr_vel.y = (rand() % 3) - 1;
	}

	// Update position based on velocity
	RefPoint.x += curr_vel.x * 2;
	RefPoint.y += curr_vel.y * 2;

	int min_x = 50;
	int max_x = config.windWidth - 50;
	int min_y = (config.toolBarHeight * 2) + 50;
	int max_y = config.windHeight - config.statusBarHeight - 50;

	// Bounce off walls if they go out of bounds
	if (RefPoint.x < min_x) { RefPoint.x = min_x; curr_vel.x = 1; }
	if (RefPoint.x > max_x) { RefPoint.x = max_x; curr_vel.x = -1; }
	if (RefPoint.y < min_y) { RefPoint.y = min_y; curr_vel.y = 1; }
	if (RefPoint.y > max_y) { RefPoint.y = max_y; curr_vel.y = -1; }
}
