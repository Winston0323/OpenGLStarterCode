#include "ObjRender.h"


////////////////////////////////////////////////////////////////////////////////
ObjRender::ObjRender(std::string inputFile)
{
	filename = inputFile;
	// Model matrix.
	model = glm::mat4(1.0f);
	this->color = glm::vec3(1.0f, 1.0f, 1.0f);
	objParser(filename);
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	//generate ebo 
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * indices.size(), indices.data(), GL_STATIC_DRAW);
	// Unbind the VAO, VBO, EBO.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
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

	// actiavte the shader program 
	glUseProgram(shader);
	// get the locations and send the uniforms to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shader, "viewProj"), 1, false, glm::value_ptr(viewProjMtx));
	//glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, (float*)&model);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(glGetUniformLocation(shader, "DiffuseColor"), 1, &color[0]);

	// Bind the VAO
	glBindVertexArray(VAO);

	// draw the points using triangles, indexed with the EBO
	//glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT,0);
	glDrawElements(GL_TRIANGLES, indices.size() * 3, GL_UNSIGNED_INT, 0);

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}
////////////////////////////////////////////////////////////////////////////////
void ObjRender::update()
{
	//this->spinWldCenter(1, glm::vec3(0, 1, 0));
}

////////////////////////////////////////////////////////////////////////////////
void ObjRender::translation(glm::vec3 destination)
{
	this->model = glm::translate(glm::mat4(1.0f), destination);
	this->origin = destination;
}
////////////////////////////////////////////////////////////////////////////////
void ObjRender::translationXY(GLfloat x, GLfloat y)
{
	glm::vec3 destination = glm::vec3(x, y,this->origin.z);
	this->model = glm::translate(glm::mat4(1.0f), destination) * model;
	this->origin += destination;
}
////////////////////////////////////////////////////////////////////////////////
void ObjRender::spinObjCenter(GLfloat deg, glm::vec3 axis)
{
	this->model = glm::translate(glm::mat4(1.0f), -origin) * model;//to world center
	this->model = glm::rotate(glm::radians(deg), axis) * model;
	this->model = glm::translate(glm::mat4(1.0f),  origin) * model;//to world center
}
////////////////////////////////////////////////////////////////////////////////
void ObjRender::spinWldCenter(GLfloat deg, glm::vec3 axis)
{
	this->model = glm::rotate(glm::radians(deg), axis) * model;
}
void ObjRender::objParser(std::string objFilename)
{
	std::ifstream objFile(objFilename); // The obj file we are reading.
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;
	std::vector<glm::ivec3> faces;

	// Check whether the file can be opened.
	if (objFile.is_open())
	{
		std::string line; // A line in the file.

		// Read lines from the file.
		while (std::getline(objFile, line))
		{
			// Turn the line into a string stream for processing.
			std::stringstream ss;
			ss << line;

			// Read the first word of the line.
			std::string label;
			ss >> label;

			// If the line is about vertex (starting with a "v").
			if (label == "v")
			{
				// Read the later three float numbers and use them as the 
				// coordinates.
				glm::vec3 point;
				ss >> point.x >> point.y >> point.z;

				// Process the point. For example, you can save it to a.
				points.push_back(point);
			}
			if (label == "vn")
			{
				// Read the later three float numbers and use them as the 
				// coordinates.
				glm::vec3 normal;
				ss >> normal.x >> normal.y >> normal.z;

				// Process the point. For example, you can save it to a.
				normals.push_back(normal);
			}
			if (label == "f")
			{
				glm::ivec3 face;
				std::string x, y, z;
				int xCord, yCord, zCord;
				ss >> x >> y >> z;
				xCord = stoi(x.substr(0, x.find("//"))) - 1;
				yCord = stoi(y.substr(0, y.find("//"))) - 1;
				zCord = stoi(z.substr(0, z.find("//"))) - 1;
				face.x = xCord;
				face.y = yCord;
				face.z = zCord;
				faces.push_back(face);
				//std::pair<int,int> edge1 = std::make_pair(face.x, face.y);
				//std::pair<int,int> edge2 = std::make_pair(face.x, face.z);
				//std::pair<int,int> edge3 = std::make_pair(face.y, face.z);
				//edges.push_back(edge1);
				//edges.push_back(edge2);
				//edges.push_back(edge3);
			}
		}

		//update points normals and faces
		this->positions = points;
		this->normals = normals;
		this->indices = faces;
	}
	else
	{
		std::cerr << "Can't open the file " << objFilename << std::endl;
	}
	objFile.close();
}
////////////////////////////////////////////////////////////////////////////////
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
}
