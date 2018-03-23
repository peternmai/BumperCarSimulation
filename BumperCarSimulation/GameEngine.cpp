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
        newVehicle.position.x = rand() % (ARENA_BOUNDARY * 2) - ARENA_BOUNDARY;
        newVehicle.position.y = rand() % (ARENA_BOUNDARY * 2) - ARENA_BOUNDARY;
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
        vehicles[i].hitWall = false;
        
        // Check if vehicle is approaching the left side of the arena
        if( vehicles[i].position.x < -ARENA_BOUNDARY && vehicles[i].orientationDegree >= 180.0f && vehicles[i].orientationDegree <= 360.0f ) {
            vehicles[i].orientationDegree = fmod(((rand() % 1000) / 1000.0f) * 180.0f + 0.0f, 360.0f);
            vehicles[i].hitWall = true;
            continue;
        }
        
        // Check if vehicle is approaching the right side of the arena
        if( vehicles[i].position.x > ARENA_BOUNDARY && vehicles[i].orientationDegree >= 0.0f && vehicles[i].orientationDegree <= 180.0f ) {
            vehicles[i].orientationDegree = fmod(((rand() % 1000) / 1000.0f) * 180.0f + 180.0f, 360.0f);
            vehicles[i].hitWall = true;
            continue;
        }
        
        // Check if the vehicle is approaching the top side of arena
        if( vehicles[i].position.y < -ARENA_BOUNDARY && vehicles[i].orientationDegree >= 90.0f && vehicles[i].orientationDegree <= 270.0f) {
            vehicles[i].orientationDegree = fmod(((rand() % 1000) / 1000.0f) * 180.0f + 270.0f, 360.0f);
            vehicles[i].hitWall = true;
            continue;
        }
        
        // Check if vehicle is approaching the bottom side of the arena
        if( vehicles[i].position.y > ARENA_BOUNDARY && (vehicles[i].orientationDegree >= 270.0f || vehicles[i].orientationDegree <= 90.0f)) {
            vehicles[i].orientationDegree = fmod(((rand() % 1000) / 1000.0f) * 180.0f + 90.0f, 360.0f);
            vehicles[i].hitWall = true;
            continue;
        }
    }
    
    // Clear past collision information
    for(int i = 0; i < vehicles.size(); i++)
        vehicles[i].collisionWithCarNumber = -1;
    
    // Check for collision
    for(int i = 0; i < vehicles.size(); i++) {
        for(int j = 0; j < vehicles.size(); j++) {
            if(i != j) {
                if( glm::length( vehicles[i].position - vehicles[j].position ) < 10.0f )
                    vehicles[i].collisionWithCarNumber = j;
            }
        }
    }
    
    // If car has collision with another car, change course
    for(int i = 0; i < vehicles.size(); i++) {
        
        // No collisions, no worries
        if( vehicles[i].collisionWithCarNumber == -1 )
            continue;
        
        // Check to see if car is heading towards colliding car
        vehicleData curCar = vehicles[i];
        vehicleData otherCar = vehicles[vehicles[i].collisionWithCarNumber];
        float collidingCarAbsoluteDegree = glm::degrees(atan2(otherCar.position.y - curCar.position.y,
                                                              otherCar.position.x - curCar.position.x));
        if( abs(curCar.orientationDegree - collidingCarAbsoluteDegree) >= 180.0f )
            continue;
        
        // Car is heading towards other car, turn away
        vehicles[i].orientationDegree = collidingCarAbsoluteDegree + 90.0f +
                                        ((rand() % 1000) / 1000.0f) * 180.0f;
        if( vehicles[i].orientationDegree < 0.0f )
            vehicles[i].orientationDegree += 360.0f;
        vehicles[i].orientationDegree = fmod(vehicles[i].orientationDegree, 360.0f);
    }
    
    // Update the vehicle's transform
    for(int i = 0; i < vehicles.size(); i++) {
    
        // Calculate the new position
        if( !(vehicles[i].hitWall && vehicles[i].collisionWithCarNumber != -1) ) {
            glm::vec2 vehiclePositionDiff;
            vehiclePositionDiff.x = sin(glm::radians(vehicles[i].orientationDegree));
            vehiclePositionDiff.y = cos(glm::radians(vehicles[i].orientationDegree));
            vehiclePositionDiff = glm::normalize(vehiclePositionDiff);
            vehicles[i].position.x = vehicles[i].position.x + vehiclePositionDiff.x;
            vehicles[i].position.y = vehicles[i].position.y + vehiclePositionDiff.y;
        }
        
        // Make sure no vehicle escape the arena
        if( vehicles[i].position.x < 0 )
            vehicles[i].position.x = fmax(vehicles[i].position.x, -ARENA_BOUNDARY-5);
        else
            vehicles[i].position.x = fmin(vehicles[i].position.x, ARENA_BOUNDARY+5);
        if( vehicles[i].position.y < 0 )
            vehicles[i].position.y = fmax(vehicles[i].position.y, -ARENA_BOUNDARY-5);
        else
            vehicles[i].position.y = fmin(vehicles[i].position.y, ARENA_BOUNDARY+5);
        

        // Change the transform of the scene graph
        vehicles[i].rotationTransform->update(glm::rotate(glm::mat4(1.0f), glm::radians(vehicles[i].orientationDegree), glm::vec3(0.0f, 1.0f, 0.0f)));
        vehicles[i].positionTransform->update(glm::translate(
            glm::mat4(1.0f), glm::vec3(vehicles[i].position.x, 0.0f, vehicles[i].position.y)));
    }
}

std::vector<bool> GameEngine::getCols() {

	// getting the collision status of each car
	std::vector<bool> temp(vehicles.size(), false);
	for (int i = 0; i < temp.size(); i++) {
		if (vehicles[i].collisionWithCarNumber != -1) temp[i] = true;
	}

	return temp;
}
