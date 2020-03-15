#include <chrono>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <SFML/Audio/SoundBuffer.hpp>
#include <utilities/shader.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <utilities/timeutils.h>
#include <utilities/mesh.h>
#include <utilities/shapes.h>
#include <utilities/glutils.h>
#include <SFML/Audio/Sound.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fmt/format.h>
#include "gamelogic.h"
#include "sceneGraph.hpp"
#include "camera.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include "utilities/imageLoader.hpp"
#include "utilities/objLoader.h"
#include "utilities/glfont.h"

Camera* camera;

SceneNode* rootNode;
SceneNode* groundNode;
SceneNode* treeNode;

// These are heap allocated, because they should not be initialised at the start of the program
Gloom::Shader* shader;
sf::Sound* sound;

const glm::vec3 groundDimensions(100, 1, 100);


CommandLineOptions options;

bool jumpedToNextFrame = false;

bool mouseLeftPressed   = false;
bool mouseLeftReleased  = false;
bool mouseRightPressed  = false;
bool mouseRightReleased = false;

// Modify if you want the music to start further on in the track. Measured in seconds.
const float debug_startTime = 0;
double totalElapsedTime = debug_startTime;
double gameElapsedTime = debug_startTime;

double mouseSensitivity = 1.0;
double lastMouseX = windowWidth / 2;
double lastMouseY = windowHeight / 2;
void mouseCallback(GLFWwindow* window, double x, double y) {
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);

    double deltaX = x - lastMouseX;
    double deltaY = y - lastMouseY;

    camera->Move(glm::vec3(), deltaY * 0.01f, deltaX * 0.01f);

    glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
}

void initGame(GLFWwindow* window, CommandLineOptions gameOptions) {
    options = gameOptions;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetCursorPosCallback(window, mouseCallback);

    // Create camera
    camera = new Camera(glm::vec3(0.0f,1.0f,20.0f), windowWidth, windowHeight);

    // Create shaders
    shader = new Gloom::Shader();
    shader->makeBasicShader("../../../res/shaders/simple.vert", "../../../res/shaders/simple.frag");
    shader->activate();

    // Create meshes
    Mesh ground = cube(groundDimensions, glm::vec2(30, 40));
    Mesh tree = OBJLoader::LoadFromFile("../../../res/meshes/tree1.obj");

    // Fill buffers
    unsigned int groundVAO = generateBuffer(ground);
    unsigned int treeVAO  = generateBuffer(tree);

    // Construct scene
    rootNode = createSceneNode();
    groundNode  = createSceneNode();
    treeNode = createSceneNode();

    rootNode->children.push_back(groundNode);
    rootNode->children.push_back(treeNode);

    groundNode->vertexArrayObjectID = groundVAO;
    groundNode->VAOIndexCount = ground.indices.size();

    treeNode->vertexArrayObjectID = treeVAO;
    treeNode->VAOIndexCount = tree.indices.size();

    getTimeDeltaSeconds();

    std::cout << fmt::format("Initialized scene with {} SceneNodes.", totalChildren(rootNode)) << std::endl;
}

void updateFrame(GLFWwindow* window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    double timeDelta = getTimeDeltaSeconds();

    glm::vec3 movement = glm::vec3();
    if (glfwGetKey(window, GLFW_KEY_A)) {
        movement += glm::vec3(-1.0f, 0.0f, 0.0f);
    } 
    else if(glfwGetKey(window, GLFW_KEY_D)) {
        movement += glm::vec3(1.0f, 0.0f, 0.0f);
    }
    else if (glfwGetKey(window, GLFW_KEY_W)) {
        movement += glm::vec3(0.0f, 0.0f, -1.0f);
    }
    else if (glfwGetKey(window, GLFW_KEY_S)) {
        movement += glm::vec3(0.0f, 0.0f, 1.0f);
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
        movement += glm::vec3(0.0f, -1.0f, 0.0f);
    }
    else if (glfwGetKey(window, GLFW_KEY_SPACE)) {
        movement += glm::vec3(0.0f, 1.0f, 0.0f);
    }
    
    camera->Move(movement, 0, 0);
    
    glm::mat4 VP = camera->Projection() * camera->View();

    // Move and rotate various SceneNodes
    groundNode->position = { 0, -1, 0 };

    treeNode->position = { 0, 0, 0 };
    treeNode->scale = glm::vec3(1, 1, 1);

    updateNodeTransformations(rootNode, VP);

}

void updateNodeTransformations(SceneNode* node, glm::mat4 transformationThusFar) {
    glm::mat4 transformationMatrix =
              glm::translate(node->position)
            * glm::translate(node->referencePoint)
            * glm::rotate(node->rotation.y, glm::vec3(0,1,0))
            * glm::rotate(node->rotation.x, glm::vec3(1,0,0))
            * glm::rotate(node->rotation.z, glm::vec3(0,0,1))
            * glm::scale(node->scale)
            * glm::translate(-node->referencePoint);

    node->currentTransformationMatrix = transformationThusFar * transformationMatrix;

    switch(node->nodeType) {
        case GEOMETRY: break;
        case POINT_LIGHT: break;
        case SPOT_LIGHT: break;
    }

    for(SceneNode* child : node->children) {
        updateNodeTransformations(child, node->currentTransformationMatrix);
    }
}

void renderNode(SceneNode* node) {
    glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(node->currentTransformationMatrix));

    switch(node->nodeType) {
        case GEOMETRY:
            if(node->vertexArrayObjectID != -1) {
                glBindVertexArray(node->vertexArrayObjectID);
                glDrawElements(GL_TRIANGLES, node->VAOIndexCount, GL_UNSIGNED_INT, nullptr);
            }
            break;
        case POINT_LIGHT: break;
        case SPOT_LIGHT: break;
    }

    for(SceneNode* child : node->children) {
        renderNode(child);
    }
}

void renderFrame(GLFWwindow* window) {
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);

    renderNode(rootNode);
}

void exitGame()
{
    delete camera;
    delete treeNode;
    delete groundNode;
    delete rootNode;
}