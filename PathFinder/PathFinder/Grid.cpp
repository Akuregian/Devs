#include "Grid.h"

const sf::Color BGNODECOL = sf::Color(119, 136, 153);
const sf::Color SEARCHNODECOL= sf::Color(70, 130, 180);
const sf::Color OUTERSEARCHNODECOL= sf::Color(139, 0, 140);
const sf::Color PATHNODECOL= sf::Color(255, 215, 0);
const sf::Color STARTNODECOL= sf::Color(72, 61, 139);
const sf::Color MOUSESTARTNODECOL= sf::Color(72, 61, 139, 100);
const sf::Color ENDNODECOL= sf::Color(255, 140, 0);
const sf::Color MOUSEENDNODECOL= sf::Color(255, 140, 0, 100);
const sf::Color LINECOL = sf::Color(0, 139, 139);

float EuclidianDistance(Node* a, Node* b)
{
	return std::sqrtf(std::powf((b->x - a->x), 2) + std::powf((b->y - a->y), 2));

}

void Grid::setStart()
{
	m_startNode = nodes[1][1];
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
	m_endNode->rect.setFillColor(sf::Color(ENDNODECOL));
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
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			nodes[i][j] = new Node;
			nodes[i][j]->rect.setFillColor(sf::Color(BGNODECOL));
			nodes[i][j]->rect.setSize(sf::Vector2f(bSIZE, bSIZE));
			nodes[i][j]->parent = nullptr;
			nodes[i][j]->x = i;
			nodes[i][j]->y = j;
			nodes[i][j]->rect.setPosition(sf::Vector2f(i * BLOCK_DISTANCE + 5, j * BLOCK_DISTANCE + 5));
		}
	}

	srand(time(NULL));
	setStart();
	setEnd();
	mouseNode.setSize(sf::Vector2f(bSIZE, bSIZE));
	for (int i = 0; i < 10; i++)
	{
		 m_buttons.push_back(new Button("Button!", sf::Color(31, 40, 51), 200, 60, window, 775, 10 + i * 75, 0));
	}
	m_buttons[0]->setText("Start");
	m_buttons[0]->id = "start";
	m_buttons[0]->tweakTextPosition(45, 5);

	m_buttons[1]->setText("A* Star");
	m_buttons[1]->id = "astar";
	m_buttons[1]->createIndicator = 1;
	m_buttons[1]->tweakTextPosition(25 , 5);

	m_buttons[2]->setText("Dijkstra");
	m_buttons[2]->id = "dijkstra";
	m_buttons[2]->createIndicator = 1;
	m_buttons[2]->tweakTextPosition(20 , 5);

	m_buttons[3]->setText("Breadth");
	m_buttons[3]->id = "breadth";
	m_buttons[3]->createIndicator = 1;
	m_buttons[3]->tweakTextPosition(15 , 5);

	m_buttons[4]->setText("Greedy");
	m_buttons[4]->id = "greedy";
	m_buttons[4]->createIndicator = 1;
	m_buttons[4]->tweakTextPosition(25 , 5);

	m_buttons[5]->setText("......");
	m_buttons[5]->id = "..";
	m_buttons[5]->tweakTextPosition(25 , 5);

	m_buttons[6]->setText("......");
	m_buttons[6]->id = "..";
	m_buttons[6]->tweakTextPosition(25 , 5);

	m_buttons[7]->setText("Rando");
	m_buttons[7]->id = "rando";
	m_buttons[7]->tweakTextPosition(25 , 5);

	m_buttons[8]->bName.setCharacterSize(35);
	m_buttons[8]->setText("Clear Path");
	m_buttons[8]->id = "clear";
	m_buttons[8]->tweakTextPosition(10, 5);

	m_buttons[9]->setText("Reset");
	m_buttons[9]->id = "reset";
	m_buttons[9]->tweakTextPosition(40, 5);

}

