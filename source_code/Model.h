#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include <GL/glew.h>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "SOIL2/SOIL2.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Mesh.h"

//GLint TextureFromFile(const char* path, string directory); // get texture from file

class Model
{
public:
	vector<Mesh> meshes; // mesh data
	vector<Texture> textures_loaded;	// Stores textures so far, only stored once

	Model() {};

	Model(GLchar* path) // constructor with filepath
	{
		this->loadModel(path);
	}

	void Draw(Shader shader) //draw model and all its meshes
	{
		for (GLuint i = 0; i < this->meshes.size(); i++)
			this->meshes[i].Draw(shader);
	}

	vector<Mesh> getMeshes()
	{
		return meshes;
	}

private:
	string  directory;

										// Processes a node recurisvely, then its children.
	void processNode(aiNode* node, const aiScene* scene)
	{
		// Process the mesh located at the current node
		for (GLuint i = 0; i < node->mNumMeshes; i++)
		{
			// The scene contains all the data(indecies to index the actual objests), node is just to keep stuff organized.
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			this->meshes.push_back(this->processMesh(mesh, scene));
		}
		// After we've processed all of the meshes (if any) we recursively process each of the child nodes
		for (GLuint i = 0; i < node->mNumChildren; i++)
		{
			// Child nodes are  stored in the node, not in the scene.
			this->processNode(node->mChildren[i], scene);
		}

	}

	void loadModel(string path) // loads model in (uses ASSIMP)
	{
		Assimp::Importer importer; // read in file
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // any errors?
		{
			cout << "ERROR::ASSIMP:: " <<  importer.GetErrorString() << endl;
			return;
		}
		this->directory = path.substr(0, path.find_last_of('/')); // get directory path

		this->processNode(scene->mRootNode, scene);// recurisvely get root node
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene) // Process the mes
	{
		// Data to be filled
		vector<Vertex> vertices;
		vector<GLuint> indices;
		vector<Texture> textures;

		// Iterate through the verticies
		for (GLuint i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector; // We declare a placeholder vector
			// Position data
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;
			// Normals data
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
			// Texture Coordinates data
			if (mesh->mTextureCoords[0]) 
			{
				glm::vec2 vec;
				// A vertex can contain up to 8 different texture coordinates.Won't use models where a vertex can have multiple texture coordinates so we always take the first set (0).
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			vertices.push_back(vertex);
		}
		for (GLuint i = 0; i < mesh->mNumFaces; i++) // Iterate through each mesh face
		{
			aiFace face = mesh->mFaces[i];
			// store all face indicies
			for (GLuint j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}
		// Process the materials
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			// Diffuse maps
			vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// Specular maps
			vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			// Reflection maps 
			//** ASSIMP NOTE:(Note that ASSIMP doesn't load reflection maps properly from wavefront objects, solution:- defining the reflection maps as ambient maps in the .obj file, which ASSIMP is able to load)
			vector<Texture> reflectionMaps = this->loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_reflection");
			textures.insert(textures.end(), reflectionMaps.begin(), reflectionMaps.end());
		}

		return Mesh(vertices, indices, textures); // return mesh
	}

	// Material textures
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
	{
		vector<Texture> textures;
		for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			// Skip loading a new texture
			GLboolean skip = false;
			for (GLuint j = 0; j < textures_loaded.size(); j++)
			{
				if (textures_loaded[j].path == str)
				{
					textures.push_back(textures_loaded[j]);
					skip = true; // A texture with same filepath has already been loaded, continue.
					break;
				}
			}
			if (!skip)
			{   // If texture hasn't been loaded already, load it now
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str;
				textures.push_back(texture);
				this->textures_loaded.push_back(texture); 
			}
		}
		return textures; // return textures
	}

	// getting the texture from file
	GLint TextureFromFile(const char* path, string directory)
	{
		string filename = string(path); // get tex id from filepath etc.
		filename = directory + '/' + filename;
		GLuint textureID;
		glGenTextures(1, &textureID); // generate the texture
		int width, height;
		unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB); // using SOIL

		glBindTexture(GL_TEXTURE_2D, textureID); // assign tex to id
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Various Params
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, 0); // free tex
		SOIL_free_image_data(image);
		return textureID;// return the tex ID
	}

};


GLint TextureFromFile(const char* path, string directory)
{
	string filename = string(path); // get tex id from filepath etc.
	filename = directory + '/' + filename;
	GLuint textureID;
	glGenTextures(1, &textureID); // generate the texture
	int width, height;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

	glBindTexture(GL_TEXTURE_2D, textureID); // assign tex to id
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Various Params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0); // free tex
	SOIL_free_image_data(image);
	return textureID;
}

#endif 
