//
//  Particles.hpp
//  BumperCarSimulation
//
//  Created by Peter Mai on 3/18/18.
//  Copyright © 2018 William Ma & Peter Mai. All rights reserved.
//

#ifndef Particles_hpp
#define Particles_hpp

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

#include <stdio.h>
#include <vector>

#define TOTAL_PARTICLES 500

class Particles {
private:
    
    // Stores GPU buffers
    GLuint VBO, VAO;
    GLuint uProjection, uView;
    
    // Stores information about each of the particle
    glm::vec3 particlePosition [TOTAL_PARTICLES];
    glm::vec3 particleColor[TOTAL_PARTICLES];
    float particleLife[TOTAL_PARTICLES];
    std::vector<glm::vec3> particleAttributes;
    
    void updateParticleAttributes();
    
public:
    Particles();
    ~Particles();
    
    void draw(GLuint shaderProgram);
    
    void update();
};


#endif /* Particles_hpp */