void Grid::GrabNeighborNodes(Node& curr)
{
	if (isValid(curr.x, curr.y - 1)) // Top
	{
		if (nodes[curr.x][curr.y]->isWalkable && !nodes[curr.x][curr.y]->isWall)
		{
			nodes[curr.x][curr.y]->NeighborNodes.push_back(nodes[curr.x][curr.y - 1]);
		}
	}

	if (isValid(curr.x - 1, curr.y)) // Left
	{
		if (nodes[curr.x - 1][curr.y]->isWalkable && !nodes[curr.x - 1][curr.y]->isWall)
		{
			nodes[curr.x][curr.y]->NeighborNodes.push_back(nodes[curr.x - 1][curr.y]);
		}
	}

	if (isValid(curr.x + 1, curr.y)) // Right
	{
		if (nodes[curr.x + 1][curr.y]->isWalkable && !nodes[curr.x + 1][curr.y]->isWall)
		{
			nodes[curr.x][curr.y]->NeighborNodes.push_back(nodes[curr.x + 1][curr.y]);
		}
	}

	if (isValid(curr.x, curr.y + 1)) // Bottom
	{
		if (nodes[curr.x][curr.y + 1]->isWalkable && !nodes[curr.x][curr.y + 1]->isWall)
		{
			nodes[curr.x][curr.y]->NeighborNodes.push_back(nodes[curr.x][curr.y + 1]);
		}
	}

}

bool Grid::A_Star_Algorithm(const float& rate)
{
	pathNotFound = false;
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

	while (!nonTestedNodes.empty() && currNode != m_endNode) // Loop until empty or weve found the end Node
	{
		if (m_clock.getElapsedTime().asMilliseconds() > rate)
		{
			m_clock.restart();

			for (auto i : nonTestedNodes)
			{

				if (i != m_endNode && i != m_startNode)
				{
					i->rect.setFillColor(sf::Color(OUTERSEARCHNODECOL));
					m_win->draw(i->rect);
				}
			}

			for (auto i : m_checkedNodes)
			{
				if (i != m_endNode && i != m_startNode)
				{
					i->rect.setFillColor(sf::Color(SEARCHNODECOL)); // checkedNodes COLOR
					m_win->draw(i->rect);
				}
			}
			//  Display Buttons
			if (!m_buttons.empty())
			{
				for (auto i : m_buttons)
				{
					i->buttonDisplay();
				}
			}
			m_win->draw(m_startNode->rect);
			m_win->draw(m_endNode->rect);
			m_win->display();

			// Sort the Global Goal so that the lowest is first, Ill be using a Lambda Func, hence the []
			nonTestedNodes.sort([](const Node* lhNode, const Node* rhNode) { return lhNode->globalfGoal < rhNode->globalfGoal; }); //lhNode is less then rhNode, Return;

			while (!nonTestedNodes.empty() && nonTestedNodes.front()->visited) // Loop while not empty && if we visited a node already, delete
			{
				m_checkedNodes.push_back(nonTestedNodes.front());
				nonTestedNodes.pop_front();
			}

			if (nonTestedNodes.empty()) // if empty, break
			{
				pathNotFound = true;
				break;
			}

			currNode = nonTestedNodes.front(); // Grab SORTED front Node and set to current
			currNode->visited = true; // set visited = true
			GrabNeighborNodes(*currNode); // Func grabs the Neighboring Nodes
			for (auto i : currNode->NeighborNodes) // Loop Through Current Nodes Neighbors
			{
				if (!i->visited && i->isWalkable) // If we havnt visited it, and its walkable -> push_back()
				{
					nonTestedNodes.push_back(i);
				}

				float possiblyLowerPath = currNode->localfGoal + EuclidianDistance(currNode, i); // Calculates a new fCost from the CurrentNode to the Neighbor Node
																								 // Adding the current fCost
				if (possiblyLowerPath < i->localfGoal) // if that new fCost is Lower than the neighbors fCost, set to current
				{
					i->parent = currNode; // Set that Neighbors parent equal too the currentNode, meaning step toward that direction
					i->localfGoal = possiblyLowerPath; // Change the localFgoal to the floatValue
					i->globalfGoal = i->localfGoal + EuclidianDistance(i, m_endNode); // set Global--> (NeighborNodes LocalFGoal + that Nodes dis to END)
				}
			}
		}
	}
	constructPath();
	return true;
}


