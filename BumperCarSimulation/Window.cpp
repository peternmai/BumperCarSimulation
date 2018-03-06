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
glm::vec3 cam_pos(0.0f, 0.0f, 500.0f);         // e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 10.0f);    // d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);          // up | What orientation "up" is

// Store basic information regarding camera movement
float Window::lookAt2DPlaneDegree(0.0f);
float Window::lookAtYDegree(0.0f);
float Window::cameraDistanceFromCenter = 500.0f;
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
    
    // Load in each of the objects as geometry node for the scene graph
    std::shared_ptr<Geometry> balloon = std::make_shared<Geometry>(BALLOON_OBJECT_PATH, geometryShaderProgramID);
    std::shared_ptr<Geometry> car1 = std::make_shared<Geometry>(CAR_01_OBJECT_PATH, geometryShaderProgramID);
    std::shared_ptr<Geometry> stadium = std::make_shared<Geometry>(RECTANGULAR_OBJECT_PATH, geometryShaderProgramID);
    
    // Attach race track to whole floating race track group
    std::shared_ptr<Group> floatingRaceTrack = std::make_shared<Group>();
    std::shared_ptr<Transform> stadiumScale =
        std::make_shared<Transform>( glm::scale(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 100.0f)));
    stadiumScale->addChild( stadium );
    floatingRaceTrack->addChild( stadiumScale );
    
    // Attach hot air balloon
    std::shared_ptr<Transform> balloonScale =
        std::make_shared<Transform>( glm::scale(glm::mat4(1.0f), glm::vec3(100.0f, 100.0f, 100.0f) ));
    balloonScale->addChild( balloon );
    std::shared_ptr<Transform> balloonMoveUp =
        std::make_shared<Transform>( glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 250.0f, 0.0f)));
    balloonMoveUp->addChild( balloonScale );
    floatingRaceTrack->addChild( balloonMoveUp );
    
    // Attach car
    std::shared_ptr<Transform> scaleCar1 =
        std::make_shared<Transform>( glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 5.0f)));
    scaleCar1->addChild( car1 );
    std::shared_ptr<Transform> moveCarsAboveGround =
        std::make_shared<Transform>( glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.0f, 0.0f)));
    moveCarsAboveGround->addChild( scaleCar1 );
    floatingRaceTrack->addChild( moveCarsAboveGround );
    
    // Attach stuffs to root of scene graph
    sceneGraphRoot = std::make_unique<Group>();
    sceneGraphRoot->addChild( floatingRaceTrack );
    
    // Initialize skybox
    Window::skybox = std::make_unique<SkyBox>(SKYBOX_PATH, 1000.0f);
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
        P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 3000.0f);
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
    
    // Draw objects in scene graph
    sceneGraphRoot->draw(glm::mat4(1.0f));
    
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
        Window::cameraXZ_angle += degreeChange2D / 10.0f;
        if( Window::cameraXZ_angle > 360.0f || Window::cameraXZ_angle < -360.0f )
            Window::cameraXZ_angle = 0.0f;
        
        
        // Update the y direction of the camera
        float degreeChangeY = lastCursorPosition.y - ypos;
        Window::cameraY_angle += degreeChangeY / 5.0f;
        if( Window::cameraY_angle > 89.0f )
            Window::cameraY_angle = 89.0f;
        if( Window::cameraY_angle < -89.0f )
            Window::cameraY_angle = -89.0f;
        
        float alpha = glm::radians(cameraXZ_angle);
        float beta = glm::radians(cameraY_angle);
        
        // Update camera position and where we're looking
        cam_pos = glm::vec3(sin(alpha) * cos(beta), sin(beta), cos(alpha) * cos(beta));
        V = glm::lookAt(cam_pos * cameraDistanceFromCenter, cam_look_at, cam_up);
        
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
    cameraDistanceFromCenter += (float) yoffset;
    
    if( cameraDistanceFromCenter < 50.0f )
        cameraDistanceFromCenter = 50.0f;
    if( cameraDistanceFromCenter > 800.0f )
        cameraDistanceFromCenter = 800.0f;
    
    V = glm::lookAt(cam_pos * cameraDistanceFromCenter, cam_look_at, cam_up);
}
