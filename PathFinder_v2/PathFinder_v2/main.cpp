#include <SFML/Graphics.hpp>
#include "Grid.h"

bool startAlgorithm = false;

int main()
{
    sf::RenderWindow window(sf::VideoMode(850, 755), "SFML works!");
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

            if (event.type == sf::Event::KeyPressed)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
                {
                    grid.reset();
                    grid = std::make_unique<Grid>(&window);
                    startAlgorithm = false;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                {
                    startAlgorithm = true;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
                {
                    grid->generateRandoCommandoMaze();
                }
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                grid->mouseContains();
            }
        }

        if (!grid->getFound() && startAlgorithm)
        {
            grid->A_Star_Algorithm();
        }
        grid->Display();
        window.display();
        window.clear();
    }

    return 0;
}