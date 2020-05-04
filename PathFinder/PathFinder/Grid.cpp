/*
	File: Grid.h
	Class Description: Creates a grid, using SFML sqaures. Each sqaure represents a Node.
*/

#include <iostream>
#include "Grid.h"

Grid::Grid(int r, int c, int s, sf::RenderWindow* w) // Takes in the rowSize, ColSize, blockSize, and Reference to the main draw window
	: m_rows(r), m_columns(c), m_size(s), m_window(w)
{
	if (!font.loadFromFile("Fonts/OpenSans-ExtraBold.ttf"))
	{
		std::cout << "Error Loading Font" << std::endl;
		exit;
	}
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


void Grid::closedList_lowestFCost(Node* lastNode) 
{
	m_path.push_back(lastNode);
	m_path.push_back(m_startNode);
	int maxVal = INT_MAX;
	int path_cost = 0;
	int index = 1;
	int maxG = m_closedList[m_closedList.size() - 1]->gCost + 1;
	while (index != maxG) // Index is theGcost Values im looking at
	{
		for (auto i : m_closedList) // Loop Through The Closed List searching for the gCost = Index
		{
			if(i->gCost == index) // If that gCost == Index we are looking for
			{
				if (i->fCost <= maxVal) // Check too see if its the smallest of the nodes were looking at
				{
					m_path.push_back(i);
					break;
				}
			}
		}
		index++;
	}
}

Node* Grid::lowestFCost()
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
	for (unsigned int i = 0; i < m_rows; i++)
	{
		for (unsigned int j = 0; j < m_columns; j++)
		{
			sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);
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

void Grid::toggleNodePickup()
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);
	if (m_endNode->nodeRect.getGlobalBounds().contains(mousePos.x, mousePos.y))
	{
		if(m_endNode->grabbed)
		{
			m_endNode->grabbed = false;
			//m_endNode = closestNode();
		}
		else
		{
			m_endNode->nodeRect.setPosition(sf::Vector2f(mousePos.x - BLOCKSIZE / 2, mousePos.y - BLOCKSIZE / 2));
			m_endNode->grabbed = true;
		}
	} 

	if (m_startNode->nodeRect.getGlobalBounds().contains(mousePos.x, mousePos.y))
	{
		if (m_startNode->grabbed)
		{
			m_startNode->grabbed = false;
			//m_startNode = closestNode();
		}
		else
		{
			m_startNode->nodeRect.setPosition(sf::Vector2f(mousePos.x - BLOCKSIZE / 2, mousePos.y - BLOCKSIZE / 2));
			m_startNode->grabbed = true;
		}
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

bool Grid::isopenList(Node* n)
{
	for (auto i : m_openList)
	{
		if (i == n)
		{
			return true;
		}
	}
	return false;
}

bool Grid::isclosedList(Node* n)
{
	for (auto i : m_closedList)
	{
		if (i == n)
		{
			return true;
		}
	}
	return false;
}

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
	//	if (isValid(currNode->parent_i - 1, currNode->parent_j - 1) && !isopenList(bgGridNodes[currNode->parent_i - 1][currNode->parent_j - 1]) 
	//		&& !isclosedList(bgGridNodes[currNode->parent_i - 1][currNode->parent_j - 1]) )
	//	{
	//		childrenNodes.push_back(bgGridNodes[currNode->parent_i - 1][currNode->parent_j - 1]);
	//	}

		// Top Middle Node From Current Position
		if (isValid(currNode->parent_i, currNode->parent_j - 1) && !isopenList(bgGridNodes[currNode->parent_i][currNode->parent_j - 1]) 
			&& !isclosedList(bgGridNodes[currNode->parent_i][currNode->parent_j - 1]) )
		{
		//	bgGridNodes[currNode->parent_i][currNode->parent_j - 1]->setParentNode(currNode);
			childrenNodes.push_back(bgGridNodes[currNode->parent_i][currNode->parent_j - 1]);
		}

		// Top Right Node From Current Position
	//	if (isValid(currNode->parent_i + 1, currNode->parent_j - 1) && !isopenList(bgGridNodes[currNode->parent_i + 1][currNode->parent_j - 1]) 
	//		&& !isclosedList(bgGridNodes[currNode->parent_i + 1][currNode->parent_j - 1]) )
	//	{
	//		childrenNodes.push_back(bgGridNodes[currNode->parent_i + 1][currNode->parent_j - 1]);
	//	}

		// Middle Left Node From Current Position
		if (isValid(currNode->parent_i - 1, currNode->parent_j) && !isopenList(bgGridNodes[currNode->parent_i - 1][currNode->parent_j]) 
			&& !isclosedList(bgGridNodes[currNode->parent_i - 1][currNode->parent_j]) )
		{
		//	bgGridNodes[currNode->parent_i - 1][currNode->parent_j]->setParentNode(currNode);
			childrenNodes.push_back(bgGridNodes[currNode->parent_i - 1][currNode->parent_j]);
		}

		// Middle Right Node From Current Position
		if (isValid(currNode->parent_i + 1, currNode->parent_j)&& !isopenList(bgGridNodes[currNode->parent_i + 1][currNode->parent_j]) 
			&& !isclosedList(bgGridNodes[currNode->parent_i + 1][currNode->parent_j]) )
		{
			//bgGridNodes[currNode->parent_i + 1][currNode->parent_j]->setParentNode(currNode);
			childrenNodes.push_back(bgGridNodes[currNode->parent_i + 1][currNode->parent_j]);
		}

		// Bottom Left Node From Current Position
	//	if (isValid(currNode->parent_i - 1, currNode->parent_j + 1) && !isopenList(bgGridNodes[currNode->parent_i - 1][currNode->parent_j + 1]) 
	//		&& !isclosedList(bgGridNodes[currNode->parent_i - 1][currNode->parent_j + 1]) )
	//	{
	//		childrenNodes.push_back(bgGridNodes[currNode->parent_i - 1][currNode->parent_j + 1]);
	//	}

		// Bottom Middle Node From Current Position
		if (isValid(currNode->parent_i, currNode->parent_j + 1) && !isopenList(bgGridNodes[currNode->parent_i][currNode->parent_j + 1])
			&& !isclosedList(bgGridNodes[currNode->parent_i][currNode->parent_j + 1]) )
		{
			//bgGridNodes[currNode->parent_i][currNode->parent_j + 1]->setParentNode(currNode);
			childrenNodes.push_back(bgGridNodes[currNode->parent_i][currNode->parent_j + 1]);
		}
		// Bottom Right Node From Current Position
	//	if (isValid(currNode->parent_i + 1, currNode->parent_j + 1)&& !isopenList(bgGridNodes[currNode->parent_i + 1][currNode->parent_j + 1 ]) 
	//		&& !isclosedList(bgGridNodes[currNode->parent_i + 1][currNode->parent_j + 1 ]) )
	//	{
	//		childrenNodes.push_back(bgGridNodes[currNode->parent_i + 1][currNode->parent_j + 1]);
	//	}
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
			if (i->isWalkable && !isopenList(i) && !isclosedList(i))
			{
				i->gCost += currNode->gCost + 1;
				i->calculatehCost(i, m_endNode);
				i->calculatefCost();
				m_openList.push_back(i);
			}
		}
	}

	std::cout << "CurrNode : GCost-> " << currNode->gCost << std::endl;
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
				if (lowerestNode == m_endNode)
				{
					std::cout << "Congrats, We have found the END!" << std::endl;
					//construct_path(lowerestNode);
					for (auto i : m_closedList)
					{
						closedList_lowestFCost(lowerestNode);
						//m_path.push_back(i);
					}
					found = true;
					break;
				}

				std::deque<Node*>::iterator it = m_openList.begin(); // Find that current node
				for (
					it; it != m_openList.end(); it++)
				{
					// Find the Loweserest node in the openList, Delete it.
					if (lowerestNode == *it) 
					{
						m_openList.erase(it); // Found, now delete it from the m-openlists
						break;
					}
				}

				// Add It too the closedList
				m_closedList.push_back(lowerestNode);

				checkNeighborNodes(lowerestNode);
			}
		}
	}
}

