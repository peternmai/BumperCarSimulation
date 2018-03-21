//
//  OBJObject.cpp
//  BumperCarSimulation
//
//  Created by Peter Mai on 3/4/18.
//  Copyright © 2018 William Ma & Peter Mai. All rights reserved.
//

#include "OBJObject.hpp"
#include "Window.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <limits>

using namespace std;

OBJObject::OBJObject(const char *filepath, float boxSize)
{
    cout << endl << "Loading in: " << filepath << endl;
    toWorld = glm::mat4(1.0f);
    parse(filepath, boxSize);
    
    this->filepath = std::string(filepath);
    this->spinDirection = glm::mat4(1.0f);
    this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    this->position = glm::vec3(0.0f, 0.0f, 0.0f);
    this->pointSize = 1;
    this->angle = 0.0f;
    this->drawShadow = true;
    
    // Set the default material coefficient
    material.ambientCoefficient = 0.2f;
    material.diffuseCoefficient = 0.5f;
    material.specularCoefficient = 1.0f;
    material.shininessConstant = 36.0f;
    directionalLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
    
    // Create container that stores all buffer info about this object -- VAO (Vertex Array Object)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // Copy all the vertices into the GPU -- VBO (Vertex Buffer Objects)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesAttributes[0]) * verticesAttributes.size(),
                 &verticesAttributes[0], GL_STATIC_DRAW);
    
    // Set position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    // Set normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    // Set the color coordinates
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    
    // Specify order to draw each vertices -- EBO (Element Buffer Object)
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);
    
    // Unbind the currently bound buffer so we don't accidentally change anything
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Force deletion of temporary memory for storing vertices because it's now stored on the GPU
    this->numberOfVerticesToDraw = indices.size();
    vector<unsigned int>().swap( this->indices );
    vector<glm::vec3>().swap( this->vertices );
    vector<glm::vec3>().swap( this->normals );
    vector<glm::vec3>().swap( this->verticesAttributes );
    unordered_map<unsigned int, Vertex>().swap( this->verticesAttributesMap );
    unordered_map<std::string, Material>().swap( this->materialMap );


	// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
	// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
	bb = new BoundingBox(extrema);
	bb->initializePos(originalNormalizeScale, originalCenterShift);
}

OBJObject::~OBJObject()
{
    // Deallocate all GPU memory used for this object
    cout << "Deallocating memory for " << filepath << " from GPU... ";
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    cout << "Successful" << endl;
}

void OBJObject::parseMTL(std::string filepath) {
    
    ifstream inFile;
    inFile.open( filepath, ifstream::in );
    if( !inFile ) {
        cout << "Unable to read: " << filepath << endl;
        exit(-1);
    }
    
    // Read in line by line until end of file
    string curLine;
    string curMaterial = "";
    while(getline(inFile, curLine)) {
        istringstream ss(curLine);
        string lineType;
        ss >> lineType;
        
        if( lineType.compare("newmtl") == 0 ) {
            ss >> curMaterial;
            this->materialMap[ curMaterial ] = Material{};
        }
        
        // Reading in ambient color
        else if( lineType.compare("Ka") == 0 ) {
            float r, g, b;
            ss >> r >> g >> b;
            this->materialMap[ curMaterial ].Ka = glm::vec3(r,g,b);
        }
        
        // Read in diffuse color
        else if( lineType.compare("Kd") == 0 ) {
            float r, g, b;
            ss >> r >> g >> b;
            this->materialMap[ curMaterial ].Kd = glm::vec3(r,g,b);
        }
        
        // Read in specular color
        else if( lineType.compare("Ks") == 0 ){
            float r, g, b;
            ss >> r >> g >> b;
            this->materialMap[ curMaterial ].Ks = glm::vec3(r,g,b);
        }
        
        else {
            // Do nothing
        }
    }
    
    
    inFile.close();
}