bool Grid::Dijkstras_Algorithm(const float& rate)
{
	pathNotFound = false;
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
	currNode->localfGoal = 0.0f;
	currNode->globalfGoal = EuclidianDistance(m_startNode, currNode); // Grab the Distance from the StartingNode -> CurrentNode
	nonTestedNodes.push_back(currNode); // push_back(StartingNode)

	while (!nonTestedNodes.empty() && currNode != m_endNode) // Loop until empty or we've found the end Node
	{
		if (m_clock.getElapsedTime().asMilliseconds() > rate)
		{
			m_clock.restart();

			for (auto i : nonTestedNodes)
			{

				if (i != m_endNode && i != m_startNode)
				{
					i->rect.setFillColor(sf::Color(OUTERSEARCHNODECOL));
					m_win->draw(i->rect);
				}
			}

			for (auto i : m_checkedNodes)
			{
				if (i != m_endNode && i != m_startNode)
				{
					i->rect.setFillColor(sf::Color(SEARCHNODECOL)); // checkedNodes COLOR
					m_win->draw(i->rect);
				}
			}
			//  Display Buttons
			if (!m_buttons.empty())
			{
				for (auto i : m_buttons)
				{
					i->buttonDisplay();
				}
			}
			m_win->draw(m_startNode->rect);
			m_win->draw(m_endNode->rect);
			m_win->display();

			// Sort the Global Goal so that the lowest is first, Ill be using a Lambda Func, hence the []
			nonTestedNodes.sort([](const Node* lhNode, const Node* rhNode) { return lhNode->globalfGoal < rhNode->globalfGoal; }); //lhNode is less then rhNode, Return;

			while (!nonTestedNodes.empty() && nonTestedNodes.front()->visited) // Loop while not empty && if we visited a node already, delete
			{
				m_checkedNodes.push_back(nonTestedNodes.front());
				nonTestedNodes.pop_front();
			}

			if (nonTestedNodes.empty()) // if empty, break
			{
				pathNotFound = true;
				break;
			}

			currNode = nonTestedNodes.front(); // Grab SORTED front Node and set to current
			currNode->visited = true; // set visited = true
			GrabNeighborNodes(*currNode); // Func grabs the Neighboring Nodes && stores them in current->neighbors <deque>
			for (auto i : currNode->NeighborNodes) // Loop Through Current Nodes Neighbors
			{
				if (!i->visited && i->isWalkable && !i->isWall) // If we havnt visited it, its walkable and is not a wall -> push_back(Neighbor)
				{
					nonTestedNodes.push_back(i);
				}

				float possiblyLowerPath =  EuclidianDistance(m_startNode, currNode) + EuclidianDistance(currNode, i); // Calculates a new fCost from the CurrentNode to the Neighbor Node
				if (possiblyLowerPath < i->globalfGoal) // Check againts the distance from rootNode
				{
					i->parent = currNode; // Set that Neighbors parent equal too the currentNode, meaning step toward that direction
					i->localfGoal = possiblyLowerPath; // Change the localFgoal to the floatValue
					i->globalfGoal = EuclidianDistance(i, m_startNode); // set Global--> (NeighborNodes LocalFGoal + that Nodes dis to END)
				}
			}
		}
	}
	constructPath(); 
	return true;

}

