#include "simulation.hpp"

// Constructor
Simulation::Simulation(const std::vector<std::shared_ptr<Particle>> &particles,
                       const Box &world_box,
                       const float dt,
                       const unsigned nb_substep) : particles(particles), world_box(world_box), qt(world_box), dt(dt), nb_substep(nb_substep) 
{
}

// Retrieve particles to draw them
const std::vector<std::shared_ptr<Particle>>& Simulation::get_particles() const
{
    return particles;
}

// Get current QuadTree
const QuadTree<Particle> &Simulation::get_quadtree() const
{
    return qt;
}
