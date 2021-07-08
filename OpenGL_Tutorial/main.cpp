#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Tile.h"

#include <iostream>
#include <string>
#include <math.h>

// Callback & helper functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);

void printVec(glm::vec3 v);
void printNum(float num);

void setArray(float arr[], glm::vec3 v, int ind);
void setAllVertices(float arr[], Tile* T);


// Screen settings
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 800;

bool blinn = true;
bool blinnKeyPressed = false;

// Camera
Camera camera(glm::vec3(0.0f, 0.2f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// Track tiles
std::vector<Tile*> Tile::tiles;
std::vector<Tile*> Tile::next;
std::vector<Tile*> Tile::created;

int main()
{
    /* --------------------------------------------------------------------------------- */

    /* GLFW initialization */
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // Multisampling

    // Create window object
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Callback functions for [window resize], [mouse movement], [scrolling]
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Capture cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* --------------------------------------------------------------------------------- */

    // Initialize GLAD before calling an OpenGL function
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable depth testing so objects render in correct order
    glEnable(GL_DEPTH_TEST);
    // Enable multisampling (already enabled by default)
    glEnable(GL_MULTISAMPLE);

    /* --------------------------------------------------------------------------------- */

    // Build shader programs
    Shader shader("shader.vs", "shader.fs");
    Shader lightCubeShader("lightcube.vs", "lightcube.fs");

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    // Box positions in world space
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  2.0f,  0.0f),
    };
    // Point light positions
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };
    float planeVertices[] = {
        // positions         // normals         // texcoords
         1.0f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        -1.0f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        -1.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,

         1.0f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        -1.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
         1.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f
    };

    // Plane VAO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(0);

    // Boxes VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Boxes VBO
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Element Buffer Object
    /*unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

    // position
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Light VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Load textures
    unsigned int diffuseMap = loadTexture("container2.png");
    unsigned int specularMap = loadTexture("container2_specular.png");
    unsigned int floorTexture = loadTexture("wood.png");


    Tile* curTile = new Tile("O");
    curTile->setStart(glm::vec3(0, 0, 0));
    bool whichStart = true;

    glm::vec3 test(0, 1, 0);
    test = translateZ(test, 0.6268697);
    test = translateX(test, 0.5306375);
    
    test = translateX(test, -1.061275);
    test = translateZ(test, 1.061275);
    test = translateX(test, 1.061275);
    printVec(test);

    printVec(curTile->Right->Up->Left->TR);
    printVec(curTile->Right->Up->Left->TL);
    printVec(curTile->Right->Up->Left->BR);
    printVec(curTile->Right->Up->Left->BL);

    // Rendering loop - runs until GLFW is instructed to close
    while (!glfwWindowShouldClose(window))
    {
        // Track time since last frame
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input
        processInput(window);

        // Background color
        glClearColor(0.529f, 0.808f, 0.98f, 1.0f);
        // Clear color buffer and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Make shader and rendering calls use our shader program (and the linked shaders)
        shader.use();
        //shader.setVec3("viewPos", glm::vec3(camera.Position);


        // World transformation
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        // View transformation
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);
        // Projection transformation
        glm::mat4 projection = glm::perspective(glm::radians(camera.FOV), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        shader.setMat4("projection", projection);
        // Custom transformation
        glm::mat4 transform = glm::mat4(1.0f); // Identity matrix
        //transform = glm::translate(transform, glm::vec3(0.0f, 2.0f, 0.0f));
        //transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        shader.setMat4("transform", transform);


        // Bind tile/plane VAO
        glBindVertexArray(planeVAO);

        
        float distCur = curTile->center.y;
        // Up
        if (distCur > curTile->Up->center.y)
        {
            camera.Position.z = asinh(curTile->Up->center.z);
            camera.Position.x = asinh(curTile->Up->center.x / cosh(camera.Position.z));
            //camera.Position.z += 2 * 0.5306375;
            curTile = curTile->Up;
            whichStart = true;
        }
        // Right
        else if (distCur > curTile->Right->center.y)
        {
            printVec(camera.Position);
            camera.Position.x = asinh(curTile->Right->center.x);
            camera.Position.z = asinh(curTile->Right->center.z / cosh(camera.Position.x));
            //camera.Position.x += 2 * 0.5306375;
            curTile = curTile->Right;
            whichStart = false;
            printVec(camera.Position);
            std::cout << "\n";
        }
        // Down
        else if (distCur > curTile->Down->center.y)
        {
            printVec(camera.Position);
            camera.Position.z = asinh(curTile->Down->center.z);
            camera.Position.x = asinh(curTile->Down->center.x / cosh(camera.Position.z));
            //camera.Position.z -= 2 * 0.5306375;
            curTile = curTile->Down;
            whichStart = true;
            printVec(camera.Position);
            std::cout << "\n";
        }
        else if (distCur > curTile->Left->center.y)
        {
            camera.Position.z = asinh(curTile->Left->center.z);
            camera.Position.x = asinh(curTile->Left->center.x / cosh(camera.Position.z));
            //camera.Position.x -= 2 * 0.5306375;
            curTile = curTile->Left;
            whichStart = false;
        }

        if (whichStart)
            curTile->setStart(camera.Position);
        else
            curTile->setStart(camera.Position);

        for (Tile* t : Tile::tiles)
        {
            setAllVertices(planeVertices, t);
            shader.setVec4("color", t->color);
            glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        // Bind box textures
        /*glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // Render the boxes
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < sizeof(cubePositions)/sizeof(cubePositions[0]); i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            //float angle = 20.0f * i;
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }*/

        // Draw the point lights
        /*lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            lightCubeShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }*/

        glfwSwapBuffers(window); // swap the color buffer (color values for each pixel in GLFW's window)
        glfwPollEvents(); // check for events (i.e. kb or mouse), update the window state, call corresponding functions
    }

    // Clean resources allocated for GLFW
    glfwTerminate();

    // Free memory allocated for all tiles recursively
    delete curTile;

    return 0;
}