bool Grid::Greedy_Algorithm(const float& rate)
{
	pathNotFound = false;
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

	while (!nonTestedNodes.empty() && currNode != m_endNode) // Loop until empty or weve found the end Node
	{
		if (m_clock.getElapsedTime().asMilliseconds() > rate)
		{
			m_clock.restart();

			for (auto i : nonTestedNodes)
			{

				if (i != m_endNode && i != m_startNode && !i->isWall)
				{
					i->rect.setFillColor(sf::Color(OUTERSEARCHNODECOL));
					m_win->draw(i->rect);
				}
			}

			for (auto i : m_checkedNodes)
			{
				if (i != m_endNode && i != m_startNode && !i->isWall)
				{
					i->rect.setFillColor(sf::Color(SEARCHNODECOL)); // checkedNodes COLOR
					m_win->draw(i->rect);
				}
			}
			//  Display Buttons
			if (!m_buttons.empty())
			{
				for (auto i : m_buttons)
				{
					i->buttonDisplay();
				}
			}
			m_win->draw(m_startNode->rect);
			m_win->draw(m_endNode->rect);
			m_win->display();

			// Sort the Global Goal so that the lowest is first, Ill be using a Lambda Func, hence the []
			nonTestedNodes.sort([](const Node* lhNode, const Node* rhNode) { return lhNode->globalfGoal < rhNode->globalfGoal; }); //lhNode is less then rhNode, Return;

			while (!nonTestedNodes.empty() && nonTestedNodes.front()->visited) // Loop while not empty && if we visited a node already, delete
			{
				m_checkedNodes.push_back(nonTestedNodes.front());
				nonTestedNodes.pop_front();
			}

			if (nonTestedNodes.empty()) // if empty, break
			{
				pathNotFound = true;
				break;
			}

			currNode = nonTestedNodes.front(); // Grab SORTED front Node and set to current
			currNode->visited = true; // set visited = true
			GrabNeighborNodes(*currNode); // Func grabs the Neighboring Nodes
			for (auto i : currNode->NeighborNodes) // Loop Through Current Nodes Neighbors
			{
				if (!i->visited && i->isWalkable && !i->isWall) // If we havnt visited it, and its walkable -> push_back()
				{
					nonTestedNodes.push_back(i);
				}

				//float possiblyLowerPath = EuclidianDistance(m_startNode, currNode) + EuclidianDistance(i, m_endNode); 

				float possiblyLowerPath = EuclidianDistance(currNode, i) + EuclidianDistance(i, m_endNode) ;
				if (possiblyLowerPath < i->globalfGoal) // Check if the new path is less than the globalCost
				{
					i->parent = currNode; // Set that Neighbors parent equal too the currentNode, meaning step toward that direction
					i->globalfGoal = EuclidianDistance(currNode, m_endNode);
					nonTestedNodes.push_back(i);
				}
			}
		}
	}
	constructPath();
	return true;
}

bool Grid::Breadth_Algorithm(const float&)
{

	return true;
}

void Grid::constructPath()
{
	if (!pathNotFound)
	{
		if (m_endNode != nullptr)
		{
			Node* par = m_endNode;
			par = par->parent;
			while (par->parent != nullptr) 
			{
				m_path.push_back(par);
				m_win->draw(par->rect);
				par = par->parent;
				if (m_path.size() > 50)
				{
					std::cout << "Breaking for Loop, Must be an error" << std::endl;
					break;
				}
			}
		}
		startAlgo = false;
		found = true;
		tracePath();
		trace = true;
	}
	else
	{
		std::cout << "No Path Was Found. End Node is SomeWhere Out there, but we got blocked off.." << std::endl;
		startAlgo = false;
		found = false;
		trace = false;
		
	}
}

void Grid::CheckButtonPress()
{

	for (auto i : m_buttons)
	{
		if (aStar) // Set to true[elsewhere] if Mouse clicks button
		{
			if (i->id == "astar")
			{
				i->indicator.setFillColor(sf::Color::Green);
			}
			else 
			{
				i->indicator.setFillColor(sf::Color::Black);
			}
		}
		else if (dijkstra)
		{
			if (i->id == "dijkstra")
			{
				i->indicator.setFillColor(sf::Color::Green);
			}
			else 
			{
				i->indicator.setFillColor(sf::Color::Black);
			}
		}
		else if (breadth)
		{
			if (i->id == "breadth")
			{
				i->indicator.setFillColor(sf::Color::Green);
			}
			else
			{
				i->indicator.setFillColor(sf::Color::Black);
			}
		}
		else if (greedy)
		{
			if (i->id == "greedy")
			{
				i->indicator.setFillColor(sf::Color::Green);
			}
			else
			{
				i->indicator.setFillColor(sf::Color::Black);
			}
		}
	}
}

