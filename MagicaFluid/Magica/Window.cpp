#include "Window.h"

unsigned int Window::SCR_WIDTH = 1600;
unsigned int Window::SCR_HEIGHT = 900;

bool Window::keyBools[10] = { false, false,false, false, false, false, false, false, false, false };
bool Window::wireframe = false;
bool Window::firstMouse = true;
float Window::lastX = SCR_WIDTH / 2.0f;
float Window::lastY = SCR_HEIGHT / 2.0f;

bool Window::mouseCursorDisabled = true;

Camera* Window::camera = 0;
ParticleSystemSolver* Window::solver = 0;

bool Window::glfwInitialized = false;

Window::Window(int& success, unsigned int scrW, unsigned int scrH, std::string name) : name(name)
{

	Window::SCR_WIDTH = scrW;
	Window::SCR_HEIGHT = scrH;
	// glfw: initialize and configure
	if (!glfwInitialized)
	{
		success = glfwInit();
		glfwSetErrorCallback(glfwErrorCallback);
		glfwInitialized = success;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	//glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation										 
	this->w = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, name.c_str(), NULL, NULL);
	if (!this->w)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		success = 0;
		return;
	}
	glfwMakeContextCurrent(this->w);
	glfwSetFramebufferSizeCallback(this->w, &Window::framebuffer_size_callback);
	glfwSetScrollCallback(this->w, &Window::scroll_callback);
	glfwSetCursorPosCallback(this->w, &Window::mouse_callback);
	

	success = gladLoader() && success;
	if (success) {
		std::cout << "GLFW window correctly initialized!" << std::endl;
		std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
		std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
		std::cout << "GLFW version: " << glGetString(GL_VENDOR) << std::endl;
	}
}

int Window::gladLoader() 
{

	// tell GLFW to capture our mouse
	glfwSetInputMode(this->w, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return 0;
	}

	return 1;
}

void Window::glfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// glfw: whenever the mouse moves, this callback is called
void Window::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float dx, dy;
	dx = (float)(xpos - camera->ox);
	dy = (float)(ypos - camera->oy);

	if (camera->buttonState == 1)
	{
		camera->xRotLength += dy / 5.0f;
		camera->yRotLength += dx / 5.0f;
	}

	camera->ox = (int)xpos; camera->oy = (int)ypos;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void Window::scroll_callback(GLFWwindow* window, double x, double y)
{
	if (camera->fov >= 1.0f && camera->fov <= 120)
		camera->fov -= (int)y;
	else if (camera->fov <= 1.0f)
		camera->fov = 1.0f;
	else if (camera->fov >= 120)
		camera->fov = 120;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glViewport(0, 0, width, height);
}
 
void Window::processInput(float frameTime) {
	if (glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(w, true);

	if (glfwGetKey(w, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		solver->setRuing(1-solver->isRuning());
	}

	if (glfwGetKey(w, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera->zTrans += 0.3f;
	}

	if (glfwGetKey(w, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera->zTrans -= 0.3f;
	}

	if (glfwGetKey(w, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera->xTrans += 0.3f;
	}

	if (glfwGetKey(w, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera->xTrans -= 0.3f;
	}

	if (glfwGetKey(w, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camera->yTrans -= 0.3f;
	}

	if (glfwGetKey(w, GLFW_KEY_E) == GLFW_PRESS)
	{
		camera->yTrans += 0.3f;
	}

	int mouseState = glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT);

	if (mouseState == GLFW_PRESS)
	{
		camera->buttonState = 1;
	}
	else if (mouseState == GLFW_RELEASE)
	{
		camera->buttonState = 0;
	}
}

Window::~Window()
{
	this->terminate();
}
