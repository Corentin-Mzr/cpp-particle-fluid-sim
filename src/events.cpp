#include "events.hpp"

#include <iostream>

void handle_events(sf::RenderWindow &window, sf::Clock &clock)
{
    sf::Event event;

    // Get the current view
    sf::View view = window.getView();

    while (window.pollEvent(event))
    {
        if (quit_events(event))
            window.close();

        if (zoom_events(event, view))
            window.setView(view);
    }

    const float dt = clock.restart().asSeconds();

    // Handle keyboard movement
    if (movement_events(view, conf::SENSITIVITY, dt))
        window.setView(view);
}

// Handle quit events that exit the program
bool quit_events(const sf::Event &event)
{
    // Quit window when closed
    if (event.type == sf::Event::Closed)
        return true;

    // Quit window when pressing escape
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
        return true;

    return false;
}

// Handle control events that change zoom view
bool zoom_events(const sf::Event &event, sf::View &view)
{

    // Handle mouse wheel scroll to zoom the view
    if (event.type == sf::Event::MouseWheelScrolled)
    {
        // Zoom factor adjustment (zoom in or out)
        float zoom_factor = 1.1f; // Control zoom sensitivity

        if (event.mouseWheelScroll.delta > 0)
        {
            // Scroll up -> zoom in
            view.zoom(1.0f / zoom_factor);
        }
        else if (event.mouseWheelScroll.delta < 0)
        {
            // Scroll down -> zoom out
            view.zoom(zoom_factor);
        }

        return true;
    }

    return false;
}

// Handle control events that change view position
bool movement_events(sf::View &view, const float sensitivity, const float dt)
{
    // Movement for camera
    sf::Vector2f movement(0.0f, 0.0f);

    // Up
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
        movement.y -= sensitivity;
    // Left
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::K))
        movement.x -= sensitivity;
    // Down
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
        movement.y += sensitivity;
    // Right
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
        movement.x += sensitivity;

    // Normalize the movement vector to maintain consistent speed
    if (movement.x != 0 || movement.y != 0)
    {
        float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
        movement /= length;
        movement *= sensitivity;
        view.move(movement * dt);

        return true;
    }

    return false;
}