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
#include <thread>
#include <cstdio>
#include <math.h>

using namespace std;

// Callback & helper functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);

// Print glm::dvec3
void printVec(glm::dvec3 v);

void setArray(double arr[], glm::dvec3 v, int ind);
void setAllVertices(double arr[], Tile* T);

// Screen settings
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 800;

// Camera
Camera camera(glm::vec3(0.0f, 1.0f, 0.0f)); // Height is alterable in Camera.GetViewMatrix()
double lastX = SCR_WIDTH / 2.0f;
double lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

double deltaTime = 0.0f; // Time between current frame and last frame
double lastFrame = 0.0f; // Time of last frame
double changed = 0.0f;   // Time of last tile change

// Limit the max number of threads (performance will tank otherwise)
const unsigned int MAX_THREADS = 1;
unsigned int numThreads = 0;

// Static vectors for tracking tiles
vector<Tile*> Tile::visible;
vector<Tile*> Tile::next;
vector<Tile*> Tile::all;
queue<Tile*> Tile::parents;

// Number of edges per tile and number of tiles per vertex
const int n = 6;
const int k = 8;
const double rad = circleRadius(n, k);

// Call python script to generate image; run in parallel to OpenGL
void genImg(vector<Tile*> t, vector<Tile*> worldTiles, unsigned int ind);

// Manage image generations
size_t index1 = 1; // tile1.png, tile2.png, ...
queue<vector<Tile*>> waiting;
queue<glm::vec3> coords;
queue<vector<Tile*>> pending;

vector<thread> allThreads;

void error_callback(int error, const char* msg) {
    std::string s;
    s = " [" + std::to_string(error) + "] " + msg + '\n';
    std::cerr << s << std::endl;
}