void OBJObject::parse(const char * filepath, float boxSize)
{
    //TODO parse the OBJ file
    // Populate the face indices, vertices, and normals vectors with the OBJ Object data
    
    ifstream inFile;
    inFile.open( filepath, ifstream::in );
    if( !inFile ) {
        cout << "Unable to read: " << filepath << endl;
        exit(-1);
    }
    
    // Keep track of the max/min x, y, z to center and scale object
    float maxX = -numeric_limits<float>::infinity();
    float maxY = -numeric_limits<float>::infinity();
    float maxZ = -numeric_limits<float>::infinity();
    float minX = numeric_limits<float>::infinity();
    float minY = numeric_limits<float>::infinity();
    float minZ = numeric_limits<float>::infinity();
    
    // Read in line by line until end of file
    string curLine;
    string curMaterial;
    string meshName;
    while(getline(inFile, curLine)) {
        istringstream ss(curLine);
        string lineType;
        ss >> lineType;
        
        // Reading in a vertex's coordinate and their color
        if( lineType.compare("v") == 0 ) {
            float x, y, z, r, b, g;
            ss >> x >> y >> z >> r >> b >> g;
            vertices.push_back(glm::vec3(x, y, z));
            
            maxX = max(maxX, x);
            maxY = max(maxY, y);
            maxZ = max(maxZ, z);
            
            minX = min(minX, x);
            minY = min(minY, y);
            minZ = min(minZ, z);
            
        }
        
        // Reading in a vertex normal
        else if( lineType.compare("vn") == 0 ) {
            float x, y , z;
            ss >> x >> y >> z;
            normals.push_back(glm::normalize(glm::vec3(x, y, z)));
        }
        
        // Reading in texture coordinates
        else if( lineType.compare("vt") == 0 ) {
            float x, y;
            ss >> x >> y;
            textureCoordinates.push_back(glm::vec3(x, y, 0.0f));
        }
        
        // Reading in a face
        else if( lineType.compare("f") == 0 ) {
            
            int numberOfVerticesPerFace = std::count(curLine.begin(), curLine.end(), '/') / 2;
            if( numberOfVerticesPerFace != 3 ) {
                std::cerr << "Not programmed to handle faces that are not triangles!" << std::endl;
                std::cerr << "  Check mesh name: " << meshName << std::endl;
                exit(-1);
            }
            
            // Read in each of the triangle's vertex
            for(int vertexNum = 0; vertexNum < 3; vertexNum++) {
                
                Vertex vertexAttribute;
                unsigned int vertexIdx, textureIdx, normalIdx;
                
                // Read in vertex index if specified
                if( ss.peek() != '/' ) {
                    ss >> vertexIdx;
                    vertexIdx--;
                    vertexAttribute.position = vertices[vertexIdx];
                    this->indices.push_back(vertexIdx);
                }
                ss.ignore();
                
                // Read in texture index if specified
                if( ss.peek() != '/' ) {
                    ss >> textureIdx;
                    textureIdx--;
                    vertexAttribute.textureCoordinate = textureCoordinates[textureIdx];
                }
                ss.ignore();
                
                // Read in normal index if specified
                if( ss.peek() != '/' ) {
                    ss >> normalIdx;
                    normalIdx--;
                    vertexAttribute.normal = normals[normalIdx];
                }
                
                // Check if object file specified a color
                if( materialMap.find( curMaterial ) != materialMap.end() ) {
                    vertexAttribute.color = materialMap[ curMaterial ].Kd;
                }
                else
                    vertexAttribute.color = vertexAttribute.normal * 0.5f + 0.5f;
                
                verticesAttributesMap[ vertexIdx ] = vertexAttribute;
            }
        }
        
        // Check if file specified a material
        else if( lineType.compare("usemtl") == 0 ) {
            ss >> curMaterial;
        }
        
        // Check which mesh we're loading in
        else if( lineType.compare("o") == 0 ) {
            ss >> meshName;
        }
        
        // Read in MTL file for material information
        else if( lineType.compare("mtllib") == 0 ) {
            std::string filename = std::string(filepath);
            std::size_t location = filename.find_last_of('.');
            parseMTL(filename.substr(0, location) + ".mtl");
        }
        
        else
            continue;
    }
    
    inFile.close();
        
    // Determine how much to shift the image to make it go to the center
    this->originalCenterShift.x = (maxX - minX)/boxSize - maxX;
    this->originalCenterShift.y = (maxY - minY)/boxSize - maxY;
    this->originalCenterShift.z = (maxZ - minZ)/boxSize - maxZ;
    
    cout << "Center Shifter: " << originalCenterShift.x << " "
    << originalCenterShift.y << " " << originalCenterShift.z << endl;
    
    // Determine how much to scale so it fits in a 2x2x2 box around center
    float longestSide = max(maxX - minX, max(maxY - minY, maxZ - minZ));
    float scaleFactor = boxSize / longestSide;
    this->originalNormalizeScale = glm::vec3(scaleFactor, scaleFactor, scaleFactor);
    cout << "Scale Factor:   " << scaleFactor << endl;
    
    // Create a long vector that stores both vertices and normal
    for(unsigned int i = 0; i < vertices.size(); i++ ) {
        verticesAttributes.push_back( verticesAttributesMap[i].position );
        verticesAttributes.push_back( verticesAttributesMap[i].normal );
        verticesAttributes.push_back( verticesAttributesMap[i].color );
    }

	// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
	// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
	this->extrema.push_back(maxX);
	this->extrema.push_back(minX);
	this->extrema.push_back(maxY);
	this->extrema.push_back(minY);
	this->extrema.push_back(maxZ);
	this->extrema.push_back(minZ);
}

