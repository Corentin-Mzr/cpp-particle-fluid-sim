#include <SFML/Graphics.hpp>

#include <memory>
#include <random>
#include <thread>

#include <condition_variable>

#include "events.hpp"
#include "config.hpp"
#include "simulation_fluid.hpp"
#include "particle.hpp"
#include "box.hpp"
#include "quadtree.hpp"
#include "hash_grid.hpp"
#include "utils.hpp"

// Create particle vertex array
// Uses a texture instead of points
sf::VertexArray create_particle_array(const std::vector<std::shared_ptr<Particle>> &particles,
                                      const sf::Texture &texture,
                                      const sf::RenderWindow &window);

// Generate random particles with the given parameters and the given seed
std::vector<std::shared_ptr<Particle>> generate_random_particles(const Params &params, const unsigned seed);

// Update particles
void update(std::vector<std::shared_ptr<Particle>> &particles, const size_t start, const size_t end, const float dt, const Boundary &boundary, const bool enable_omp);

int main()
{
    // Define the window
    sf::RenderWindow window(sf::VideoMode(conf::WINDOW_SIZE.x, conf::WINDOW_SIZE.y), conf::WINDOW_TITLE, sf::Style::Fullscreen);

    // View is centered on (0, 0)
    sf::View view(conf::WORLD_CENTER, sf::Vector2f(conf::WORLD_WIDTH, conf::WORLD_HEIGHT / conf::ASPECT_RATIO));
    window.setView(view);

    // Create a random device and a generator
    std::random_device rd;
    unsigned seed = rd();

    // Generate N random particles
    std::vector<std::shared_ptr<Particle>> particles = generate_random_particles(conf::GENERATOR_PARAMS, seed);

    // Vertex array to draw particles
    sf::VertexArray array;

    // World box
    const Box world_box{conf::WORLD_CENTER, {conf::XMAX, conf::YMAX}};
    const Boundary boundary = world_box.get_boundary();

    // Clock
    sf::Clock clock;

    // Vertices drawn
    sf::Text vertices_drawn;
    sf::Font arial;

    if (!arial.loadFromFile("resources/fonts/Arial.ttf"))
        std::cerr << "Could not load font\n";
    else
        vertices_drawn.setFont(arial);

    vertices_drawn.setString("Vertices drawn : ");
    vertices_drawn.setCharacterSize(256);
    vertices_drawn.setFillColor(sf::Color::Green);

    // Load texture for particles
    sf::Texture particle_texture;
    if (!particle_texture.loadFromFile(conf::PARTICLE_TEXTURE_PATH))
        std::cerr << "Could not load particle texture\n";
    else
        particle_texture.setSmooth(true);

    // Create HashGrid
    // HashGrid hashgrid(2.0f);

    // // Initial hashGrid insertion
    // for (const auto &p : particles)
    //     hashgrid.insert(p);

    // Main loop
    while (window.isOpen())
    {
        // Events
        handle_events(window, clock);

        // Particles as a vertex array
        array = create_particle_array(particles, particle_texture, window);

        // GOAL : Collision detection + particle update <= 50 ms
        // Initial FPS : 180

        // OpenMP particle update : 165
        // update(particles, 0, conf::NB_PARTICLES, conf::DT, boundary, true);

        // Naive collision detection -> O(n²)
        // for (size_t i = 0; i < particles.size(); ++i)
        // {
        //     auto &p = particles[i];
        //     for (size_t j= i + 1; j< particles.size(); ++j)
        //     {
        //         auto &other = particles[j];
        //         if (p->is_colliding(*other))
        //             p->solve_collision(*other);
        //     }
        // }

        // Quadtree collision detection -> O(log(n))
        QuadTree<Particle> qt(world_box);
        qt.batch_insert(particles);

        // Mouse attraction -> only to particle near
        const auto mouse_pos = sf::Mouse::getPosition(window);
        const auto world_pos = window.mapPixelToCoords(mouse_pos);
        const Box mouse_box{world_pos, {100.0f, 100.0f}};
        const auto near_mouse = qt.query(mouse_box);
        const bool should_attract = sf::Mouse::isButtonPressed(sf::Mouse::Left);
        const bool should_repulse = sf::Mouse::isButtonPressed(sf::Mouse::Right);

        // Apply mouse attraction only if near
        if (should_attract)
        {
            for (size_t j = 0; j < near_mouse.size(); ++j)
            {
                auto &p = near_mouse[j];
                sf::Vector2f axis = world_pos - p->get_position();
                const float length = std::sqrt((axis.x * axis.x) + (axis.y * axis.y));
                if (length != 0.0f)
                    axis /= length;
                p->apply_force(axis * 250.f);
            }
        }
        else if (should_repulse)
        {
            for (size_t j = 0; j < near_mouse.size(); ++j)
            {
                auto &p = near_mouse[j];
                sf::Vector2f axis = world_pos - p->get_position();
                const float length = std::sqrt((axis.x * axis.x) + (axis.y * axis.y));
                if (length != 0.0f)
                    axis /= length;
                p->apply_force(-axis * 250.f);
            }
        }

#pragma omp parallel for
        for (size_t i = 0; i < particles.size(); ++i)
        {
            auto &p = particles[i];

            // Boundary detection
            p->handle_boundaries(boundary.xmin, boundary.xmax, boundary.ymin, boundary.ymax);

            // Collision detection
            const Box p_box{p->get_position(), sf::Vector2f{2 * p->get_radius(), 2 * p->get_radius()}};
            auto neighbors = qt.query(p_box);
            for (size_t j = 0; j < neighbors.size(); ++j)
            {
                auto &neighbor = neighbors[j];
                if (p != neighbor && p->is_colliding(*neighbor))
#pragma omp critical
                {
                    p->solve_collision(*neighbor);
                }
            }

            // Gravity
            p->apply_force({0.0f, 50.f});

            // Physics update
            p->update(conf::DT);
        }

        // Draw
        window.clear();

        window.draw(array, &particle_texture);
        window.draw(world_box);
        // window.draw(sim.get_quadtree());
        // window.draw(vertices_drawn);

        window.display();
    }

    return 0;
}

