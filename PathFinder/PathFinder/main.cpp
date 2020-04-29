/*
	File: main.cpp
	Class Description: main file that is the driver code for the classes
	author: Anthony Kuregian
	email: AnthonyKuregian@yahoo.com
*/

#include <SFML/Graphics.hpp>
#include<iostream>
#include "Grid.h"

int main()
{
	unsigned int click = 0;
	bool startNodeSelected = false;
	bool endNodeSelected = false;
	sf::RenderWindow window(sf::VideoMode(1100, 1000), "SFML Pathdfinding!"); //1100, 755
	std::unique_ptr<Grid> grid = std::make_unique<Grid>(ROWS_COLS_SIZE, ROWS_COLS_SIZE, BLOCKSIZE, &window); // smart pointer

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			if (sf::Event::MouseButtonPressed)
			{
				bool one = false;
				if (click == 0 && sf::Mouse::isButtonPressed(sf::Mouse::Left) && !startNodeSelected)
				{
					startNodeSelected = true;
					grid->add_start_node();
					click++;
					one = true;
				}
				if (click == 1 && sf::Mouse::isButtonPressed(sf::Mouse::Left) && !endNodeSelected && !one)
				{
					endNodeSelected = true;
					grid->add_end_node();
					click++;
				}
				grid->clickCheck();
			}
			if (event.type ==  sf::Event::KeyPressed)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
				{
					std::cout << "Suppose to Reset the Game when this Key is Pressed" << std::endl;
				}
			}
		}

		grid->display();
		grid->aStarAlgorithm();
		window.display();
	}

	return 0;
}



