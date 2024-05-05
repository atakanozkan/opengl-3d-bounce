#define _USE_MATH_DEFINES
#include <cmath>
#include "sphere.h"

/**
   In the constructor, it generates the sphere object and sets up mesh for it.
 */
Sphere::Sphere() {
    generateSphere(1.0f, 20, 20);
    setupMesh();
}

/**
 This function generates vertices and indices to create a sphere mesh and triangle faces.
 It calculates the position of each vertex on the sphere based on the given radius.
 Then, it generates indices for triangle faces of the sphere by iterating and calculates indices for each triangle face.
 */
void Sphere::generateSphere(float radius, unsigned int longDiv, unsigned int latDiv) {
    for (unsigned int y = 0; y <= latDiv; ++y) {
        for (unsigned int x = 0; x <= longDiv; ++x) {
            float xSegment = (float)x / (float)longDiv;
            float ySegment = (float)y / (float)latDiv;
            float xPos = radius * std::cos(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);
            float yPos = radius * std::cos(ySegment * M_PI);
            float zPos = radius * std::sin(xSegment * 2.0f * M_PI) * std::sin(ySegment * M_PI);

            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);
        }
    }

    for (unsigned int y = 0; y < latDiv; ++y) {
        for (unsigned int x = 0; x < longDiv; ++x) {
            indices.push_back((y + 1) * (longDiv + 1) + x);
            indices.push_back(y * (longDiv + 1) + x);
            indices.push_back(y * (longDiv + 1) + x + 1);

            indices.push_back((y + 1) * (longDiv + 1) + x);
            indices.push_back(y * (longDiv + 1) + x + 1);
            indices.push_back((y + 1) * (longDiv + 1) + x + 1);
        }
    }
}

/**
 This function sets up the mesh for the sphere.
 It generates vertex array, vertex buffer, and element buffer objects and then binds the vertex array object.
 It arranges the layout of vertex data for position attribute.
 */
void Sphere::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

/**
 This function binds vertex array object of sphere and draw elements
 */
void Sphere::render() {

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