int main() {
    /* --------------------------------------------------------------------------------- */

    /* GLFW initialization */
    glfwSetErrorCallback(error_callback);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // Multisampling

    // Create window object
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mercator", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
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
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // Enable depth testing so objects render in correct order
    glEnable(GL_DEPTH_TEST);
    // Enable multisampling (already enabled by default)
    glEnable(GL_MULTISAMPLE);

    /* --------------------------------------------------------------------------------- */

    // Build shader programs
    Shader shader("shader.vs", "shader.fs");
    Shader imageShader("image.vs", "image.fs");

    double vertices[] = {
        // positions         // normals        // texture coords
         1.0,  1.0, 0.0,  0.0, 1.0, 0.0,  1.0, 0.0,
        -1.0,  1.0, 0.0,  0.0, 1.0, 0.0,  0.0, 0.0,
        -1.0, -1.0, 0.0,  0.0, 1.0, 0.0,  0.0, 1.0,

         1.0,  1.0, 0.0,  0.0, 1.0, 0.0,  1.0, 0.0,
        -1.0, -1.0, 0.0,  0.0, 1.0, 0.0,  0.0, 1.0,
         1.0, -1.0, 0.0,  0.0, 1.0, 0.0,  1.0, 1.0
    };

    double planeVertices[3 * (n - 2) * 8] = { 0.0 };
    
    /*
    double planeVertices[] = {
        // positions         // normals         // texcoords
         1.0f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        -1.0f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        -1.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,

         1.0f, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
        -1.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
         1.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f
    };
    */

    // Plane VAO/VBO
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 8 * sizeof(double), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 8 * sizeof(double), (void*)(3 * sizeof(double)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_DOUBLE, GL_FALSE, 8 * sizeof(double), (void*)(6 * sizeof(double)));
    glBindVertexArray(0);

    // Image VAO/VBO
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 8 * sizeof(double), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_DOUBLE, GL_FALSE, 8 * sizeof(double), (void*)(3 * sizeof(double)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_DOUBLE, GL_FALSE, 8 * sizeof(double), (void*)(6 * sizeof(double)));
    glEnableVertexAttribArray(2);

    /*unsigned int diffuseMap = loadTexture("container2.png");
    unsigned int specularMap = loadTexture("container2_specular.png");
    unsigned int floorTexture = loadTexture("wood.png");*/

    unsigned int placeholder = loadTexture("placeholder.png");

    // Initialize origin
    Tile* curTile = new Tile(n, k);
    Tile::all.push_back(curTile);

    curTile->setStart(glm::vec3(0, 0, 0));
    //curTile->Down->texture = loadTexture("gaben.png");


    // Test of future, async, launch::async
    // #include <future>
    //auto fut = async(launch::async, genImg, glm::vec3(0.1, 0, 0.2), curTile, 0, placeholder);
    
    //glm::vec3 test1 = line(glm::vec3(0, 1, 0), glm::vec3(0, 1, 1), 1.0612750619);
    //printVec(test1);
    //printVec(line(test1, glm::vec3(0, 1, 0), 1.0612750619));

    // Set random seed
    srand(time(0));

    // Rendering loop - runs until GLFW is instructed to close
    while (!glfwWindowShouldClose(window)) {
        // Track time since last frame
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input
        processInput(window);

        // Background color
        glClearColor(0.529f, 0.808f, 0.98f, 1.0f);
        //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        // Clear color buffer and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Setup shader
        shader.use();

        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);

        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);

        glm::mat4 projection = glm::perspective(glm::radians(camera.FOV), (double)SCR_WIDTH / (double)SCR_HEIGHT, 0.1, 100.0);
        shader.setMat4("projection", projection);

        glm::mat4 transform = glm::mat4(1.0f); // Identity matrix
        //transform = glm::translate(transform, glm::vec3(0.0f, 2.0f, 0.0f));
        //transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        shader.setMat4("transform", transform);

        // Setup image shader
        imageShader.use();
        imageShader.setMat4("model", model);
        imageShader.setMat4("view", view);
        imageShader.setMat4("projection", projection);

        // Check for tile change
        if (currentFrame - changed > 0.3) {
            double distCur = curTile->center.y;
            for (Tile* neighbor : curTile->getNeighbors()) {
                if (distCur > neighbor->center.y) {
                    curTile = neighbor;
                    camera.Position = getXZ(curTile->center);

                    glm::vec3 reversed = reverseXZ(curTile->vertices.at(0)->getPos(), camera.Position.x, camera.Position.z);
                    double ang = atan2(reversed.z, reversed.x);
                    curTile->angle = ang;

                    changed = currentFrame;
                    break;
                }
            }
        }

        // Update tiles to be created/rendered based on current tile
        curTile->setStart(camera.Position);

        // Generate images for megatiles
        if (!Tile::parents.empty()) {
            vector<Tile*> megatile;

            Tile* p = Tile::parents.front();
            
            p->texture = placeholder;
            megatile.push_back(p);
            
            for (Tile* t : p->getNeighbors()) {
                if (t->parent == p) {
                    t->texture = placeholder;
                    megatile.push_back(t);
                }
            }

            Tile::parents.pop();
            waiting.push(megatile);
        }

        // Megatiles waiting to be threaded
        if (!waiting.empty()) {
            if (numThreads < MAX_THREADS) {
                numThreads++;

                // Find nearby tiles that already have images / latent vectors
                vector<Tile*> worldTiles;
                for (Tile* t : Tile::visible) {
                    if (t->queueNum != -1)
                        worldTiles.push_back(t);
                }

                vector<Tile*> megatile = waiting.front();
                allThreads.emplace_back(thread(genImg, megatile, worldTiles, index1));
                index1 += megatile.size();
                waiting.pop();
            }
        }

        // Link tiles with fully generated images
        while (!pending.empty()) {
            // Load texture from generated image for the tile
            vector<Tile*> megatile = pending.front();
            for (auto& t : megatile) {
                string name = "../world_data/images/tile" + to_string(t->queueNum) + ".png";
                t->texture = loadTexture(name.c_str());
            }
            pending.pop();
            numThreads--;
        }

        // Draw tiles (and images)
        for (Tile* t : Tile::visible) {
            shader.use();
            setAllVertices(planeVertices, t);
            shader.setVec4("color", t->color);
            glBindVertexArray(planeVAO);
            glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
            glDrawArrays(GL_TRIANGLES, 0, 3 * (n - 2));

            if (t->texture != -1)
            {
                imageShader.use();
                model = glm::translate(glm::dmat4(1.0f), getPoincare(t->center));
                // float imgScale = glm::distance(getPoincare(t->TL), getPoincare(t->BR));
                float imgScale = rad * 0.3;
                model = glm::scale(model, glm::vec3(imgScale));
                model = glm::translate(model, glm::vec3(0, 1, 0));
                glm::dvec3 target = glm::dvec3(0) - getPoincare(t->center);
                model = glm::rotate(model, (float) atan2(-target.z, target.x) + glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                imageShader.setMat4("model", model);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, t->texture);
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }

        glfwSwapBuffers(window); // swap the color buffer (color values for each pixel in GLFW's window)
        glfwPollEvents(); // check for events (i.e. kb or mouse), update the window state, call corresponding functions
    }

    // Clean resources allocated for GLFW
    glfwTerminate();

    // Delete generated images after joining all threads
    for (auto& th : allThreads)
        th.join();
    /*for (int i = 0; i < index1; i++)
    {
        string name = to_string(i).append(".png");
        remove(name.c_str());
    }*/
    //std::remove("image_sampler.pkl");

    // Free tile memory
    for (Tile* t : Tile::all)
        delete t;

    return 0;
}

