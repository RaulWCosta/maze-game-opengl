

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
    float mov_delta = 0.01f;
    glm::vec3 movement_vec = get_movement_from_input(window);

    if (glm::length(movement_vec) <= mov_delta) {
        return;
    }
    // movement_vec.y = 0.0f;
    movement_vec = glm::normalize(movement_vec);

    // glm::vec3 collision_vector4 = checkCollision(
    //     camera.Position,
    //     movement_vec,
    //     0.1f,
    //     maze,
    //     maze_size
    // );

    // movement_vec = collision_vector4 * movement_vec;

    if (glm::length(movement_vec) <= mov_delta) {
        return;
    }
    movement_vec = (delta_time * camera->MovementSpeed) * glm::normalize(movement_vec);

    camera->Position += movement_vec;

}


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------

    Camera camera(SCR_WIDTH, SCR_HEIGHT);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    
    Shader floor_shader("shaders/floor.vert", "shaders/floor.frag");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    float *cube_vertices;
    // int cube_vertices_size;
    // std::tie(cube_vertices, cube_vertices_size) = Cube::get_vertices();

    float *floor_vertices;
    int floor_vertices_size;
    std::tie(floor_vertices, floor_vertices_size) = Floor::get_vertices();

    char **maze;
    int maze_size;
    std::tie(maze, maze_size) = get_maze();

    std::vector<glm::vec3> cubePositions = get_positions(maze, maze_size);

    // std::tie(cubeVAO, cubeVBO) = Cube::get_buffers(cube_vertices, cube_vertices_size);

    unsigned int floorVAO, floorVBO;
    std::tie(floorVAO, floorVBO) = Floor::get_buffers(floor_vertices, floor_vertices_size);


    camera.Position = get_camera_position(maze, maze_size);

    // load and create a texture 
    // -------------------------
    // unsigned int wall_texture;
    unsigned int texture2;
    // texture 1
    // ---------
    // // glGenTextures(1, &wall_texture);
    // // glBindTexture(GL_TEXTURE_2D, wall_texture);
    // // set the texture wrapping parameters
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // // set texture filtering parameters
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // // load image, create texture and generate mipmaps
    // int width, height, nrChannels;
    // stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // unsigned char *data = stbi_load("resources/textures/brick_wall.jpg", &width, &height, &nrChannels, 0);
    // if (data)
    // {
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    //     glGenerateMipmap(GL_TEXTURE_2D);
    // }
    // else
    // {
    //     std::cout << "Failed to load texture" << std::endl;
    // }
    // stbi_image_free(data);
    // // texture 2
    // // ---------
    // glGenTextures(1, &texture2);
    // glBindTexture(GL_TEXTURE_2D, texture2);
    // // set the texture wrapping parameters
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // // set texture filtering parameters
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // // load image, create texture and generate mipmaps
    // data = stbi_load("resources/textures/smile.png", &width, &height, &nrChannels, 0);
    // if (data)
    // {
    //     // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    //     glGenerateMipmap(GL_TEXTURE_2D);
    // }
    // else
    // {
    //     std::cout << "Failed to load texture" << std::endl;
    // }
    // stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    
    // cube_shader.setInt("wall_texture", 0);
    // ourShader.setInt("texture2", 1);

    float target_frame_rate = 60.0;
    float last_time = static_cast<float>(glfwGetTime());

    Cube cube;

    // render loop
    // -----------
    while (!glfwWindowShouldClose(camera.mWindow))
    {
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // per-frame time logic
        // --------------------
        float current_time = static_cast<float>(glfwGetTime());
        float delta_time = current_time - last_time;
        if(delta_time >= 1.0 / target_frame_rate) {
            last_time = current_time;
        } else {
            continue;
        }

        // input
        // -----
        get_input(camera.mWindow);
        move_camera(camera.mWindow, delta_time, maze, maze_size);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // bind textures on corresponding texture units

        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, texture2);
        cube.bind_texture();


        for (int i = 0; i < cubePositions.size(); i++)
        {

            cube.render(camera, cubePositions[i]);

        }

        floor_shader.use();
        glBindVertexArray(floorVAO);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        floor_shader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        floor_shader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        // model = glm::translate(model, cubePositions[i]);
        floor_shader.setMat4("model", Floor::get_model_mat(maze_size));

        glDrawArrays(GL_TRIANGLES, 0, 6);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(camera.mWindow);
        glfwPollEvents();
    }


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
