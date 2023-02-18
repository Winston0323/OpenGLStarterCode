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
	GLuint VBO_positions, VBO_normals, EBO;

	glm::mat4 model;
	glm::mat4 tModel;
	glm::mat4 rModel;
	glm::vec3 color;
	Mesh* mesh;

	// ObjRender Information
	glm::vec3 origin;
	std::vector<glm::vec3> positions;
	std::vector<bool> isBoundary;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> vNormals;
	std::vector<glm::vec3> currNorm;
	std::vector<unsigned int> indices;
	std::vector<std::pair<glm::vec3, glm::vec3>> edges;

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
};

////////////////////////////////////////////////////////////////////////////////

#endif
