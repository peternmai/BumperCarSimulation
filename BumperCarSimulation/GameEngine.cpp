//
//  GameEngine.cpp
//  BumperCarSimulation
//
//  Created by Peter Mai on 3/20/18.
//  Copyright © 2018 William Ma & Peter Mai. All rights reserved.
//

#include "GameEngine.hpp"

GameEngine::GameEngine(std::shared_ptr<Transform>& carSceneGraph, int numberOfCars,
                       std::vector<std::shared_ptr<Geometry>>& possibleCars,
                       std::vector<std::shared_ptr<Transform>>& obstacleDetectionTracker) {
    
    // Generate the specified number of cars
    for(int i = 0; i < numberOfCars; i++) {
        
        // Create vehicle with random position and orientation
        vehicleData newVehicle;
        newVehicle.position.x = rand() % (ARENA_BOUNDRY * 2) - ARENA_BOUNDRY;
        newVehicle.position.y = rand() % (ARENA_BOUNDRY * 2) - ARENA_BOUNDRY;
        newVehicle.orientationDegree = ((rand() % 1000) / 1000.0f) * 360.0f;
        newVehicle.positionTransform = std::make_shared<Transform>(glm::translate(
            glm::mat4(1.0f), glm::vec3(newVehicle.position.x, 0.0f, newVehicle.position.y)));
        newVehicle.rotationTransform = std::make_shared<Transform>(glm::rotate(glm::mat4(1.0f), glm::radians(newVehicle.orientationDegree), glm::vec3(0.0f, 1.0f, 0.0f)));
        std::shared_ptr<Transform> scaleTransform =
            std::make_shared<Transform>( glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 5.0f)));
        
        // Attach vehicle to scene graph
        carSceneGraph->addChild( newVehicle.positionTransform );
        newVehicle.positionTransform->addChild( newVehicle.rotationTransform );
        newVehicle.rotationTransform->addChild( scaleTransform );
        scaleTransform->addChild( possibleCars[ rand() % possibleCars.size() ]);
        
        // Keep track of this vehicle and make sure obstacle detection tracks it
        vehicles.push_back( newVehicle );
        obstacleDetectionTracker.push_back( newVehicle.positionTransform );
    }
}


GameEngine::~GameEngine() {
}

void GameEngine::update() {
    
    // Update the car's movement direction
    for(int i = 0; i < vehicles.size(); i++) {
        
        // Check if vehicle is approaching the left side of the arena
        if( vehicles[i].position.x < -ARENA_BOUNDRY && vehicles[i].orientationDegree >= 180.0f && vehicles[i].orientationDegree <= 360.0f ) {
            vehicles[i].orientationDegree = fmod(((rand() % 1000) / 1000.0f) * 180.0f + 0.0f, 360.0f);
            continue;
        }
        
        // Check if vehicle is approaching the right side of the arena
        if( vehicles[i].position.x > ARENA_BOUNDRY && vehicles[i].orientationDegree >= 0.0f && vehicles[i].orientationDegree <= 180.0f ) {
            vehicles[i].orientationDegree = fmod(((rand() % 1000) / 1000.0f) * 180.0f + 180.0f, 360.0f);
            continue;
        }
        
        // Check if the vehicle is approaching the top side of arena
        if( vehicles[i].position.y < -ARENA_BOUNDRY && vehicles[i].orientationDegree >= 90.0f && vehicles[i].orientationDegree <= 270.0f) {
            vehicles[i].orientationDegree = fmod(((rand() % 1000) / 1000.0f) * 180.0f + 270.0f, 360.0f);
            continue;
        }
        
        // Check if vehicle is approaching the bottom side of the arena
        if( vehicles[i].position.y > ARENA_BOUNDRY && (vehicles[i].orientationDegree >= 270.0f || vehicles[i].orientationDegree <= 90.0f)) {
            vehicles[i].orientationDegree = fmod(((rand() % 1000) / 1000.0f) * 180.0f + 90.0f, 360.0f);
            continue;
        }
    }
    
    // Update the vehicle's transform
    for(int i = 0; i < vehicles.size(); i++) {
        
        // Calculate the new position
        glm::vec2 vehiclePositionDiff;
        vehiclePositionDiff.x = sin(glm::radians(vehicles[i].orientationDegree));
        vehiclePositionDiff.y = cos(glm::radians(vehicles[i].orientationDegree));
        vehiclePositionDiff = glm::normalize(vehiclePositionDiff);
        vehicles[i].position.x = vehicles[i].position.x + vehiclePositionDiff.x;
        vehicles[i].position.y = vehicles[i].position.y + vehiclePositionDiff.y;

        
        // Change the transform of the scene graph
        vehicles[i].rotationTransform->update(glm::rotate(glm::mat4(1.0f), glm::radians(vehicles[i].orientationDegree), glm::vec3(0.0f, 1.0f, 0.0f)));
        vehicles[i].positionTransform->update(glm::translate(
            glm::mat4(1.0f), glm::vec3(vehicles[i].position.x, 0.0f, vehicles[i].position.y)));
    }
}