void Grid::Display()
{
	// Draws Nodes
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			m_win->draw(nodes[i][j]->rect);
		}
	}


	//  Display Buttons
	if (!m_buttons.empty())
	{
		for (auto i : m_buttons)
		{
			i->buttonDisplay();
		}
	}

	// For Each Button..
	CheckButtonPress();

	// Creates effect of picking and placing the Start and EndNode
	if (m_startNode != nullptr)
	{
		if (m_startNode->grabbed)
		{
			m_startNode->rect.setFillColor(sf::Color(BGNODECOL));
			sf::Vector2i mPos = sf::Mouse::getPosition(*m_win);
			mouseNode.setPosition(mPos.x - bSIZE / 2, mPos.y - bSIZE / 2);
			mouseNode.setFillColor(sf::Color(MOUSESTARTNODECOL));
			nodes[m_startNode->x][m_startNode->y]->rect.setFillColor(sf::Color(BGNODECOL));
			m_win->draw(mouseNode);
		}
		else if (!m_startNode->grabbed)
		{
			m_startNode->rect.setFillColor(sf::Color(STARTNODECOL));
			m_win->draw(m_startNode->rect);
		}
	}

	if (m_endNode != nullptr)
	{
		if (m_endNode->grabbed)
		{
			m_endNode->rect.setFillColor(sf::Color(BGNODECOL));
			sf::Vector2i mPos = sf::Mouse::getPosition(*m_win);
			mouseNode.setPosition(mPos.x - bSIZE / 2, mPos.y - bSIZE / 2);
			mouseNode.setFillColor(sf::Color(MOUSEENDNODECOL));
			m_win->draw(mouseNode);
		}
		else if (!m_endNode->grabbed)
		{
			m_endNode->rect.setFillColor(sf::Color(ENDNODECOL));
			m_win->draw(m_endNode->rect);
		}
	}

	if (found) 
	{ 
		//AnimationNodes(40.0f);
		AnimationNodes(0);
		found = false;
		complete = true;
	}

	else if (complete && trace)
	{
		m_win->draw(&tracedLines[0], m_path.size(), sf::LinesStrip);
		//complete = false;
	}
}

