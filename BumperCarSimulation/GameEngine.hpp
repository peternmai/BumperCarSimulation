//
//  GameEngine.hpp
//  BumperCarSimulation
//
//  Created by Peter Mai on 3/20/18.
//  Copyright © 2018 William Ma & Peter Mai. All rights reserved.
//

#ifndef GameEngine_hpp
#define GameEngine_hpp

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <OpenGL/gl.h> // Remove this line in future projects
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <memory>

#include "SceneGraph.hpp"

#include <stdio.h>

#define ARENA_BOUNDARY 95

class GameEngine {
private:
    
    struct vehicleData {
        glm::vec2 position;
        float orientationDegree;
        std::shared_ptr<Transform> positionTransform;
        std::shared_ptr<Transform> rotationTransform;
        int collisionWithCarNumber;
        bool hitWall;
    };
    
    std::vector<vehicleData> vehicles;
    
public:
    
    GameEngine(std::shared_ptr<Transform>& carSceneGraph, int numberOfCars,
               std::vector<std::shared_ptr<Geometry>>& possibleCars,
               std::vector<std::shared_ptr<Transform>>& obstacleDetectionTracker);
    ~GameEngine();
    
    void update();
    
};

#endif /* GameEngine_hpp */
