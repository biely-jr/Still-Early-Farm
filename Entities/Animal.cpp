#include "Animal.h"
#include "../Config/GameConfig.h"
#include "../Core/Game.h"
#include <iostream>
#include <utility>
#include <cstdlib>
using namespace std;

namespace
{
	void keepAnimalInsideField(point& position, point& velocity, int width, int height)
	{
		const int minX = 0;
		const int maxX = config.windWidth - width;
		const int minY = config.toolBarHeight * 2;
		const int maxY = config.windHeight - config.statusBarHeight - height;

		if (position.x < minX)
		{
			position.x = minX;
			velocity.x = (velocity.x == 0) ? 1 : -velocity.x;
		}
		else if (position.x > maxX)
		{
			position.x = maxX;
			velocity.x = (velocity.x == 0) ? -1 : -velocity.x;
		}

		if (position.y < minY)
		{
			position.y = minY;
			velocity.y = (velocity.y == 0) ? 1 : -velocity.y;
		}
		else if (position.y > maxY)
		{
			position.y = maxY;
			velocity.y = (velocity.y == 0) ? -1 : -velocity.y;
		}
	}

	void avoidStillVelocity(point& velocity)
	{
		if (velocity.x == 0 && velocity.y == 0)
			velocity.x = 1;
	}
}

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

bool Animal::advanceProduction(int elapsedSeconds)
{
	// 1. Increase the timer
	productionElapsedSeconds += elapsedSeconds;

	// 2. Check if the limit is reached
	if (productionElapsedSeconds >= productionIntervalSeconds)
	{
		// Reset the timer for the next product
		productionElapsedSeconds = 0;

		return true; // Product is ready!
	}

	return false; // Not ready yet
}

void Animal::addProductionTime(int seconds)
{
	productionElapsedSeconds += seconds;
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
	cout << "Icon Chick Clicked" << endl;

	// Update position based on velocity
	RefPoint.x += curr_vel.x * 3;
	RefPoint.y += curr_vel.y * 3;

	keepAnimalInsideField(RefPoint, curr_vel, width, height);
}

Cow::Cow(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Animal(r_pGame, r_point, r_width, r_height, img_path, 15, "Milk")
{
}

void Cow::moveStep()
{
	cout << "Icon Cow Clicked" << endl;

	// Cows change direction slightly less often than chicks
	if (rand() % 100 < 5) {
		curr_vel.x = (rand() % 3) - 1;
		curr_vel.y = (rand() % 3) - 1;
		avoidStillVelocity(curr_vel);
	}

	// Update position based on velocity
	RefPoint.x += curr_vel.x * 2;
	RefPoint.y += curr_vel.y * 2;

	keepAnimalInsideField(RefPoint, curr_vel, width, height);
}
