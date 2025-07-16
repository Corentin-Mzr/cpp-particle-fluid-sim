#pragma once

// Define a concept for get_position()
// C++20 REQUIRED
// template<typename T>
// concept HasGetPosition = requires(T t) {
//     { t.get_position() } -> std::convertible_to<sf::Vector2f>;
// };

#include <SFML/Graphics.hpp>

#include <iostream>

// Overload the << operator for sf::Vector2f
std::ostream& operator<<(std::ostream& os, const sf::Vector2f& vec) {
    os << "Vector2f(" << vec.x << ", " << vec.y << ")";
    return os;
}