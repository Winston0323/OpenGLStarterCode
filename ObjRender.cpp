#include "ObjRender.h"


////////////////////////////////////////////////////////////////////////////////
ObjRender::ObjRender(char inputFile[])
{
	// Model matrix.
	model = glm::mat4(1.0f);
	this->color = glm::vec3(1, 1, 1);
	mesh = new Mesh();
	bool flag = mesh->readOBJFile(inputFile);

	if (!flag) {
		std::cerr << "Fail to read mesh " << inputFile << ".\n";
	}

	//read all vertex
	std::cout << "Total Number of vertex: " << mesh->numVertices() << "\n";
	for (MeshVertexIterator vit(mesh); !vit.end(); ++vit) {
		Vertex* ver = *vit;
		this->positions.push_back(glm::vec3(ver->point()[0], ver->point()[1], ver->point()[2]));
		if (!ver->boundary()) {
			this->isBoundary.push_back(false);
		}
		else {
			this->isBoundary.push_back(true);
		}
		this->normals.push_back(glm::vec3(0.0f));
		this->vNormals.push_back(glm::vec3(0.0f));
		//std::cout << "Its 1-ring neighboring vertices are: \n";
		//for (VertexVertexIterator vvit(ver); !vvit.end(); ++vvit) {
		//	Vertex* nv = *vvit;
		//	std::cout << "Vertex " << nv->index() << " (" << nv->point()[0] << ", "
		//		<< nv->point()[1] << ", " << nv->point()[2] << ")\n";
		//}
		//std::cout << "\n All the halfedges that go out from this vertex are: \n";
		//for (VertexOutHalfedgeIterator vheit(ver); !vheit.end(); ++vheit) {
		//	Halfedge* he = *vheit;
		//	std::cout << " (" << he->source()->index() << ", " << he->target()->index() << ") ; ";
		//	std::cout << "halfedge index = " << he->index() << ".\n";
		//}
		//std::cout << "\n";
		
	}

	//iterate through faces 
	for (MeshFaceIterator fit(mesh); !fit.end(); ++fit) {
		Face* f = *fit;
		Halfedge* he1 = f->he(); // one halfedge
		Halfedge* he2 = he1->next(); // its next halfedge inside this face
		Vertex* pt1 = he1->source();
		Vertex* pt2 = he1->target();
		Vertex* pt3 = he2->target();
		//get index of the points 
		int ind1 = pt1->index();
		int ind2 = pt2->index();
		int ind3 = pt3->index();
		//push indices into array
		this->indices.push_back(ind1);
		this->indices.push_back(ind2);
		this->indices.push_back(ind3);
		//calculating face normals
		glm::vec3 p1 = this->positions[ind1];
		glm::vec3 p2 = this->positions[ind2];
		glm::vec3 p3 = this->positions[ind3];
		glm::vec3 normal = glm::normalize(glm::cross(p2 - p1, p3 - p1));
		this->normals[ind1] = normal;
		this->normals[ind2] = normal;
		this->normals[ind3] = normal;

		//calculateing vertex normals
		Halfedge* hes[3];
		hes[0] = f->he();
		hes[1] = hes[0]->next();
		hes[2] = hes[0]->prev();
		Point& point0 = hes[0]->target()->point();
		Point& point1 = hes[1]->target()->point();
		Point& point2 = hes[2]->target()->point();
		double l20 = (point0 - point2).norm();
		double l01 = (point1 - point0).norm();
		double l12 = (point2 - point1).norm();
		GLfloat angle1 = acos((l20 * l20 + l01 * l01 - l12 * l12) / (2 * l20 * l01));
		GLfloat angle2 = acos((l01 * l01 + l12 * l12 - l20 * l20) / (2 * l01 * l12));
		GLfloat angle3 = acos((l12 * l12 + l20 * l20 - l01 * l01) / (2 * l12 * l20));
		glm::vec3 norm1 = normal * angle1/(3.14f);
		glm::vec3 norm2 = normal * angle2/(3.14f); 
		glm::vec3 norm3 = normal * angle3/(3.14f);
		this->vNormals[ind1] += norm1;
		this->vNormals[ind2] += norm2;
		this->vNormals[ind3] += norm3;
		currNorm = normals;
	}
	for (glm::vec3 vNorm : this->vNormals) {
		vNorm = glm::normalize(vNorm);
	}
	moveToWorldCenter();
	// Generate a vertex array (VAO) and two vertex buffer objects (VBO).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO_positions);
	glGenBuffers(1, &VBO_normals);

	// Bind to the VAO.
	glBindVertexArray(VAO);

	// Bind to the first VBO - We will use it to store the vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind to the second VBO - We will use it to store the normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * currNorm.size(), currNorm.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Generate EBO, bind the EBO to the bound VAO and send the data
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Unbind the VBOs.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

////////////////////////////////////////////////////////////////////////////////
ObjRender::~ObjRender()
{
	// Delete the VBOs and the VAO.
	glDeleteBuffers(1, &VBO_positions);
	glDeleteBuffers(1, &VBO_normals);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

////////////////////////////////////////////////////////////////////////////////

void ObjRender::draw(const glm::mat4& viewProjMtx, GLuint shader)
{

	glDisable(GL_CULL_FACE);
	// actiavte the shader program 
	glUseProgram(shader);

	// get the locations and send the uniforms to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, (float*)&viewProjMtx);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
	glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

	// Bind the VAO
	glBindVertexArray(VAO);

	// draw the points using triangles, indexed with the EBO
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

////////////////////////////////////////////////////////////////////////////////

void ObjRender::update()
{

}
////////////////////////////////////////////////////////////////////////////////

void ObjRender::update(GLfloat deltaTime)
{
	//this->spin(0.05, glm::vec3(0, 1, 0));
	//this->translation(glm::vec3(0, 0, 1));
}
////////////////////////////////////////////////////////////////////////////////
void ObjRender::translation(glm::vec3 destination)
{
	this->model = glm::translate(glm::mat4(1.0f), destination)* model;
	this->origin = destination;
}
////////////////////////////////////////////////////////////////////////////////
void ObjRender::translationXY(GLfloat x, GLfloat y)
{
	glm::vec3 destination = glm::vec3(x, y,this->origin.z);
	this->model = glm::translate(glm::mat4(1.0f), destination) * model;
	this->origin = destination;
}
////////////////////////////////////////////////////////////////////////////////
void ObjRender::spin(GLfloat deg, glm::vec3 axis)
{
	this->model = glm::rotate(glm::radians(deg), axis) * model;
}
void ObjRender::moveToWorldCenter() {
	std::vector<GLfloat> xCord;
	std::vector<GLfloat> yCord;
	std::vector<GLfloat> zCord;
	for (int i = 0; i < positions.size(); i++) {
		xCord.push_back(this->positions[i].x);
		yCord.push_back(this->positions[i].y);
		zCord.push_back(this->positions[i].z);
	}

	GLfloat midX = (*std::max_element(xCord.begin(), xCord.end()) + *std::min_element(xCord.begin(), xCord.end())) / 2;
	GLfloat midY = (*std::max_element(yCord.begin(), yCord.end()) + *std::min_element(yCord.begin(), yCord.end())) / 2;
	GLfloat midZ = (*std::max_element(zCord.begin(), zCord.end()) + *std::min_element(zCord.begin(), zCord.end())) / 2;
	//initialize
	glm::vec3 geoCenter = glm::vec3(-midX, -midY, -midZ);
	this->translation(geoCenter);
	this->origin = glm::vec3(0.0f);
	//for (int i = 0; i < points.size(); i++) {

	//	points[i].x = points[i].x - midX;
	//	points[i].y = points[i].y - midY;
	//	points[i].z = points[i].z - midZ;
	//}
}

void ObjRender::switchNorm() 
{
	if (this->currNorm == normals) {
		this->currNorm = vNormals;
	}
	else {
		this->currNorm = normals;
	}
	// Bind to the second VBO - We will use it to store the normals
	glBindBuffer(GL_ARRAY_BUFFER, VBO_normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * currNorm.size(), currNorm.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

}
