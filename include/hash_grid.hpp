#pragma once

#include <unordered_map>
#include <memory>
#include <math.h>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "grid_cell.hpp"
#include "hash_function.hpp"
#include "object.hpp"

// Hash grid class
class HashGrid
{
public:
    // Constructor
    HashGrid(const float cell_size);

    // Insert multiple objects in the grid
    void batch_insert(const std::vector<std::shared_ptr<Object>> &obj_list);

    // Insert an object in the grid
    void insert(const std::shared_ptr<Object> &obj);

    // Remove an object from the grid
    void remove(const std::shared_ptr<Object> &obj);

    // Update an object from the grid
    void update(const std::shared_ptr<Object> &obj);

    // Find all objects in the cells or neighbor cells
    std::vector<std::shared_ptr<Object>> query(const sf::Vector2f &pos) const;

    // Clear grid
    void clear();

    // Get number of elements stored in grid
    size_t count() const;

private:
    // Spacing between cells
    float cell_size;

    // Grid
    std::unordered_map<GridCell, std::vector<std::shared_ptr<Object>>, HashFunction> grid;

    // Index where the object is stored in the vector
    // std::unordered_map<std::shared_ptr<Object>, std::pair<unsigned, std::shared_ptr<GridCell>>> index;

    // Store where each object is in the grid
    std::unordered_map<std::shared_ptr<Object>, std::shared_ptr<GridCell>> obj_to_cell;

    // Convert position to a grid cell coord
    GridCell get_cell(const sf::Vector2f &pos) const;
};