// Callback function for when window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

// Process keyboard input
void processInput(GLFWwindow* window)
{
    // Close window on esc press
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // WASD to move
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime, true);
        //printVec(camera.Position);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime, true);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime, true);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime, true);

    // Shift to sprint
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.StartSprint();
    else
        camera.EndSprint();

    // B to toggle phong/blinn-phong
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
    {
        blinn = !blinn;
        blinnKeyPressed = true;
        std::cout << (blinn ? "Blinn-Phong" : "Phong") << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        blinnKeyPressed = false;
    }
}

// Process mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) // Ignore first mouse movement to prevent initial jumps
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// Load 2D texture from file
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_RED;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set the texture wrapping/filtering options (on the currently bound texture object)
        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
    }

    stbi_image_free(data);
    return textureID;
}

// Print a vec3
void printVec(glm::vec3 v)
{
    std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
}

// Print a float
void printNum(float num)
{
    std::cout << num << std::endl;
}

// Set consecutive elements in array to a vec3
void setArray(float arr[], glm::vec3 v, int ind)
{
    arr[ind] = v.x;
    arr[ind + 1] = v.y;
    arr[ind + 2] = v.z;
}

void setAllVertices(float arr[], Tile* tile)
{
    setArray(arr, getPoincare(tile->TR), 0);
    setArray(arr, getPoincare(tile->TL), 8);
    setArray(arr, getPoincare(tile->BL), 16);
    setArray(arr, getPoincare(tile->TR), 24);
    setArray(arr, getPoincare(tile->BL), 32);
    setArray(arr, getPoincare(tile->BR), 40);
    /*setArray(arr, getBeltrami(tile->TR), 0);
    setArray(arr, getBeltrami(tile->TL), 8);
    setArray(arr, getBeltrami(tile->BL), 16);
    setArray(arr, getBeltrami(tile->TR), 24);
    setArray(arr, getBeltrami(tile->BL), 32);
    setArray(arr, getBeltrami(tile->BR), 40);*/
}