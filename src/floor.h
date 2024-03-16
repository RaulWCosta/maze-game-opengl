#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tuple>
#include <vector>

#include "shader_s.h"

class Floor {

public:

    Shader mShader = Shader("shaders/floor.vert", "shaders/floor.frag");
    unsigned int mVAO;
    unsigned int mVBO;
    int mMazeSize;
    glm::mat4 mProjection;
    glm::mat4 mModel;

    Floor(int maze_size) : mMazeSize(maze_size) {

        float *floor_vertices;
        int floor_vertices_size;
        std::tie(floor_vertices, floor_vertices_size) = get_vertices();

        get_buffers(floor_vertices, floor_vertices_size);

        mProjection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        mModel = get_model_mat();
    }

    ~Floor() {
        
    }

    void render(Camera& camera) {
        mShader.use();
        glBindVertexArray(mVAO);

        mShader.setMat4("projection", mProjection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        mShader.setMat4("view", view);

        mShader.setMat4("model", mModel);

        glDrawArrays(GL_TRIANGLES, 0, 6);

    }

private:

    std::tuple<float*, int> get_vertices() {
        std::vector<float> vertices = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
            0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
            0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        };

        // TODO fix memory leak
        float *d_vertices = new float[vertices.size()];
        for (int i = 0; i < vertices.size(); i++) {
            d_vertices[i] = vertices[i];
        }

        return std::make_tuple(d_vertices, vertices.size());
    }

    void get_buffers(float *vertices, int n) {
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);

        glBindVertexArray(mVAO);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, n * sizeof(vertices), vertices, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

    }

    glm::mat4 get_model_mat() {
        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        float angle = 90.0f;
        model = glm::translate(model, glm::vec3(0.0, -0.5, 0.0));
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3((float)mMazeSize, (float)mMazeSize, 0.0));
        return model;
    }
};