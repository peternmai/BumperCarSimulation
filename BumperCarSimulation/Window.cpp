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
GLint Window::bbShaderProgramID;
GLuint Window::skyboxShaderProgramID;
GLuint Window::geometryShaderProgramID;
GLuint Window::lineShaderProgramID;
GLuint Window::shadowFirstPassShaderProgramID;
GLuint Window::shadowMapShaderProgramID;
GLuint Window::particleShaderProgramID;

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
std::shared_ptr<Group> Window::sceneGraphRoot;
std::unordered_map<int, std::shared_ptr<SceneNode>> Window::sceneMapNodes;

// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
CollisionDetectionNP cdnp;
BoundingBox* boundingbox;

std::vector<glm::vec3> carFaces;
std::vector<std::shared_ptr<Transform>> transformVector;
std::vector<std::vector<bool>> collisions;


// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
// for linear fog
bool Window::linearFog;
std::vector<std::shared_ptr<Geometry>> geometryVector;

// Relating to the sun
std::unique_ptr<OBJObject> Window::sun;
float Window::sunDegree;
glm::mat4 Window::sunTransform;
glm::vec3 Window::sunLightDirection;

// Shadow Map
GLuint Window::FBO;
GLuint Window::depthMapID;

// Particles
std::unique_ptr<Particles> Window::particles;

// Game Engine
std::unique_ptr<GameEngine> Window::gameEngine;

// User Input and the Default Values
bool Window::showShadowMap = false;
bool Window::showShadows = true;
bool Window::showParticles = true;

void Window::initialize_objects()
{
    // Load the shader program. Make sure you have the correct filepath up top
    Window::skyboxShaderProgramID = LoadShaders(SKYBOX_VERTEX_SHADER_PATH, SKYBOX_FRAGMENT_SHADER_PATH);
    Window::geometryShaderProgramID = LoadShaders(GEOMETRY_VERTEX_SHADER_PATH, GEOMETRY_FRAGMENT_SHADER_PATH);
    Window::lineShaderProgramID = LoadShaders(LINE_VERTEX_SHADER_PATH, LINE_FRAGMENT_SHADER_PATH);
	Window::bbShaderProgramID = LoadShaders(BBOX_VERTEX_SHADER_PATH, BBOX_FRAGMENT_SHADER_PATH);
    Window::particleShaderProgramID = LoadShaders(PARTICLE_VERTEX_SHADER_PATH, PARTICLE_FRAGMENT_SHADER_PATH);
    Window::shadowMapShaderProgramID = LoadShaders(SHADOW_MAP_VERTEX_SHADER, SHADOW_MAP_FRAGMENT_SHADER);
    Window::shadowFirstPassShaderProgramID = LoadShaders(SHADOW_FIRST_PASS_VERTEX_SHADER_PATH, SHADOW_FIRST_PASS_FRAGMENT_SHADER_PATH);
    
    // Load in the sun
    Window::sun = std::make_unique<OBJObject>(SPHERE_OBJECT_PATH, 2.0f);
    Window::sun->setPhongCoefficient(1.0f, 0.0f, 0.0f, 0.0f);
    Window::sun->setShowShadow(false);
    Window::sunDegree = 0.0f;
    
    // Load in shadow map
    initShadowMap();
    
    // Load in particle class
    particles = std::make_unique<Particles>();
    
    // Load in each of the objects as geometry node for the scene graph
    std::shared_ptr<Geometry> balloon = std::make_shared<Geometry>(BALLOON_OBJECT_PATH, geometryShaderProgramID, shadowFirstPassShaderProgramID, shadowMapShaderProgramID, bbShaderProgramID);
    std::shared_ptr<Geometry> car1 = std::make_shared<Geometry>(CAR_01_OBJECT_PATH, geometryShaderProgramID, shadowFirstPassShaderProgramID, shadowMapShaderProgramID, bbShaderProgramID);
    std::shared_ptr<Geometry> stadium = std::make_shared<Geometry>(RECTANGULAR_OBJECT_PATH, geometryShaderProgramID, shadowFirstPassShaderProgramID, shadowMapShaderProgramID, bbShaderProgramID);

	// Need Geometry for FOG
	geometryVector.push_back(balloon);
	geometryVector.push_back(stadium);
	geometryVector.push_back(car1);

	boundingbox = car1->getBoundingBoxPointer();
	boundingbox->toggle(); // turn on any object's bounding boxes
	carFaces = boundingbox->getFaces(); //get the bounding box's faces
    
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

	// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
	// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
	std::shared_ptr<Transform> scaleCar2 =
		std::make_shared<Transform>(glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 5.0f)));
	scaleCar2->addChild(car1);
	std::shared_ptr<Transform> extraCartoRight =
		std::make_shared<Transform>(glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0.0f, 2.0f)));

	transformVector.push_back(scaleCar1);
	transformVector.push_back(scaleCar2);

	extraCartoRight->addChild(scaleCar2); // I added!
    moveCarsAboveGround->addChild( scaleCar1 );
	moveCarsAboveGround->addChild(extraCartoRight); // I added!
    floatingRaceTrack->addChild( moveCarsAboveGround );
    
    // Create the game engine and attach to scene graph
    std::vector<std::shared_ptr<Geometry>> vehicles;
    vehicles.push_back(car1);
    Window::gameEngine = std::make_unique<GameEngine>(moveCarsAboveGround, TOTAL_CARS, vehicles, transformVector);
    
    // Attach stuffs to root of scene graph
    sceneGraphRoot = std::make_unique<Group>();
    sceneGraphRoot->addChild( floatingRaceTrack );
    
    // Initialize skybox
    Window::skybox = std::make_unique<SkyBox>(SKYBOX_PATH, 1000.0f);

	// Set linear fog to false
	// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
	// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
	Window::linearFog = false;
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
    glDeleteProgram(skyboxShaderProgramID);
    glDeleteProgram(geometryShaderProgramID);
    glDeleteProgram(lineShaderProgramID);
    
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &depthMapID);
}

