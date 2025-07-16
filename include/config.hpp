#pragma once

#include <SFML/Graphics.hpp>

// List of parameters to generate random points
struct Params
{
    float xmin, xmax, ymin, ymax;
    float vmin, vmax;
    float radius_min, radius_max;
    float mass_min, mass_max;
    unsigned nb_particles;
};

namespace conf
{
    // Window config
    constexpr unsigned WINDOW_WIDTH = 1920;
    constexpr unsigned WINDOW_HEIGHT = 1080;
    constexpr float ASPECT_RATIO = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT);
    constexpr char WINDOW_TITLE[] = "My SFML Project";
    const sf::Vector2u WINDOW_SIZE{WINDOW_WIDTH, WINDOW_HEIGHT};
    const sf::Vector2f WINDOW_SIZE_F = static_cast<sf::Vector2f>(WINDOW_SIZE);

    // Particle config
    constexpr unsigned NB_PARTICLES = 2000;
    constexpr unsigned TOTAL_VERTICES = 6 * NB_PARTICLES;
    constexpr char PARTICLE_TEXTURE_PATH[] = "resources/images/circle.png";
    constexpr float VMIN = -0.5f;
    constexpr float VMAX = 0.5f;
    constexpr float RADIUS_MIN = 1.0f;
    constexpr float RADIUS_MAX = 1.0f;
    constexpr float MASS_MIN = 1.0f;
    constexpr float MASS_MAX = 5.0f;

    // Simulation config
    constexpr float FRAMERATE = 144.0f;
    constexpr float DT = 1.0f / FRAMERATE;
    constexpr float G = 9.81f;
    constexpr float WALL_DAMPING = 0.1f;
    constexpr float PARTICLE_DAMPING = 0.3f;
    constexpr unsigned SUBSTEPS = 8;

    // World - View config
    const sf::Vector2f WORLD_CENTER = {0.f, 0.f};
    constexpr float WORLD_WIDTH = 500.f;
    constexpr float WORLD_HEIGHT = 500.f;
    constexpr float XMIN = - WORLD_WIDTH / 2.0f;
    constexpr float XMAX = WORLD_WIDTH / 2.0f;
    constexpr float YMIN = - WORLD_HEIGHT / (2.0f * ASPECT_RATIO);
    constexpr float YMAX = WORLD_HEIGHT / (2.0f * ASPECT_RATIO);

    // Particle random generator
    constexpr Params GENERATOR_PARAMS = {
        XMIN, XMAX, YMIN, YMAX,
        VMIN, VMAX,
        RADIUS_MIN, RADIUS_MAX,
        MASS_MIN, MASS_MAX,
        NB_PARTICLES
    };

    // Control config
    constexpr float SENSITIVITY = 100.0f;

};