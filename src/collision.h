
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>

glm::vec3 checkCollision(glm::vec3 oldPos, glm::vec3 movement_vec, char** maze, int maze_size) {
    // get potential collision cubes
    // check if collides with any
    float step = 0.5;

}




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

bool check_position_collides(glm::vec3 pos, char **maze, int maze_size, int objectSize, int blockSize) {
    bool collides = false;

    int pos_i, pos_j;
    std::tie(pos_i, pos_j) = Cube::get_indexes_from_position(pos.x, pos.z, maze_size);

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
    std::cout << "----------------------------------" << std::endl;
    std::cout << "obj pos = " << glm::to_string(pos) << std::endl;
    std::cout << "obj indices = " << pos_i << ", " << pos_j << std::endl;
    for (const auto& delta : deltas) {
        int val1 = std::get<0>(delta);
        int val2 = std::get<1>(delta);

        if (pos_i + val1 < 0 || pos_i + val1 >= maze_size || pos_j + val2 < 0 || pos_j + val2 >= maze_size)
            continue;

        if (maze[pos_i + val1][pos_j + val2] == 'x') {
            glm::vec3 block = Cube::get_position_from_indexes(pos_i + val1, pos_j + val2, maze_size);
            collides = collides || rectCollide(pos, objectSize, block, blockSize);
            std::cout << "blockpos = " << pos_i + val1 << ", " << pos_j + val2 << std::endl;
            std::cout << "block vec = " << glm::to_string(block) << std::endl;
            std::cout << "val1 = " << val1 << " val2 = " << val2 << " collides = " << collides << std::endl;
        }
    }


    return collides;
}

glm::vec3 checkCollision(glm::vec3 oldPos, glm::vec3 movement_vec, float delta, char** maze, int maze_size) {
    // glm::vec3 collisionVector = glm::vec3(0.0f);
    float mov_delta = 0.05f;
    float blockSize = 1.05f;
    float objectSize = 0.2;
    
    glm::vec3 newPos = oldPos + (movement_vec * delta);
    
    // std::cout << "mov vec; " << glm::to_string(movement_vec) << std::endl;

    glm::vec3 movement_vec_cp = movement_vec;
    if (glm::length(movement_vec_cp) > mov_delta) {
        movement_vec_cp = glm::normalize(movement_vec_cp);
        newPos = oldPos + (movement_vec_cp * delta);
        if (!check_position_collides(newPos, maze, maze_size, objectSize, blockSize)) {
            // no collision
            // std::cout << "no collision default" << std::endl;
            return glm::vec3(1.0, 0.0f, 1.0f);
        }
    }

    // test moving in x direction
    movement_vec_cp = movement_vec;
    movement_vec_cp.z = 0.0;
    if (glm::length(movement_vec_cp) > mov_delta) {
        movement_vec_cp = glm::normalize(movement_vec_cp);
        newPos = oldPos + (movement_vec_cp * delta);
        if (!check_position_collides(newPos, maze, maze_size, objectSize, blockSize)) {
            // no collision
            // std::cout << "no collision x" << std::endl;
            return glm::vec3(1.0, 0.0f, 0.0f);
        }
    }

    // test moving in z direction
    movement_vec_cp = movement_vec;
    movement_vec_cp.x = 0.0;
    if (glm::length(movement_vec_cp) > mov_delta) {
        movement_vec_cp = glm::normalize(movement_vec_cp);
        newPos = oldPos + (movement_vec_cp * delta);
        if (!check_position_collides(newPos, maze, maze_size, objectSize, blockSize)) {
            // no collision
            // std::cout << "no collision z" << std::endl;
            return glm::vec3(0.0, 0.0f, 1.0f);
        }
    }

    // std::cout << "-----------------" << std::endl;
    return glm::vec3(0.0f, 0.0f, 0.0f);
}
