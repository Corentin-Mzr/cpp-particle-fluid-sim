#pragma once

// Represents a grid cell
class GridCell
{
public:
    // Constructor
    GridCell(const int x, const int y);

    // Check if two cells are identical
    bool operator==(const GridCell &other) const;

    // Get x
    int x() const;

    // Get y
    int y() const;

private:
    int _x, _y;
};