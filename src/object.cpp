#include "object.hpp"

// Protected constructor
Object::Object(const sf::Vector2f &position) : position(position)
{
}

// Retrieve particle position
sf::Vector2f Object::get_position() const
{
    return position;
}