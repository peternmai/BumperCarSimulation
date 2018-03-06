//
//  OBJObject.hpp
//  BumperCarSimulation
//
//  Created by Peter Mai on 3/4/18.
//  Copyright © 2018 William Ma & Peter Mai. All rights reserved.
//

#ifndef OBJObject_hpp
#define OBJObject_hpp

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
#include <vector>
#include <unordered_map>


class OBJObject
{
private:
    
    std::string filepath;
    
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
        glm::vec3 textureCoordinate;
    };
    
    struct Material {
        glm::vec3 Ka;
        glm::vec3 Kd;
        glm::vec3 Ks;
        glm::vec3 Ke;
    };
    
    // These variables are needed for the shader program
    GLuint VBO, VAO, EBO;
    GLuint uProjection, uModelview;
    
    unsigned int numberOfVerticesToDraw = 0;
    
    std::vector<glm::vec3> verticesAttributes;
    std::unordered_map<unsigned int, Vertex> verticesAttributesMap;
    std::unordered_map<std::string, Material> materialMap;

    std::vector<unsigned int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> textureCoordinates;
    
    
    glm::mat4 toWorld;
    
    float size;
    float angle;
    float pointSize;
    
    glm::vec3 originalCenterShift;
    glm::vec3 originalNormalizeScale;
    
    glm::vec3 scale;
    glm::vec3 position;
    glm::mat4 spinDirection;
    
    void parseMTL(std::string filepath);
    
public:
    
    OBJObject(const char* filepath, float boxSize);
    ~OBJObject();
    
    void parse(const char* filepath, float boxSize);
    void draw(GLuint shaderProgram, glm::mat4 & transformation);
    
    void update();
    
};
#endif /* OBJObject_hpp */
