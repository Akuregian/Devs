/*
	File: Grid.h
	Class Description: Creates a grid, using SFML sqaures. Each sqaure represents a Node.
*/

#include <iostream>
#include "Grid.h"

Grid::Grid(int r, int c, int s, sf::RenderWindow* w) // Takes in the rowSize, ColSize, blockSize, and Reference to the main draw window
	: m_rows(r), m_columns(c), m_size(s), m_window(w)
{
	buttonInit();
	for (int i = 0; i < m_rows; i++)
	{
		for (int j = 0; j < m_columns; j++)
		{
			bgGridNodes[i][j] = new Node(BLOCKSIZE, sf::Color(150, 150, 150)); // ?? ***** Pretty sure this will cause a MEMORY LEAK? ***** ??
			bgGridNodes[i][j]->parent_i = i;
			bgGridNodes[i][j]->parent_j = j;
			bgGridNodes[i][j]->nodeRect.setPosition(sf::Vector2f(i * BLOCK_DISTANCE + 5, j * BLOCK_DISTANCE + 5)); // sets each nodes postions, with there distance apart from eachother
		}
	}
	srand(time(NULL));
	start_node();
	end_node();
}

void Grid::buttonInit()
{
	if (!button.loadFromFile("Images/startbutton.png"))
	{
		std::cout << "Could Not Load/Find the Button Texture Image" << std::endl;
	}
	button.setSmooth(true);
	buttonSprite.setTexture(button);
	buttonSprite.setScale(sf::Vector2f(0.55f, 0.55f));
	buttonSprite.setPosition(sf::Vector2f(960, 20));
}

Node* Grid::lowestFCost() // ******ERROR when there is a fCost that is the smallest BUT its a NONWALKABLE NODE, so we have to choose the next lowest fCost until we get a walkable one.....********
{
	Node* lowestFCostNode = m_openList[0];
	for (auto i : m_openList)
	{
		if (i->fCost < lowestFCostNode->fCost) // Skips Here When Evaluating, since One of the Nodes are being Appneded AGAIN causing it too choose that node. since fCost is smaller
		{
			if (!i->isWalkable)
			{
				continue;
			}

			lowestFCostNode = i;
		}
	}
	return lowestFCostNode;
}

void Grid::isClicked()
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);
	for (unsigned int i = 0; i < m_rows; i++)
	{
		for (unsigned int j = 0; j < m_columns; j++)
		{
			if (bgGridNodes[i][j]->nodeRect.getGlobalBounds().contains(mousePos.x, mousePos.y) && bgGridNodes[i][j]->clicked)
			{
				bgGridNodes[i][j]->clicked = false;
				bgGridNodes[i][j]->isWalkable = true;
			}
			else if (bgGridNodes[i][j]->nodeRect.getGlobalBounds().contains(mousePos.x, mousePos.y) && !bgGridNodes[i][j]->clicked)
			{
				bgGridNodes[i][j]->clicked = true;
				bgGridNodes[i][j]->isWalkable = false;
			}
		}
	}
}

void Grid::buttonToggle()
{
	if (buttonState)
	{
		buttonState = false;
	}
	else
	{
		buttonState = true;
	}

}

bool Grid::StartButtonClicked()
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);
	if (buttonSprite.getGlobalBounds().contains(mousePos.x, mousePos.y) && sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		return true;
	}
	return false;
}

bool Grid::isNOTclosedList(Node* n)
{
	for (auto i : m_closedList)
	{
		if (i == n)
		{
			return false;
		}
	}
	return true;
}

// Error to Fix : if its already in the CLOSEDLIST dont APPEND it to the OPEN_LIST. b/c its getting re-evalauted and ReChosen as the LowestFCost Node.
void Grid::checkNeighborNodes(Node* currNode)
{

	std::deque<Node*> childrenNodes; // Where I can store the Children Nodes Surrounding the Current Node.
	if (isValid(currNode->parent_i, currNode->parent_j)) // If the Node is a Valid node; meaning its within [m_rows][m_columns], then enter
	{
		// Make sure the Node is Valid. Add it too the Children Nodes. Calculate [f = g + h]
		//
		//     n[i-1][j-1]  n[i][j-1]  n[i+1][j-1]
		//	            \      |      /
		//	             \     |     /
		//	   n[i-1][j] --- currNode --- n[i+1][j]
		//	             /     |     \
		//              /      |      \
		//     n[i-1][j+1]   n[i][j+1]   n[i+1][j+1]

		// Top Left Node From Current Position
		if (isValid(currNode->parent_i - 1, currNode->parent_j - 1))
		{
			childrenNodes.push_back(bgGridNodes[currNode->parent_i - 1][currNode->parent_j - 1]);
		}

		// Top Middle Node From Current Position
		if (isValid(currNode->parent_i, currNode->parent_j - 1))
		{
			childrenNodes.push_back(bgGridNodes[currNode->parent_i][currNode->parent_j - 1]);
		}

		// Top Right Node From Current Position
		if (isValid(currNode->parent_i + 1, currNode->parent_j - 1))
		{
			childrenNodes.push_back(bgGridNodes[currNode->parent_i + 1][currNode->parent_j - 1]);
		}

		// Middle Left Node From Current Position
		if (isValid(currNode->parent_i - 1, currNode->parent_j))
		{
			childrenNodes.push_back(bgGridNodes[currNode->parent_i - 1][currNode->parent_j]);
		}

		// Middle Right Node From Current Position
		if (isValid(currNode->parent_i + 1, currNode->parent_j))
		{
			childrenNodes.push_back(bgGridNodes[currNode->parent_i + 1][currNode->parent_j]);
		}

		// Bottom Left Node From Current Position
		if (isValid(currNode->parent_i - 1, currNode->parent_j + 1))
		{
			childrenNodes.push_back(bgGridNodes[currNode->parent_i - 1][currNode->parent_j + 1]);
		}

		// Bottom Middle Node From Current Position
		if (isValid(currNode->parent_i, currNode->parent_j + 1))
		{
			childrenNodes.push_back(bgGridNodes[currNode->parent_i][currNode->parent_j + 1]);
		}
		// Bottom Right Node From Current Position
		if (isValid(currNode->parent_i + 1, currNode->parent_j + 1))
		{
			childrenNodes.push_back(bgGridNodes[currNode->parent_i + 1][currNode->parent_j + 1]);
		}
	}
	else
	{
		try
		{
			throw 22;
		}
		catch (const char* e)
		{
			std::cout << "Invalid Location was Selected" << std::endl;
		}
	}



	for (auto i : childrenNodes) // Loop Through Children Nodes
	{
		for (auto j : m_closedList) // if a child is in the closed List, Conitue cause we dont like wasting time
		{
			if (i == j)
			{
				continue;
			}
		}

		i->calculategCost(i, m_startNode);
		i->calculatehCost(i, m_endNode);
		i->calculatefCost();

		for (auto k : m_openList)
		{
			if (i == k && (i->gCost > k->gCost))
			{
				continue;
			}
		}

		if (i->isWalkable && isNOTclosedList(i))
		{
			m_openList.push_back(i);
		}
	}
}

