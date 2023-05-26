#include "Functions.h"

bool Functions::samePos(int currentX, int currentY, int endX, int endY)
{
	// Checks of the current and end node are the same if so return true
	if (currentX == endX && currentY == endX)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Functions::validUp(int currentX, int currentY, int grid[750][750])
{
	// Checks if the node above is valid if so return true
	if (grid[currentX][currentY - 1] == -1)
	{
		return true;
	}
	if (currentY - 1 <= -1)
	{
		return false;
	}
	else
	{
		return false;
	}
}

bool Functions::validDown(int currentX, int currentY, int grid[750][750])
{
	// Checks if the node below is valid if so return true
	if (grid[currentX][currentY + 1] == -1)
	{
		return true;
	}
	if (currentY + 1 >= 750)
	{
		return false;
	}
	else
	{
		return false;
	}
}

bool Functions::validRight(int currentX, int currentY, int grid[750][750])
{
	// Checks if the node to the right is valid if so return true
	if (grid[currentX + 1][currentY] == -1)
	{
		return true;
	}
	if (currentX + 1 >= 750)
	{
		return false;
	}
	else
	{
		return false;
	}
}

bool Functions::validLeft(int currentX, int currentY, int grid[750][750])
{
	// Checks if the node to the left is valid if so return true
	if (grid[currentX - 1][currentY] == -1)
	{
		return true;
	}
	if (currentX - 1 <= -1)
	{
		return false;
	}
	else
	{
		return false;
	}
}

int Functions::heuristic(int currentX, int currentY, int endX, int endY)
{
	// Calculates the heuristic
	int dx = endX - currentX;
	int dy = endY - currentY;
	return dx + dy;
}
