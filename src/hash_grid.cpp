#include "hash_grid.hpp"

// Constructor
HashGrid::HashGrid(const float cell_size) : cell_size(cell_size)
{
}

// Convert position to a grid cell coord
GridCell HashGrid::get_cell(const sf::Vector2f &pos) const
{
    const int x = static_cast<int>(std::floor(pos.x / cell_size));
    const int y = static_cast<int>(std::floor(pos.y / cell_size));
    return GridCell(x, y);
}

// Insert multiple objects in the grid
void HashGrid::batch_insert(const std::vector<std::shared_ptr<Object>> &obj_list)
{
    for (const auto &obj : obj_list)
        insert(obj);
}

// Insert an object in the grid
void HashGrid::insert(const std::shared_ptr<Object> &obj)
{
    // Check if object is not already stored in the hashgrid (should be in obj_to_cell)
    if (obj_to_cell.find(obj) != obj_to_cell.end())
        return;

    // Store object
    const GridCell cell = get_cell(obj->get_position());
    grid[cell].push_back(obj);

    // Link
    obj_to_cell[obj] = std::make_shared<GridCell>(cell);
    // const size_t i = grid[cell].size() - 1;
    // index[obj].first = i;
    // index[obj].second = std::make_shared<GridCell>(cell);
}

// Remove an object from the grid
void HashGrid::remove(const std::shared_ptr<Object> &obj)
{
    // Idea : try to find object, swap it to make it last item, then remove it

    // Check if object is stored in the hashgrid
    if (obj_to_cell.find(obj) == obj_to_cell.end())
        return;

    // Check if cell exist
    // const GridCell cell = get_cell(obj->get_position());
    // if (grid.find(cell) == grid.end())
    //     return;
    
    // If object is in the hashgrid, we know in which cell it is
    const auto cell = *obj_to_cell[obj];

    // Find its position
    std::vector<std::shared_ptr<Object>> &grid_vals = grid[cell];
    const size_t n = grid_vals.size();
    // const size_t i = it2->second.first; // index[obj].first;

    size_t i = 0;
    for (auto &e : grid_vals)
    {
        if (e == obj)
            break;
        i++;
    }

    // Invalid index
    if (i >= n)
        return;

    // Swap if not last element
    if (i != n - 1)
    {
        std::swap(grid_vals[i], grid_vals[n - 1]);
        // index[grid_vals[i]].first = i;
    }

    // Remove last element
    grid_vals.pop_back();

    // Clean up if empty
    if (grid_vals.empty())
        grid.erase(cell);

    // Remove link
    // index.erase(obj);
    obj_to_cell.erase(obj);
}

// Update an object from the grid
// TODO: CHANGE THINGS HERE (particles dont collide)
void HashGrid::update(const std::shared_ptr<Object> &obj)
{
    // Check if object is in the grid
    if (obj_to_cell.find(obj) == obj_to_cell.end())
        return;

    // Retrieve previous cell associated to the object
    auto &old_grid_cell = obj_to_cell[obj];

    // Check if update is needed, i.e. check if grid cell is different than before
    const GridCell new_grid_cell = get_cell(obj->get_position());

    if (new_grid_cell != *old_grid_cell)
    {
        // Remove the object and put it back in the grid
        remove(obj);
        insert(obj);
    }
}

// Find all objects in the cells or neighbor cells
std::vector<std::shared_ptr<Object>> HashGrid::query(const sf::Vector2f &pos) const
{
    std::vector<std::shared_ptr<Object>> output;
    const GridCell cell = get_cell(pos);

    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            GridCell neighbor(cell.x() + dx, cell.y() + dy);
            auto it = grid.find(neighbor);
            if (it != grid.end())
            {
                const std::vector<std::shared_ptr<Object>> &grid_vals = it->second;
                output.insert(output.end(), grid_vals.begin(), grid_vals.end());
            }
        }
    }

    return output;
}

// Clear grid
void HashGrid::clear()
{
    grid.clear();
    // index.clear();
}

// Get number of elements stored in grid
size_t HashGrid::count() const
{
    size_t total = 0;
    for (const auto &p : grid)
        total += p.second.size();
    
    return total;
}