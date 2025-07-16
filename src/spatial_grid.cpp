#include "spatial_grid.hpp"

// Constructor
SpatialGrid::SpatialGrid(const sf::Vector2f center,
                         const float width,
                         const float height,
                         const unsigned num_cells_x,
                         const unsigned num_cells_y) :center(center), width(width), height(height), num_x(num_cells_x), num_y(num_cells_y)
{
    cell_size = {width / static_cast<float>(num_x), height / static_cast<float>(num_y)};
    grid.reserve(num_x * num_y);
}

// Insert an object in the grid
void SpatialGrid::insert([[maybe_unused]]const std::shared_ptr<Object> &obj)
{
}

// Remove an object from the grid
void SpatialGrid::remove([[maybe_unused]]const std::shared_ptr<Object> &obj)
{
}

// Clear the grid
void SpatialGrid::clear()
{
    for(auto &e: grid)
        e.clear();
}

// Find objects near given object
std::vector<std::shared_ptr<Object>> SpatialGrid::query([[maybe_unused]]const std::shared_ptr<Object> &obj) const
{
    std::vector<std::shared_ptr<Object>> result;

    return result;
}