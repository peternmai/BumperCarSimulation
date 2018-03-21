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
#include "CollisionDetectionNP.hpp"
#include "Particles.hpp"
#include "GameEngine.hpp"

// Specify the location of each of the shader files
#define SKYBOX_VERTEX_SHADER_PATH "../Shaders/skyboxShader.vert"
#define SKYBOX_FRAGMENT_SHADER_PATH "../Shaders/skyboxShader.frag"
#define GEOMETRY_VERTEX_SHADER_PATH "../Shaders/geometryShader.vert"
#define GEOMETRY_FRAGMENT_SHADER_PATH "../Shaders/geometryShader.frag"
#define LINE_VERTEX_SHADER_PATH "../Shaders/lineShader.vert"
#define LINE_FRAGMENT_SHADER_PATH "../Shaders/lineShader.frag"
#define BBOX_VERTEX_SHADER_PATH "../Shaders/boundingboxShader.vert"
#define BBOX_FRAGMENT_SHADER_PATH "../Shaders/boundingboxShader.frag"
#define SHADOW_FIRST_PASS_VERTEX_SHADER_PATH "../Shaders/shadowFirstPass.vert"
#define SHADOW_FIRST_PASS_FRAGMENT_SHADER_PATH "../Shaders/shadowFirstPass.frag"
#define SHADOW_MAP_VERTEX_SHADER "../Shaders/shadowDepthMap.vert"
#define SHADOW_MAP_FRAGMENT_SHADER "../Shaders/shadowDepthMap.frag"
#define PARTICLE_VERTEX_SHADER_PATH "../Shaders/particles.vert"
#define PARTICLE_FRAGMENT_SHADER_PATH "../Shaders/particles.frag"

// Specify the location of each of the object file
#define BALLOON_OBJECT_PATH "../Objects/Blimp.obj"
#define CAR_01_OBJECT_PATH "../Objects/Car1.obj"
#define CAR_02_OBJECT_PATH "../Objects/Car2.obj"
#define CAR_03_OBJECT_PATH "../Objects/Car3.obj"
#define CAR_04_OBJECT_PATH "../Objects/Car4.obj"
#define CAR_05_OBJECT_PATH "../Objects/Car5.obj"
#define RECTANGULAR_OBJECT_PATH "../Objects/RectangleBox.obj"
#define SPHERE_OBJECT_PATH "../Objects/sphere.obj"

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

// Define shadow map specifications
#define SHADOW_MAP_WIDTH  10000
#define SHADOW_MAP_HEIGHT 10000
#define NEAR_PLANE 200.0
#define FAR_PLANE 800.0

// Define the number of cars
#define TOTAL_CARS 10

class Window
{
    
private:
    
    static float lookAt2DPlaneDegree;
    static float lookAtYDegree;
    
    // Store all the car information
    static std::unique_ptr<GameEngine> gameEngine;
    
    // Store cursor information for controlling camera
    static glm::vec2 lastCursorPosition;
    static glm::vec3 lastPoint;
    static glm::vec3 trackBallMapping(GLFWwindow * window, double xpos, double ypos);
    
    // Information about camera in 3D space
    static float cameraDistanceFromCenter;
    static float cameraXZ_angle;
    static float cameraY_angle;
    
    // Store information about the skybox
    static GLuint skyboxShaderProgramID;
    static std::unique_ptr<SkyBox> skybox;
    
    // Store information about the scene graph
    static GLuint geometryShaderProgramID;
    static std::shared_ptr<Group> sceneGraphRoot;
    static std::unordered_map<int, std::shared_ptr<SceneNode>> sceneMapNodes;
    
	// Store fog information
	static GLint bbShaderProgramID;
	static bool linearFog;

	// Store information about the lines
    static GLuint lineShaderProgramID;
    
    // Relating to the sun
    static float sunDegree;
    static std::unique_ptr<OBJObject> sun;
    static glm::mat4 sunTransform;
    static glm::vec3 sunLightDirection;
    
    // Shadow map related
    static GLuint shadowFirstPassShaderProgramID, shadowMapShaderProgramID;
    static GLuint FBO, depthMapID;
    static void initShadowMap();
    
    // Particle effects
    static GLuint particleShaderProgramID;
    static std::unique_ptr<Particles> particles;
    
    // Camera rotatation
    static void rotateCameraAroundOrigin();
    
public:
    
    // User Inputs
    static bool showShadowMap;
    static bool showShadows;
    static bool showParticles;
    static bool rotateCamera;
    static bool rotateSun;
    
    // Stores the width and height of the window
    static int width;
    static int height;
    
    // Store the projection and the view matrix
    static glm::mat4 P;
    static glm::mat4 V;
	static glm::vec3 eyePos;

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
    
    // Accessor forward declaration
    static glm::vec3 getSunLightDirection();
    static GLuint getDepthMapTextureID();
    static float getSunRadian();
};

#endif /* Window_hpp */
