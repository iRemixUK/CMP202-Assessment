// Existing includes from CMP201 assignment
#include <iostream>
#include <cstdlib> // for rand and srand functions
#include <ctime> // for time functions
#include <list> // for list
#include <chrono> // clock
#include <thread> // clock
#include <fstream> // writing to csv file
#include <vector>
#include <queue>
#include "Functions.h"

// New includes for CMP202 Assignment
#include <thread>
#include <condition_variable>
#include <mutex>

using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::this_thread::sleep_for;
typedef std::chrono::steady_clock the_clock;

// Thread, mutex and condition variable -- NEW
using std::thread;
using std::mutex;
using std::condition_variable;

mutex lee_phase1; // Mutex to lock the phase 1 of the algorithim -- NEW
mutex lee_mutex; // Lee mutex to lock the calculations -- NEW
mutex astar_mutex; // Mutex for astar -- NEW
bool leeready; // Bool to check if lee phase 1 is complete -- NEW
condition_variable lee_cv; // Condition variable for lee phase 1 -- NEW

// Size of map
#define WIDTH 750
#define HEIGHT 750

bool skip = false; // Decides if the a* should be skipped if maze cant be solved ]]
bool threaded = false; // Decides if the algorithims are going to be threaded or not -- NEW

int grid[WIDTH][HEIGHT]; // Grid that will be used for the pathfinding
Functions f; // Class for the functions for the a* algorithim

// Node for a* algorithim
struct Node
{
	int x, y; // X and Y of node in map
	int f, g, h; // Heuristic values - defualt to zero
	Node* parentNode; // Pointer to the node that created this node - Backpropagation
};

// Coords for lee algorithim
struct Coord
{
	int x;
	int y;
};

bool lowestF(Node* a, Node* b)
{
	// Returns the node with the lowest F value
	return a->f < b->f;
}

// Create set of visited nodes - CLOSED SET
std::list <Node*> closedSet;
std::list <Node*> openSet;

// Init and fill grid 
void intialisegrid()
{
	for (int coloumn = 0; coloumn < HEIGHT; coloumn++)
	{
		for (int row = 0; row < WIDTH; row++)
		{
			grid[row][coloumn] = -1;
		}
	}
}

void randobstacles(int x, int y)
{
	srand(time(0)); //intialise random num generator using time
	int r;
	for (int coloumn = 0; coloumn < HEIGHT; coloumn++)
	{
		for (int row = 0; row < WIDTH; row++)
		{
			r = rand() % 10 + 1;
			if (r > 2)
			{
				grid[row][coloumn] = -1;
			}
			else
			{
				grid[row][coloumn] = -2;
			}
		}
	}
	grid[0][0] = -1;
	grid[0][1] = -1; // So grid has more chance of having a path
	grid[x][y] = -1;
}

