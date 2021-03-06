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
#include "ssao.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include "utilities/imageLoader.hpp"
#include "utilities/objLoader.h"
#include "utilities/glfont.h"
#include "utilities/textures.h"
#include "firefly.h"

#define RESOURCE_PATH std::string("../../../res/")

Camera* camera;
Fireflies* fireflies;
SSAO* ssao;
int renderMode = 0; // 0-Normal, 1-NoSSAO, 2-SSAONoise, 3-SSAOBlur

// Objects
SceneNode* rootNode;
SceneNode* groundNode;
unsigned int squareVAO;
unsigned int squareIndexCount;


// These are heap allocated, because they should not be initialised at the start of the program
Gloom::Shader* objectShader;
Gloom::Shader* postShader;

// Textures
Textures::RenderTarget g_buffer;

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

namespace
{
    void addTree(SceneNode* parent, const glm::vec3& pos, unsigned int treeVAO, unsigned int treeTextureID, unsigned int treeIndices)
    {
        SceneNode* treeNode;

        treeNode = createSceneNode();
        parent->children.push_back(treeNode);
        treeNode->vertexArrayObjectID = treeVAO;
        treeNode->VAOIndexCount = treeIndices;
        treeNode->textureID = treeTextureID;
        treeNode->position = pos;
        treeNode->scale = glm::vec3(1, 1, 1);
    }
}

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

    std::cout << "Use WASD, Left-shift, Enter and mouse to move camera." << std::endl;
    std::cout << "Press 1, 2, 3 or 4 to change SSAO rendering mode." << std::endl;

    // Create camera
    camera = new Camera(glm::vec3(0.0f,3.0f,12.0f), windowWidth, windowHeight);

    // Create shaders
    objectShader = new Gloom::Shader();
    objectShader->makeBasicShader(RESOURCE_PATH + "shaders/object.vert", RESOURCE_PATH + "shaders/object.frag");
    postShader = new Gloom::Shader();
    postShader->makeBasicShader(RESOURCE_PATH + "shaders/post.vert", RESOURCE_PATH + "shaders/post.frag");

    // Create textures
    g_buffer = Textures::CreateRenderTarget(windowWidth, windowHeight, { {GL_RGBA, GL_UNSIGNED_BYTE}, {GL_RGBA16F, GL_FLOAT } }, true);
    unsigned int treeTextureID = Textures::LoadPNG(RESOURCE_PATH + "textures/tree.png");
    unsigned int groundTextureID = Textures::LoadPNG(RESOURCE_PATH + "textures/ground.png");

    // Create meshes
    Mesh ground = cube(groundDimensions, glm::vec2(30, 40));
    Mesh tree = OBJLoader::LoadFromFile(RESOURCE_PATH + "meshes/tree1.obj");
    Mesh square = generateSquare();

    // Fill buffers
    unsigned int groundVAO = generateBuffer(ground);
    unsigned int treeVAO  = generateBuffer(tree);
    squareVAO = generateBuffer(square);
    squareIndexCount = square.indices.size();

    // Construct scene
    rootNode = createSceneNode();
    groundNode  = createSceneNode();

    rootNode->children.push_back(groundNode);

    groundNode->vertexArrayObjectID = groundVAO;
    groundNode->VAOIndexCount = ground.indices.size();
    groundNode->textureID = groundTextureID;


    // Add trees
    addTree(rootNode, { 0,0,0 }, treeVAO, treeTextureID, tree.indices.size());
    for (int x = -20; x < 20; x += 4)
    for (int z = -20; z < 20; z += 4)
    {
        if (abs(x) + abs(z) > 8)
        {

            glm::vec3 pos = { (float)x, 0.0, (float)z };
            pos += glm::vec3((float)rand() / RAND_MAX - 0.5f, 0.0f, (float)rand() / RAND_MAX - 0.5f) * 4.0f;
            addTree(rootNode, pos, treeVAO, treeTextureID, tree.indices.size());
        }
    }

    fireflies = new Fireflies(rootNode, windowWidth, windowHeight, 300, RESOURCE_PATH);

    ssao = new SSAO(windowWidth, windowHeight, RESOURCE_PATH);

    getTimeDeltaSeconds();

    std::cout << fmt::format("Initialized scene with {} SceneNodes.", totalChildren(rootNode)) << std::endl;
}

