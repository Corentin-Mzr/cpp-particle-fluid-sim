#pragma once

#include <cmath>
#include <memory>

#include <SFML/Graphics.hpp>

#include "particle.hpp"
#include "config.hpp"

constexpr unsigned POINT_COUNT = 18;

// Create particles in a vertex array based on a list of Particles
sf::VertexArray create_particles(const std::vector<Particle> &particles, const unsigned point_count, const sf::RenderWindow &window)
{
    // Not optimal, extract the list of positions, radius and colors of the particles
    const size_t nb_particles = particles.size();
    std::vector<sf::Vector2f> positions;
    std::vector<sf::Color> colors;
    std::vector<float> radiuses;

    // Reserve maximum memory
    positions.reserve(nb_particles);
    colors.reserve(nb_particles);
    radiuses.reserve(nb_particles);

    // Current view rect
    const sf::Vector2f view_center = window.getView().getCenter();
    const sf::Vector2f view_size = window.getView().getSize();

    // Calculate the view bounds (visible area)
    const sf::FloatRect view_bounds(view_center - view_size / 2.0f, view_size);

    // OpenMP seems not optimal for this
    size_t nb_draw = 0;
    for (size_t i = 0; i < nb_particles; ++i)
    {
        const sf::Vector2f position = particles[i].get_position();
        const sf::Color color = particles[i].get_color();
        const float radius = particles[i].get_radius();

        // Dont retrieve particles that are out of view
        const sf::FloatRect particle_box(position - sf::Vector2f{radius, radius}, {2.0f * radius, 2.0f * radius});
        if (view_bounds.intersects(particle_box))
        {
            positions.push_back(position);
            colors.push_back(color);
            radiuses.push_back(radius);
            nb_draw++;
        }
    }

    // Resize vectors
    positions.resize(nb_draw);
    colors.resize(nb_draw);
    radiuses.resize(nb_draw);

    // Create particles
    sf::VertexArray particles_as_array = create_particles(positions, radiuses, colors, point_count);

    return particles_as_array;
}


// Function to create multiple circles in a vertex array
sf::VertexArray create_particles(const std::vector<sf::Vector2f> &positions,
                                 const std::vector<float> &radiuses,
                                 const std::vector<sf::Color> &colors,
                                 const unsigned point_count)
{
    // Lists of positions, radiuses and colors must have the same size but we dont check
    const size_t nb_particles = positions.size();

    // Preallocate vertex array size based on the number of circles (N * 3 * point_count)
    const unsigned total_vertices = nb_particles * 3 * point_count;
    sf::VertexArray array(sf::Triangles, total_vertices);

    // Define angle increment between each point of a circle
    const float angle_increment = 2 * M_PI / static_cast<float>(point_count);

    // Index to track position in the vertex array
    unsigned int index = 0;

    // Loop through each position in the input vector
#pragma omp parallel for
    for (size_t i = 0; i < nb_particles; ++i)
    {
        const sf::Vector2f center = positions[i];
        const float radius = radiuses[i];

        // Variables for parallelization
        const unsigned particle_start_index = i * point_count * 3;
        unsigned local_index = particle_start_index;

        // Define center vertex for each circle
        const sf::Vertex center_vertex(center, colors[i]);

        // Generate points on the circumference of the circle
        sf::Vector2f first_point(center.x + radius, center.y);
        sf::Vector2f prev_point = first_point;

        for (unsigned j = 1; j < point_count; ++j)
        {
            // Compute next point
            const float angle = j * angle_increment;
            const float x = center.x + radius * cosf(angle);
            const float y = center.y + radius * sinf(angle);
            sf::Vector2f current_point(x, y);

            // Create a triangle using center, previous point and computed point
            array[local_index++] = center_vertex;
            array[local_index++] = sf::Vertex(prev_point, colors[i]);
            array[local_index++] = sf::Vertex(current_point, colors[i]);

            // Update previous point to be the current point
            prev_point = current_point;
        }

        // Close the circle by connecting the last point back to the first point
        array[local_index++] = center_vertex;
        array[local_index++] = sf::Vertex(prev_point, colors[i]);
        array[local_index++] = sf::Vertex(first_point, colors[i]);
    }

    return array;
}



