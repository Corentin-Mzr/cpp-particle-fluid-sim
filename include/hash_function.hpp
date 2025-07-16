#pragma once

#include <vector>

#include "grid_cell.hpp"

// Hash function for GridCell
class HashFunction
{
public:
    // Simple hash function
    std::size_t operator()(const GridCell &cell) const;
};