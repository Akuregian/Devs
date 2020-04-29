/*
	File: Grid.h
	Class Description: Creates a grid, using SFML sqaures. Each sqaure represents a Node.
*/

#include <iostream>
#include "Grid.h"

Grid::Grid(int r, int c, int s, sf::RenderWindow* w) // Takes in the rowSize, ColSize, blockSize, and Reference to the main draw window
	: m_rows(r), m_columns(c), m_size(s), m_window(w)
{
	for (int i = 0; i < m_rows; i++)
	{
		for (int j = 0; j < m_columns; j++)
		{
			bgGridNodes[i][j] = Node(m_size, sf::Color::White); // Add a node at each location of the 2 dimensional grid of node structs
			bgGridNodes[i][j].parent_i = i;
			bgGridNodes[i][j].parent_j = j;
			bgGridNodes[i][j].nodeRect.setPosition(sf::Vector2f(i * 30 + 5, j * 30 + 5)); // sets each nodes postions, with there distance apart from eachother
		}
	}
}

Node Grid::lowestFCost()
{
	int tmpCost = m_openList[0].fCost;
	Node lowestFCostNode;
	for (auto i : m_openList)
	{
		if (i.fCost <= tmpCost)
		{
			tmpCost = i.fCost;
			lowestFCostNode = i;
		}
	}
	return lowestFCostNode;
}

void Grid::checkNeighborNodes(Node currNode)
{

	//m_openList.clear();
	std::deque<Node> childrenNodes; // Where I can store the Children Nodes Surrounding the Current Node.
	if (isValid(currNode.parent_i, currNode.parent_j)) // If the Node is a Valid node; meaning its within [m_rows][m_columns], then enter
	{

		//	calculate their values [f = g + h], then Add the adjacent sqaures to the open list to be evaluated
		//
		//     n[i-1][j-1]  n[i][j-1]  n[i+1][j-1]
		//	            \      |      /
		//	             \     |     /
		//	   n[i-1][j] --- currNode --- n[i+1][j]
		//	             /     |     \
		//              /      |      \
		//     n[i-1][j+1]   n[i][j+1]   n[i+1][j+1]


		// Top Left Node From Current Position
		if (isValid(currNode.parent_i - 1, currNode.parent_j - 1))
		{
			childrenNodes.push_back(bgGridNodes[currNode.parent_i - 1][currNode.parent_j - 1]);
		}

		// Top Middle Node From Current Position
		if (isValid(currNode.parent_i, currNode.parent_j - 1))
		{
			childrenNodes.push_back(bgGridNodes[currNode.parent_i][currNode.parent_j - 1]);
		}

		// Top Right Node From Current Position
		if (isValid(currNode.parent_i + 1, currNode.parent_j - 1))
		{
			childrenNodes.push_back(bgGridNodes[currNode.parent_i + 1][currNode.parent_j - 1]);
		}

		// Middle Left Node From Current Position
		if (isValid(currNode.parent_i - 1, currNode.parent_j))
		{
			childrenNodes.push_back(bgGridNodes[currNode.parent_i - 1][currNode.parent_j]); 
		}

		// Middle Right Node From Current Position
		if (isValid(currNode.parent_i + 1, currNode.parent_j))
		{
			childrenNodes.push_back(bgGridNodes[currNode.parent_i + 1][currNode.parent_j]);
		}

		// Bottom Left Node From Current Position
		if (isValid(currNode.parent_i - 1, currNode.parent_j + 1))
		{
			childrenNodes.push_back(bgGridNodes[currNode.parent_i - 1][currNode.parent_j + 1]);
		}

		// Bottom Middle Node From Current Position
		if (isValid( currNode.parent_i, currNode.parent_j + 1))
		{
			childrenNodes.push_back(bgGridNodes[currNode.parent_i][currNode.parent_j + 1]);
		}
		// Bottom Right Node From Current Position
		if (isValid(currNode.parent_i + 1, currNode.parent_j + 1))
		{
			childrenNodes.push_back(bgGridNodes[currNode.parent_i + 1][currNode.parent_j + 1]);
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
			std::cout << "Invalid Location" << std::endl;
		}
	}

	for (auto i : childrenNodes) // Loop Through Children Nodes
	{

		for (auto k : m_closedList) // if a child is in the closed List, Conitue
		{
			if (i == k)
			{
				continue;
			}
		}

		for (auto j : m_openList)
		{
			if (i == j && (i.gCost > j.gCost))
			{
				continue;
			}
		}
			i.calculategCost(i, m_startNode);
			i.calculatehCost(i, m_endNode);
			i.calculatefCost();
			m_openList.push_back(i);
	}

}

