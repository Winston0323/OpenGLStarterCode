////////////////////////////////////////
// Window.cpp
////////////////////////////////////////

#include "Window.h"

////////////////////////////////////////////////////////////////////////////////
//render object
Plain* Window::plain;
// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "Cloth_Simulator";
bool Window::simStart = false;
bool Window::RK4 = true;
bool Window::launching = false;
bool Window::launch = false;

//time
GLfloat Window::speed = 0.0f;
GLfloat Window::lastFrameTime = 0.0f;
GLfloat Window::thisFrameTime = 0.0f;
GLfloat Window::deltaTime = 0.0f;
GLfloat Window::calTime = 0.0f;
GLfloat Window::renderRate = DEFAULT_RENDRATE;
GLfloat Window::simulationRate = DEFAULT_SIMRATE;
GLfloat Window::renderTimeStep = 1.0f / renderRate;
GLfloat Window::renderTime = renderTimeStep;
GLfloat Window::simTimeStep = 1.0f / simulationRate;
// Objects to render
// Camera Properties
Camera* Cam;

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;
////////////////////////////////////////////////////////////////////////////////

// Constructors and desctructors 
bool Window::initializeProgram() {

	// Create a shader program with a vertex shader and a fragment shader.
	shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

	// Check the shader program.
	if (!shaderProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	return true;

}

bool Window::initializeObjects()
{
	// STEP 1 : Create new object
	plain = new Plain(5, glm::vec3(0),glm::vec3(0,0,1));
	return true;
}

void Window::cleanUp()
{

	// Delete the shader program.
	glDeleteProgram(shaderProgram);
}
////////////////////////////////////////////////////////////////////////////////
// for the Window
GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// set up the camera
	Cam = new Camera();
	Cam->SetAspect(float(width) / float(height));

	// initialize the interaction variables
	LeftDown = RightDown = false;
	MouseX = MouseY = 0;

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height); 
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size.
	glViewport(0, 0, width, height);

	Cam->SetAspect(float(width) / float(height));
}

////////////////////////////////////////////////////////////////////////////////
void Window::idleCallback()
{
	//STEP 2 update
	if (simStart) {
		//calculate time
		thisFrameTime = glfwGetTime();
		deltaTime = thisFrameTime - lastFrameTime;
		lastFrameTime = thisFrameTime;
		calTime += deltaTime;
		renderTime = renderTime - deltaTime;

	}
	else {//STEP 2.3 renderupdate when simulation not started  

		lastFrameTime = glfwGetTime();
	}	
	//STEP 2.4 always update values
	Cam->Update(deltaTime);
}

void Window::displayCallback(GLFWwindow* window)
{	
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// STEP3 Render the object.
	//test->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	plain->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
	drawGUI();

	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	// Swap buffers.
	glfwSwapBuffers(window);
}

////////////////////////////////////////////////////////////////////////////////

// helper to reset the camera
void Window::resetCamera() 
{
	Cam->Reset();
	Cam->SetAspect(float(Window::width) / float(Window::height));
}
////////////////////////////////////////////////////////////////////////////////

// callbacks - for Interaction 
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/*
	 * TODO: Modify below to add your key callbacks.
	 */
	
	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		switch (key) 
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);				
			break;

		case GLFW_KEY_R:
			resetCamera();
			break;
		case GLFW_KEY_A:
			
			break;
		case GLFW_KEY_D:
			break;
		case GLFW_KEY_LEFT:

			break;
		case GLFW_KEY_RIGHT:
			break;

		case GLFW_KEY_SPACE:
			break;
		default:
			break;
		}
	}
	else if (action == GLFW_RELEASE) {
	
		switch (key)
		{
		case GLFW_KEY_D:
			Cam->setStartFlip(true);
			break;
		case GLFW_KEY_SPACE:
		default:
			break;
		}
	}
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureMouse) return;
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		LeftDown = (action == GLFW_PRESS);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		RightDown = (action == GLFW_PRESS);
	}
}

void Window::cursor_callback(GLFWwindow* window, double currX, double currY) {

	int maxDelta = 100;
	int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
	int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

	MouseX = (int)currX;
	MouseY = (int)currY;
	// Move camera
	// NOTE: this should really be part of Camera::Update()
	if (LeftDown) {
		const float rate = 1.0f;
		Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
		Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
	}
	if (RightDown) {
		const float rate = 0.005f;
		float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
		Cam->SetDistance(dist);
	}
}

////////////////////////////////////////////////////////////////////////////////
void Window::drawGUI() {

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImGui::GetIO().WantCaptureMouse = true;
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	//Windspeed show
	ImGui::Begin("Pin Ball Engine");                          // Create a window called "Hello, world!" and append into it.
	
	ImGui::SetWindowSize(ImVec2(350, Window::height/2));
	ImGui::SetWindowPos(ImVec2(Window::width -350,0));
	if (ImGui::TreeNode("PIN BALL")) {
		ImGui::TreePop();
	}
	int bbindex = 1;
	if (ImGui::TreeNode("Bounce balls")) {

		ImGui::TreePop();
	}


	ImGui::End();
	ImGui::Begin("Scores");                          // Create a window called "Hello, world!" and append into it.

	ImGui::SetWindowSize(ImVec2(350, Window::height / 2));
	ImGui::SetWindowPos(ImVec2(0, Window::height-500));
	if (ImGui::Button("start")) {
		simStart = !simStart;
	}
	

	if (ImGui::Button("restore default")) {
		resetGame();
	}
	ImGui::Text("Speed:%f", speed);
	ImGui::End();
	//draw imgui
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void Window::resetGame() {
}