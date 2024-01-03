

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
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

// settings
const unsigned int SCR_WIDTH = 900;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;


void get_input(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

glm::vec3 get_movement_from_input(GLFWwindow *window) {
    glm::vec3 movement_vec = glm::vec3(0.0f);
    glm::vec3 frontMovement = glm::vec3(camera.Front.x, 0.0, camera.Front.z);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        movement_vec += camera.Front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        movement_vec += -camera.Front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        movement_vec += -camera.Right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        movement_vec += camera.Right;

    return movement_vec;
}

glm::vec2 rectCollide(glm::vec2 oldPos, glm::vec2 newPos, float size1, glm::vec2 pos2, float size2) {
    glm::vec2 result = glm::vec2(0.0f, 0.0f);

    if (
        newPos.x + size1 < pos2.x ||
        newPos.x - size1 > pos2.x + size2 * size2 ||
        oldPos.y + size1 < pos2.y ||
        oldPos.y - size1 > pos2.y + size2 * size2
    )
        result.x = 1.0f;

    if (
        oldPos.x + size1 < pos2.x ||
        oldPos.x - size1 > pos2.x + size2 * size2 ||
        newPos.y + size1 < pos2.y ||
        newPos.y - size1 > pos2.y + size2 * size2
    )
        result.y = 1.0f;

    return result;
}

glm::vec3 checkCollision(glm::vec2 oldPos, glm::vec2 newPos, int maze_size) {
    glm::vec2 collisionVector = glm::vec2(1.0f, 1.0f);
    glm::vec2 movementVector = newPos - oldPos;

    if(movementVector.length() > 0) {
        float blockSize = 1.0f;
        float objectSize = 0.2;

        int old_i, old_j;
        std::tie(old_i, old_j) = Cube::get_indexes_from_position(maze_size, oldPos.x, oldPos.y);

        int new_i, new_j;
        std::tie(new_i, new_j) = Cube::get_indexes_from_position(maze_size, newPos.x, newPos.y);

        glm::vec3 block1 = Cube::get_position_from_indexes(new_i, new_j, maze_size);
        glm::vec3 block2 = Cube::get_position_from_indexes(old_i, new_j, maze_size);
        glm::vec3 block3 = Cube::get_position_from_indexes(new_i, old_j, maze_size);

        glm::vec2 block1_vec2 = glm::vec2(block1.z, block1.x);
        glm::vec2 block2_vec2 = glm::vec2(block2.z, block2.x);
        glm::vec2 block3_vec2 = glm::vec2(block3.z, block3.x);

        glm::vec2 collide1 = rectCollide(oldPos, newPos, objectSize, block1_vec2, blockSize);
        glm::vec2 collide2 = rectCollide(oldPos, newPos, objectSize, block2_vec2, blockSize);
        glm::vec2 collide3 = rectCollide(oldPos, newPos, objectSize, block3_vec2, blockSize);

        collisionVector = collisionVector * collide1 * collide2 * collide3;
    }

    return glm::vec3(collisionVector.x, 0.0f, collisionVector.y);
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void move_camera(GLFWwindow *window, char **maze, int maze_size)
{
    float mov_delta = 0.01f;
    glm::vec3 movement_vec = get_movement_from_input(window);

    if (glm::length(movement_vec) <= mov_delta) {
        return;
    }
    movement_vec = glm::normalize(movement_vec);

    // glm::vec3 collision_vector = checkCollision(
    //     glm::vec2(camera.Position.z, camera.Position.x),
    //     glm::vec2(camera.Position.z, camera.Position.x) + glm::vec2(movement_vec.z, movement_vec.x),
    //     maze_width,
    //     maze_depth
    // );

    // movement_vec = collision_vector * movement_vec;

    if (glm::length(movement_vec) <= mov_delta) {
        return;
    }
    movement_vec = (deltaTime * camera.MovementSpeed) * glm::normalize(movement_vec);

    camera.Position += movement_vec;

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    Shader cube_shader("shaders/cube.vert", "shaders/cube.frag");
    Shader floor_shader("shaders/floor.vert", "shaders/floor.frag");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    float *cube_vertices;
    int cube_vertices_size;
    std::tie(cube_vertices, cube_vertices_size) = Cube::get_vertices();

    float *floor_vertices;
    int floor_vertices_size;
    std::tie(floor_vertices, floor_vertices_size) = Floor::get_vertices();

    char **maze;
    int maze_size;
    std::tie(maze, maze_size) = get_maze();

    std::vector<glm::vec3> cubePositions = Cube::get_positions(maze, maze_size);

    unsigned int cubeVAO, cubeVBO;
    std::tie(cubeVAO, cubeVBO) = Cube::get_buffers(cube_vertices, cube_vertices_size);

    unsigned int floorVAO, floorVBO;
    std::tie(floorVAO, floorVBO) = Floor::get_buffers(floor_vertices, floor_vertices_size);

    camera.Position = Cube::get_camera_position(maze, maze_size);

    // load and create a texture 
    // -------------------------
    unsigned int wall_texture, texture2;
    // texture 1
    // ---------
    glGenTextures(1, &wall_texture);
    glBindTexture(GL_TEXTURE_2D, wall_texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
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
    cube_shader.use();
    cube_shader.setInt("wall_texture", 0);
    // ourShader.setInt("texture2", 1);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        get_input(window);
        move_camera(window, maze, maze_size);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wall_texture);
        // glActiveTexture(GL_TEXTURE1);
        // glBindTexture(GL_TEXTURE_2D, texture2);

        // activate shader
        cube_shader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        cube_shader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        cube_shader.setMat4("view", view);

        // render boxes
        glBindVertexArray(cubeVAO);
        for (unsigned int i = 0; i < cubePositions.size(); i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, cubePositions[i]);
            // model = glm::translate(model, glm::vec3(0.0, 0.5, 0.0));
            // float angle = 20.0f * i;
            // model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            cube_shader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        floor_shader.use();
        glBindVertexArray(floorVAO);

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        floor_shader.setMat4("projection", projection);

        // camera/view transformation
        view = camera.GetViewMatrix();
        floor_shader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        // model = glm::translate(model, cubePositions[i]);
        floor_shader.setMat4("model", Floor::get_model_mat(maze_size));

        glDrawArrays(GL_TRIANGLES, 0, 6);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
