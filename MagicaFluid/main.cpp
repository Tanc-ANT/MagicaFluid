#include "pch.h"

#include "Window.h"
#include "Shader.h"
#include "Camera.h"
#include "Timer.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"


#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "PhysicsAnimation.h"
#include "SphSystemData.h"
#include "PciSphSolver.h"

PciSphSolver* solver;

Timer *timer;
char *title;

Shader* baseShader;

GLuint vao, vbo;
Vector3D render_word;
Vector3D render_ratio;


// Particle data to upload to GPU
struct RenderParticleData
{
	glm::vec3 Position;
	glm::vec4 Color;
	float Radius;
};

std::vector<RenderParticleData> sParticles;

void set_shaders()
{
	baseShader = new Shader("../Assets/Shaders/particle.vert","../Assets/Shaders/particle.frag","../Assets/Shaders/particle.gs");
	//baseShader = new Shader("../Assets/Shaders/base.vert", "../Assets/Shaders/base.frag");
}

void init_sph_system()
{
	solver = new PciSphSolver();
	solver->initialize();

	timer=new Timer();
	title=(char *)malloc(sizeof(char)*50);

	render_word = Vector3D(20.0, 20.0, 20.0);
	render_ratio = render_word / solver->worldSize();
}


void render_particles(glm::mat4 view)
{
	sParticles.clear();
	for (uint i = 0; i < solver->particleSystemData()->numberOfParticles(); i++)
	{

		RenderParticleData sp;
		const auto& pos = solver->particleSystemData()->positions();

		sp.Position = glm::vec3(pos[i].x * render_ratio.x,
			pos[i].y * render_ratio.y,
			pos[i].z * render_ratio.z);

		//sp.Position = glm::vec3(pos[i].x,pos[i].y,pos[i].z);

		sp.Color = glm::vec4(0.2f, 0.2f, 1.0f, 1.0f);

		sp.Radius = 0.5f;

		sParticles.push_back(sp);
	}

	size_t count = sParticles.size();

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(RenderParticleData), sParticles.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, (GLsizei)count);
}

void init_particles()
{

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// the max number particles is 30000
	glBufferData(GL_ARRAY_BUFFER, 30000 * sizeof(RenderParticleData), NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(RenderParticleData),
		(GLvoid*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(RenderParticleData),
		(GLvoid*)12);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(RenderParticleData),
		(GLvoid*)28);

	glBindVertexArray(0);

}

int main(int argc, char **argv)
{
	int success;
	Window window(success, 1600, 900);
	if (!success) return -1;

	Camera* camera = new Camera();
	window.camera = camera;

	init_sph_system();
	window.solver = solver;

	Frame frame;

	set_shaders();

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);
	glEnable(GL_POINT_SPRITE_ARB);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	init_particles();

	while (window.continueLoop())
	{
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (camera->buttonState == 1)
		{
			camera->xRot += (camera->xRotLength - camera->xRot) * 0.1f;
			camera->yRot += (camera->yRotLength - camera->yRot) * 0.1f;
		}

		solver->update(frame);

		baseShader->use();
		glm::mat4 projection = glm::perspective(glm::radians(camera->fov), (float)window.SCR_WIDTH / (float)window.SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera->GetViewMatrix();
		// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		baseShader->setMat4("projection", projection); 
		baseShader->setMat4("view", view);
		render_particles(view);

		timer->update();
		memset(title, 0, 50);
		sprintf(title, "MagicaFilud. FPS: %f", timer->get_fps());
		glfwSetWindowTitle(window.w, title);
		
		window.swapBuffersAndPollEvents();
	}
    return 0;
}
