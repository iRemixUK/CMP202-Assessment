#pragma once
class Functions
{
public:
	// Class which the a* algorithim will use 
	bool samePos(int currentX, int currentY, int endX, int endY);

	bool validUp(int currentX, int currentY, int grid[750][750]);
	bool validDown(int currentX, int currentY, int grid[750][750]);
	bool validRight(int currentX, int currentY, int grid[750][750]);
	bool validLeft(int currentX, int currentY, int grid[750][750]);

	int heuristic(int currentX, int currentY, int endX, int endY);

};

