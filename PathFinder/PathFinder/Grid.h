#pragma once
#ifndef GRID_H
#define GRID_H
#include<iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include<SFML/Graphics.hpp>
#include <deque>
#include <list>


const int ROWS = 25;
const int COLS = 25;
const int bSIZE = 25;
const int BLOCK_DISTANCE = 30;

struct Button
{
	std::string id;
	void setPos(int posx, int posy){button.setPosition(posx, posy);}
	void buttonDisplay() {
		button_win->draw(button); button_win->draw(bName);
		if (createIndicator)
		{
			button_win->draw(indicator);
		}
	}

	int createIndicator = 0;
	sf::RenderWindow* button_win = nullptr;
	sf::Text bName;
	sf::Font m_font;
	sf::RectangleShape button;
	sf::RectangleShape indicator;

	Button(const std::string name, sf::Color col, int w, int h, sf::RenderWindow* win, int px, int py, const int &p)
		: button_win(win), createIndicator(p)
	{
		if (!m_font.loadFromFile("Font/OpenSans-Bold.ttf"))
		{
			try
			{
				throw 22;
			}
			catch (const int e)
			{
				std::cout << "Error Code [" << e << "] <-- Couldnt Find/Load in Font File" << std::endl;
			}
		}
		button.setFillColor(col);
		button.setSize(sf::Vector2f(w, h));
		button.setPosition(px, py);
		indicator.setFillColor(sf::Color::Black);
		indicator.setSize(sf::Vector2f(10, 25));
		indicator.setPosition(button.getPosition().x + w - 15, button.getPosition().y + h / 2 - 12);
		bName.setFont(m_font);
		bName.setString(name);
		bName.setCharacterSize(40);
		bName.setPosition(px, py);

	}

	void displayIndicator()
	{
		if (createIndicator) { button_win->draw(indicator); }
	}
	void setText(std::string buttonName)
	{
		bName.setString(buttonName);
	}
	void tweakTextPosition(int x, int y)
	{
		bName.setPosition(bName.getPosition().x + x, bName.getPosition().y + y);
	}
};

struct Node
{
	Node() {}
	bool isWalkable = true;
	bool isWall = false;
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
	bool A_Star_Algorithm(const float&);
	bool Dijkstras_Algorithm(const float&);
	bool Breadth_Algorithm(const float&);
	bool Greedy_Algorithm(const float&);
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
	void tracePath();
	void setindicator(int);
	void AnimationNodes(float);
	void CheckButtonPress();
	void tick() { std::this_thread::sleep_for(std::chrono::milliseconds(500)); };
	void clear();

	bool startAlgo = false;
	bool clrScreen = false;
	bool aStar = false;
	bool dijkstra = false;
	bool breadth = false;
	bool greedy = false;
	bool complete = false;

private:
	bool found = false;
	bool trace = false;
	bool pathNotFound = false;
	Node* nodes[ROWS][COLS];
	Node* m_startNode = nullptr;
	Node* m_endNode = nullptr;
	std::list<Node*> nonTestedNodes;
	std::deque<Node*> m_path;
	std::deque<Node*> m_checkedNodes;
	std::deque<Button*> m_buttons;
	std::vector<sf::Vertex> tracedLines;
	sf::RectangleShape mouseNode;
	sf::RenderWindow* m_win;
	sf::Clock m_clock;
};

#endif
