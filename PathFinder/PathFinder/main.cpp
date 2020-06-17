/*
	File: main.cpp
	Class Description: main file that is the driver code for the classes
	author: Anthony Kuregian
	email: AnthonyKuregian@yahoo.com
*/
#include <SFML/Graphics.hpp>
#include <SFML/Graphics.hpp>
#include "Grid.h"


int main()
{
	sf::Clock clock;
    sf::RenderWindow window(sf::VideoMode(1000, 755), "SFML works!");
    std::unique_ptr<Grid> grid = std::make_unique<Grid>(&window);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                grid->mouseContains();
            }
        }

        if (grid->clrScreen)
        {
           grid.reset();
           grid = std::make_unique<Grid>(&window);
           continue;
        }

        if (!grid->getFound() && grid->startAlgo)
        {
            if (grid->aStar)
            {
                std::cout << "A_Star_Algorithm" << std::endl;
                if (grid->A_Star_Algorithm(10.0f))
                {
		            grid->Display();
                }
            }
            else if (grid->dijkstra)
            {
                std::cout << "Dijkstras_Algorithm" << std::endl;
                if (grid->Dijkstras_Algorithm(10.0f))
                {
                    grid->Display();
                }
            }
            else if (grid->breadth)
            {
                std::cout << "Breadth_Algorithm" << std::endl;
                if (grid->Breadth_Algorithm(10.0f))
                {
                    grid->Display();
                }
            }
            else if (grid->greedy)
            {
                std::cout << "Greedy_Algorithm" << std::endl;
                if (grid->Greedy_Algorithm(10.0f))
                {
                    grid->Display();
                }
            }
            else
            {
                std::cout << "No Algorithm Selected.. Ive Chosen AStar for you." << std::endl;
                grid->aStar = true;
            }
        }
		window.clear();
		grid->Display();
		window.display();
    }

    return 0;
}