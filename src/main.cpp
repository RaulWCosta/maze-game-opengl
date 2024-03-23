
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>

#include "shader_s.h"
#include "camera.h"
#include "maze.h"
#include "cube.h"
#include "floor.h"
#include "collision.h"

#include <iostream>
#include <algorithm>
#include <tuple>

#include "settings.h"


void get_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

glm::vec3 get_movement_from_input(GLFWwindow *window) {
    Camera* camera = (Camera *)glfwGetWindowUserPointer(window);

    glm::vec3 movement_vec = glm::vec3(0.0f);
    glm::vec3 frontMovement = glm::vec3(camera->Front.x, 0.0, camera->Front.z);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        movement_vec += camera->Front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        movement_vec += -camera->Front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        movement_vec += -camera->Right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        movement_vec += camera->Right;

    return movement_vec;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void move_camera(GLFWwindow *window, float delta_time, char **maze, int maze_size)
{
    Camera* camera = (Camera *)glfwGetWindowUserPointer(window);
    glm::vec3 movement_vec = get_movement_from_input(window);

    if (glm::length(movement_vec) <= 0.01f) {
        return;
    }
    movement_vec.y = 0.0f;
    movement_vec = glm::normalize(movement_vec);

    glm::vec3 collision_vec = checkCollision(
        camera->Position,
        movement_vec,
        delta_time * camera->MovementSpeed,
        maze,
        maze_size
    );

    movement_vec = collision_vec * movement_vec;

    if (glm::length(movement_vec) <= 0.01f) {
        return;
    }
    movement_vec = (delta_time * camera->MovementSpeed) * glm::normalize(movement_vec);

    camera->Position += movement_vec;

}


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    Camera camera(SCR_WIDTH, SCR_HEIGHT);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    char **maze;
    int maze_size;
    std::tie(maze, maze_size) = get_maze();

    std::vector<glm::vec3> cubePositions = get_positions(maze, maze_size);

    camera.Position = get_camera_position(maze, maze_size);

    float target_frame_rate = 60.0;
    float last_time = static_cast<float>(glfwGetTime());

    CubeCollection cube_collection = CubeCollection(cubePositions);

    Floor floor = Floor(maze_size);

    // render loop
    while (!glfwWindowShouldClose(camera.mWindow))
    {

        float current_time = static_cast<float>(glfwGetTime());
        float delta_time = current_time - last_time;
        if(delta_time >= 1.0 / target_frame_rate) {
            last_time = current_time;
        } else {
            continue;
        }

        get_input(camera.mWindow);
        move_camera(camera.mWindow, delta_time, maze, maze_size);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        cube_collection.render(camera);

        floor.render(camera);

        glfwSwapBuffers(camera.mWindow);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