// Function to create multiple circles in a vertex array
// Uses a particle texture instead of vertices, so we only need to define six vertices per particle
sf::VertexArray create_particles(const std::vector<sf::Vector2f> &positions,
                                 const std::vector<float> &radiuses,
                                 const std::vector<sf::Color> &colors,
                                 const sf::Texture &texture,
                                 const std::vector<bool> &to_draw,
                                 const size_t nb_to_draw)
{
    // Lists of positions, radiuses and colors must have the same size but we dont check
    const size_t nb_particles = positions.size();

    // Preallocate vertex array size based on the number of circles (6 vertices per particles)
    const unsigned total_vertices = nb_to_draw * 6;
    sf::VertexArray array(sf::Triangles, total_vertices);

    // Texture dimensions
    const float width = texture.getSize().x;
    const float height = texture.getSize().y;

    // Loop through each position in the input vector
#pragma omp parallel for
    for (size_t i = 0; i < nb_particles; ++i)
    {
        // Dont draw the particle
        if (!to_draw[i])
            continue;

        const sf::Vector2f center = positions[i];
        const float radius = radiuses[i];
        const sf::Color color = colors[i];

        // Define vertices position
        array[6 * i + 0].position = {center.x - radius, center.y - radius}; // Bottom left
        array[6 * i + 1].position = {center.x - radius, center.y + radius}; // Top left
        array[6 * i + 2].position = {center.x + radius, center.y + radius}; // Top right

        array[6 * i + 3].position = {center.x - radius, center.y - radius}; // Bottom left
        array[6 * i + 4].position = {center.x + radius, center.y + radius}; // Top right
        array[6 * i + 5].position = {center.x + radius, center.y - radius}; // Bottom right

        // Define texture coords
        array[6 * i + 0].texCoords = {0.0f, 0.0f};
        array[6 * i + 1].texCoords = {0.0f, 1.0f * height};
        array[6 * i + 2].texCoords = {1.0f * width, 1.0f * height};

        array[6 * i + 3].texCoords = {0.0f, 0.0f};
        array[6 * i + 4].texCoords = {1.0f * width, 1.0f * height};
        array[6 * i + 5].texCoords = {1.0f * width, 0.0f};

        // Define color
        array[6 * i + 0].color = color;
        array[6 * i + 1].color = color;
        array[6 * i + 2].color = color;

        array[6 * i + 3].color = color;
        array[6 * i + 4].color = color;
        array[6 * i + 5].color = color;
    }

    return array;
}

// Create particles in a vertex array based on a list of Particles
sf::VertexArray create_particles(const std::vector<std::shared_ptr<Particle>> &particles,
                                 const sf::Texture &texture,
                                 const sf::RenderWindow &window)
{
    // Not optimal, extract the list of positions, radius and colors of the particles
    const size_t nb_particles = particles.size();
    std::vector<sf::Vector2f> positions(nb_particles);
    std::vector<sf::Color> colors(nb_particles);
    std::vector<float> radiuses(nb_particles);

    // Pass this list to the next function, so we know which particles to draw
    std::vector<bool> to_draw(nb_particles, false);

    // Current view rect
    const sf::Vector2f view_center = window.getView().getCenter();
    const sf::Vector2f view_size = window.getView().getSize();

    // Calculate the view bounds (visible area)
    const sf::FloatRect view_bounds(view_center - view_size / 2.0f, view_size);

    size_t nb_draw = 0;
    // #pragma omp parallel for
    for (size_t i = 0; i < nb_particles; ++i)
    {
        const sf::Vector2f position = particles[i]->get_position();
        const sf::Color color = particles[i]->get_color();
        const float radius = particles[i]->get_radius();

        // Retrieve only particles that are in the view
        const sf::FloatRect particle_box(position - sf::Vector2f{radius, radius}, {2.0f * radius, 2.0f * radius});
        if (view_bounds.intersects(particle_box))
        {
            positions[i] = position;
            colors[i] = color;
            radiuses[i] = radius;

            // To know where we are currently
            to_draw[i] = true;
        }
    }

    // Create particles
    sf::VertexArray particles_as_array = create_particles(positions, radiuses, colors, texture, to_draw, nb_particles);

    return particles_as_array;
}