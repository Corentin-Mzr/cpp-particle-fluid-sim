#pragma once

#include <SFML/Graphics.hpp>

#include <iostream>
#include <cmath>

#include "object.hpp"
#include "config.hpp"

// Particle class as data, no draw (but contains graphic parameters)
class Particle : public Object
{
public:
    // Graphic constructor
    Particle(const float radius,
             const sf::Color &color,
             const sf::Vector2f &position);

    // Physic constructor
    Particle(const float mass,
             const sf::Vector2f &position,
             const sf::Vector2f &velocity,
             const sf::Vector2f &acceleration);

    // All constructor
    Particle(const float radius,
             const sf::Color &color,
             const float mass,
             const sf::Vector2f &position,
             const sf::Vector2f &velocity,
             const sf::Vector2f &acceleration);

    // Retrieve particle radius
    float get_radius() const;

    // Retrieve particle color
    sf::Color get_color() const;

    // Retrieve particle mass
    float get_mass() const;

    // Update particle position, velocity, acceleration
    void update(const float dt);

    // Apply a force to the particle
    void apply_force(const sf::Vector2f &force);

    // Reset particle acceleration
    void reset_acceleration();

    // Check collision with another particle
    bool is_colliding(const Particle &other) const;

    // Solve collision with another particle
    void solve_collision(Particle &other);

    // Handle boundaries
    void handle_boundaries(const float xmin, const float xmax, const float ymin, const float ymax);

private:
    // Graphic params
    float radius;
    sf::Color color;

    // Physic params
    float mass;
    sf::Vector2f position_old;
    sf::Vector2f acceleration;

    // Change color based on speed
    void change_color();

    // Get current velocity
    sf::Vector2f get_velocity() const;

    // Length of a vector
    static float length(const sf::Vector2f &vector) {
        return std::sqrt(vector.x * vector.x + vector.y * vector.y);
    }
};
