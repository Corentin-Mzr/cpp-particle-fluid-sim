#pragma once

#include <random>
#include <iostream>
#include <omp.h>
#include <memory>

#include "particle.hpp"
#include "config.hpp"
#include "box.hpp"
#include "quadtree.hpp"

// Abstract class containing the world to simulate (particles and how to update them)
class Simulation {

public:
    // Constructor
    Simulation(const std::vector<std::shared_ptr<Particle>> &particles, const Box &world_box, const float dt, const unsigned nb_substep);

    // Define here how to update the simulation
    virtual void update() = 0;

    // Retrieve particles to draw them
    const std::vector<std::shared_ptr<Particle>>& get_particles() const;

    // Get current QuadTree
    const QuadTree<Particle>& get_quadtree() const;

protected:
    // Particles of the simulation
    std::vector<std::shared_ptr<Particle>> particles;

    // World border
    Box world_box;

    // QuadTree, will be updated every time the update function is called (just so we can draw it)
    QuadTree<Particle> qt;

    // Delta time and substeps for more accurate result
    float dt;
    unsigned nb_substep;
};