//
//  Window.cpp
//  BumperCarSimulation
//
//  Created by Peter Mai on 3/4/18.
//  Copyright © 2018 William Ma & Peter Mai. All rights reserved.
//

#include "Window.hpp"
#include <memory>
#include <cmath>

// Specify the title of the program
const char* window_title = "Bumper Car Simulation";

// Stores the shader's program ID
GLint Window::skyboxShaderProgramID;
GLint Window::geometryShaderProgramID;
GLint Window::lineShaderProgramID;

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 0.0f);         // e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 10.0f);    // d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);          // up | What orientation "up" is

// Store basic information regarding camera movement
float Window::lookAt2DPlaneDegree(0.0f);
float Window::lookAtYDegree(0.0f);
float Window::cameraDistanceFromCenter = 400.0f;
float Window::cameraXZ_angle = 0.0f;
float Window::cameraY_angle = 0.0f;

// Store the width and height of the window
int Window::width;
int Window::height;

// Store the projection and view matrix
glm::mat4 Window::P;
glm::mat4 Window::V;

// Store previous cursor location to determine how much cursor has moved
glm::vec2 Window::lastCursorPosition;
glm::vec3 Window::lastPoint;

// Store different graphic objects
std::unique_ptr<SkyBox> Window::skybox;
std::unique_ptr<Group> Window::sceneGraphRoot;
std::unordered_map<int, std::shared_ptr<SceneNode>> Window::sceneMapNodes;

void Window::initialize_objects()
{
    // Load the shader program. Make sure you have the correct filepath up top
    Window::skyboxShaderProgramID = LoadShaders(SKYBOX_VERTEX_SHADER_PATH, SKYBOX_FRAGMENT_SHADER_PATH);
    Window::geometryShaderProgramID = LoadShaders(GEOMETRY_VERTEX_SHADER_PATH, GEOMETRY_FRAGMENT_SHADER_PATH);
    Window::lineShaderProgramID = LoadShaders(LINE_VERTEX_SHADER_PATH, LINE_FRAGMENT_SHADER_PATH);
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
    glDeleteProgram(skyboxShaderProgramID);
    glDeleteProgram(geometryShaderProgramID);
    glDeleteProgram(lineShaderProgramID);
}

GLFWwindow* Window::create_window(int width, int height)
{
    // Initialize GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return NULL;
    }
    
    // 4x antialiasing
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    #ifdef __APPLE__ // Because Apple hates comforming to standards
    // Ensure that minimum OpenGL version is 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Enable forward compatibility and allow a modern OpenGL context
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    
    // Create the GLFW window
    GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);
    
    // Check if the window could not be created
    if (!window)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
        glfwTerminate();
        return NULL;
    }
    
    // Make the context of the window
    glfwMakeContextCurrent(window);
    
    // Set swap interval to 1
    glfwSwapInterval(1);
    
    // Get the width and height of the framebuffer to properly resize the window
    glfwGetFramebufferSize(window, &width, &height);
    // Call the resize callback to make sure things get drawn immediately
    Window::resize_callback(window, width, height);
    
    return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
    #ifdef __APPLE__
    glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
    #endif
    Window::width = width;
    Window::height = height;
    // Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
    glViewport(0, 0, width, height);
    
    if (height > 0)
    {
        P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 5000.0f);
        V = glm::lookAt(cam_pos, cam_look_at, cam_up);
    }
}

void Window::idle_callback()
{
    
}

void Window::display_callback(GLFWwindow* window)
{
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Render the skybox
    skybox->draw(skyboxShaderProgramID);
    
    // Gets events, including input such as keyboard and mouse or window resizing
    glfwPollEvents();
    // Swap buffers
    glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Check for a key press
    if (action == GLFW_PRESS)
    {
        // Check if escape was pressed
        if (key == GLFW_KEY_ESCAPE)
        {
            // Close the window. This causes the program to also terminate.
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        
        else {
            // Do nothing
        }
        
    }
}

glm::vec3 Window::trackBallMapping(GLFWwindow *window, double xpos, double ypos) {
    
    // Get window's width and height
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    
    // Project mouse 2D position onto 3D vritual trackball sphere
    glm::vec3 curPoint;
    curPoint.x = (2.0 * xpos - windowWidth) / windowWidth;
    curPoint.y = (windowHeight - 2.0 * ypos) / windowHeight;
    curPoint.z = 0.0;
    double d = fmin( glm::length(curPoint), 1.0 );
    curPoint.z = sqrtf(1.001f - d * d);
    curPoint = glm::normalize(curPoint);
    
    return curPoint;
}

void Window::cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    // Get window's width and height
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    
    // Determine the direction and velocity of how much we moved
    glm::vec3 curPoint = Window::trackBallMapping(window, xpos, ypos);
    glm::vec3 direction = curPoint - Window::lastPoint;
    float velocity = glm::length(direction);
    
    // Left click is being held - Move camera when camera is at origin
    if( glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ) {
        
        // Calculate the new direction to look at on the XZ plane
        float degreeChange2D = lastCursorPosition.x - xpos;
        Window::lookAt2DPlaneDegree += degreeChange2D / 10.0f;
        if( Window::lookAt2DPlaneDegree > 360.0f || Window::lookAt2DPlaneDegree < -360.0f )
            Window::lookAt2DPlaneDegree = 0.0f;
        
        // Update the y direction of the camera
        float degreeChangeY = lastCursorPosition.y - ypos;
        Window::lookAtYDegree += degreeChangeY / 5.0f;
        if( Window::lookAtYDegree > 90.0f )
            Window::lookAtYDegree = 90.0f;
        if( Window::lookAtYDegree < -90.0f )
            Window::lookAtYDegree = -90.0f;
        
        // Update where the camera is looking at
        cam_look_at = glm::vec3(sin(glm::radians( Window::lookAt2DPlaneDegree )),
                                sin(glm::radians( Window::lookAtYDegree )),
                                cos(glm::radians( Window::lookAt2DPlaneDegree )));
        
        // Update the view transformation
        V = glm::lookAt(cam_pos, cam_look_at, cam_up);
        
    }
    
    // Right click is being held and move
    if( glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS ) {
        
    }
    
    // Keep track of the last point
    Window::lastPoint = curPoint;
    Window::lastCursorPosition = glm::vec2(xpos, ypos);
}

void Window::scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    
}