// Create particle vertex array
sf::VertexArray create_particle_array(const std::vector<std::shared_ptr<Particle>> &particles,
                                      const sf::Texture &texture,
                                      const sf::RenderWindow &window)
{
    const size_t nb_particles = particles.size();
    sf::VertexArray array(sf::Triangles, 6 * nb_particles);

    // Current view, calculate view bounds (visible area)
    const sf::Vector2f &view_center = window.getView().getCenter();
    const sf::Vector2f &view_size = window.getView().getSize();
    const sf::FloatRect view_bounds(view_center - view_size / 2.0f, view_size);

    // Compute texture coords
    const float width = texture.getSize().x;
    const float height = texture.getSize().y;
    const sf::Vector2f tex_coords[4] = {
        {0.0f, 0.0f},
        {0.0f, height},
        {width, height},
        {width, 0.0}};

    // Variable for possible future resizing
    size_t vertex_count = 0;

// Parkour all particles and add only if visible
#pragma omp parallel for
    for (size_t i = 0; i < nb_particles; ++i)
    {
        const sf::Vector2f &position = particles[i]->get_position();
        const sf::Color &color = particles[i]->get_color();
        const float radius = particles[i]->get_radius();
        const sf::FloatRect particle_box(position - sf::Vector2f{radius, radius}, {2.0f * radius, 2.0f * radius});

        if (!view_bounds.intersects(particle_box))
            continue;

        const size_t idx = __sync_fetch_and_add(&vertex_count, 6);

        // Define vertices position
        array[idx + 0].position = {position.x - radius, position.y - radius}; // Bottom left
        array[idx + 1].position = {position.x - radius, position.y + radius}; // Top left
        array[idx + 2].position = {position.x + radius, position.y + radius}; // Top right

        array[idx + 3].position = {position.x - radius, position.y - radius}; // Bottom left
        array[idx + 4].position = {position.x + radius, position.y + radius}; // Top right
        array[idx + 5].position = {position.x + radius, position.y - radius}; // Bottom right

        // Define texture coords
        array[idx + 0].texCoords = tex_coords[0];
        array[idx + 1].texCoords = tex_coords[1];
        array[idx + 2].texCoords = tex_coords[2];

        array[idx + 3].texCoords = tex_coords[0];
        array[idx + 4].texCoords = tex_coords[2];
        array[idx + 5].texCoords = tex_coords[3];

        // Define color
        for (size_t j = 0; j < 6; ++j)
            array[idx + j].color = color;
    }

    // Resize array
    array.resize(vertex_count);

    return array;
}

