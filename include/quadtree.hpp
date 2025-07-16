#pragma once

#include <memory>

#include <SFML/Graphics.hpp>

#include "box.hpp"
#include "particle.hpp"

// QuadTree class
template<typename T>
class QuadTree : public sf::Drawable
{

public:
    // Constructor
    QuadTree(const Box &b);

    // Move constructor
    QuadTree(QuadTree &&other) noexcept;

    // Move operator
    QuadTree &operator=(QuadTree &&other) noexcept;

    // Insert multiple objetcs (particles) at once
    void batch_insert(const std::vector<std::shared_ptr<T>> &particles);

    // Insert an object (particle) in the node
    bool insert(const std::shared_ptr<T> &p);

    // Find all objects in the given range
    std::vector<std::shared_ptr<T>> query(const Box &b) const;

private:
    // Number of elements that can be stored in a node
    const unsigned node_capacity = 16;

    // Box
    Box boundary;

    // Objects (particles) of this node of the QuadTree
    std::vector<std::shared_ptr<T>> objects;

    // Children
    std::unique_ptr<QuadTree> north_west = nullptr;
    std::unique_ptr<QuadTree> north_east = nullptr;
    std::unique_ptr<QuadTree> south_west = nullptr;
    std::unique_ptr<QuadTree> south_east = nullptr;

    // Subdivide the QuadTree into four new children
    void subdivide();

    // Draw QuadTree
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        target.draw(boundary, states);

        if (north_west != nullptr)
        {
            north_west->draw(target, states);
            north_east->draw(target, states);
            south_west->draw(target, states);
            south_east->draw(target, states);
        }
    }
};


// Constructor
template <typename T>
QuadTree<T>::QuadTree(const Box &b) : boundary(b)
{
    objects.reserve(node_capacity);
}

// Move constructor
template <typename T>
QuadTree<T>::QuadTree(QuadTree &&other) noexcept : boundary(std::move(other.boundary)),
                                                   objects(std::move(other.objects)),
                                                   north_west(std::move(other.north_west)),
                                                   north_east(std::move(other.north_east)),
                                                   south_west(std::move(other.south_west)),
                                                   south_east(std::move(other.south_east))
{
}

// Move operator
template <typename T>
QuadTree<T> &QuadTree<T>::operator=(QuadTree &&other) noexcept
{
    if (this != &other)
    {
        boundary = std::move(other.boundary);
        objects = std::move(other.objects);
        north_west = std::move(other.north_west);
        north_east = std::move(other.north_east);
        south_west = std::move(other.south_west);
        south_east = std::move(other.south_east);
    }
    return *this;
}

// Insert multiple objects at once
template <typename T>
void QuadTree<T>::batch_insert(const std::vector<std::shared_ptr<T>> &objects)
{
    for (auto &p : objects)
    {
        if (boundary.contains(*p))
            insert(p);
    }
}

// Insert an object in the node
template <typename T>
bool QuadTree<T>::insert(const std::shared_ptr<T> &p)
{
    // Ignore object that dont belong to the QuadTree
    if (!boundary.contains(*p))
        return false;

    // Check if there is enough place to insert the object
    if (objects.size() < node_capacity)
    {
        objects.push_back(p);
        return true;
    }

    // Else subdivise the QuadTree and add the object to the node
    if (north_west == nullptr)
        subdivide();

    if (north_west->insert(p))
        return true;
    if (north_east->insert(p))
        return true;
    if (south_west->insert(p))
        return true;
    if (south_east->insert(p))
        return true;

    // Point cannot be inserted (should not happen)
    return false;
}

// Subdivide the QuadTree into four new children
template <typename T>
void QuadTree<T>::subdivide()
{
    // Retrieve current node params
    const sf::Vector2f &center = boundary.get_center();
    const sf::Vector2f &hdim = boundary.get_half_dimension();

    // Define children boxes params
    const sf::Vector2f box_dim = hdim / 2.0f;

    const sf::Vector2f nw_center = center + sf::Vector2f{-hdim.x / 2.0f, -hdim.y / 2.0f};
    const sf::Vector2f ne_center = center + sf::Vector2f{hdim.x / 2.0f, -hdim.y / 2.0f};
    const sf::Vector2f sw_center = center + sf::Vector2f{-hdim.x / 2.0f, hdim.y / 2.0f};
    const sf::Vector2f se_center = center + sf::Vector2f{hdim.x / 2.0f, hdim.y / 2.0f};

    // Build boxes based on params
    Box nw_box{nw_center, box_dim};
    Box ne_box{ne_center, box_dim};
    Box sw_box{sw_center, box_dim};
    Box se_box{se_center, box_dim};

    // Build children using boxes created
    north_west = std::make_unique<QuadTree>(nw_box);
    north_east = std::make_unique<QuadTree>(ne_box);
    south_west = std::make_unique<QuadTree>(sw_box);
    south_east = std::make_unique<QuadTree>(se_box);
}

// Find all objects in the given range
template <typename T>
std::vector<std::shared_ptr<T>> QuadTree<T>::query(const Box &b) const
{
    // Output array
    std::vector<std::shared_ptr<T>> objects_found;

    // Interrupt if the research zone does not intersect the QuadTree
    if (!boundary.intersect(b))
        return objects_found;

    // Check objets in the QuadTree
    for (const auto &p : objects)
    {
        if (b.contains(*p))
            objects_found.push_back(p);
    }

    // Stop if no children
    if (north_west == nullptr)
        return objects_found;

    // Else do the research on children
    const auto nw_objects = north_west->query(b);
    const auto ne_objects = north_east->query(b);
    const auto sw_objects = south_west->query(b);
    const auto se_objects = south_east->query(b);

    objects_found.insert(objects_found.end(), nw_objects.begin(), nw_objects.end());
    objects_found.insert(objects_found.end(), ne_objects.begin(), ne_objects.end());
    objects_found.insert(objects_found.end(), sw_objects.begin(), sw_objects.end());
    objects_found.insert(objects_found.end(), se_objects.begin(), se_objects.end());

    return objects_found;
}