// Animate the Nodes as It searches through the algorithm
void Grid::AnimationNodes(float rate)
{
	for (int i = 0; i < m_path.size(); i++)
	{
		if (m_clock.getElapsedTime().asMilliseconds() > rate)
		{
			m_clock.restart();
			m_path[i]->rect.setFillColor(sf::Color(PATHNODECOL));
			m_win->draw(m_path[i]->rect);
		}
		else if (i != 0)
		{
			i--;
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

	if (!m_buttons.empty())
	{
		for (auto i : m_buttons)
		{
			sf::Vector2i mPos = sf::Mouse::getPosition(*m_win);

			if (i->button.getGlobalBounds().contains(mPos.x, mPos.y) && i->id == "start")
			{
				tracedLines.clear();
				clear();
				// If Node isWalkable AND node IS NOT a Wall, Set color back to original
				for (int i = 0; i < ROWS; i++)
				{
					for (int j = 0; j < COLS; j++)
					{
						if (nodes[i][j]->isWalkable && !nodes[i][j]->isWall)
						{
							nodes[i][j]->rect.setFillColor(sf::Color(BGNODECOL));
							m_win->draw(nodes[i][j]->rect);
						}
					}
				}
				//  Display Buttons
				if (!m_buttons.empty())
				{
					for (auto i : m_buttons)
					{
						i->buttonDisplay();
					}
				}
				
				startAlgo = true;
				break;
			}

			if (i->button.getGlobalBounds().contains(mPos.x, mPos.y) && i->id == "astar")
			{
				aStar = true;
				i->indicator.setFillColor(sf::Color::Green);
				// Any Others are set to false here
				breadth = false;
				greedy = false;
				dijkstra = false;
			}

			if (i->button.getGlobalBounds().contains(mPos.x, mPos.y) && i->id == "dijkstra")
			{
				dijkstra = true;
				i->indicator.setFillColor(sf::Color::Green);
				// Any Others are set to false here
				aStar = false;
				breadth = false;
				greedy = false;
			}

			if (i->button.getGlobalBounds().contains(mPos.x, mPos.y) && i->id == "breadth")
			{
				breadth = true;
				// Any Others are set to false here
				aStar = false;
				greedy = false;
				dijkstra = false;
			}

			if (i->button.getGlobalBounds().contains(mPos.x, mPos.y) && i->id == "greedy")
			{
				greedy = true;
				// Any Others are set to false here
				aStar = false;
				dijkstra = false;
				breadth = false;
			}

			if (i->button.getGlobalBounds().contains(mPos.x, mPos.y) && i->id == "rando")
			{
				generateRandoCommandoMaze();
			}

			if (i->button.getGlobalBounds().contains(mPos.x, mPos.y) && i->id == "clear")
			{
				clear();
			}
			if (i->button.getGlobalBounds().contains(mPos.x, mPos.y) && i->id == "reset")
			{
				clrScreen = true;
			}
		}
		m_win->display();
	}

	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			if (m_startNode != nullptr && m_endNode != nullptr)
			{
				sf::Vector2i mPos = sf::Mouse::getPosition(*m_win);
				if (nodes[i][j]->rect.getGlobalBounds().contains(mPos.x, mPos.y) && nodes[i][j]->isWalkable && !m_startNode->grabbed && !m_endNode->grabbed && !nodes[i][j]->isWall)
				{
					nodes[i][j]->isWalkable = false;
					nodes[i][j]->isWall = true;
					nodes[i][j]->rect.setFillColor(sf::Color::Black);
				}
				else if (nodes[i][j]->rect.getGlobalBounds().contains(mPos.x, mPos.y) && !nodes[i][j]->isWalkable && nodes[i][j]->isWall)
				{
					nodes[i][j]->isWalkable = true;
					nodes[i][j]->isWall = false;
					nodes[i][j]->rect.setFillColor(sf::Color(BGNODECOL));
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

void Grid::clear()
{
	m_win->clear(sf::Color::Black);
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			if (nodes[i][j]->isWalkable && !nodes[i][j]->isWall)
			{
				nodes[i][j]->rect.setFillColor(sf::Color(BGNODECOL));
				m_win->draw(nodes[i][j]->rect);
			}
		}
	}

	nonTestedNodes.clear();
	m_checkedNodes.clear();
	m_path.clear();
	m_win->display();
	found = false;
	m_win->clear(sf::Color::Black);
	m_win->draw(m_endNode->rect);

}

void Grid::setindicator(int index)
{
	m_buttons[index]->indicator.setFillColor(sf::Color::Green);
}

void Grid::generateRandoCommandoMaze()
{
	for (int i = 0; i < ROWS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			int chance = rand() % 100;
			if (chance < 30 && nodes[i][j] != m_startNode && nodes[i][j] != m_endNode)
			{
				nodes[i][j]->isWalkable = false;
				nodes[i][j]->isWall = true;
				nodes[i][j]->rect.setFillColor(sf::Color::Black);
			}
		}
	}
}

void Grid::tracePath()
{
	for (int i = 0; i < m_path.size(); i++)
	{
		tracedLines.emplace_back(sf::Vector2f(m_path[i]->rect.getPosition().x + bSIZE / 2, m_path[i]->rect.getPosition().y + bSIZE / 2), LINECOL);
	}
}
