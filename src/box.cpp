#include "box.hpp"

// Constructor
Box::Box(const sf::Vector2f &center, const sf::Vector2f &half_dimension) : center(center), half_dimension(half_dimension)
{
    // Draw Box
    vertices.setPrimitiveType(sf::LineStrip);
    vertices.resize(5);

    const sf::Vector2f v1 = center + sf::Vector2f{-half_dimension.x, half_dimension.y};
    const sf::Vector2f v2 = center + half_dimension;
    const sf::Vector2f v3 = center + sf::Vector2f{half_dimension.x, -half_dimension.y};
    const sf::Vector2f v4 = center - half_dimension;

    vertices[0].position = v1;
    vertices[1].position = v2;
    vertices[2].position = v3;
    vertices[3].position = v4;
    vertices[4].position = v1;    

    for (size_t i = 0; i < 5; ++i)
        vertices[i].color = sf::Color::Green;

    // Define boundaries
    boundary.xmin = center.x - half_dimension.x;
    boundary.xmax = center.x + half_dimension.x;
    boundary.ymin = center.y - half_dimension.y;
    boundary.ymax = center.y + half_dimension.y;
}

// Check if the box contains a particle
bool Box::contains(const Object &p) const
{
    const sf::Vector2f &pos = p.get_position();

    const float xmin = center.x - half_dimension.x;
    const float xmax = center.x + half_dimension.x;
    const float ymin = center.y - half_dimension.y;
    const float ymax = center.y + half_dimension.y;

    return xmin <= pos.x && pos.x < xmax && ymin <= pos.y && pos.y < ymax;
}

// Check if two boxes intersect
bool Box::intersect(const Box &other) const
{
    const float xmin = center.x - half_dimension.x;
    const float xmax = center.x + half_dimension.x;
    const float ymin = center.y - half_dimension.y;
    const float ymax = center.y + half_dimension.y;

    const sf::Vector2f other_min = other.get_center() - other.get_half_dimension();
    const sf::Vector2f other_max = other.get_center() + other.get_half_dimension();

    // Check if a box is on the left of the other
    if (xmax < other_min.x || other_max.x < xmin)
        return false;
    
    // Check if a box is above the other
    if (ymax < other_min.y || other_max.y < ymin)
        return false;
    
    // Conditions are not met, boxes intersect
    return true;
}

// Retrieve center
sf::Vector2f Box::get_center() const
{
    return center;
}

// Retrieve half dimension
sf::Vector2f Box::get_half_dimension() const
{
    return half_dimension;
}

// Get boundaries
Boundary Box::get_boundary() const
{
    return boundary;
}