//
//  Window.hpp
//  BumperCarSimulation
//
//  Created by Peter Mai on 3/4/18.
//  Copyright © 2018 William Ma & Peter Mai. All rights reserved.
//

#ifndef Window_hpp
#define Window_hpp

#include <iostream>

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
#include "OBJObject.hpp"
#include "SkyBox.hpp"
#include "SceneGraph.hpp"
#include "shader.hpp"

// Specify the location of each of the shader files
#define SKYBOX_VERTEX_SHADER_PATH "../Shaders/skyboxShader.vert"
#define SKYBOX_FRAGMENT_SHADER_PATH "../Shaders/skyboxShader.frag"
#define GEOMETRY_VERTEX_SHADER_PATH "../Shaders/geometryShader.vert"
#define GEOMETRY_FRAGMENT_SHADER_PATH "../Shaders/geometryShader.frag"
#define LINE_VERTEX_SHADER_PATH "../Shaders/lineShader.vert"
#define LINE_FRAGMENT_SHADER_PATH "../Shaders/lineShader.frag"

// Specify the location of each of the object file
#define BALLOON_OBJECT_PATH "../Objects/Low-Poly_airship.obj"
#define RECTANGULAR_OBJECT_PATH "../Objects/RectangleBox.obj"

// Specify where the skybox data is
#define SKYBOX_PATH "../Skybox/rainb"

// Define each scene graph's key
#define SCENE_GRAPH_HEAD_MOVEMENT 1
#define SCENE_GRAPH_LEFT_ARM_MOVEMENT 2
#define SCENE_GRAPH_RIGHT_ARM_MOVEMENT 3
#define SCENE_GRAPH_LEFT_LEG_MOVEMENT 4
#define SCENE_GRAPH_RIGHT_LEG_MOVEMENT 5
#define SCENE_GRAPH_SQUAD_ROTATE 6
#define SCENE_GRAPH_SQUAD_MOVEMENT 7
#define SCENE_GRAPH_SQUAD_SCALE 8

class Window
{
    
private:
    
    static float lookAt2DPlaneDegree;
    static float lookAtYDegree;
    
    // Store cursor information for controlling camera
    static glm::vec2 lastCursorPosition;
    static glm::vec3 lastPoint;
    static glm::vec3 trackBallMapping(GLFWwindow * window, double xpos, double ypos);
    
    // Information about camera in 3D space
    static float cameraDistanceFromCenter;
    static float cameraXZ_angle;
    static float cameraY_angle;
    
    // Store information about the skybox
    static GLint skyboxShaderProgramID;
    static std::unique_ptr<SkyBox> skybox;
    
    // Store information about the scene graph
    static GLint geometryShaderProgramID;
    static std::unique_ptr<Group> sceneGraphRoot;
    static std::unordered_map<int, std::shared_ptr<SceneNode>> sceneMapNodes;
    
    // Store information about the lines
    static GLint lineShaderProgramID;
    
    
public:
    
    // Stores the width and height of the window
    static int width;
    static int height;
    
    // Store the projection and the view matrix
    static glm::mat4 P;
    static glm::mat4 V;
    
    // Forward declaration to initialize and clean up object
    static void initialize_objects();
    static void clean_up();
    static GLFWwindow* create_window(int width, int height);
    
    // Forward declaration for each of the callbacks
    static void resize_callback(GLFWwindow* window, int width, int height);
    static void idle_callback();
    static void display_callback(GLFWwindow*);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursor_position_callback(GLFWwindow * window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow * window, double xoffset, double yoffset);
};

#endif /* Window_hpp */
