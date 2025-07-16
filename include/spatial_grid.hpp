#pragma once

#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "box.hpp"
#include "object.hpp"

// Divide space into cells of same size
// For bounded environment
class SpatialGrid
{

public:
    // Constructor
    SpatialGrid(const sf::Vector2f center, const float width, const float height, const unsigned num_cells_x, const unsigned num_cells_y);

    // Insert an object in the grid
    void insert(const std::shared_ptr<Object> &obj);

    // Remove an object from the grid
    void remove(const std::shared_ptr<Object> &obj);

    // Clear the grid
    void clear();

    // Find objects near given object
    std::vector<std::shared_ptr<Object>> query(const std::shared_ptr<Object> &obj) const;

private:
    sf::Vector2f center;
    float width, height;
    unsigned num_x, num_y;
    
    sf::Vector2f cell_size;

    std::vector<std::vector<std::shared_ptr<Object>>> grid;
};
