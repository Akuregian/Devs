#include "Grid.h"

bool Grid::isValid(int x, int y)
{
	if (x > ROWS || y > COLS)
	{
		return false;
	}
	return (x >= 0) && (x < ROWS) && (y >= 0) && (y < COLS);
}

void Grid::mouseContains()
{
	sf::Vector2i mPos = sf::Mouse::getPosition(*m_win);
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			if (nodes[i][j]->rect.getGlobalBounds().contains(mPos.x, mPos.y) && nodes[i][j]->isWalkable)
			{
				nodes[i][j]->isWalkable = false;
				nodes[i][j]->rect.setFillColor(sf::Color::Black);
			}
			else if (nodes[i][j]->rect.getGlobalBounds().contains(mPos.x, mPos.y) && !nodes[i][j]->isWalkable)
			{
				nodes[i][j]->isWalkable = true;
				nodes[i][j]->rect.setFillColor(sf::Color::White);
			}
		}
	}
}

float EuclidianDistance(Node* a, Node* b)
{
	return std::sqrtf(std::powf((b->x - a->x), 2) + std::powf((b->y - a->y), 2));

}

Grid::Grid(sf::RenderWindow* window)
	: m_win(window)
{

	for (int i = 0; i <ROWS ; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			nodes[i][j] = new Node;
			nodes[i][j]->rect.setFillColor(sf::Color::White);
			nodes[i][j]->rect.setSize(sf::Vector2f(bSIZE, bSIZE));
			nodes[i][j]->parent = nullptr;
			nodes[i][j]->x = i;
			nodes[i][j]->y = j;
			nodes[i][j]->rect.setPosition(sf::Vector2f(i * BLOCK_DISTANCE + 5, j * BLOCK_DISTANCE + 5));
		}
	}
	m_startNode = nodes[0][0];
	m_endNode = nodes[24][24];
	m_startNode->rect.setFillColor(sf::Color::Green);
	m_endNode->rect.setFillColor(sf::Color::Red);
}


void Grid::GrabNeighborNodes(Node& curr)
{

	if (isValid(curr.x, curr.y - 1)) // Top
	{
		if (nodes[curr.x][curr.y]->isWalkable)
		{
			nodes[curr.x][curr.y]->NeighborNodes.push_back(nodes[curr.x][curr.y - 1]);
		}
	}

	if (isValid(curr.x - 1, curr.y)) // Left
	{
		if (nodes[curr.x - 1][curr.y]->isWalkable)
		{
			nodes[curr.x][curr.y]->NeighborNodes.push_back(nodes[curr.x - 1][curr.y]);
		}
	}

	if (isValid(curr.x + 1, curr.y)) // Right
	{
		if (nodes[curr.x + 1][curr.y]->isWalkable)
		{
			nodes[curr.x][curr.y]->NeighborNodes.push_back(nodes[curr.x + 1][curr.y]);
		}
	}

	if (isValid(curr.x, curr.y + 1)) // Bottom
	{
		if (nodes[curr.x][curr.y + 1]->isWalkable)
		{
			nodes[curr.x][curr.y]->NeighborNodes.push_back(nodes[curr.x][curr.y + 1]);
		}
	}

}

bool Grid::A_Star_Algorithm()
{
	std::cout << "Entered" << std::endl;
	// Reset All The Nodes back to Default Before Algorithm Starts
	for (int i = 0; i <ROWS ; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			nodes[i][j]->visited = false;
			nodes[i][j]->globalfGoal = INFINITY;
			nodes[i][j]->localfGoal = INFINITY;
			nodes[i][j]->parent = nullptr;
		}
	}

	// Starting Setup
	Node* currNode = m_startNode; // Set the CurrentNode to the Starting Node
	currNode->localfGoal = 0.0f; // Set Its Local Goal to 0;
	currNode->globalfGoal = EuclidianDistance(m_startNode, m_endNode); // Grab the Distance from the StartingNode -> EndingNode
	nonTestedNodes.push_back(m_startNode); // push_back(StartingNode)

	while (!nonTestedNodes.empty() && currNode != m_endNode)
	{
		// Sort the Global Goal so thgat the lowest is first, Ill be using a Lambda Func, hence the []
		nonTestedNodes.sort([](const Node* lhNode, const Node* rhNode) { return lhNode->globalfGoal < rhNode->globalfGoal; });

		while (!nonTestedNodes.empty() && nonTestedNodes.front()->visited)
		{
			nonTestedNodes.pop_front();
		}

		if (nonTestedNodes.empty())
		{
			break;
		}

		currNode = nonTestedNodes.front();
		currNode->visited = true;
		GrabNeighborNodes(*currNode);
		for (auto i : currNode->NeighborNodes) 
		{
			if (!i->visited && i->isWalkable)
			{
				nonTestedNodes.push_back(i);
			}

			float possiblyLowerPath = currNode->localfGoal + EuclidianDistance(currNode, i);

			if (possiblyLowerPath < i->localfGoal)
			{
				i->parent = currNode;
				i->localfGoal = possiblyLowerPath;
				i->globalfGoal = i->localfGoal + EuclidianDistance(i, m_endNode);
			}
		}
	}
	constructPath();
	return true;
}

void Grid::constructPath()
{
	if (m_endNode != nullptr)
	{
		Node* par = m_endNode;
		while(par->parent != nullptr)
		{
			m_path.push_back(par);
			m_win->draw(par->rect);
			par = par->parent;
		}
	}
	found = true;
}


void Grid::resetAlgorithm()
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			nodes[i][j] = nullptr;
		}
	}
	for (int i = 0; i <ROWS ; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			nodes[i][j] = new Node;
			nodes[i][j]->rect.setFillColor(sf::Color::White);
			nodes[i][j]->rect.setSize(sf::Vector2f(bSIZE, bSIZE));
			nodes[i][j]->parent = nullptr;
			nodes[i][j]->x = i;
			nodes[i][j]->y = j;
			nodes[i][j]->rect.setPosition(sf::Vector2f(i * BLOCK_DISTANCE + 5, j * BLOCK_DISTANCE + 5));
		}
	}
	m_startNode = nodes[0][0];
	m_endNode = nodes[24][24];
	m_startNode->rect.setFillColor(sf::Color::Green);
	m_endNode->rect.setFillColor(sf::Color::Red);

	
}

void Grid::Display()
{
	for (int i = 0; i <ROWS ; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			m_win->draw(nodes[i][j]->rect);
		}
	}
	if (m_startNode != nullptr)
	{
		m_win->draw(m_startNode->rect);
	}
	if (found)
	{
		for (auto i : m_path)
		{
			i->rect.setFillColor(sf::Color::Magenta);
			m_win->draw(i->rect);
		}
	}
}
