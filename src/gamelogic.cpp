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
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include "utilities/imageLoader.hpp"
#include "utilities/objLoader.h"
#include "utilities/glfont.h"

enum KeyFrameAction {
    BOTTOM, TOP
};

#include <timestamps.h>

unsigned int currentKeyFrame = 0;
unsigned int previousKeyFrame = 0;

SceneNode* rootNode;
SceneNode* groundNode;
SceneNode* treeNode;

// These are heap allocated, because they should not be initialised at the start of the program
sf::SoundBuffer* buffer;
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

    //padPositionX -= mouseSensitivity * deltaX / windowWidth;
    //padPositionZ -= mouseSensitivity * deltaY / windowHeight;

    //if (padPositionX > 1) padPositionX = 1;
    //if (padPositionX < 0) padPositionX = 0;
    //if (padPositionZ > 1) padPositionZ = 1;
    //if (padPositionZ < 0) padPositionZ = 0;

    glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
}

//// A few lines to help you if you've never used c++ structs
// struct LightSource {
//     bool a_placeholder_value;
// };
// LightSource lightSources[/*Put number of light sources you want here*/];

void initGame(GLFWwindow* window, CommandLineOptions gameOptions) {
    /*buffer = new sf::SoundBuffer();
    if (!buffer->loadFromFile("../../../res/Hall of the Mountain King.ogg")) {
        return;
    }*/

    options = gameOptions;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetCursorPosCallback(window, mouseCallback);

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

    std::cout << "Ready. Click to start!" << std::endl;
}

void updateFrame(GLFWwindow* window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    double timeDelta = getTimeDeltaSeconds();

    
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
        mouseLeftPressed = true;
        mouseLeftReleased = false;
    } else {
        mouseLeftReleased = mouseLeftPressed;
        mouseLeftPressed = false;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
        mouseRightPressed = true;
        mouseRightReleased = false;
    } else {
        mouseRightReleased = mouseRightPressed;
        mouseRightPressed = false;
    }
    
    glm::mat4 projection = glm::perspective(glm::radians(80.0f), float(windowWidth) / float(windowHeight), 0.1f, 350.f);

    

    // Some math to make the camera move in a nice way
    glm::vec3 cameraPosition = glm::vec3(0, 2, 20);
    float lookRotationY = 0;
    float lookRotationX = 0;
    glm::mat4 cameraTransform = 
                    glm::rotate(lookRotationY, glm::vec3(0, 1, 0)) *
                    glm::rotate(lookRotationX, glm::vec3(1, 0, 0)) *
                    glm::translate(-cameraPosition);

    glm::mat4 VP = projection * cameraTransform;

    // Move and rotate various SceneNodes
    groundNode->position = { 0, -1, 0 };

    treeNode->position = {0, 0, 0};
    treeNode->scale = glm::vec3(1,1,1);

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
