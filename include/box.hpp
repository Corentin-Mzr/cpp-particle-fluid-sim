#pragma once

#include <SFML/Graphics.hpp>

#include "particle.hpp"
#include "object.hpp"

struct Boundary
{
    float xmin, xmax, ymin, ymax;
};

// Box class for QuadTree
class Box : public sf::Drawable
{
public:
    // Constructor
    Box(const sf::Vector2f &center, const sf::Vector2f &half_dimension);

    // Check if the box contains an object
    bool contains(const Object &p) const;

    // Check if two boxes intersect
    bool intersect(const Box &other) const;

    // Retrieve center
    sf::Vector2f get_center() const;

    // Retrieve half dimension
    sf::Vector2f get_half_dimension() const;

    // Get boundaries
    Boundary get_boundary() const;

private:
    // Box params
    sf::Vector2f center;
    sf::Vector2f half_dimension;
    Boundary boundary;

    // Graphic params
    sf::VertexArray vertices;

    // Draw box
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        target.draw(vertices, states);
    }
};

