#pragma once
#ifndef GRID_H
#define GRID_H
#include<iostream>
#include<SFML/Graphics.hpp>
#include <deque>
#include <list>
#include <deque>


const int ROWS = 25;
const int COLS = 25;
const int bSIZE = 25;
const int BLOCK_DISTANCE = 30;


struct Node
{
	Node() {}
	bool isWalkable = true;
	bool visited = false;
	bool grabbed = false;
	double globalfGoal = 0; // Cost of the route to goal
	double localfGoal = 0; // distance too the goal if we took an alternative route
	int x = NULL;
	int y = NULL;
	sf::RectangleShape rect;
	std::deque<Node*> NeighborNodes;
	Node* parent = nullptr;

};


class Grid
{
public:
	Grid(sf::RenderWindow*);
	bool A_Star_Algorithm();
	bool getFound() { return found; }
	void setFound(bool v) { found = v; }
	void GrabNeighborNodes(Node&);
	void constructPath();
	void mouseContains();
	bool isValid(int, int);
	void setStart();
	void placeStart();
	void setEnd();
	void placeEnd();
	void Display();

	void generateRandoCommandoMaze();

private:
	bool found = false;
	Node* nodes[ROWS][COLS];
	Node* m_startNode = nullptr;
	Node* m_endNode = nullptr;
	std::list<Node*> nonTestedNodes;
	std::deque<Node*> m_path;
	sf::RectangleShape mouseNode;
	sf::RenderWindow* m_win;
};

#endif

