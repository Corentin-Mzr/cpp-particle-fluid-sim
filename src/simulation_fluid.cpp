#include "simulation_fluid.hpp"

// Constructor
SimulationFluid::SimulationFluid(const std::vector<std::shared_ptr<Particle>> &particles,
                                 const Box &world_box,
                                 const float dt,
                                 const unsigned nb_substep) : Simulation(particles, world_box, dt, nb_substep)
{
}

// Update the simulation
void SimulationFluid::update()
{
    // QuadTree for world
    qt = QuadTree<Particle>(world_box);
    qt.batch_insert(particles);

    // Boundaries
    const Boundary boundary = world_box.get_boundary();
    const float xmin = boundary.xmin;
    const float xmax = boundary.xmax;
    const float ymin = boundary.ymin;
    const float ymax = boundary.ymax;

// Parallelize particle updates
#pragma omp parallel for
    for (size_t i = 0; i < particles.size(); ++i)
    {
        // Each thread gets its own particle reference
        auto &p = particles[i];

        // Handle boundaries
        p->handle_boundaries(xmin, xmax, ymin, ymax);

        // Apply gravity
        //p->apply_force({0.0f, 10.0f});

        // Add repulsion between particles (querying nearby particles in the quadtree)
        //         const Box repulsive_box{p->get_position(), {20.0f * p->get_radius(), 20.0f * p->get_radius()}};
        //         auto repulsive_particles = qt.query(repulsive_box);

        //         // Iterate through repulsive particles
        //         for (size_t j = 0; j < repulsive_particles.size(); ++j)
        //         {
        //             auto &other = repulsive_particles[j];
        //             if (p != other)
        //             {
        // // Minimize critical sections (only where forces are applied)
        // //#pragma omp critical
        //                 {
        //                     // const float dist_to_other =
        //                     //     (other->get_position().x - p->get_position().x) * (other->get_position().x - p->get_position().x) +
        //                     //     (other->get_position().y - p->get_position().y) * (other->get_position().y - p->get_position().y);

        //                     // sf::Vector2f axis = (other->get_position() - p->get_position());
        //                     // // if (dist_to_other != 0)
        //                     // //     axis /= dist_to_other;
        //                     // // p->apply_force(-axis);
        //                     // // other->apply_force(axis);

        //                     // // Test to add viscosity
        //                     // const sf::Vector2f vel = p->get_position() * dt;
        //                     // const sf::Vector2f viscosity = 0.1f * sf::Vector2f{-axis.x * vel.x * vel.x, -axis.y * vel.y * vel.y};
        //                     // p->apply_force(viscosity);

        //                     // const sf::Vector2f other_vel = other->get_position() * dt;
        //                     // const sf::Vector2f other_viscosity = 0.1f * sf::Vector2f{axis.x * other_vel.x * other_vel.x, axis.y * other_vel.x * other_vel.y};
        //                     // other->apply_force(other_viscosity);
        //                 }
        //             }
        //         }

        // Handle every forces around the particle
        const Box query_box{p->get_position(), {4.0f * p->get_radius(), 4.0f * p->get_radius()}};
        auto neighbors = qt.query(query_box);

        for (size_t j = 0; j < neighbors.size(); ++j)
        {
            auto &neighbor = neighbors[j];
            if (p != neighbor)
            {
#pragma omp critical
                {
                    // Apply pressure
                    apply_pressure(p, neighbor, dt);

                    // Apply viscosity
                    //apply_viscosity(p, neighbor, dt);

                    // Apply cohesion force
                    //apply_cohesion(p, neighbor, dt);

                    // Handle collision with other particles (using quadtree for nearby particles)
                    if (p->is_colliding(*neighbor))
                    {
                        p->solve_collision(*neighbor);
                    }
                }
            }
        }

        // Update position, velocity, acceleration
        p->update(dt);
    }
}

// Apply pressure force to the given particles
void SimulationFluid::apply_pressure(std::shared_ptr<Particle> &p, std::shared_ptr<Particle> &neighbor, [[maybe_unused]]const float dt)
{
    const sf::Vector2f pos1 = p->get_position();
    const sf::Vector2f pos2 = neighbor->get_position();
    // std::cout << pos1.x << "," << pos1.y << " / " << pos2.x << "," << pos2.y << std::endl;
    const float dist = std::sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y));

    // Direction of the force
    sf::Vector2f normal = (pos1 - pos2);
    if (dist != 0)
        normal /= (dist * dist);

    // Apply force
    // const float threshold_dist = 10.0f;
    // if (dist < threshold_dist)
    {
        const float pressure_coef = 1.0f;
        const sf::Vector2f pressure_force = pressure_coef * normal;
        p->apply_force(pressure_force);
        neighbor->apply_force(-pressure_force);
    }
}

// Apply viscosity force to the given particles
void SimulationFluid::apply_viscosity(std::shared_ptr<Particle> &p, std::shared_ptr<Particle> &neighbor, const float dt)
{
    const sf::Vector2f vel1 = p->get_position() * dt;
    const sf::Vector2f vel2 = p->get_position() * dt;
    const sf::Vector2f relative_vel = vel2 - vel1;

    // Apply force
    const float viscosity_coef = 1.0f;
    const sf::Vector2f viscosity_force = viscosity_coef * sf::Vector2f{relative_vel.x * relative_vel.x, relative_vel.y * relative_vel.y};
    p->apply_force(viscosity_force);
    neighbor->apply_force(-viscosity_force);
}

// Apply cohesion force to the given particles
void SimulationFluid::apply_cohesion(std::shared_ptr<Particle> &p, std::shared_ptr<Particle> &neighbor, [[maybe_unused]]const float dt)
{
    const sf::Vector2f pos1 = p->get_position();
    const sf::Vector2f pos2 = neighbor->get_position();
    const float dist = std::sqrt((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y));

    // Direction of the force
    sf::Vector2f normal = (pos1 - pos2);
    if (dist != 0)
        normal /= dist;

    // Apply force
    const float min_dist = 0.0f;
    const float max_dist = 4.0f;
    const float cohesion_coef = 1.0f;
    if (dist > min_dist && dist < max_dist)
    {
        sf::Vector2f cohesion_force = cohesion_coef * normal;
        p->apply_force(cohesion_force);
        neighbor->apply_force(-cohesion_force);
    }
}