// One problem is: mOpenList is appending the same start Node and LowestFCost keeps getting assigned too the starting node..... ENSURE that the m_openList cannot append a node thats already
//	in the closedList...
void Grid::aStarAlgorithm()
{
	if (!found)
	{
		if (startNodeAdded && endNodeAdded) // If Start and End Nodes are set
		{
			while (!m_openList.empty()) // loop while the openList is not empty 
			{
				Node* lowerestNode = lowestFCost(); // grabs the lowest fCost and sets it too the currentNode.
				std::deque<Node*>::iterator it = m_openList.begin(); // Find that current node
				for (it; it != m_openList.end(); it++)
				{
					if (lowerestNode == *it) // Find the Loweserest node in the openList
					{
						m_openList.erase(it); // Find the it, now delete it from the m-openlists
						break;
					}
				}

				// Add It too the closedList
				m_closedList.push_back(lowerestNode);

				if (lowerestNode == m_endNode)
				{
					std::cout << "Congrats, We have found the END!" << std::endl;
					std::deque<Node*>::reverse_iterator i = m_closedList.rbegin();
					for (i = m_closedList.rbegin(); i != m_closedList.rend(); i++)
					{
						m_path.push_back(*i);
					}

					found = true;
					break;
				}

				checkNeighborNodes(lowerestNode);
			}
		}
	}
}

void Grid::start_node()
{
	int start_i = 1; // rand() % (m_rows);
	int start_j = 1; // rand() % (m_columns);
	m_startNode = bgGridNodes[start_i][start_j];
	m_openList.push_back(m_startNode);
	startNodeAdded = true;
}

void Grid::end_node()
{
	int end_i = 22; //rand() % (m_rows);
	int end_j = 22; //rand() % (m_columns);
	m_endNode = bgGridNodes[end_i][end_j];
	endNodeAdded = true;
}

void Grid::display()
{
	for (int i = 0; i < m_rows; i++)
	{
		for (int j = 0; j < m_columns; j++)
		{
			if (bgGridNodes[i][j]->clicked && !bgGridNodes[i][j]->startNode && !bgGridNodes[i][j]->endNode) // check if a bgNode was clicked and its not the start or end node
			{
				bgGridNodes[i][j]->nodeRect.setFillColor(sf::Color::Black); // sets the clicked node to the color black and marks it as none walkable for the search algorithms
			}
			else if (!bgGridNodes[i][j]->clicked && !bgGridNodes[i][j]->startNode && !bgGridNodes[i][j]->endNode) // check if a bgNode was clicked and its not the start or end node
			{
				bgGridNodes[i][j]->nodeRect.setOutlineThickness(0.5f);
				bgGridNodes[i][j]->nodeRect.setOutlineColor(sf::Color(220, 220, 220));
				bgGridNodes[i][j]->nodeRect.setFillColor(sf::Color(150, 150, 150)); // sets the clicked node to the color black and marks it as none walkable for the search algorithms
			}
			m_window->draw(bgGridNodes[i][j]->nodeRect); // draws the nodes of the Background
		}
	}

	m_window->draw(buttonSprite);

	if (startNodeAdded) // Draws the StartNode, IF added
	{
		m_startNode->nodeRect.setFillColor(sf::Color::Cyan);
		m_window->draw(m_startNode->nodeRect);
	}
	if (endNodeAdded) // Draws the EndNode If added
	{
		m_endNode->nodeRect.setFillColor(sf::Color::Red);
		m_window->draw(m_endNode->nodeRect);
	}

	if (found) // Draw Path if END has been reached
	{
		for (auto i : m_path)
		{
			i->nodeRect.setOutlineThickness(0.2);
			i->nodeRect.setOutlineColor(sf::Color(60, 60, 60));
			i->nodeRect.setFillColor(sf::Color::Cyan);
			m_window->draw(i->nodeRect);
		}
	}

}

bool Grid::isValid(int parent_i, int parent_j)
{
	if (parent_i >= m_rows || parent_j >= m_columns)
	{
		return false;
	}
	return (parent_i >= 0) && (parent_i <= m_rows) && (parent_j >= 0) && (parent_j <= m_columns) && bgGridNodes[parent_i][parent_j]->isWalkable;
}