//
//  Particles.cpp
//  BumperCarSimulation
//
//  Created by Peter Mai on 3/18/18.
//  Copyright © 2018 William Ma & Peter Mai. All rights reserved.
//

#include "Particles.hpp"
#include "Window.hpp"

Particles::Particles()
{
    // Initialize all the particles
    for(int i = 0; i < TOTAL_PARTICLES; i++)
        particleLife[i] = 0.0f;
    
    // Store the updated attributes
    particleAttributes = std::vector<glm::vec3>(TOTAL_PARTICLES * 3);
    updateParticleAttributes();
    
    // Create container that stores all buffer info about this object -- VAO (Vertex Array Object)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // Copy all the vertices into the GPU -- VBO (Vertex Buffer Objects)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9 * TOTAL_PARTICLES, &particleAttributes[0], GL_STATIC_DRAW);
    
    // Set the position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    // Set the color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    // Set the color attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    
    // Unbind the currently bound buffer so we don't accidentally change anything
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
}

Particles::~Particles()
{
    std::cout << "Deallocating memory for particles..." << std::endl;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Particles::updateParticleAttributes()
{
    for(int i = 0; i < TOTAL_PARTICLES; i++) {
        particleAttributes[i * 3 + 0] = particlePosition[i];
        particleAttributes[i * 3 + 1] = particleColor[i];
        particleAttributes[i * 3 + 2] = glm::vec3(particleLife[i]);
    }
}

void Particles::update()
{
    // For each particle that are dead, revive them
    // For each particle that that are alive, update its position
    int particleGenerated = 0;
    for(int i = 0; i < TOTAL_PARTICLES; i++) {
        
        bool particleAlive = (particleLife[i] <= 0) ? false : true;
        
        // Particle is still alive, update its position
        if( particleAlive ) {
            
            float zDirectionChange = ((rand() % 100) - 50.0f) / 25.0f;
            float xDirectionChange = ((rand() % 100) - 50.0f) / 25.0f;
            float yDirectionChange = -1.0f;
            
            particlePosition[i].x += xDirectionChange;
            particlePosition[i].y += yDirectionChange;
            particlePosition[i].z += zDirectionChange;
            
            particleLife[i] -= 1.0f;
        }
        
        // Particle has no more life, give it a new life
        else if(particleGenerated < (TOTAL_PARTICLES * 0.05)){
            particlePosition[i].x = (rand() % 500) - 250.0f;
            particlePosition[i].y = (rand() % 1000) / 1000.0f * 50.0f + 200.0f;
            particlePosition[i].z = (rand() % 500) - 250.0f;
            
            particleColor[i].x = (rand() % 1000) / 1000.0f;
            particleColor[i].y = (rand() % 1000) / 1000.0f;
            particleColor[i].z = (rand() % 1000) / 1000.0f;
            
            particleLife[i] = ((rand() % 1000) / 1000.0f) * 300.0f;
            particleGenerated++;
        }
        
        // Do nothing
        else {
            particleLife [i] = 0.0f;
        }
    }
    
    // Update the data on the GPU
    updateParticleAttributes();
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 7 * TOTAL_PARTICLES, &particleAttributes[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Particles::draw(GLuint shaderProgram)
{
    // Specify which shader program we are using
    glUseProgram(shaderProgram);
    
    // Get location of uniform variables projection and view to forward to shader
    this->uProjection = glGetUniformLocation(shaderProgram, "projection");
    this->uView = glGetUniformLocation(shaderProgram, "view");
    
    // Forward the variables to the shader
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(uView, 1, GL_FALSE, &Window::V[0][0]);
    
    // Draw our object (Vertex Array Object)
    glBindVertexArray(VAO);
    
    // Enable setting point size and transparency
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Draw each of the triangles
    glDrawArrays(GL_POINTS, 0, TOTAL_PARTICLES);
    
    // Disable setting transparency
    glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glDisable(GL_BLEND);
    
    // Unbind VAO (Vertex Array Object) once complete so we won't mess it up
    glBindVertexArray(0);
}
