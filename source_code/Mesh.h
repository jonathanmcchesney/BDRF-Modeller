#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "Shader.h"
#include <assimp\types.h>
using namespace std;

struct Texture {
	GLuint id;
	aiString path;
	string type;
};

// Position, Normal and Tex Vectors
struct Vertex {
	glm::vec3 Position, Normal, Tangent;
	glm::vec2 TexCoords;
};

// Main Class
class Mesh {
public:
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;

	Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices = indices;
		this->textures = textures;

		this->setupMesh(); // set up buffers and pointers
	}

	void Draw(Shader shader) // Render
	{
		GLuint diffuseNr = 1, specularNr = 1, reflectionNr = 1; // Bind Textures

		// iterate through textures.size
		for (GLuint i = 0; i < this->textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i); // Relevant texture is activated for binding, retrieve tex number 
			stringstream ss;
			string number, name = this->textures[i].type;

			if (name == "texture_diffuse")
				ss << diffuseNr++; // diffuse GLuint to stream
			else if (name == "texture_specular")
				ss << specularNr++; // specular GLuint to stream
			else if (name == "texture_reflection")	// REFLECTION**
				ss << reflectionNr++;
			number = ss.str();

			glUniform1i(glGetUniformLocation(shader.Program, (name + number).c_str()), i); // sampler set to correct tex unit

			glBindTexture(GL_TEXTURE_2D, this->textures[i].id); // Bind texture
		}
		glActiveTexture(GL_TEXTURE0); // Reset, set shininess to default & draw mesh

		glBindVertexArray(this->VertexArrayObject);
		glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0); // reset vertex array
	}

public:
	GLuint VertexArrayObject, VertexBufferObject, ElementBufferObject;

	// Initialise all the buffer objects/arrays
	void setupMesh()
	{
		glGenVertexArrays(1, &this->VertexArrayObject);
		glGenBuffers(1, &this->VertexBufferObject);
		glGenBuffers(1, &this->ElementBufferObject);

		glBindVertexArray(this->VertexArrayObject); // load in data
		glBindBuffer(GL_ARRAY_BUFFER, this->VertexBufferObject); // bind
		
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ElementBufferObject);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

		// Vertex Positions Pointers
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		// Vertex Normals Pointers
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
		// Vertex Tangent Pointers
		/*
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Tangent));
		*/
		// Vertex Texture Coords Pointers
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

		glBindVertexArray(0); // reset
	}
};


