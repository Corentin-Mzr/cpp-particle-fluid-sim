#include "particle.hpp"

// Graphic constructor
Particle::Particle(const float radius,
                   const sf::Color &color,
                   const sf::Vector2f &position) : Object(position), radius(radius), color(color), mass(1.0f), position_old(position), acceleration({0.0f, 0.0f})
{
}

// Physic constructor
Particle::Particle(const float mass,
                   const sf::Vector2f &position,
                   const sf::Vector2f &velocity,
                   const sf::Vector2f &acceleration) : Object(position), radius(1.0f), color(sf::Color::White), mass(mass), position_old(position - velocity), acceleration(acceleration)
{
}

// All constructor
Particle::Particle(const float radius,
                   const sf::Color &color,
                   const float mass,
                   const sf::Vector2f &position,
                   const sf::Vector2f &velocity,
                   const sf::Vector2f &acceleration) : Object(position), radius(radius), color(color), mass(mass), position_old(position - velocity), acceleration(acceleration)
{
}

// Retrieve particle radius
float Particle::get_radius() const
{
    return radius;
}

// Retrieve particle color
sf::Color Particle::get_color() const
{
    return color;
}

// Retrieve particle mass
float Particle::get_mass() const
{
    return mass;
}

// Update particle position, velocity, acceleration
void Particle::update(const float dt)
{
    const sf::Vector2f velocity = get_velocity();

    // Save current position
    position_old = position;

    // Update position
    position += velocity + acceleration * dt * dt;
    reset_acceleration();

    // Change color based on speed
    change_color();
}

// Apply a force to the particle
void Particle::apply_force(const sf::Vector2f &force)
{
    acceleration += force;
}

// Reset particle acceleration
void Particle::reset_acceleration()
{
    acceleration = {0.f, 0.f};
}

// Check collision with another particle
bool Particle::is_colliding(const Particle &other) const
{
    // Compute distance to the other particle
    const sf::Vector2f other_position = other.get_position();
    const float distance = length(position - other_position);
    const float threshold = radius + other.get_radius();

    return distance <= threshold;
}

// Solve collision with another particle
void Particle::solve_collision(Particle &other)
{
    const sf::Vector2f collision_axis = position - other.position;
    const float dist = length(collision_axis);

    // Calculate overlap
    const float overlap = (radius + other.get_radius()) - dist;

    if (overlap > 0)
    {
        // Normalize the distance vector
        const sf::Vector2f normal = collision_axis / dist;

        // Mass ratio
        const float total_mass = mass + other.get_mass();
        const float ratio_other = other.get_mass() / total_mass;
        const float ratio_current = mass / total_mass;

        // Move particles apart
        position += normal * (overlap * ratio_other * conf::PARTICLE_DAMPING);
        other.position -= normal * (overlap * ratio_current * conf::PARTICLE_DAMPING);

        // Simple elastic collision response
        // const sf::Vector2f temp_velocity = velocity;
        // velocity = other.velocity * conf::PARTICLE_DAMPING;
        // other.velocity = temp_velocity * conf::PARTICLE_DAMPING;
    }
}

void Particle::handle_boundaries(const float xmin, const float xmax, const float ymin, const float ymax)
{
    // Handle boundary collisions
    const sf::Vector2f temp = position;

    // Left boundary
    if (position.x - radius < xmin)
    {
        position.x = radius + xmin;

        const sf::Vector2f vel = get_velocity();
        position_old.x = temp.x + vel.x * conf::WALL_DAMPING;
    }

    // Right boundary
    else if (position.x + radius > xmax)
    {
        position.x = xmax - radius;

        const sf::Vector2f vel = get_velocity();
        position_old.x = temp.x + vel.x * conf::WALL_DAMPING;
    }

    // Top boundary
    if (position.y - radius < ymin)
    {
        position.y = radius + ymin;

        const sf::Vector2f vel = get_velocity();
        position_old.y = temp.y + vel.y * conf::WALL_DAMPING;
    }

    // Bottom boundary
    else if (position.y + radius > ymax)
    {
        position.y = ymax - radius;

        const sf::Vector2f vel = get_velocity();
        position_old.y = temp.y + vel.y * conf::WALL_DAMPING;
    }
}

void Particle::change_color()
{
    const float speed = length(get_velocity());

    float normalized_speed = speed;
    normalized_speed = std::clamp(normalized_speed, 0.0f, 1.0f); // Clamp between 0 and 1

    // Interpolate between blue (0, 0, 255) and red (255, 0, 0)
    uint8_t r = static_cast<uint8_t>(255 * normalized_speed);
    uint8_t g = 0; // Green is always 0
    uint8_t b = static_cast<uint8_t>(255 * (1 - normalized_speed));

    color = {r, g, b};
}

// Get current velocity
sf::Vector2f Particle::get_velocity() const
{
    return position - position_old;
}