void OBJObject::draw(GLuint shaderProgram, glm::mat4 & transformation)
{
    this->update();
    this->toWorld = glm::mat4(1.0f);
    
    // Move object to center to scale and rotate
    this->toWorld = glm::translate(glm::mat4(1.0f), this->originalCenterShift) * this->toWorld;
    this->toWorld = glm::scale(glm::mat4(1.0f), this->originalNormalizeScale) * this->toWorld;
    
    // Apply transformation
    this->toWorld = transformation * this->toWorld;
    
    // Find the model view matrices
    glm::mat4 modelview = Window::V * this->toWorld;
    
    // Specify which shader program we are using
    glUseProgram(shaderProgram);
    
    // Get location of uniform variables projection and modelview to forward to shader
    this->uProjection = glGetUniformLocation(shaderProgram, "projection");
    this->uModelview = glGetUniformLocation(shaderProgram, "modelview");
    this->uModel = glGetUniformLocation(shaderProgram, "toWorld");
    this->uAmbient = glGetUniformLocation(shaderProgram, "ambient");
    this->uDiffuse = glGetUniformLocation(shaderProgram, "diffuse");
    this->uSpecular = glGetUniformLocation(shaderProgram, "specular");
    this->uShiny = glGetUniformLocation(shaderProgram, "shininess");
    this->uDirectionalLightColor = glGetUniformLocation(shaderProgram, "lightColor");
    this->uDirectinalLightDirection = glGetUniformLocation(shaderProgram, "lightDirection");
    this->uDrawShadow = glGetUniformLocation(shaderProgram, "drawShadow");
	this->uFog = glGetUniformLocation(shaderProgram, "fog");
    
    // Send variables over to shader program
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
    glUniformMatrix4fv(uModel, 1, GL_FALSE, &this->toWorld[0][0]);
    glUniform1f(uAmbient, material.ambientCoefficient);
    glUniform1f(uDiffuse, material.diffuseCoefficient);
    glUniform1f(uSpecular, material.specularCoefficient);
    glUniform1f(uShiny, material.shininessConstant);
    glUniform3fv(uDirectionalLightColor, 1, &directionalLight.color[0]);
    glUniform3fv(uDirectinalLightDirection, 1, &directionalLight.direction[0]);
    glUniform1f(uDrawShadow, (drawShadow && Window::showShadows) );


	// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
	// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
	int boolFog = fog ? 1 : 0;
	glUniform1i(uFog, boolFog);
    
    // Send information about shadows
    glm::mat4 depthBiasMVP = biasMatrix * this->worldToLightSpace;
    GLuint uShadowBias = glGetUniformLocation(shaderProgram, "shadowDepthBias");
    glUniformMatrix4fv(uShadowBias, 1, GL_FALSE, &depthBiasMVP[0][0]);
    
    // Send transformation over to GPU
    GLuint uWorldToLight = glGetUniformLocation(shaderProgram, "worldToLightSpace");
    glUniformMatrix4fv(uWorldToLight, 1, GL_FALSE, &worldToLightSpace[0][0]);
    
    // Draw our object (Vertex Array Object)
    glBindVertexArray(VAO);
    
    // Draw each of the triangles
    glDrawElements(GL_TRIANGLES, this->numberOfVerticesToDraw, GL_UNSIGNED_INT, 0);
    
    // Unbind VAO (Vertex Array Object) once complete so we won't mess it up
    glBindVertexArray(0);

	// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
	// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
	bb->drawFaces(transformation);
}