// Generate random particles with the given parameters and the given seed
std::vector<std::shared_ptr<Particle>> generate_random_particles(const Params &params, const unsigned seed)
{
    // Output array
    std::vector<std::shared_ptr<Particle>> particles(params.nb_particles);

    // Randomizer with given seed
    std::mt19937 gen(seed);

    // Position
    std::uniform_real_distribution<float> dist_x(params.xmin, params.xmax);
    std::uniform_real_distribution<float> dist_y(params.ymin, params.ymax);

    // Speed
    std::uniform_real_distribution<float> dist_vx(params.vmin, params.vmax);
    std::uniform_real_distribution<float> dist_vy(params.vmin, params.vmax);

    // Radius
    std::uniform_real_distribution<float> rad(params.radius_min, params.radius_max);

    // Mass
    std::uniform_real_distribution<float> mass(params.mass_min, params.mass_max);

    for (size_t i = 0; i < params.nb_particles; ++i)
    {
        // Define particle params
        const float x = dist_x(gen);
        const float y = dist_y(gen);
        const float vx = dist_vx(gen);
        const float vy = dist_vy(gen);
        const float radius = rad(gen);
        const float m = mass(gen);
        const sf::Vector2f pos{x, y};
        const sf::Vector2f vel{vx, vy};
        const sf::Vector2f acc{0.0f, 0.0f};

        // Create a particle
        auto particle = std::make_shared<Particle>(radius * std::sqrt(m), sf::Color::White, m, pos, vel, acc);
        particles[i] = particle;
    }

    return particles;
}

// Update particles
void update(std::vector<std::shared_ptr<Particle>> &particles, const size_t start, const size_t end, const float dt, const Boundary &boundary, const bool enable_omp)
{
    if (enable_omp)
    {
#pragma omp parallel for
        for (size_t i = start; i < end; ++i)
        {
            auto &p = particles[i];
            p->handle_boundaries(boundary.xmin, boundary.xmax, boundary.ymin, boundary.ymax);
            p->update(dt);
        }
    }
    else
    {
        for (size_t i = start; i < end; ++i)
        {
            auto &p = particles[i];
            p->handle_boundaries(boundary.xmin, boundary.xmax, boundary.ymin, boundary.ymax);
            p->update(dt);
        }
    }
}

// QuadTree initial Box
// const Box box{conf::WORLD_CENTER, {conf::WORLD_WIDTH / 2.0f, conf::WORLD_HEIGHT / (2.0f * conf::ASPECT_RATIO)}};

// Simulation
// SimulationFluid sim(particles, box, conf::DT, conf::SUBSTEPS);

// // BENCHMARKING
// sf::Clock timer;
// unsigned NUM = 100;
// int time = 0;

// // Create hashgrid
// for (size_t i = 0; i < NUM; ++i)
// {
//     timer.restart();
//     HashGrid<Particle> hashgrid(2.0f);
//     time += timer.getElapsedTime().asMicroseconds();
// }
// std::cout << "Create hashgrid : " << static_cast<float>(time) / static_cast<float>(NUM) << " us" << std::endl;