// Lee algorithim
void leephase1(struct Coord start, struct Coord end)
{
	std::unique_lock<mutex> lee_lock(lee_phase1); // Locks the lee phase 1 -- NEW
	grid[start.x][start.y] = 0; // Marks start point
	grid[end.x][end.y] = -3; // Marks end point
	int it = 0;

	bool foundEnd = false;

	while (!foundEnd)
	{
		bool foundEmpty = false;
		for (int x = 0; x < WIDTH && !foundEnd; ++x)
		{
			for (int y = 0; y < HEIGHT; ++y)
			{
				if (grid[x][y] == it)
				{
					// Threaded version of the lee algorithim -- NEW
					if (threaded == true)
					{
						// Starts a new thread to deal with the right coord -- NEW 
						thread right([&] {
							if (x < WIDTH - 1)
							{
								std::unique_lock<mutex> lock(lee_mutex); // Locks thread if updating the grid -- NEW

								if (grid[x + 1][y] == -3)
								{
									foundEnd = true;
								}
								else if (grid[x + 1][y] == -1)
								{
									grid[x + 1][y] = it + 1;
									foundEmpty = true;
								}
							}
							});

						// Starts a new thread to deal with the left coord -- NEW
						thread left([&] {
							if (x > 0)
							{
								std::unique_lock<mutex> lock(lee_mutex); // Locks thread if updating the grid -- NEW
								if (grid[x - 1][y] == -3)
								{
									foundEnd = true;
								}
								else if (grid[x - 1][y] == -1)
								{
									grid[x - 1][y] = it + 1;
									foundEmpty = true;
								}
							}
							});

						// Starts a new thread to deal with the below coord -- NEW
						thread down([&] {
							if (y < HEIGHT - 1)
							{
								std::unique_lock<mutex> lock(lee_mutex); // Locks thread if updating the grid -- NEW
								if (grid[x][y + 1] == -3)
								{
									foundEnd = true;
								}
								else if (grid[x][y + 1] == -1)
								{
									grid[x][y + 1] = it + 1;
									foundEmpty = true;
								}
							}
							});

						// Starts a new thread to deal with the above coord -- NEW
						thread up([&] {
							if (y > 0)
							{
								std::unique_lock<mutex> lock(lee_mutex); // Locks thread if updating the grid -- NEW
								if (grid[x][y - 1] == -3)
								{
									foundEnd = true;
								}
								else if (grid[x][y - 1] == -1)
								{
									grid[x][y - 1] = it + 1;
									foundEmpty = true;
								}
							}
							});

						// Join all threads together -- NEW 
						right.join();
						left.join();
						down.join();
						up.join();
					}
					
					// Non threaded version of the lee algorithim
					if (threaded == false)
					{
						// Right
						if (x < WIDTH - 1)
						{
							if (grid[x + 1][y] == -3)
							{
								foundEnd = true;
								break;
							}
							else if (grid[x + 1][y] == -1)
							{
								grid[x + 1][y] = it + 1;
								foundEmpty = true;
							}
						}

						// Left
						if (x > 0)
						{
							if (grid[x - 1][y] == -3)
							{
								foundEnd = true;
								break;
							}
							else if (grid[x - 1][y] == -1)
							{
								grid[x - 1][y] = it + 1;
								foundEmpty = true;
							}
						}

						// Down
						if (y < HEIGHT - 1)
						{
							if (grid[x][y + 1] == -3)
							{
								foundEnd = true;
								break;
							}
							else if (grid[x][y + 1] == -1)
							{
								grid[x][y + 1] = it + 1;
								foundEmpty = true;
							}
						}

						// Up
						if (y > 0)
						{
							if (grid[x][y - 1] == -3)
							{
								foundEnd = true;
								break;
							}
							else if (grid[x][y - 1] == -1)
							{
								grid[x][y - 1] = it + 1;
								foundEmpty = true;
							}
						}
					}
				}
			}
		}
		
		if (!foundEnd && !foundEmpty)
		{
			std::cout << "This maze can't be solved!" << std::endl << std::endl;
			break;
		}
		it++;
	}
	
	grid[end.x][end.y] = it;
	
	// Signal that the phase 1 of the algorithim is complete and can now move on to the second phase -- NEW
	leeready = true;
	lee_cv.notify_one();
}

void leephase2(struct Coord start, struct Coord end)
{
	std::unique_lock<mutex> lee_lock(lee_phase1); // have another mutex so we can lock the condition variable so the thread doesn't begin until phase 1 is complete -- NEW
	while (!leeready)
	{
		lee_cv.wait(lee_lock); // waits until phase 1 has completed so it can then move on to complete phase 2 of the algorithim -- NEW
	}
	// Above is new for the CMP202 Assignment 

	std::list<Coord> path;
	Coord current;
	bool foundStart = false;
	current.x = end.x;
	current.y = end.y;
	int distance = grid[end.x][end.y];
	path.push_front(current);
	clock_t s = clock(); // To ensure lee doesn't get stuck
	clock_t n;

	while (!foundStart)
	{
		n = clock();
		if (((n - s) / CLOCKS_PER_SEC) >= 2) // Sometimes the lee algortihim gets stuck so if it lasts for more than 2000ms the algorithm will be skipped
		  {
			std::cout << "This maze cant be solved" << "\n";
			skip = true;
			return;
		  }

		// Right
		if (grid[current.x + 1][current.y] == distance - 1)
		{
			current.x = current.x + 1;
			distance = distance - 1;
			if (distance != 0)
			{
				path.push_front(current);
			}
		}

		if (current.x == start.x && current.y == start.y)
		{
			path.push_front(current);
			foundStart = true;
			break;
		}

		// Left
		if (grid[current.x - 1][current.y] == distance - 1)
		{
			current.x = current.x - 1;
			distance = distance - 1;
			if (distance != 0)
			{
				path.push_front(current);
			}

		}

		if (current.x == start.x && current.y == start.y)
		{
			path.push_front(current);
			foundStart = true;
			break;
		}

		// Up
		if (grid[current.x][current.y + 1] == distance - 1)
		{
			current.y = current.y + 1;
			distance = distance - 1;
			if (distance != 0)
			{
				path.push_front(current);
			}
		}

		if (current.x == start.x && current.y == start.y)
		{

			path.push_front(current);
			foundStart = true;
			break;
		}

		// Down
		if (grid[current.x][current.y - 1] == distance - 1)
		{
			current.y = current.y - 1;
			distance = distance - 1;
			if (distance != 0)
			{
				path.push_front(current);
			}
		}

		if (current.x == start.x && current.y == start.y)
		{
			path.push_front(current);
			foundStart = true;
			break;
		}
	}
	leeready = false; // Ensures that phase 2 doesnt run straight away when the application loops around to the next iteration
}

