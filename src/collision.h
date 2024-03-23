
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>

#include <iostream>
#include <tuple>
#include <vector>

#include "maze.h"


bool rectCollide(glm::vec3 obj_pos, float obj_size, glm::vec3 block_pos, float block_size) {
    // Calculate the half sizes of the objects
    float obj_half_size_x = obj_size / 2.0f;
    float obj_half_size_z = obj_size / 2.0f;
    float block_half_size_x = block_size / 2.0f;
    float block_half_size_z = block_size / 2.0f;

    // Check for collision along the x-axis
    bool collide_x = (obj_pos.x + obj_half_size_x >= block_pos.x - block_half_size_x) &&
                     (obj_pos.x - obj_half_size_x <= block_pos.x + block_half_size_x);

    // Check for collision along the z-axis
    bool collide_z = (obj_pos.z + obj_half_size_z >= block_pos.z - block_half_size_z) &&
                     (obj_pos.z - obj_half_size_z <= block_pos.z + block_half_size_z);

    // Return true if there is a collision along both axes
    return collide_x && collide_z;
}

bool check_position_collides(glm::vec3 pos, char **maze, int maze_size, float object_size, float block_size) {
    bool collides = false;

    int pos_i, pos_j;
    std::tie(pos_i, pos_j) = get_indexes_from_position(pos.x, pos.z, maze_size);

    std::vector<std::tuple<int, int>> deltas = {
        {0, 0},
        {-1, -1},
        {-1, 0},
        {-1, 1},
        {0, -1},
        {0, 1},
        {1, -1},
        {1, 0},
        {1, 1}
    };

    for (const auto& delta : deltas) {
        int val1 = std::get<0>(delta);
        int val2 = std::get<1>(delta);

        if (pos_i + val1 < 0 || pos_i + val1 >= maze_size || pos_j + val2 < 0 || pos_j + val2 >= maze_size)
            continue;

        if (maze[pos_i + val1][pos_j + val2] == 'x') {
            glm::vec3 block = get_position_from_indexes(pos_i + val1, pos_j + val2, maze_size);
            collides = collides || rectCollide(pos, object_size, block, block_size);
        }
    }

    return collides;
}

glm::vec3 checkCollision(glm::vec3 oldPos, glm::vec3 movement_vec, float total_movement, char** maze, int maze_size) {

    float block_size = 1.05f;
    float object_size = 0.3;

    glm::vec3 newPos = oldPos + (movement_vec * total_movement);


    if (!check_position_collides(newPos, maze, maze_size, object_size, block_size)) {
        // no collision
        return glm::vec3(1.0, 0.0f, 1.0f);
    }

    // collided with something!

    bool collided_x = false;
    bool collided_z = false;

    // test moving in x direction
    glm::vec3 newPos_x = oldPos + (glm::normalize(movement_vec * glm::vec3(1.0f, 0.0f, 0.0f)) * total_movement);
    if (check_position_collides(newPos_x, maze, maze_size, object_size, block_size)) {
        // collides x
        collided_x = true;
    }

    // test moving in z direction
    glm::vec3 newPos_z = oldPos + (glm::normalize(movement_vec * glm::vec3(0.0f, 0.0f, 1.0f)) * total_movement);
    if (check_position_collides(newPos_z, maze, maze_size, object_size, block_size)) {
        // collides z
        collided_z = true;
    }

    if (collided_x && collided_z)
        return glm::vec3(0.0f, 0.0f, 0.0f);
    else if (collided_x)
        return glm::vec3(0.0f, 0.0f, 1.0f);
    else if (collided_z)
        return glm::vec3(1.0f, 0.0f, 0.0f);
}
