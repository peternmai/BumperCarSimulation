//
//  BoundingBox.hpp
//  BumperCarSimulation
//
//  Created by William Ma on 3/11/2018
//  Copyright © 2018 William Ma & Peter Mai. All rights reserved.
//

#ifndef BoundingBox_hpp
#define BoundingBox_hpp

#include <iostream>

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


class BoundingBox {
public:

	// constructor and destructor
	BoundingBox(std::vector<float> extrema);
	~BoundingBox();


	// functions to give the bounding box its requirements to be able to draw
	// mainly setters and getters
	void drawFaces(glm::mat4 & transformation);
	void initializePos(glm::vec3 & scale, glm::vec3 & translation);
	void toggle();
	void setShaderID(GLint shaderID);
	void setupBoxLines(std::vector<float> pts);
	std::vector<glm::vec3> getFaces();
	void setCol(std::vector<bool> colFaces);
	int count = 0;


private:

	// data storage
	std::vector<float> boxlines;
	std::vector<glm::vec3> faces;

	// things required for drawing the box
	GLint shaderID;
	GLuint bbVAO, bbVBO;
	GLuint uProjection, uModelView, uColor;


	// mathematical characteristics of the bounding box
	glm::mat4 toWorld;
	bool prevCollision = false;
	glm::vec3 originalScale = glm::vec3(1.0f);
	glm::vec3 originalTrans = glm::vec3(1.0f);
	glm::mat4 identity = glm::mat4(1.0f);
	std::vector<bool> collisionFaces;


	// functions to draw the points
	void draw(glm::mat4 & transformation, bool col);
	void facepts(glm::vec3 pt1, glm::vec3 pt2, glm::vec3 pt3, glm::vec3 pt4);
	void drawpts(glm::vec3 pt0, glm::vec3 pt1, glm::vec3 pt2, glm::vec3 pt3);


};



#endif