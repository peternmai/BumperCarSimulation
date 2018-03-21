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
#include "BoundingBox.hpp"


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
    
    struct MaterialForShader {
        glm::vec3 color;
        float ambientCoefficient;
        float diffuseCoefficient;
        float specularCoefficient;
        float shininessConstant;
    };
    
    struct DirectionalLightSource {
        glm::vec3 color;
        glm::vec3 direction;
    };
    
    // These variables are needed for the shader program
    GLuint VBO, VAO, EBO;
    GLuint uProjection, uModelview, uModel;
    GLuint uAmbient, uDiffuse, uSpecular, uShiny, uDrawShadow;
    GLuint uDirectionalLightColor, uDirectinalLightDirection;
    
    // Store the object's material and light source
    MaterialForShader material;
    DirectionalLightSource directionalLight;
    
    unsigned int numberOfVerticesToDraw = 0;
    
    std::vector<glm::vec3> verticesAttributes;
    std::unordered_map<unsigned int, Vertex> verticesAttributesMap;
    std::unordered_map<std::string, Material> materialMap;

    std::vector<unsigned int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> textureCoordinates;
    
    // Store information regarding shadows
    bool drawShadow;
    glm::mat4 worldToLightSpace;
    glm::mat4 biasMatrix = glm::mat4(
                         0.5, 0.0, 0.0, 0.0,
                         0.0, 0.5, 0.0, 0.0,
                         0.0, 0.0, 0.5, 0.0,
                         0.5, 0.5, 0.5, 1.0
                         );
    
    
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

	// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
	// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
	std::vector<float> extrema;
	BoundingBox* bb;
	bool fog = false;
	GLuint uFog;
    
public:
    
    OBJObject(const char* filepath, float boxSize);
    ~OBJObject();
    
    void parse(const char* filepath, float boxSize);
    void draw(GLuint shaderProgram, glm::mat4 & transformation);
    void firstPassShadowMap(GLuint shaderProgram, glm::mat4 &transformation);
    void drawShadowDepthMap(GLuint shaderProgram, glm::mat4 &transformation);
    
    void setPhongCoefficient(float ambient, float diffuse, float specular, float shininess);
    void setShowShadow(bool choice);
    
    void update();

	// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
	// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
	BoundingBox* getBBPointer(); // Use for cleaning up OBJ to disassociate more of it
								 // toggleFog
	void toggleF();
    
};
#endif /* OBJObject_hpp */
