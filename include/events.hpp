#pragma once

#include <cmath>

#include <SFML/Graphics.hpp>

#include "config.hpp"

// Handle all events
void handle_events(sf::RenderWindow& window, sf::Clock &clock);

// Handle quit events that exit the program
bool quit_events(const sf::Event &event);

// Handle control events that change view zoom
bool zoom_events(const sf::Event &event, sf::View &view);

// Handle control events that change view position
bool movement_events(sf::View &view, const float sensitivity, const float dt);
