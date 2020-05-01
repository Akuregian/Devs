/*
	File: Grid.h
	Class Description: Creates a grid, using SFML sqaures. Each sqaure represents a Node.
*/
#pragma once
#ifndef GRID_H
#define GRID_H
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Font.hpp>
#include <cmath>
#include <deque>

const int ROWS_COLS_SIZE = 24; // 30
const int BLOCKSIZE = 35;      // 25
const int BLOCK_DISTANCE = 40; // 30


struct Node // node structure
{
public:
	int blocksize = 0; // the size of each blocl
	sf::Color blockcolor; // The Color of the created block
	sf::RectangleShape nodeRect; // the Rectangle shape 
	Node() { blocksize = 25; blockcolor = sf::Color::Cyan; }
	Node(int s, sf::Color col ) : blocksize(s), blockcolor(col)
	{
		nodeRect.setFillColor(blockcolor); nodeRect.setSize(sf::Vector2f(blocksize, blocksize));
	}
	int parent_i = 0;
	int	parent_j = 0;
	double fCost = 0;
	int gCost = 0;
	double hCost = 0;
	bool startNode = false;
	bool endNode = false;
	bool isWalkable = true;
	bool clicked = false;


	bool operator==(const Node& b)
	{
		return parent_i == b.parent_i && parent_j == b.parent_j;
	}

	void calculatefCost() // Total Cost of Node
	{
		//f = g + h
		fCost = hCost + gCost;
	}

	void calculategCost(Node* currNode, Node* startNode) // Distance From CurrentNode to startNode
	{
		// Store that value inside gCost
		gCost = currNode->gCost + 1;
	}

	void calculatehCost(Node* currnode, Node* endNode) // Distance from the currentNode to EndNode
	{
		// use my man, pythagorus's Formula to calculate dist from currNode to endNode
		currnode->hCost = std::sqrt((std::pow(currnode->nodeRect.getPosition().x - endNode->nodeRect.getPosition().x, 2) +
									std::pow(currnode->nodeRect.getPosition().y - endNode->nodeRect.getPosition().y, 2)));
		hCost = currnode->hCost / 10;
	}
};


class Grid
{
public:
	Grid(int, int, int, sf::RenderWindow*);
	void aStarAlgorithm();
	void checkNeighborNodes(Node*);
	void start_node();
	void end_node();
	void buttonInit();
	bool getbuttonState() { return buttonState; }
	bool StartButtonClicked();
	void buttonToggle();
	Node* lowestFCost();
	bool isValid(int, int); // Utility Function that will Alow me Check if a node is valid or not. [row, column]
	bool isNOTclosedList(Node*);
	void isClicked();
	void display();

private:
	bool found = false;
	int m_rows;
	int m_columns;
	int m_size;
	bool buttonState = false;
	bool startNodeAdded = false;
	bool endNodeAdded = false;
	sf::RenderWindow* m_window;
	Node* bgGridNodes[ROWS_COLS_SIZE][ROWS_COLS_SIZE];
	Node* m_endNode = nullptr;
	Node* m_startNode = nullptr;
	std::deque<Node*> m_openList;
	std::deque<Node*> m_closedList;
	std::deque<Node*> m_path;
	sf::Texture button;
	sf::Sprite buttonSprite;

};

#endif