// One problem is: repeatidly checking the same node and calc scores
void Grid::aStarAlgorithm()
{
	if (!found)
	{
		if (startNodeAdded && endNodeAdded) // If Start and End Nodes are set
		{
			Node currentNode;

			while (!m_openList.empty()) // loop while the openList is not empty
			{
				currentNode = m_openList[0]; // currentNode = the first element in the openList
				Node lowerestNode = lowestFCost(); // grabs the lowest fCost and sets it too the currentNode

				//m_openList.clear();

				std::deque<Node>::iterator it = m_openList.begin(); // Find that current node
				if (it != m_openList.end())
				{
					if (lowerestNode == *it) // Find the Loweserest node in the openList
					{
						m_openList.erase(it); // Find the it, now delete it from the m-openlists
					}
				}

				// Add It too the closedList
				m_closedList.push_back(lowerestNode);

				if (lowerestNode == m_endNode)
				{
					std::cout << "Congrats, We have found the END!" << std::endl;
					std::deque<Node>::reverse_iterator i = m_closedList.rbegin();
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

void Grid::add_start_node()
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);
	for (int i = 0; i < m_rows; i++)
	{
		for (int j = 0; j < m_columns; j++)
		{
			if (bgGridNodes[i][j].nodeRect.getGlobalBounds().contains(mousePos.x, mousePos.y) && sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				bgGridNodes[i][j].nodeRect.setFillColor(sf::Color::Blue); // set the selected nodes color
				bgGridNodes[i][j].startNode = true; // sets the startNode variable inside the node to true, indicating its the startnode
				startNodeAdded = true;
				m_startNode = bgGridNodes[i][j];
				m_openList.push_back(bgGridNodes[i][j]);	// pushes that startnode on the openList making it the only node in that list, for now
				break;
			}
		}
	}
}


void Grid::add_end_node()
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window);
	for (int i = 0; i < m_rows; i++)
	{
		for (int j = 0; j < m_columns; j++)
		{
			if (bgGridNodes[i][j].nodeRect.getGlobalBounds().contains(mousePos.x, mousePos.y) && sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				bgGridNodes[i][j].nodeRect.setFillColor(sf::Color::Red); // sets end node color	
				bgGridNodes[i][j].startNode = true; // variable indicationg its the end node
				m_endNode = bgGridNodes[i][j]; //sets the private var m_endNode in grid class to that endNode ..
				endNodeAdded = true;
				break;
			}
		}
	}
}

void Grid::clickCheck()   // loops through the bgGridnodes and checks if that gridNode has been clicked on
{
	for (int i = 0; i < m_rows; i++)
	{
		for (int j = 0; j < m_columns; j++)
		{ 
			bgGridNodes[i][j].isClicked(bgGridNodes[i][j], m_window); // isClicked is a method from the Node class
		}
	}
}

void Grid::display()
{
	for (int i = 0; i < m_rows; i++)
	{
		for (int j = 0; j < m_columns; j++)
		{

			m_window->draw(bgGridNodes[i][j].nodeRect); // draws the nodes of the Background
			if (bgGridNodes[i][j].clicked && !bgGridNodes[i][j].startNode && !bgGridNodes[i][j].endNode) // check if a bgNode was clicked and its not the start or end node
			{
				bgGridNodes[i][j].nodeRect.setFillColor(sf::Color::Black); // sets the clicked node to the color black and marks it as none walkable for the search algorithms
			}
			else if (!bgGridNodes[i][j].clicked && !bgGridNodes[i][j].startNode && !bgGridNodes[i][j].endNode) // check if a bgNode was clicked and its not the start or end node
			{
				bgGridNodes[i][j].nodeRect.setFillColor(sf::Color::White); // sets the clicked node to the color black and marks it as none walkable for the search algorithms
			}
		//	for (auto i : m_openList)
		//	{
		//		i.nodeRect.setFillColor(sf::Color::Cyan);
		//		m_window->draw(i.nodeRect);
		//	}
			if (found)
			{
				for (auto i : m_path)
				{
					i.nodeRect.setFillColor(sf::Color::Green);
					m_window->draw(i.nodeRect);
				}
				exit; // Successfully Drawn the Path and now exiting.. Or I could reset and redraw... Later after we've finxed the bugs
			}
		}
	}
}

bool Grid::isValid(int parent_i, int parent_j)
{
	if (parent_i >= m_rows || parent_j >= m_columns)
	{
		return false;
	}
	return (parent_i >= 0) && (parent_i <= m_rows) && (parent_j >= 0) && (parent_j <= m_columns);
}