void OBJObject::firstPassShadowMap(GLuint shaderProgram, glm::mat4 &transformation)
{
    this->toWorld = glm::mat4(1.0f);
    
    // Move object to center to scale and rotate
    this->toWorld = glm::translate(glm::mat4(1.0f), this->originalCenterShift) * this->toWorld;
    this->toWorld = glm::scale(glm::mat4(1.0f), this->originalNormalizeScale) * this->toWorld;
    
    // Apply transformation
    this->toWorld = transformation * this->toWorld;
    
    // Specify which shader program we are using
    glUseProgram(shaderProgram);
    
    // Get the inverse light direction
    glm::vec3 inverseLightDirection = Window::getSunLightDirection() * -1.0f;
    
    // Calculate the camera's up direction
    float sunDegreeRadian = Window::getSunRadian();
    glm::vec3 cam_up = glm::vec3(-1.0f * sin(sunDegreeRadian), cos(sunDegreeRadian), 0.0f);
    
    // Calculate the directional light's projection matrix
    glm::mat4 lightProjectionMatrix = glm::ortho<float>(-350.0f, 350.0f, -350.0f, 350.0f, NEAR_PLANE, FAR_PLANE);
    
    // Calculate transform of each object so they're in perspective of light's POV
    glm::mat4 lightViewMatrix = glm::lookAt(inverseLightDirection, glm::vec3(0.0f), cam_up);
    
    // Calculate the transform from world space to light space
    this->worldToLightSpace = lightProjectionMatrix * lightViewMatrix * glm::mat4(1.0f);
    
    // Send transformation over to GPU
    GLuint uWorldToLight = glGetUniformLocation(shaderProgram, "worldToLightSpace");
    glUniformMatrix4fv(uWorldToLight, 1, GL_FALSE, &worldToLightSpace[0][0]);
    
    // Pass the to world transformation over to shader
    GLuint uShadowDepthMap = glGetUniformLocation(shaderProgram, "toWorld");
    glUniformMatrix4fv(uShadowDepthMap, 1, GL_FALSE, &this->toWorld[0][0]);
    
    // Draw our object (Vertex Array Object)
    glBindVertexArray(VAO);
    
    // Draw each of the triangles
    //glCullFace(GL_FRONT);
    glDrawElements(GL_TRIANGLES, this->numberOfVerticesToDraw, GL_UNSIGNED_INT, 0);
    //glCullFace(GL_BACK);
    
    // Unbind VAO (Vertex Array Object) once complete so we won't mess it up
    glBindVertexArray(0);
}

void OBJObject::drawShadowDepthMap(GLuint shaderProgram, glm::mat4 &transformation)
{
    this->update();
    this->toWorld = glm::mat4(1.0f);
    
    // Move object to center to scale and rotate
    this->toWorld = glm::translate(glm::mat4(1.0f), this->originalCenterShift) * this->toWorld;
    this->toWorld = glm::scale(glm::mat4(1.0f), this->originalNormalizeScale) * this->toWorld;
    
    // Apply transformation
    this->toWorld = transformation * this->toWorld;
    
    // Specify which shader program we are using
    glUseProgram(shaderProgram);
    
    // Pass the to world transformation over to shader
    GLuint uToWorld = glGetUniformLocation(shaderProgram, "toWorld");
    glUniformMatrix4fv(uToWorld, 1, GL_FALSE, &this->toWorld[0][0]);
    
    // Send shadow map bias to GPU
    glm::mat4 depthBiasMVP = biasMatrix * this->worldToLightSpace;
    GLuint uShadowBias = glGetUniformLocation(shaderProgram, "shadowDepthBias");
    glUniformMatrix4fv(uShadowBias, 1, GL_FALSE, &depthBiasMVP[0][0]);
    
    // Send transformation to light space over to GPU
    GLuint uWorldToLight = glGetUniformLocation(shaderProgram, "worldToLightSpace");
    glUniformMatrix4fv(uWorldToLight, 1, GL_FALSE, &worldToLightSpace[0][0]);
    
    // Send near and far plane
    GLuint uNearPlane = glGetUniformLocation(shaderProgram, "zNear");
    GLuint uFarPlane = glGetUniformLocation(shaderProgram, "zFar");
    glUniform1f(uNearPlane, NEAR_PLANE);
    glUniform1f(uFarPlane, FAR_PLANE);
    
    // Draw our object (Vertex Array Object)
    glBindVertexArray(VAO);
    
    // Draw each of the triangles
    glDrawElements(GL_TRIANGLES, this->numberOfVerticesToDraw, GL_UNSIGNED_INT, 0);
    
    // Unbind VAO (Vertex Array Object) once complete so we won't mess it up
    glBindVertexArray(0);
}

void OBJObject::update()
{
    // Update the location of the sun
    directionalLight.direction = Window::getSunLightDirection();
}

void OBJObject::setPhongCoefficient(float ambient, float diffuse, float specular, float shininess)
{
    this->material.ambientCoefficient = ambient;
    this->material.diffuseCoefficient = diffuse;
    this->material.specularCoefficient = specular;
    this->material.shininessConstant = shininess;
}

void OBJObject::setShowShadow(bool choice)
{
    this->drawShadow = choice;
}

// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
// BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP BEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEP Needed!
BoundingBox * OBJObject::getBBPointer() {
	return bb;
}

void OBJObject::toggleF() {
	fog = !fog;
}