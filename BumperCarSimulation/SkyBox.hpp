//
//  SkyBox.hpp
//  BumperCarSimulation
//
//  Created by Peter Mai on 3/4/18.
//  Copyright © 2018 William Ma & Peter Mai. All rights reserved.
//

#ifndef SkyBox_hpp
#define SkyBox_hpp

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

#include <vector>
#include <array>
#include <string>

#define SKYBOX_WIDTH 2000
#define SKYBOX_HEIGHT 2000

static const std::array<const std::string, 6>  SKY_BOX_ENDING {"_rt.jpg", "_lf.jpg",
    "_up.jpg", "_dn.jpg", "_ft.jpg", "_bk.jpg"};

static const float SKY_BOX_VERTICES[] = {
    
    // positions front
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    
    // position left
    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    
    // Position right
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    
    // position back
    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,
    
    // position up
    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,
    
    // position down
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
};

class SkyBox {
private:
    
    struct SkyBoxPixelData {
        unsigned char * skyboxFacesRGB;
        int width;
        int height;
        GLuint textureID;
    };
    
    std::array<SkyBoxPixelData, 6> skyBoxPixelData;
    std::array<float, 3 * 6 * 6> skyBoxVertices;
    GLuint skyBoxTextureID;
    GLuint VBO, VAO, EBO;
    GLuint uView, uProjection;
    
public:
    
    // Forward declaration for constructor and de-constructor
    SkyBox(const std::string filename, const float verticesScale);
    ~SkyBox();
    
    // Forawrd declaration to draw out the skybox
    void draw(GLuint shaderProgram);
    void setSkyBoxVerticesSize(float scaleAmount);
};

#endif /* SkyBox_hpp */
