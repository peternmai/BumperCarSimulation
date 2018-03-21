//
//  SceneGraph.cpp
//  BumperCarSimulation
//
//  Created by Peter Mai on 3/4/18.
//  Copyright © 2018 William Ma & Peter Mai. All rights reserved.
//

#include "SceneGraph.hpp"

/******************************************************************************
 * GROUP
 ******************************************************************************/


Group::Group()
{
    this->toWorld = glm::mat4(1.0f);
}

void Group::draw(const glm::mat4 & C)
{
    glm::mat4 newModelView = C * this->toWorld;
    for( std::shared_ptr<SceneNode> child : this->children)
        child->draw( newModelView );
}

void Group::update(const glm::mat4 & C)
{
    this->toWorld = C;
}

void Group::addChild( std::shared_ptr<SceneNode> newChild )
{
    this->children.push_back(newChild);
}

void Group::removeChild(std::shared_ptr<SceneNode> toRemoveChild)
{
    this->children.remove(toRemoveChild);
}


/******************************************************************************
 * TRANSFORM
 ******************************************************************************/


Transform::Transform( const glm::mat4 transformMatrix )
{
    this->transformationMatrix = transformMatrix;
}

void Transform::draw(const glm::mat4 & C)
{
    glm::mat4 newModelView = C * this->transformationMatrix;
	lastTransMat = newModelView;  // added
    for( std::shared_ptr<SceneNode> child : this->children )
        child->draw( newModelView );
}

void Transform::update(const glm::mat4 & C)
{
    this->transformationMatrix = C;
}

void Transform::addChild( std::shared_ptr<SceneNode> newChild )
{
    this->children.push_back(newChild);
}

void Transform::removeChild(std::shared_ptr<SceneNode> toRemoveChild)
{
    this->children.remove(toRemoveChild);
}

glm::mat4 Transform::getLastTrans()
{
	return lastTransMat;
}


/******************************************************************************
 * GEOMETRY
 ******************************************************************************/

Geometry::Geometry( std::string OBJ_filename, GLuint shaderProgram, GLuint bbShaderProgram )
{
    this->object = std::make_unique<OBJObject>(OBJ_filename.c_str(), 2.0f);
    this->shaderProgramID = shaderProgram;

	this->box = this->object->getBBPointer();
	this->box->setShaderID(bbShaderProgram);
}

void Geometry::draw(const glm::mat4 & C)
{
    glm::mat4 toWorld = C;
    this->object->draw(this->shaderProgramID, toWorld);
}

void Geometry::update(const glm::mat4 & C)
{
    
}

BoundingBox * Geometry::getBoundingBoxPointer() {
	return box;
}

void Geometry::toggleFog() {
	this->object->toggleF();
}