// // Insert particles and clear grid
// HashGrid<Particle> hashgrid(2.0f);
// time = 0;
// for (size_t i = 0; i < NUM; ++i)
// {
//     timer.restart();
//     for (auto &p : particles)
//         hashgrid.insert(p);
//     hashgrid.clear();
//     time += timer.getElapsedTime().asMilliseconds();
// }
// std::cout << "Insert " << conf::NB_PARTICLES << " particles : " << static_cast<float>(time) / static_cast<float>(NUM) << " ms" << std::endl;

// // Update a random particle
// time = 0;
// std::uniform_int_distribution<int> random(0, conf::NB_PARTICLES - 1);
// for (auto &p : particles)
//     hashgrid.insert(p);
// for (size_t i = 0; i < NUM; ++i)
// {
//     timer.restart();
//     const int n = random(gen);
//     hashgrid.update(particles[n]);
//     time += timer.getElapsedTime().asMicroseconds();
// }
// std::cout << "Update a particle : " << static_cast<float>(time) / static_cast<float>(NUM) << " us" << std::endl;

// Update simulation
// sim.update();

// Apply gravity with mouse
// if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
// {
//     const sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
//     const sf::Vector2f world_pos = window.mapPixelToCoords(mouse_pos);
//     const float dist_to_mouse = (world_pos.x - p->get_position().x) * (world_pos.x - p->get_position().x) + (world_pos.y - p->get_position().y) * (world_pos.y - p->get_position().y);
//     sf::Vector2f axis = (world_pos - p->get_position());
//     if (dist_to_mouse != 0)
//         axis /= dist_to_mouse;
//     axis *= 1000.0f;
//     if (dist_to_mouse < 30.0f)
//         p->apply_force(-axis);
// }

// for (auto &p : particles)
//     hashgrid.insert(p);

// for (auto &p: particles)
//     hashgrid.remove(p);

// Loop : clear insert query
// hashgrid.clear();
// for (size_t i = 0; i < particles.size(); ++i)
// {
//     hashgrid.insert(particles[i]);
// }

// for (size_t i = 0; i < particles.size(); ++i)
// {
//     auto &p = particles[i];
//     auto boundaries = box.get_boundary();
//     p->handle_boundaries(boundaries.xmin, boundaries.xmax, boundaries.ymin, boundaries.ymax);

//     auto found = hashgrid.query(p->get_position());

//     for (auto &other : found)
//     {
//         if (p != other && p->is_colliding(*other))
//             p->solve_collision(*other);
//     }

//     p->update(conf::DT);
//     hashgrid.update(p);
// }

// #pragma omp parallel for
//         for (size_t i = 0; i < particles.size(); ++i)
//         {
//             auto &p = particles[i];
//             auto boundaries = box.get_boundary();
//             p->handle_boundaries(boundaries.xmin, boundaries.xmax, boundaries.ymin, boundaries.ymax);

//             auto found = hashgrid.query(p->get_position());
//             for (auto &other: found)
//             {
//                 if (p != other && p->is_colliding(*other))
// #pragma omp critical
//                 {
//                     p->solve_collision(*other);
//                 }
//             }

//             p->update(conf::DT);
//         }

// Create array to draw particles
// const sf::VertexArray array = create_particles(particles, particle_texture, window);
// const unsigned nb_drawn = array.getVertexCount();

// // Change HUD based on view
// vertices_drawn.setString("Vertices drawn : " + std::to_string(nb_drawn) + "/" + std::to_string(conf::TOTAL_VERTICES));
// const sf::Vector2f view_center = window.getView().getCenter();
// const sf::Vector2f view_size = window.getView().getSize();
// const float scale = 0.00025f * (view_size.x - view_size.y);
// vertices_drawn.setScale({scale, scale});
// vertices_drawn.setPosition(view_center.x - view_size.x / 2.0f, view_center.y + view_size.y / 2.0f - 2 * vertices_drawn.getGlobalBounds().height);