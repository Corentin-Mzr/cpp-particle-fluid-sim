#pragma once

#include <SFML/Graphics.hpp>

class Object {

public:
    // Retrieve particle position
    sf::Vector2f get_position() const;

protected:
    // Protected constructor
    Object(const sf::Vector2f &position);
    virtual ~Object() = default;

    sf::Vector2f position;

};