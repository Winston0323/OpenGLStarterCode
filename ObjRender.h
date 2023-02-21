#pragma once
#ifndef _OBJRENDER_H_
#define _OBJRENDER_H_

#include "core.h"
#include "Mesh.h"
#include "Iterators.h"

////////////////////////////////////////////////////////////////////////////////

class ObjRender
{
private:
	GLuint VAO;
	GLuint VBO_positions, VBO_normals, EBO, EBOEdge;

	glm::mat4 model;
	glm::mat4 tModel;
	glm::mat4 rModel;
	glm::vec3 color;
	glm::vec3 lineColor;
	Mesh* mesh;
	int triSize;
	int vertexSize;
	int normalSize;
	int vNormalSize;
	int loopSize;
	int connectedComponent;
	// ObjRender Information
	glm::vec3 origin;
	std::vector<glm::vec3> positions;
	std::vector<bool> isBoundary;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> vNormals;
	std::vector<glm::vec3> currNorm;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> edgeInd;
	std::vector<bool> edgeVis;
	//std::vector<std::pair<glm::vec3, glm::vec3>> edges;
	std::vector<std::tuple<bool, glm::vec3, glm::vec3>> edges;
	std::vector<std::pair<int, int>> halfedgeInds;

public:

	ObjRender(char inputFile[]);
	~ObjRender();

	void draw(const glm::mat4& viewProjMtx, GLuint shader);
	void update();
	void update(GLfloat deltaTime);
	void spin(GLfloat deg, glm::vec3 axis);
	
	glm::vec3 getColor() { return color; }
	void translation(glm::vec3 destination);
	void translationXY(GLfloat x, GLfloat y);
	void setColor(glm::vec3 val) { this->color = val; }
	void moveToWorldCenter();
	void switchNorm();
	void drawEdges();
	int compute_c(Mesh* mesh);
	
	//getters
	glm::vec3 getOrigin() { return this->origin; }
	int getTriangleSize() { return this->triSize; }
	int getVertexSize() { return this->vertexSize; }
	int getNormSize() { return this->normalSize; }
	int getVNormSize() { return this->vNormalSize; }
	int getLoopSize() { return this->loopSize; }
	int getConnectedComponent() { return this->connectedComponent; }

};

////////////////////////////////////////////////////////////////////////////////

#endif
