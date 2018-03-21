//
//  BoundingBox.cpp
//  BumperCarSimulation
//
//  Created by William Ma 3/11/2018
//  Copyright © 2018 William Ma & Peter Mai. All rights reserved.
//


#include "BoundingBox.hpp"
#include "Window.hpp"

BoundingBox::BoundingBox(std::vector<float> extrema) {
	
	std::vector<bool> temp_bools(4, false);
	collisionFaces.push_back(temp_bools);

	setupBoxLines(extrema);

	glGenVertexArrays(1, &bbVAO);
	glBindVertexArray(bbVAO);

	glGenBuffers(1, &bbVBO);
	glBindBuffer(GL_ARRAY_BUFFER, bbVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxlines[0]) * boxlines.size(),
		&boxlines[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

BoundingBox::~BoundingBox() {
	std::cout << "Deleting Vertex Arrays" << std::endl;
	glDeleteVertexArrays(1, &bbVAO);
	glDeleteBuffers(1, &bbVBO);
	std::cout << "Successfully deleted vertex arrays" << std::endl;
}

void BoundingBox::drawFaces(glm::mat4 & transformation) {
	count = count % collisionFaces.size();
	for (unsigned int i = 0; i < 6; i++) {
		if (i < 4) {
			draw(transformation, collisionFaces[count][i], i);
		}
		else {
			draw(transformation, false, i);
		}
	}
	count++;
}

void BoundingBox::draw(glm::mat4 & transformation, bool col, int index) {

	glm::vec3 color(1.0f, 1.0f, 1.0f);

	if (!this->prevCollision) return;
	if (col) color = glm::vec3(1.0f, 0.0f, 0.0f);

	this->toWorld = identity;

	this->toWorld = glm::translate(identity, this->originalTrans) * this->toWorld;
	this->toWorld = glm::scale(identity, this->originalScale) * this->toWorld;

	this->toWorld = transformation * this->toWorld;

	glm::mat4 modelview = Window::V * this->toWorld;

	glUseProgram(this->shaderID);

	glDisable(GL_CULL_FACE);
	this->uColor = glGetUniformLocation(shaderID, "ucolor");
	this->uProjection = glGetUniformLocation(shaderID, "projection");
	this->uModelView = glGetUniformLocation(shaderID, "modelview");

	glUniform3fv(uColor, 1, &color[0]);
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
	glUniformMatrix4fv(uModelView, 1, GL_FALSE, &modelview[0][0]);

	glBindVertexArray(bbVAO);

	glDrawArrays(GL_LINES, index * 10 , 10);

	glBindVertexArray(0);
	glEnable(GL_CULL_FACE);
	
}

void BoundingBox::initializePos(glm::vec3 & scale, glm::vec3 & translation) {
	this->originalScale = scale;
	this->originalTrans = translation;
}

void BoundingBox::toggle() {
	this->prevCollision = !this->prevCollision;
}

void BoundingBox::setShaderID(GLint shaderID) {
	this->shaderID = shaderID;
}

void BoundingBox::setupBoxLines(std::vector<float> pts) {
	// The order of max pts given should be:
	// +x, -x, +y, -y, +z, -z
	// there might be padding added in later
	float padding = 0;

	float px = pts[0] + padding;
	float nx = pts[1] - padding;
	float py = pts[2] + padding;
	float ny = pts[3] - padding;
	float pz = pts[4] + padding;
	float nz = pts[5] - padding;

	// these are the vertices of the bounding box
	// for the first letter: f - front, b - back
	// for the second letter: b - bottom, t - top
	// for the third letter: l - left, r - right
	glm::vec3 fbl = glm::vec3(nx, ny, pz);
	glm::vec3 fbr = glm::vec3(px, ny, pz);
	glm::vec3 ftl = glm::vec3(nx, py, pz);
	glm::vec3 ftr = glm::vec3(px, py, pz);
	glm::vec3 bbl = glm::vec3(nx, ny, nz);
	glm::vec3 bbr = glm::vec3(px, ny, nz);
	glm::vec3 btl = glm::vec3(nx, py, nz);
	glm::vec3 btr = glm::vec3(px, py, nz);

	// front face
	drawpts(fbl, ftl, ftr, fbr);

	// right face
	drawpts(fbr, ftr, btr, bbr);

	// back face
	drawpts(bbr, btr, btl, bbl);

	// left face
	drawpts(bbl, btl, ftl, fbl);

	// top face
	drawpts(ftl, btl, btr, ftr);

	// bottom face
	drawpts(bbr, fbr, fbl, bbl);



	// front face
	facepts(fbl, ftl, ftr, fbr);

	// right face
	facepts(fbr, ftr, btr, bbr);

	// back face
	facepts(bbr, btr, btl, bbl);

	// left face
	facepts(bbl, btl, ftl, fbl);

}

void BoundingBox::facepts(glm::vec3 pt1, glm::vec3 pt2, glm::vec3 pt3, glm::vec3 pt4) {
	faces.push_back(pt1);
	faces.push_back(pt2);
	faces.push_back(pt4);

	glm::vec3 inb4 = (pt1 - pt4) * 0.5f + pt4;
	faces.push_back(inb4);
}

void BoundingBox::drawpts(glm::vec3 pt0, glm::vec3 pt1, glm::vec3 pt2, glm::vec3 pt3) {

	boxlines.push_back(pt0.x);
	boxlines.push_back(pt0.y);
	boxlines.push_back(pt0.z);
	boxlines.push_back(pt1.x);
	boxlines.push_back(pt1.y);
	boxlines.push_back(pt1.z);

	boxlines.push_back(pt1.x);
	boxlines.push_back(pt1.y);
	boxlines.push_back(pt1.z);
	boxlines.push_back(pt2.x);
	boxlines.push_back(pt2.y);
	boxlines.push_back(pt2.z);

	boxlines.push_back(pt2.x);
	boxlines.push_back(pt2.y);
	boxlines.push_back(pt2.z);
	boxlines.push_back(pt3.x);
	boxlines.push_back(pt3.y);
	boxlines.push_back(pt3.z);

	boxlines.push_back(pt3.x);
	boxlines.push_back(pt3.y);
	boxlines.push_back(pt3.z);
	boxlines.push_back(pt0.x);
	boxlines.push_back(pt0.y);
	boxlines.push_back(pt0.z);

	boxlines.push_back(pt0.x);
	boxlines.push_back(pt0.y);
	boxlines.push_back(pt0.z);
	boxlines.push_back(pt2.x);
	boxlines.push_back(pt2.y);
	boxlines.push_back(pt2.z);
}

void BoundingBox::setCol(std::vector<std::vector<bool>> colFaces) {
	collisionFaces = colFaces;
}

std::vector<glm::vec3> BoundingBox::getFaces()
{
	return faces;
}