void Grid::start_node()
{
	int start_i = 6; // rand() % (m_rows);
	int start_j = 9; // rand() % (m_columns);
	m_startNode = bgGridNodes[start_i][start_j];
	m_openList.push_back(m_startNode);
	startNodeAdded = true;
}

void Grid::end_node()
{
	int end_i = 10; //rand() % (m_rows);
	int end_j = 9; //rand() % (m_columns);
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

		if (m_startNode->grabbed)
		{
			m_startNode->nodeRect.setFillColor(sf::Color(93, 138, 255));
			sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);
			m_startNode->nodeRect.setPosition(sf::Vector2f(mousePos.x - BLOCKSIZE / 2, mousePos.y - BLOCKSIZE / 2));
		}
		else if (!m_startNode->grabbed)
		{
			m_startNode->nodeRect.setFillColor(sf::Color(93, 138, 169, 200));
		}

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
			i->CostDisplay(font, m_window, i);
		}
		for (auto i : m_closedList)
		{
			i->CostDisplay(font, m_window, i);
		}
	}
}

void Grid::construct_path(Node* lastNode)
{
	m_path.push_back(lastNode);
	//	 Iteratate Through the Closed List in Reverse
//	std::deque<Node*>::const_reverse_iterator iterate = m_closedList.rbegin();
//	for (iterate = m_closedList.rbegin(); iterate != m_closedList.rend(); iterate++)
//	{
//		if (*iterate == tmpNode)
//		{
//			m_path.push_back(*iterate);
//		}
//		tmpNode = lastNode->parentNode;
//	}

	std::cout << "m_path: " << m_path.size() << std::endl;
}

bool Grid::isValid(int parent_i, int parent_j)
{
	if (parent_i >= m_rows || parent_j >= m_columns)
	{
		return false;
	}

	return (parent_i >= 0) && (parent_i <= m_rows) && (parent_j >= 0) && (parent_j <= m_columns) && bgGridNodes[parent_i][parent_j]->isWalkable;
}