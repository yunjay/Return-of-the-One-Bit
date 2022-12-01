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
			bool isSet = false;

			Mesh(const std::vector<glm::vec3> inVertices,
				const std::vector<glm::vec3> inNormals,
				const std::vector<unsigned int> inIndices) {

				this->vertices = inVertices;
				this->normals = inNormals;
				this->indices = inIndices;
				
				this->setup();
			}

			void render(){
				if (!isSet)this->setup();

				glBindVertexArray(VAO);
	        	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    	    	glBindVertexArray(0);
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

				isSet = true;
			}
	};

class Model {
	public:	
		std::string path;
		std::vector<Mesh> meshes;
		glm::vec3 position = glm::vec3(0.0,0.0,0.0);
		GLfloat scale;
		GLfloat diffuseScale=1.0f;
		glm::vec3 maxBoxPoint;
		glm::vec3 minBoxPoint;


		Model(std::string p){
			this->path=p;
			loadAssimp(this->path);			
			getBoundingBox();
		}
		void render(GLuint shader) {
			glUniform1f(glGetUniformLocation(shader, "diffuseScale"), this->diffuseScale);

			for (unsigned int i = 0; i < meshes.size(); i++)
				meshes[i].render();
		}
		void setPosition(glm::vec3 pos) { this->position = pos; }
		void setScale(GLfloat scl){ this->scale = scl; }
		void getBoundingBox() {
			float maxX = meshes[0].vertices[0].x, maxY = meshes[0].vertices[0].y, maxZ = meshes[0].vertices[0].z;
			float minX = meshes[0].vertices[0].x, minY = meshes[0].vertices[0].y, minZ = meshes[0].vertices[0].z;
			for (int meshNum = 0; meshNum < this->meshes.size(); meshNum++) {
				for (int i = 0; i < meshes[meshNum].vertices.size(); i++) {
					(meshes[meshNum].vertices[i].x > maxX) ? maxX = meshes[meshNum].vertices[i].x : 0;
					(meshes[meshNum].vertices[i].y > maxY) ? maxY = meshes[meshNum].vertices[i].y : 0;
					(meshes[meshNum].vertices[i].z > maxZ) ? maxZ = meshes[meshNum].vertices[i].z : 0;
					(meshes[meshNum].vertices[i].x < minX) ? minX = meshes[meshNum].vertices[i].x : 0;
					(meshes[meshNum].vertices[i].y < minY) ? minY = meshes[meshNum].vertices[i].y : 0;
					(meshes[meshNum].vertices[i].z < minZ) ? minZ = meshes[meshNum].vertices[i].z : 0;
				}
			}
			this->maxBoxPoint = glm::vec3(maxX,maxY,maxZ);
			this->minBoxPoint = glm::vec3(minX,minY,minZ);
			(maxY-minY)>(maxX-minX)? this->scale = 1.0f/(maxX-minX): this->scale = 1.0f/(maxY-minY) ;
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
			for (unsigned int meshId = 0; meshId < scene->mNumMeshes; meshId++){
				aiMesh* mesh = scene->mMeshes[meshId];

				//vertice positions
				for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
					aiVector3D pos = mesh->mVertices[i];
					tempVertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
				}
				//normals
				for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
					aiVector3D n = mesh->mNormals[i];
					tempNormals.push_back(glm::normalize(glm::vec3(n.x, n.y, n.z)));
				}
				//indices
				for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
					for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
						tempIndices.push_back(mesh->mFaces[i].mIndices[j]);
				}

				//push to model
				this->meshes.push_back(Mesh(tempVertices,tempNormals,tempIndices));

				tempVertices.clear();
				tempNormals.clear();
				tempIndices.clear();
			}

			return true;
		}
};