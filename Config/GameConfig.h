#pragma once
#include "../CMUgraphicsLib/CMUgraphics.h"

__declspec(selectany) // Keeps the shared config object from being defined twice.
struct
{
	int	windWidth = 1500, windHeight = 800,
		wx = 5, wy = 5,

		toolBarHeight = 50,
		toolbarItemWidth = 40,
		statusBarHeight = 50;

	int playingAreaHeight = windHeight - toolBarHeight - statusBarHeight;

	color	penColor = BLUE,
		bkGrndColor = POWDERBLUE,
		statusBarColor = BLACK;
	int penWidth = 3;

	int	iconWidth = 70;
} config;