void lee()
{
	struct Coord start = { 0, 0 };
	struct Coord end = { 9, 9 };

	// Start two threads to run the Lee algorithim
	thread Lee1(leephase1, start, end);
	thread Lee2(leephase2, start, end);

	// Join both the threads back together
	Lee1.join();
	Lee2.join();
}

// A* algorithim
void backTrack(Node* endNode, Node* startNode)
{
	if (endNode->parentNode == nullptr)
	{
		std::cout << "A path is not possible for this maze" << "\n";
		return;
	}

	Node* i = endNode;
	while (i->x != startNode->x && i->y != startNode->y)
	{
		
		if (i->parentNode == nullptr)
		{
			std::cout << "A path is not possible for this maze" << "\n";
			break;
		}
		i = i->parentNode;
	}
}

void astar()
{
	// Creates the end node 
	Node* endNode = new Node;
	endNode->x = 9;
	endNode->y = 9;
	
	// Creates the start node
	Node* startNode = new Node;
	startNode->x = 0;
	startNode->y = 0;
	startNode->g = 0;
	startNode->h = f.heuristic(startNode->x, startNode->y, endNode->x, endNode->y); // gets the heuristic of the starting node
	startNode->f = startNode->g + startNode->h;
	startNode->parentNode = NULL;
	
	// Add start node to available nodes
	openSet.push_back(startNode);
	intialisegrid();
	
	while (!openSet.empty())
	{
		openSet.sort(lowestF); // Sorts the open set to have the best F value at the time
		Node* currentNode = openSet.front(); // Get the node with the best F value 
		openSet.pop_front(); // Pop node from the open set

		// Check if current and end node are the same if so end = current and path complete
		if (f.samePos(currentNode->x, currentNode->y, endNode->x, endNode->y))
		{
			endNode = currentNode;
			break;
		}
		
		// Threaded version of the a* algorithim -- NEW
		if (threaded == true)
		{
			// Checks if the node above is valid -- NEW
			thread up([&] {
				if (f.validUp(currentNode->x, currentNode->y, grid))
				{
					std::unique_lock<mutex> lee_lock(astar_mutex); // Mutex to lock thread -- NEW
					int d = 0; // Checks to see if node should be pushed to open 
					Node* newNode = new Node();
					newNode->x = currentNode->x;
					newNode->y = currentNode->y - 1;
					newNode->g = currentNode->g + 1; // Cost of the path
					newNode->h = f.heuristic(newNode->x, newNode->y, endNode->x, endNode->y); // heuristic
					newNode->f = newNode->g + newNode->h; // Combination - how we sort the open set
					newNode->parentNode = currentNode;

					// check if node is in closed set
					for (auto item : closedSet)
					{
						if (item->x == newNode->x && item->y == newNode->y)
						{
							d = 1;
						}
					}

					// check if node is in open set
					for (auto item : openSet)
					{
						if (item->x == newNode->x && item->y == newNode->y)
						{
							if (item->f < newNode->f)
							{
								d = 1;
							}
						}
					}
					if (d == 0)
					{
						openSet.push_back(newNode);
					}
				}
				});

			// Checks if the node below is valid -- NEW
			thread down([&] {
				if (f.validDown(currentNode->x, currentNode->y, grid))
				{
					std::unique_lock<mutex> lee_lock(astar_mutex); // Mutex to lock thread -- NEW
					int d = 0; // Checks to see if node should be pushed to open 
					Node* newNode = new Node();
					newNode->x = currentNode->x;
					newNode->y = currentNode->y + 1;
					newNode->g = currentNode->g + 1; // Cost of the path
					newNode->h = f.heuristic(newNode->x, newNode->y, endNode->x, endNode->y); // heuristic
					newNode->f = newNode->g + newNode->h; // Combination - how we sort the open set
					newNode->parentNode = currentNode;

					// check if node is in closed set
					for (auto item : closedSet)
					{
						if (item->x == newNode->x && item->y == newNode->y)
						{
							d = 1;
						}
					}
					// check if node is in open set
					for (auto item : openSet)
					{
						if (item->x == newNode->x && item->y == newNode->y)
						{
							if (item->f < newNode->f)
							{
								d = 1;
							}
						}
					}
					if (d == 0)
					{
						openSet.push_back(newNode);
					}
				}
				});

			// Checks if the node on the right is valid -- NEW
			thread right([&] {
				if (f.validRight(currentNode->x, currentNode->y, grid))
				{
					std::unique_lock<mutex> lee_lock(astar_mutex); // Mutex to lock thread -- NEW
					int d = 0; // Checks to see if node should be pushed to open 
					Node* newNode = new Node();
					newNode->x = currentNode->x + 1;
					newNode->y = currentNode->y;
					newNode->g = currentNode->g + 1; // Cost of the path
					newNode->h = f.heuristic(newNode->x, newNode->y, endNode->x, endNode->y); // heuristic
					newNode->f = newNode->g + newNode->h; // Combination - how we sort the open set
					newNode->parentNode = currentNode;

					// check if node is in closed set
					for (auto item : closedSet)
					{
						if (item->x == newNode->x && item->y == newNode->y)
						{
							d = 1;
						}
					}

					// check if node is in open set
					for (auto item : openSet)
					{
						if (item->x == newNode->x && item->y == newNode->y)
						{
							if (item->f < newNode->f)
							{
								d = 1;
							}
						}
					}
					if (d == 0)
					{
						openSet.push_back(newNode);
					}

				}
				});

			// Checks if the node on the left is valid -- NEW
			thread left([&] {
				if (f.validLeft(currentNode->x, currentNode->y, grid))
				{
					std::unique_lock<mutex> lee_lock(astar_mutex); // Mutex to lock thread -- NEW
					int d = 0; // Checks to see if node should be pushed to open 
					Node* newNode = new Node();
					newNode->x = currentNode->x - 1;
					newNode->y = currentNode->y;
					newNode->g = currentNode->g + 1; // Cost of the path
					newNode->h = f.heuristic(newNode->x, newNode->y, endNode->x, endNode->y); // heuristic
					newNode->f = newNode->g + newNode->h; // Combination - how we sort the open set
					newNode->parentNode = currentNode;

					// check if node is in closed set
					for (auto item : closedSet)
					{
						if (item->x == newNode->x && item->y == newNode->y)
						{
							d = 1;
						}
					}

					// check if node is in open set
					for (auto item : openSet)
					{
						if (item->x == newNode->x && item->y == newNode->y)
						{
							if (item->f < newNode->f)
							{
								d = 1;
							}
						}
					}
					if (d == 0)
					{
						openSet.push_back(newNode);
					}
				}
				});

			// Joins the threads back together -- NEW
			up.join();
			down.join();
			right.join();
			left.join();
		}
		
		// Non threaded version of the a* algorithim
		if (threaded == false)
		{
			if (f.validUp(currentNode->x, currentNode->y, grid))
			{
				int d = 0; // Checks to see if node should be pushed to open 
				Node* newNode = new Node();
				newNode->x = currentNode->x;
				newNode->y = currentNode->y - 1;
				newNode->g = currentNode->g + 1; // Cost of the path
				newNode->h = f.heuristic(newNode->x, newNode->y, endNode->x, endNode->y); // heuristic
				newNode->f = newNode->g + newNode->h; // Combination - how we sort the open set
				newNode->parentNode = currentNode;

				// check if node is in closed set
				for (auto item : closedSet)
				{
					if (item->x == newNode->x && item->y == newNode->y)
					{
						d = 1;
					}
				}

				// check if node is in open set
				for (auto item : openSet)
				{
					if (item->x == newNode->x && item->y == newNode->y)
					{
						if (item->f < newNode->f)
						{
							d = 1;
						}
					}
				}
				if (d == 0)
				{
					openSet.push_back(newNode);
				}
			}

			if (f.validDown(currentNode->x, currentNode->y, grid))
			{
				int d = 0; // Checks to see if node should be pushed to open 
				Node* newNode = new Node();
				newNode->x = currentNode->x;
				newNode->y = currentNode->y + 1;
				newNode->g = currentNode->g + 1; // Cost of the path
				newNode->h = f.heuristic(newNode->x, newNode->y, endNode->x, endNode->y); // heuristic
				newNode->f = newNode->g + newNode->h; // Combination - how we sort the open set
				newNode->parentNode = currentNode;

				// check if node is in closed set
				for (auto item : closedSet)
				{
					if (item->x == newNode->x && item->y == newNode->y)
					{
						d = 1;
					}
				}
				// check if node is in open set
				for (auto item : openSet)
				{
					if (item->x == newNode->x && item->y == newNode->y)
					{
						if (item->f < newNode->f)
						{
							d = 1;
						}
					}
				}
				if (d == 0)
				{
					openSet.push_back(newNode);
				}
			}

			if (f.validRight(currentNode->x, currentNode->y, grid))
			{
				int d = 0; // Checks to see if node should be pushed to open 
				Node* newNode = new Node();
				newNode->x = currentNode->x + 1;
				newNode->y = currentNode->y;
				newNode->g = currentNode->g + 1; // Cost of the path
				newNode->h = f.heuristic(newNode->x, newNode->y, endNode->x, endNode->y); // heuristic
				newNode->f = newNode->g + newNode->h; // Combination - how we sort the open set
				newNode->parentNode = currentNode;

				// check if node is in closed set
				for (auto item : closedSet)
				{
					if (item->x == newNode->x && item->y == newNode->y)
					{
						d = 1;
					}
				}

				// check if node is in open set
				for (auto item : openSet)
				{
					if (item->x == newNode->x && item->y == newNode->y)
					{
						if (item->f < newNode->f)
						{
							d = 1;
						}
					}
				}
				if (d == 0)
				{
					openSet.push_back(newNode);
				}

			}

			if (f.validLeft(currentNode->x, currentNode->y, grid))
			{
				int d = 0; // Checks to see if node should be pushed to open 
				Node* newNode = new Node();
				newNode->x = currentNode->x - 1;
				newNode->y = currentNode->y;
				newNode->g = currentNode->g + 1; // Cost of the path
				newNode->h = f.heuristic(newNode->x, newNode->y, endNode->x, endNode->y); // heuristic
				newNode->f = newNode->g + newNode->h; // Combination - how we sort the open set
				newNode->parentNode = currentNode;

				// check if node is in closed set
				for (auto item : closedSet)
				{
					if (item->x == newNode->x && item->y == newNode->y)
					{
						d = 1;
					}
				}

				// check if node is in open set
				for (auto item : openSet)
				{
					if (item->x == newNode->x && item->y == newNode->y)
					{
						if (item->f < newNode->f)
						{
							d = 1;
						}
					}
				}
				if (d == 0)
				{
					openSet.push_back(newNode);
				}
			}
		}

		closedSet.push_back(currentNode); // Push current node into the closed set

		openSet.sort(); // Sort open set
		openSet.unique(); // Make sure only unique nodes are in the open set (no duplicates)

		closedSet.sort(); // Sort closed set
		closedSet.unique();// Make sure only unique nodes are in the closed set (no duplicates)
	}

	backTrack(endNode, startNode); // Back track to get from the end to the start
}

