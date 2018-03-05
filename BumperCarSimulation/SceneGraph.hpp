//
//  SceneGraph.hpp
//  BumperCarSimulation
//
//  Created by Peter Mai on 3/4/18.
//  Copyright © 2018 William Ma & Peter Mai. All rights reserved.
//

#ifndef SceneGraph_hpp
#define SceneGraph_hpp

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

#include "OBJObject.hpp"

#include <memory>
#include <list>
#include <stdio.h>


/******************************************************************************
 * Abstract scene node class. This is the base class for all other scene
 * graph nodes to inherit.
 ******************************************************************************/
class SceneNode
{
public:
    virtual ~SceneNode() = default;
    
    virtual void draw(const glm::mat4 & C) = 0;
    virtual void update(const glm::mat4 & C) = 0;
};

/******************************************************************************
 * Represents a group of object (e.g. one android)
 ******************************************************************************/
class Group : public SceneNode
{
private:
    glm::mat4 toWorld;
    std::list< std::shared_ptr<SceneNode> > children;
    
public:
    
    Group();
    
    void draw(const glm::mat4 & C);
    void update(const glm::mat4 & C);
    
    void addChild( std::shared_ptr<SceneNode> newChild );
    
    void removeChild( std::shared_ptr<SceneNode> toRemoveChild );
};

/******************************************************************************
 * Transform Node that derives from the base SceneNode class.
 ******************************************************************************/
class Transform : public SceneNode
{
private:
    glm::mat4 transformationMatrix;
    std::list< std::shared_ptr<SceneNode> > children;
    
public:
    
    Transform(const glm::mat4 transformMatrix);
    
    void draw(const glm::mat4 & C);
    void update(const glm::mat4 & C);
    
    void addChild( std::shared_ptr<SceneNode> newChild );
    void removeChild( std::shared_ptr<SceneNode> toRemoveChild );
};

/******************************************************************************
 * Geometry Node that dervies from the base SceneNode class
 ******************************************************************************/
class Geometry : public SceneNode
{
private:
    GLuint shaderProgramID;
    std::unique_ptr<OBJObject> object;
    
    
public:
    
    Geometry( std::string OBJ_filename, GLuint shaderProgram );
    
    void draw(const glm::mat4 & C);
    void update(const glm::mat4 & C);
};

#endif /* SceneGraph_hpp */
