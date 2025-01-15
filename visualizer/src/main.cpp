#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include "camera.h"
#include "space.h"
#include "configs.h"



// Globals
Camera camera;
Space space;
bool leftMousePressed = false;
double lastMouseX = 0.0, lastMouseY = 0.0;
float horizontalAngle = 0.0f, verticalAngle = 0.0f; // 카메라의 회전 각도
float orbitRadius = 5.0f;                           // 궤도 반지름

// Function Prototypes
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void setupViewport(const GLuint width, const GLuint height);

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Orbiting Camera", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    setupViewport(800, 600);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Set callbacks
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
    
    /*********************************************/
    /******************set scene******************/
    /*********************************************/

    space.addPoint(glm::vec3(0.0f, 0.0f, 0.0f));  // (0,0,0)
    space.addPoint(glm::vec3(1.0f, 1.0f, 0.0f));  // (1,1,0)
    space.addPoint(glm::vec3(-1.0f, -1.0f, 0.0f));  // (-1,-1,0)
    space.addPoint(glm::vec3(-1.0f, -1.0f, -1.0f));  // (-1,-1,-1)
    space.addPoint(glm::vec3(1.0f, 1.0f, -1.0f));  // (1,1,-1)
    space.addObj(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    space.addLine(glm::vec3(1.1f, 2.2f, -1.2f), glm::vec3(-5.0f, 10.0f, 13.0f), 1.0f, 0.0f, 1.0f);


    // Add central object
    space.addPoint(glm::vec3(0.0f, 0.0f, 0.0f));  // 중앙 객체

    // Add Floor
    for (int i = 0; i < 100; ++i) {
        float angle = glm::radians(i * 360.0f / 100.0f);
        space.addPoint(glm::vec3(glm::cos(angle) * 2.0f, glm::sin(angle) * 2.0f, 0.0f));
    }

    /*********************************************/
    /******************set scene******************/
    /*********************************************/

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update camera position based on spherical coordinates
        float x = orbitRadius * glm::cos(verticalAngle) * glm::sin(horizontalAngle);
        float y = orbitRadius * glm::sin(verticalAngle);
        float z = orbitRadius * glm::cos(verticalAngle) * glm::cos(horizontalAngle);
        camera.setPosition(glm::vec3(x, y, z));
        camera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));

        // Apply camera view matrix
        glm::mat4 view = camera.getViewMatrix();
        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(&view[0][0]);

        // Render scene
        space.render();

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_W) {
            verticalAngle += TRANSLATION_SPEED * ORBIT_SPEED;
        } else if (key == GLFW_KEY_S) {
            verticalAngle -= TRANSLATION_SPEED * ORBIT_SPEED;
        } else if (key == GLFW_KEY_A) {
            horizontalAngle -= TRANSLATION_SPEED * ORBIT_SPEED;
        } else if (key == GLFW_KEY_D) {
            horizontalAngle += TRANSLATION_SPEED * ORBIT_SPEED;
        } else if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        leftMousePressed = (action == GLFW_PRESS);
    }
}


void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    if (leftMousePressed) {
        double dx = xpos - lastMouseX;
        double dy = ypos - lastMouseY;

        horizontalAngle += (float)(dx * ORBIT_SPEED);
        verticalAngle -= (float)(dy * ORBIT_SPEED);
        
    }

    lastMouseX = xpos;
    lastMouseY = ypos;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    orbitRadius -= (float)(yoffset * ZOOM_SPEED);
    if (orbitRadius < 1.0f) orbitRadius = 1.0f; // 최소 거리 제한
}

void setupViewport(const GLuint width, const GLuint height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, static_cast<double>(width) / height, 0.1, 100.0);
}