GLFWwindow* Window::create_window(int width, int height)
{
    // Initialize GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return NULL;
    }
    
    // 16x antialiasing
    glfwWindowHint(GLFW_SAMPLES, 16);
    
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
    // Rotate the sun
    if(sunDegree < 180.0f)
        sunDegree = fmod(sunDegree + 0.25f, 360.0f);
    else
        sunDegree = fmod(sunDegree + 2.0f, 360.0f);
    float sunRadian = glm::radians(sunDegree);
    glm::mat4 sunScale = glm::scale(glm::mat4(1.0f), glm::vec3(20.0f, 20.0f, 20.0f));
    sunTransform = glm::translate(glm::mat4(1.0f),
                                  glm::vec3(500 * cos(sunRadian), 500 * sin(sunRadian),0.0f)) * sunScale;
    sunLightDirection = glm::vec3(sunTransform * glm::vec4(1.0f)) * -1.0f;
    
    // Update all the particles
    particles->update();
    
    // Update all the vehicles
    gameEngine->update();
}

void Window::display_callback(GLFWwindow* window)
{
    // Bind to shadow map buffer for write
    glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    // First pass: Generate shadow depth map
    sceneGraphRoot->firstPassShadowMap(glm::mat4(1.0f));
    
    // Unbind from the frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Clear the color and depth buffers
    glViewport(0, 0, width, height);
    glBindTexture(GL_TEXTURE_2D, depthMapID);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// color everything in background to fog
	if(linearFog) glClearColor(0.5, 0.5, 0.5, 1);
    
	// Render the skybox only if linearFog isn't on
    // Render the skybox
	if(!linearFog) skybox->draw(skyboxShaderProgramID);
    
    // Draw objects in scene graph
    sceneGraphRoot->draw(glm::mat4(1.0f));
	
	std::vector<std::vector<glm::vec3>> totalTrans;

	for (auto i = transformVector.begin(); i != transformVector.end(); i++) {
		glm::mat4 temp_car_trans = (*i)->getLastTrans();

		std::vector<glm::vec3> temp_vector;
		for (int j = 0; j < carFaces.size(); j++) {
			glm::vec4 temp = temp_car_trans * glm::vec4(carFaces[j], 1.0f);
			glm::vec3 correct_temp = glm::vec3(temp.x, temp.y, temp.z);
			temp_vector.push_back(correct_temp);
		}

		totalTrans.push_back(temp_vector);

	}

	collisions = cdnp.intersectionTest(totalTrans);

	/*
	std::cout << "BOOL:" << std::endl;

	for (int i = 0; i < collisions.size(); i++) {
		std::cout << "CAR " << i << " ";
		for (int j = 0; j < collisions[i].size(); j++) {
			if (collisions[i][j] == true) {
				std::cout << " true,";
			}
			else {
				std::cout << " false,";
			}
		}
		std::cout << " end" << std::endl;
	}
	*/

	for (auto i = geometryVector.begin(); i != geometryVector.end(); i++) {

	}

	boundingbox->setCol(collisions);
	
    
    // Draw Sun
    sun->draw(geometryShaderProgramID, sunTransform);
    
    // Draw the particles
    if( showParticles )
        particles->draw(particleShaderProgramID);
    
    // Draw out the shadow depth map
    if( showShadowMap ) {
        glScissor(0, 0, width/5, width/5);
        glEnable(GL_SCISSOR_TEST);
        glViewport(0, 0, width/5, width/5);
		glClearColor(1.0, 1.0, 1.0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        sceneGraphRoot->drawShadowMap(glm::mat4(1.0f));
        glDisable(GL_SCISSOR_TEST);
    }
    
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
		if (key == GLFW_KEY_B) boundingbox->toggle();
		if (key == GLFW_KEY_F) {
			Window::linearFog = !Window::linearFog;
			for (auto i = geometryVector.begin(); i != geometryVector.end(); i++) {
				(*i)->toggleFog();
			}

			if (linearFog)
				showShadowMap = false;
		}

        // Check if escape was pressed
        if (key == GLFW_KEY_ESCAPE)
        {
            // Close the window. This causes the program to also terminate.
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        
        // Turn shadow feature on and off
        else if( key == GLFW_KEY_S && mods == GLFW_MOD_SHIFT) {
            showShadows = !showShadows;
            showShadowMap = false;
        }
        
        // Toggle shadow map display
        else if( key == GLFW_KEY_S ) {
            if( showShadows && !linearFog )
                showShadowMap = !showShadowMap;
        }
        
        // Toggle Particles
        else if( key == GLFW_KEY_P ) {
            showParticles = !showParticles;
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
    cameraDistanceFromCenter -= 30.0f * (float) yoffset;
    
    if( cameraDistanceFromCenter < 50.0f )
        cameraDistanceFromCenter = 50.0f;
    if( cameraDistanceFromCenter > 800.0f )
        cameraDistanceFromCenter = 800.0f;

	float alpha = glm::radians(cameraXZ_angle);
	float beta = glm::radians(cameraY_angle);

	float x = cameraDistanceFromCenter * sin(alpha) * cos(beta);
	float y = cameraDistanceFromCenter * sin(beta);
	float z = cameraDistanceFromCenter * cos(alpha) * cos(beta);

	cam_pos = glm::vec3(x, y, z);
    V = glm::lookAt(cam_pos, cam_look_at, cam_up);
}

glm::vec3 Window::getSunLightDirection() {
    return sunLightDirection;
}

GLuint Window::getDepthMapTextureID() {
    return depthMapID;
}

float Window::getSunRadian() {
    return glm::radians(sunDegree);
}

void Window::initShadowMap()
{
    // Create a Frame Buffer Object (FBO) to store the depth map from the light source
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    
    // Create 2D texture to represent depth map
    glGenTextures(1, &depthMapID);
    glBindTexture(GL_TEXTURE_2D, depthMapID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOW_MAP_WIDTH,
                 SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Attach the depth texture as the FBO's depth buffer
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMapID, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Ensure that our Frame Buffer is good
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        exit(-1);
}
