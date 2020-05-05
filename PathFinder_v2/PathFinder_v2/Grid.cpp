#include "Grid.h"


float EuclidianDistance(Node* a, Node* b)
{
	return std::sqrtf(std::powf((b->x - a->x), 2) + std::powf((b->y - a->y), 2));

}

void Grid::setStart()
{
	m_startNode = nodes[1][1];
	m_startNode->rect.setFillColor(sf::Color::Blue);
}

void Grid::placeStart()
{
	sf::Vector2i mPos = sf::Mouse::getPosition();
	if (m_startNode->rect.getGlobalBounds().contains(mPos.x, mPos.y) && m_startNode->grabbed)
	{
		mouseContains();
	}
}

void Grid::setEnd()
{
	m_endNode = nodes[21][21];
	m_endNode->rect.setFillColor(sf::Color::Green);
}

void Grid::placeEnd()
{
	sf::Vector2i mPos = sf::Mouse::getPosition();
	if (m_endNode->rect.getGlobalBounds().contains(mPos.x, mPos.y) && m_endNode->grabbed)
	{
		mouseContains();
	}
}

Grid::Grid(sf::RenderWindow* window)
	: m_win(window)
{
	srand(time(NULL));
	for (int i = 0; i < ROWS; i++)
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
	setStart();
	setEnd();
	mouseNode.setSize(sf::Vector2f(bSIZE, bSIZE));
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
	for (int i = 0; i < ROWS; i++)
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
		while (par->parent != nullptr)
		{
			m_path.push_back(par);
			m_win->draw(par->rect);
			par = par->parent;
		}
	}
	found = true;
}

void Grid::Display()
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			m_win->draw(nodes[i][j]->rect);
		}
	}

	if (m_startNode != nullptr)
	{
		if (m_startNode->grabbed)
		{
			m_startNode->rect.setFillColor(sf::Color::White);
			sf::Vector2i mPos = sf::Mouse::getPosition(*m_win);
			mouseNode.setPosition(mPos.x - bSIZE / 2, mPos.y - bSIZE / 2);
			mouseNode.setFillColor(sf::Color::Blue);
			m_win->draw(mouseNode);
		}
		else if(!m_startNode->grabbed)
		{
			m_startNode->rect.setFillColor(sf::Color::Blue);
			m_win->draw(m_startNode->rect);
		}
	}

	if (m_endNode != nullptr)
	{
		if (m_endNode->grabbed)
		{
			m_endNode->rect.setFillColor(sf::Color::White);
			sf::Vector2i mPos = sf::Mouse::getPosition(*m_win);
			mouseNode.setPosition(mPos.x - bSIZE / 2, mPos.y - bSIZE / 2);
			mouseNode.setFillColor(sf::Color::Green);
			m_win->draw(mouseNode);
		}
		else if(!m_endNode->grabbed)
		{
			m_endNode->rect.setFillColor(sf::Color::Green);
			m_win->draw(m_endNode->rect);
		}
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
	if (m_startNode != nullptr)
	{
		sf::Vector2i mPos = sf::Mouse::getPosition(*m_win);
		if (m_startNode->rect.getGlobalBounds().contains(mPos.x, mPos.y) && !m_startNode->grabbed)
		{
			m_startNode->grabbed = true;
		}
		else if (m_startNode->rect.getGlobalBounds().contains(mPos.x, mPos.y) && m_startNode->grabbed)
		{

			placeStart();
			m_startNode->grabbed = false;
		}

		if (m_endNode->rect.getGlobalBounds().contains(mPos.x, mPos.y) && !m_endNode->grabbed)
		{
			m_endNode->grabbed = true;
		}
		else if (m_endNode->rect.getGlobalBounds().contains(mPos.x, mPos.y) && m_endNode->grabbed)
		{

			placeEnd();
			m_endNode->grabbed = false;
		}
	}

	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			if (m_startNode != nullptr && m_endNode != nullptr)
			{
				sf::Vector2i mPos = sf::Mouse::getPosition(*m_win);
				if (nodes[i][j]->rect.getGlobalBounds().contains(mPos.x, mPos.y) && nodes[i][j]->isWalkable && !m_startNode->grabbed && !m_endNode->grabbed)
				{
					nodes[i][j]->isWalkable = false;
					nodes[i][j]->rect.setFillColor(sf::Color::Black);
				}
				else if (nodes[i][j]->rect.getGlobalBounds().contains(mPos.x, mPos.y) && !nodes[i][j]->isWalkable)
				{
					nodes[i][j]->isWalkable = true;
					nodes[i][j]->rect.setFillColor(sf::Color::White);
				}
				if (nodes[i][j]->rect.getGlobalBounds().contains(mPos.x, mPos.y) && nodes[i][j]->isWalkable && m_startNode->grabbed)
				{
					m_startNode = nodes[i][j];
					break;
				}
				if (nodes[i][j]->rect.getGlobalBounds().contains(mPos.x, mPos.y) && nodes[i][j]->isWalkable && m_endNode->grabbed)
				{
					m_endNode = nodes[i][j];
					break;
				}
			}
		}
	}
}

void Grid::generateRandoCommandoMaze()
{	
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			int chance = rand() % 100;
			if (chance < 30)
			{
				nodes[i][j]->isWalkable = false;
				nodes[i][j]->rect.setFillColor(sf::Color::Black);
			}
		}
	}
}

