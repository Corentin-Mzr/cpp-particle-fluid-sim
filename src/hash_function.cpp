#include "hash_function.hpp"

// Simple hash function
std::size_t HashFunction::operator()(const GridCell &cell) const 
{
    return std::hash<int>()(cell.x()) ^ std::hash<int>()(cell.y());
}