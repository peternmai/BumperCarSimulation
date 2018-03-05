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
    
    struct TriangleDescriptionIndices {
        std::vector<unsigned int> vertexIndex;
        std::vector<unsigned int> textureIndex;
        std::vector<unsigned int> normalIndex;
    };
    
    // These variables are needed for the shader program
    GLuint VBO, VAO, EBO;
    GLuint uProjection, uModelview;
    
    unsigned int numberOfVerticesToDraw = 0;
    
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> verticesAttributes;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<TriangleDescriptionIndices> triangleIndices;
    glm::mat4 toWorld;
    
    float size;
    float angle;
    float pointSize;
    
    glm::vec3 originalCenterShift;
    glm::vec3 originalNormalizeScale;
    
    glm::vec3 scale;
    glm::vec3 position;
    glm::mat4 spinDirection;
    
public:
    
    OBJObject(const char* filepath, float boxSize);
    ~OBJObject();
    
    void parse(const char* filepath, float boxSize);
    void draw(GLuint shaderProgram, glm::mat4 & transformation);
    
    void update();
    
};
#endif /* OBJObject_hpp */
