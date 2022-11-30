#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <AssimpLoader.h>
using std::cout;
class Mesh {
		public:
			GLuint VAO, EBO, VBOnormal, VBOposition;
			std::vector<glm::vec3> vertices; //positions
			std::vector<glm::vec3> normals;
			std::vector<unsigned int> indices;

			void render(){
				glBindVertexArray(VAO);
	        	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    	    	glBindVertexArray(0);
			}
			Mesh(const std::vector<glm::vec3> inVertices,
				const std::vector<glm::vec3> inNormals,
				const std::vector<unsigned int> inIndices) {

				this->vertices = inVertices;
				this->normals = inNormals;
				this->indices = inIndices;
			}
		private:
			void setup(){
				glGenVertexArrays(1, &VAO); //vertex array object
        		glGenBuffers(1, &EBO); //element buffer object
				glGenBuffers(1, &VBOposition); //vertex buffer object
				glGenBuffers(1, &VBOnormal); //vertex buffer object
		
				glBindVertexArray(VAO);
      			
				//Bind and buffer data
				glBindBuffer(GL_ARRAY_BUFFER, VBOposition);
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

				glBindBuffer(GL_ARRAY_BUFFER, VBOnormal);
				glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
		
				//EBO
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

					
				//Bind to attributes
				glEnableVertexAttribArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, VBOposition);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, VBOnormal);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        		glBindVertexArray(0);
			}
	};

class Model {
	public:	
		std::string path;
		std::vector<Mesh> meshes;
		void render(){
			for(unsigned int i = 0; i < meshes.size(); i++)
            	meshes[i].render();
		}
		Model(std::string p){
			this->path=p;
			
		}
	private:
		bool loadAssimp(std::string const path){
			Assimp::Importer importer;
			cout<<"Loading file at "<<path<< ".\n";
			const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals); //aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
			//error catching
			if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){ 
				cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << "\n";
				return false;
			}
			//iterate through meshes
			std::vector<glm::vec3> tempVertices;
			std::vector<glm::vec3> tempNormals;
			std::vector<unsigned int> tempIndices;
			for (unsigned int i = 0; i < scene->mNumMeshes; i++){
				aiMesh* mesh = scene->mMeshes[i];
				meshes.push_back(Mesh(tempVertices,tempNormals,tempIndices));

				//
				for (unsigned int j = 0; j < mesh->mNumVertices; i++) {
					aiVector3D pos = mesh->mVertices[i];
					tempVertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
				}



				//

				tempVertices.clear();
				tempNormals.clear();
				tempIndices.clear();
			}


		}
};