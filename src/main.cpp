

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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, const char **maze, int maze_width, int maze_depth);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

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

    const char **maze;
    int maze_width, maze_depth;
    std::tie(maze, maze_width, maze_depth) = get_maze();

    std::vector<glm::vec3> cubePositions = Cube::get_positions(maze, maze_width, maze_depth);

    unsigned int cubeVAO, cubeVBO;
    std::tie(cubeVAO, cubeVBO) = Cube::get_buffers(cube_vertices, cube_vertices_size);

    unsigned int floorVAO, floorVBO;
    std::tie(floorVAO, floorVBO) = Floor::get_buffers(floor_vertices, floor_vertices_size);

    camera.Position = Cube::get_camera_position(maze, maze_width, maze_depth);

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
        processInput(window, maze, maze_width, maze_depth);

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
        floor_shader.setMat4("model", Floor::get_model_mat(maze_width, maze_depth));

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

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window, const char **maze, int maze_width, int maze_depth)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    glm::vec3 movement_vec = glm::vec3(0.0f);
    glm::vec3 frontMovement = glm::normalize(glm::vec3(camera.Front.x, 0.0, camera.Front.z));
    glm::mat3 camera_base = glm::mat3(frontMovement, camera.Up, camera.Right);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        // camera.ProcessKeyboard(FORWARD, deltaTime);
        movement_vec.x = 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        // camera.ProcessKeyboard(BACKWARD, deltaTime);
        movement_vec.x = -1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        // camera.ProcessKeyboard(LEFT, deltaTime);
        movement_vec.z = -1.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        // camera.ProcessKeyboard(RIGHT, deltaTime);
        movement_vec.z = 1.0f;

    glm::vec3 camera_mov_vec = camera_base * (movement_vec * deltaTime * camera.MovementSpeed);

    int old_i, old_j, new_i, new_j;

    std::tie(old_i, old_j) = Cube::get_block_in_position(maze_width, maze_depth, camera.Position.x, camera.Position.z);
    // std::cout << "block = " << old_pos_block << std::endl;

    std::tie(new_i, new_j) = Cube::get_block_in_position(
        maze_width,
        maze_depth,
        camera.Position.x + camera_mov_vec.x,
        camera.Position.z + camera_mov_vec.z
    );

    // if (maze[new_i][new_j] == 'x') {
    //     if (old_i != new_i) {
    //         camera_mov_vec.x = 0.0f;
    //     } else if (old_j != new_j) {
    //         camera_mov_vec.z = 0.0f;
    //     } else {
    //         camera_mov_vec.x = 0.0f;
    //         camera_mov_vec.z = 0.0f;
    //     }
    // }

    camera.Position += camera_mov_vec;

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
