#include "grid_cell.hpp"

// Constructor
GridCell::GridCell(const int x, const int y) : _x(x), _y(y)
{
}

// Check if two cells are identical
bool GridCell::operator==(const GridCell &other) const
{
    return _x == other._x && _y == other._y;
}

// Get x
int GridCell::x() const
{
    return _x;
}

// Get y
int GridCell::y() const
{
    return _y;
}