int main()
{
	std::ofstream results("Results.csv"); // Creates csv file to write results to
	results << "Lee" << "," << "A*" << "\n"; // Write column headers 

	int choice = 0; // Used for user input to decide between a threaded or non threaded version -- NEW
	
	std::cout << "Would you like to use the threaded or non threaded version?" << "\n";
	std::cout << "Press 1 for threaded or 2 for non threaded" << "\n";
	
	std::cin >> choice;

	if (choice == 1)
	{
		threaded = true;
	}

	if (choice == 2)
	{
		threaded = false;
	}

	for (int i = 0; i < 100; i++)
	{
		skip = false; // Skips the a* if lee can't find a path
		intialisegrid(); 
		randobstacles(9, 9);

		the_clock::time_point leeStart = the_clock::now();
		lee(); // Do lee algorithim
		the_clock::time_point leeEnd = the_clock::now();

		auto leeTime = duration_cast<milliseconds>(leeEnd - leeStart).count(); // Calculates the time for the lee algorithim

		if (skip == false)
		{
			the_clock::time_point aStart = the_clock::now();
			astar(); // Do A* algorithim
			the_clock::time_point aEnd = the_clock::now();

			auto aTime = duration_cast<milliseconds>(aEnd - aStart).count(); // Calculates the time for the a* algorithim
			
			// Displays the time that both the algorithims took
			std::cout << "The time it took for Lee was - " << leeTime << "\n";
			std::cout << "The time it took for A* was - " << aTime << "\n";

			results << leeTime << "," << aTime << "\n"; // Write results out to file 
		}

		// Clears the sets for the next iteration of the algorithim
		openSet.clear();
		closedSet.clear();
	}
	system("pause");
	return 0;
}