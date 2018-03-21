//
// CollisionDetectionNP.hpp
// BumperCarSimulation
//
// Created by William Ma on 3/18/2017
// Copyright © 2018 William Ma & Peter Mai. All rights reserved.
//

#ifndef CollisionDetectionNP_hpp
#define CollisionDetectionNP_hpp

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <vector>
#include <algorithm>
#include <iostream>

class CollisionDetectionNP {
public:
	CollisionDetectionNP();
	~CollisionDetectionNP();

	std::vector<std::vector<bool>> intersectionTest(std::vector<std::vector<glm::vec3>> minmaxpts);
};

#endif