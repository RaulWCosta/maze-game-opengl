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
#include <memory>

#include "shader_s.h"



class CubeComponent {

public:
    Shader mShader = Shader("shaders/cube.vert", "shaders/cube.frag");
    unsigned int mVAO;
    unsigned int mVBO;
    glm::mat4 mProjection;
    glm::mat4 mModel;
    unsigned int mWallTexture;
    float mDiagLength = 1.0f;


    CubeComponent() {

        mShader.setInt("wall_texture", 0);

        float *cube_vertices;
        int cube_vertices_size;
        std::tie(cube_vertices, cube_vertices_size) = get_vertices();
        get_buffers(cube_vertices, cube_vertices_size);

        mShader.use();
        mProjection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        init_texture();

    }

    ~CubeComponent() {
        // TODO check why this is not working
        // glDeleteBuffers(1, &mVBO);
        // glDeleteVertexArrays(1, &mVAO);
    }

    void render(Camera& camera, glm::vec3 position) {
        mShader.use();
        glBindVertexArray(mVAO);
        
        mShader.setMat4("projection", mProjection);

        glm::mat4 view = camera.GetViewMatrix();
        mShader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);

        mShader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);

    }

    void bind_texture() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mWallTexture);
    }

    bool collide(glm::vec3 obj_pos, float diag_length) {

        // // (Math.abs(p1.x - p2.x) < r && Math.abs(p1.y - p2.y) < r)
        // float r1_width = mDiagLength /

        // // If one rectangle is on left side of the other
        // if (rect1.x + rect1.width < rect2.x || rect2.x + rect2.width < rect1.x)
        //     return false;

        // // If one rectangle is above the other
        // if (rect1.y + rect1.height < rect2.y || rect2.y + rect2.height < rect1.y)
        //     return false;

        // Rectangles overlap
        return true;
    }

private:

    void init_texture() {
        glGenTextures(1, &mWallTexture);
        glBindTexture(GL_TEXTURE_2D, mWallTexture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        unsigned char *data = stbi_load("resources/textures/brick_wall.jpg", &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
    }

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

};


class Cube {

public:
    std::shared_ptr<CubeComponent> mCubeComponent;
    glm::vec3 mPosition;

    Cube(glm::vec3 position) : mPosition(position) {
        mCubeComponent = std::make_shared<CubeComponent>();
    }

    void render(Camera &camera) {
        this->mCubeComponent->render(camera, this->mPosition);
    }

    void bind_texture() {
        this->mCubeComponent->bind_texture();
    }

    void collide(glm::vec3 obj_pos, float diag_length) {
        this->mCubeComponent->collide(obj_pos, diag_length);
    }

};