void genImg(vector<Tile*> mega, vector<Tile*> worldTiles, unsigned int ind) {
    //t->texture = placeholder; // set placeholder earlier
    string coords = to_string(worldTiles.size());

    for (auto& tile : worldTiles) {
        glm::dvec3 c = tile->center;
        coords += " " + to_string(tile->queueNum) + " " + to_string(c.x) + " " + to_string(c.z);
    }

    for (auto& tile : mega) {
        glm::dvec3 c = tile->center;
        coords += " " + to_string(c.x) + " " + to_string(c.z);
        tile->queueNum = ind;
        ind++;
    }

    string input = "python ../sendrequest.py " + coords;
    //input.append(" " + to_string(ind));
    system(input.c_str());

    pending.push(mega);
}

// Callback function for when window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

// Process keyboard input
void processInput(GLFWwindow* window) {
    // Close window on esc press
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // WASD to move
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
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
}

// Process mouse movement
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse){ // Ignore first mouse movement to prevent initial jumps
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

// Load 2D texture from file
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
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
        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
        cout << "Texture failed to load at path: " << path << endl;

    stbi_image_free(data);
    return textureID;
}

// Print a dvec3
void printVec(glm::dvec3 v) {
    cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << endl;
}

// Set consecutive elements in array to a dvec3
void setArray(double arr[], glm::dvec3 v, int ind) {
    arr[ind] = v.x;
    arr[ind + 1] = v.y;
    arr[ind + 2] = v.z;
}

void setAllVertices(double arr[], Tile* tile) {
    int count = 0;
    for (int i = 0; i < n - 2; i++) {
        setArray(arr, getPoincare(tile->vertices.at(0)->getPos()), 8 * count++);
        setArray(arr, getPoincare(tile->vertices.at(i + 1)->getPos()), 8 * count++);
        setArray(arr, getPoincare(tile->vertices.at(i + 2)->getPos()), 8 * count++);
    }

    /*for (int i = 0; i < n - 2; i++) {
        setArray(arr, getBeltrami(tile->vertices.at(0)->getPos()), 8 * count++);
        setArray(arr, getBeltrami(tile->vertices.at(i + 1)->getPos()), 8 * count++);
        setArray(arr, getBeltrami(tile->vertices.at(i + 2)->getPos()), 8 * count++);
    }*/
}