// #pragma once

#include "simulation.hpp"

// Extended class of Simulation to do a fluid simulation
class SimulationFluid : public Simulation
{

public:
    // Constructor
    SimulationFluid(const std::vector<std::shared_ptr<Particle>> &particles, const Box &world_box, const float dt, const unsigned nb_substep);

    // Update the simulation
    virtual void update();

private:
    // Apply pressure force to the given particles
    static void apply_pressure(std::shared_ptr<Particle> &p, std::shared_ptr<Particle> &neighbor, const float dt);

    // Apply viscosity force to the given particles
    static void apply_viscosity(std::shared_ptr<Particle> &p, std::shared_ptr<Particle> &neighbor, const float dt);

    // Apply cohesion force to the given particles
    static void apply_cohesion(std::shared_ptr<Particle> &p, std::shared_ptr<Particle> &neighbor, const float dt);
};