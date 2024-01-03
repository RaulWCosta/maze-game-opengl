#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>

#include <iostream>
#include <tuple>
#include <vector>

namespace Cube {

    std::tuple<float*, int> get_vertices() {
        std::vector<float> vertices = {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        };

        float *d_vertices = new float[vertices.size()];
        for (int i = 0; i < vertices.size(); i++) {
            d_vertices[i] = vertices[i];
        }

        return std::make_tuple(d_vertices, vertices.size());
    }

    std::tuple<unsigned int, unsigned int> get_buffers(float *vertices, int n) {
        unsigned int VBO, VAO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, n * sizeof(vertices), vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        return std::make_tuple(VAO, VBO);
    }

    glm::vec3 get_camera_position(char **maze, int width, int depth) {

        std::vector<glm::vec3> cubePositions;

        for (int i = 0; i < width; i++) {
            for (int j = 0; j < depth; j++) {
                // std::cout << maze[i][j] << std::endl; 

                if (maze[i][j] == 'c') {
                    float x = (float) (i - (int) (width/2));
                    float z = (float) (j - (int) (depth/2));
                    return glm::vec3(x, 0.0f, z);
                }
            }
        }
    }

    glm::vec3 get_position_from_indexes(int i, int j, int width, int depth) {
        float x = (float) (i - (int) (width/2));
        float z = (float) (j - (int) (depth/2));
        return glm::vec3(x, 0.0f, z);
    }

    std::vector<glm::vec3> get_positions(char **maze, int width, int depth) {

        std::vector<glm::vec3> cubePositions;

        for (int i = 0; i < width; i++) {
            for (int j = 0; j < depth; j++) {
                // std::cout << maze[i][j] << std::endl; 

                if (maze[i][j] == 'x') {
                    glm::vec3 pos = get_position_from_indexes(i, j, width, depth);
                    cubePositions.push_back(pos);
                    // float x = (float) (i - (int) (width/2));
                    // float z = (float) (j - (int) (depth/2));
                    // cubePositions.push_back(glm::vec3(x, 0.0f, z)); 
                }
            }
        }

        return cubePositions;
    }

    glm::mat4 get_model_mat(glm::vec3 position) {
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        model = glm::translate(model, position);
        // float angle = 20.0f * i;
        // model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        return model;
    }

    std::tuple<int, int> get_indexes_from_position(int maze_width, int maze_depth, float x, float y) {
        // float x = (float) (i - (int) (mazewidth/2));
        // float z = (float) (j - (int) (depth/2));

        int i = (int) (y + (int)(maze_depth / 2) + 0.5);
        int j = (int) (x + (int)(maze_width / 2) + 0.5);

        // std::cout << "i= " << i << ", j= " << j << std::endl;
        return std::make_tuple(i, j);
    }
}