void updateFrame(GLFWwindow* window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    double timeDelta = getTimeDeltaSeconds();
    totalElapsedTime += timeDelta;

    glm::vec3 movement = glm::vec3();
    if (glfwGetKey(window, GLFW_KEY_A))             movement += glm::vec3(-1.0f, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_D))             movement += glm::vec3(1.0f, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_W))             movement += glm::vec3(0.0f, 0.0f, -1.0f);
    if (glfwGetKey(window, GLFW_KEY_S))             movement += glm::vec3(0.0f, 0.0f, 1.0f);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))    movement += glm::vec3(0.0f, -1.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_SPACE))         movement += glm::vec3(0.0f, 1.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_1)) {
        renderMode = 0;
    }
    else if (glfwGetKey(window, GLFW_KEY_2)) {
        renderMode = 1;
    }
    else if (glfwGetKey(window, GLFW_KEY_3)) {
        renderMode = 2;
    }
    else if (glfwGetKey(window, GLFW_KEY_4)) {
        renderMode = 3;
    }
    
    camera->Move(movement*0.1f, 0, 0);
    
    // Move and rotate various SceneNodes
    groundNode->position = { 0, -0.5, 0 };

    fireflies->Update(totalElapsedTime);

    updateNodeTransformations(rootNode, camera->View(), camera->Projection());

}

void updateNodeTransformations(SceneNode* node, glm::mat4 transformationThusFar, const glm::mat4& P) {
    glm::mat4 transformationMatrix =
              glm::translate(node->position)
            * glm::translate(node->referencePoint)
            * glm::rotate(node->rotation.y, glm::vec3(0,1,0))
            * glm::rotate(node->rotation.x, glm::vec3(1,0,0))
            * glm::rotate(node->rotation.z, glm::vec3(0,0,1))
            * glm::scale(node->scale)
            * glm::translate(-node->referencePoint);

    node->MV = transformationThusFar * transformationMatrix;
    node->MVP = P * node->MV;

    switch(node->nodeType) {
        case GEOMETRY: break;
        case POINT_LIGHT: break;
        case SPOT_LIGHT: break;
    }

    for(SceneNode* child : node->children) {
        updateNodeTransformations(child, node->MV, P);
    }
}

void renderNode(SceneNode* node) {
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(node->MV));
    glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(node->MVP));
    glBindTextureUnit(0, node->textureID);

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

    glBindFramebuffer(GL_FRAMEBUFFER, g_buffer.targetID);
    //glClearColor(0.3f, 0.5f, 0.8f, 1.0f);
    glClearColor(0.7f, 1.0f, -100.0f, 1.0f);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    objectShader->activate();
    renderNode(rootNode);

    ssao->Render(
        g_buffer.textureIDs[1], 
        camera->Projection(), 
        camera->InvProjection(),
        renderMode
    );

    if (renderMode <= 1)
    {
        fireflies->RenderLights(
            g_buffer.textureIDs[1],
            g_buffer.depthID,
            camera->View(),
            camera->Projection()
        );

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glBindTextureUnit(0, g_buffer.textureIDs[0]);
        glBindTextureUnit(1, g_buffer.textureIDs[1]);
        glBindTextureUnit(2, fireflies->GetFireflyTexture());
        glBindTextureUnit(3, fireflies->GetLightTexture());
        glBindTextureUnit(4, ssao->GetTexture());
        glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(camera->InvProjection()));
        postShader->activate();
        glBindVertexArray(squareVAO);
        glDrawElements(GL_TRIANGLES, squareIndexCount, GL_UNSIGNED_INT, nullptr);
        glEnable(GL_DEPTH_TEST);

        fireflies->RenderFlies(
            g_buffer.depthID,
            camera->View(),
            camera->Projection()
        );
    }
    
}

void exitGame()
{
    delete camera;
    delete groundNode;
    delete rootNode;
}