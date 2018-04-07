//
//  SkyBox.cpp
//  BumperCarSimulation
//
//  Created by Peter Mai on 3/4/18.
//  Copyright © 2018 William Ma & Peter Mai. All rights reserved.
//

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "SkyBox.hpp"
#include "Window.hpp"
#include <memory>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>   // include math functions, such as sin, cos, M_PI
#include <iostream> // allow c++ style console printouts

using namespace std; // allow console printouts without std::

/**
 * SkyBox constructor. Load the specified sky map into the GPU.
 */
SkyBox::SkyBox(const string filename, const float scaleAmount)
{
    // Scale each vertices coordinate
    for(unsigned int i = 0; i < skyBoxVertices.size(); i++ )
        skyBoxVertices[i] = SKY_BOX_VERTICES[i] * scaleAmount;
    
    // Load image file
    for(unsigned int i = 0; i < SKY_BOX_ENDING.size(); i++ ) {
        int twidth, theight, nrChannel;
        this->skyBoxPixelData[i].skyboxFacesRGB = stbi_load((filename + SKY_BOX_ENDING[i]).c_str(), &twidth, &theight, &nrChannel, 0);
        
        // Store the width and height of the image
        if( this->skyBoxPixelData[i].skyboxFacesRGB == NULL ) {
            std::cout << "Cannot read in " << (filename + SKY_BOX_ENDING[i]).c_str() << std::endl;
            exit(-1);
        }
        else {
            this->skyBoxPixelData[i].width = twidth;
            this->skyBoxPixelData[i].height = theight;
        }
    }
    
    // Create container that stores all buffer about this skybox -- VAO (Vertex Array Object)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // Copy all of the vertices into the GPU -- VBO (Vertex Buffer Objects)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * skyBoxVertices.size(), &skyBoxVertices[0], GL_STATIC_DRAW);
    
    // Set position attribute (Each is a vec3 of floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    // Create ID for texture
    glGenTextures(1, &this->skyBoxTextureID);
    
    // Set this texture to be the one we are working with
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->skyBoxTextureID);
    
    // Bind over to GPU
    for(unsigned int i = 0; i < this->skyBoxPixelData.size(); i++ ) {
        
        // Generate the texture for that face
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0, GL_RGB, this->skyBoxPixelData[i].width, this->skyBoxPixelData[i].height,
                     0, GL_RGB, GL_UNSIGNED_BYTE, this->skyBoxPixelData[i].skyboxFacesRGB);
    }
    
    // Set bi-linear filtering for both minification and magnification
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    // Unbind the currently bound buffer so we don't accidentally change anything
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

SkyBox::~SkyBox()
{
    for(unsigned int i = 0; i < skyBoxPixelData.size(); i++ )
        delete [] skyBoxPixelData[i].skyboxFacesRGB;
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void SkyBox::draw(GLuint shaderProgram)
{
    // Specify which shader program we are using
    glDepthMask(GL_FALSE);
    glUseProgram(shaderProgram);
    
    // Get location of uniform variables projection and view to forward to shader
    this->uView = glGetUniformLocation(shaderProgram, "view");
    this->uProjection = glGetUniformLocation(shaderProgram, "projection");
    
    // Send variables over to shader program
    glUniformMatrix4fv(uView, 1, GL_FALSE, &Window::V[0][0]);
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    
    // Draw out our object (Vertex Array Object)
    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(VAO);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // Draw each of the texture triangle
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    
    // Unbind VAO (Vertex Array Object) once complete so we won't mess it up
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
    
    glDepthMask(GL_TRUE);
}
