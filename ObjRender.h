#pragma once
#ifndef _OBJRENDER_H_
#define _OBJRENDER_H_

#include "core.h"
////////////////////////////////////////////////////////////////////////////////

class ObjRender
{
private:
	
	//filename
	std::string filename;
	//primitives
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::ivec3> indices;

	//rendering matrix 
	glm::mat4 model;
	glm::vec3 origin;
	//rendering colors
	glm::vec3 color;
	//rendering buffers
	GLuint VAO;
	GLuint VBO_positions, VBO_normals, EBO, EBOEdge;

public:

	ObjRender(std::string inputFile);
	~ObjRender();

	void draw(const glm::mat4& viewProjMtx, GLuint shader);
	void update();
	void spinObjCenter(GLfloat deg, glm::vec3 axis);
	void spinWldCenter(GLfloat deg, glm::vec3 axis);
	
	void translation(glm::vec3 destination);
	void translationXY(GLfloat x, GLfloat y);
	void setColor(glm::vec3 val) { this->color = val; }
	void moveToWorldCenter();

	void ObjRender::objParser(std::string objFilename);

	
	//getters
	glm::vec3 getColor() { return this->color; }

		
};

////////////////////////////////////////////////////////////////////////////////

#endif
