#include "pch.h"

#include "Window.h"
#include "Shader.h"
#include "Camera.h"
#include "Timer.h"
#include "Skybox.h"
#include "Screen.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"


#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp";

#include "PhysicsAnimation.h"
#include "SphSystemData.h"
#include "PciSphSolver.h"

Window* window;
Camera* camera;
SphSolver* solver;
Skybox* skybox;
Screen* screen;

Timer *timer;
char *title;

//Shader* particleShader;
//Shader* gaussShader;
//Shader* waterShader;

Shader* planeShader;
Shader* depthShader;
Shader* blurShader;
Shader* ticknessShader;
Shader* fluidShader;


GLuint vao, vbo;
GLuint pvao, pvbo, pebo;
GLuint svao, svbo, sebo;
Vector3D render_world;
Vector3D render_ratio;

GLuint planeFbo;
GLuint planeMap;
GLuint depthFbo;
GLuint depthMap;
GLuint colorMap;
GLuint normalMap;
GLuint blurFboV;
GLuint blurMapV;
GLuint blurFboH;
GLuint blurMapH;
GLuint thickFbo;
GLuint thickMap;

GLuint particleFbo;
GLuint particleMap;

int renderMode = 1;

// Particle data to upload to GPU
struct RenderParticleData
{
	glm::vec3 Position;
	glm::vec4 Color;
	float Radius;
};

std::vector<RenderParticleData> RenderParticles;

void set_shaders()
{
	planeShader = new Shader("../Assets/Shaders/plane.vert", "../Assets/Shaders/plane.frag");
	depthShader = new Shader("../Assets/Shaders/depth.vert", "../Assets/Shaders/depth.frag", "../Assets/Shaders/depth.gs");
	blurShader = new Shader("../Assets/Shaders/blur.vert", "../Assets/Shaders/blur.frag");
	ticknessShader = new Shader("../Assets/Shaders/thickness.vert", "../Assets/Shaders/thickness.frag", "../Assets/Shaders/thickness.gs");
	fluidShader = new Shader("../Assets/Shaders/fluidFinal.vert", "../Assets/Shaders/fluidFinal.frag");
}

void init_sph_system()
{
	//solver = new SphSolver();
	solver = new PciSphSolver();
	solver->initialize();

	timer=new Timer();
	title=(char *)malloc(sizeof(char)*50);

	render_world = Vector3D(20.0, 20.0, 20.0);
	render_ratio = render_world / solver->worldSize();
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

	// plane
	glGenVertexArrays(1, &pvao);
	glGenBuffers(1, &pvbo);
	glGenBuffers(1, &pebo);

	//screen
	glGenVertexArrays(1, &svao);
	glGenBuffers(1, &svbo);
	glGenBuffers(1, &sebo);
}

void shaderVaoPlane()
{
	GLfloat vertices[] = {
		100.0f, 0.0f, 100.0f,
		100.0f, 0.0f, -100.0f,
		-100.0f, 0.0f, -100.0f,
		-100.0f, 0.0f, 100.0f
	};
	GLuint indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	glBindVertexArray(pvao);

	glBindBuffer(GL_ARRAY_BUFFER, pvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);
}

void shaderVAOQuad()
{
	GLfloat vertices[] = {
			1.0f, 1.0f,	   // Top Right
			1.0f, -1.0f,   // Bottom Right
			-1.0f, -1.0f,  // Bottom Left
			-1.0f, 1.0f	   // Top Left 
	};
	GLuint indices[] = {  // Note that we start from 0!
		0, 1, 3,	// First Triangle
		1, 2, 3	// Second Triangle
	};

	glBindVertexArray(svao);

	glBindBuffer(GL_ARRAY_BUFFER, svbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(0);
}

void init_water()
{
	glm::vec2 mapSize(window->SCR_WIDTH, window->SCR_HEIGHT);

	// Plane buffer
	glGenTextures(1, &planeMap);
	glBindTexture(GL_TEXTURE_2D, planeMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mapSize.x, mapSize.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &planeFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, planeFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, planeMap, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Particle buffer
	// Plane buffer
	/*glGenTextures(1, &particleMap);
	glBindTexture(GL_TEXTURE_2D, particleMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mapSize.x, mapSize.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &particleFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, particleFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, particleMap, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

	// Depth buffer
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mapSize.x, mapSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1, &depthFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Blur buffer
	glGenTextures(1, &blurMapV);
	glBindTexture(GL_TEXTURE_2D, blurMapV);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mapSize.x, mapSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1, &blurFboV);
	glBindFramebuffer(GL_FRAMEBUFFER, blurFboV);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, blurMapV, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &blurMapH);
	glBindTexture(GL_TEXTURE_2D, blurMapH);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, mapSize.x, mapSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1, &blurFboH);
	glBindFramebuffer(GL_FRAMEBUFFER, blurFboH);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, blurMapH, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//Thickness buffer
	glGenTextures(1, &thickMap);
	glBindTexture(GL_TEXTURE_2D, thickMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mapSize.x, mapSize.y, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1, &thickFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, thickFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, thickMap, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void render_particles()
{
	RenderParticles.clear();
	for (uint i = 0; i < solver->particleSystemData()->numberOfParticles(); i++)
	{

		RenderParticleData renderParticle;
		const auto& pos = solver->particleSystemData()->positions();

		renderParticle.Position = glm::vec3(pos[i].x * render_ratio.x,
			pos[i].y * render_ratio.y,
			pos[i].z * render_ratio.z);

		//sp.Position = glm::vec3(pos[i].x,pos[i].y,pos[i].z);

		renderParticle.Color = glm::vec4(0.2f, 0.2f, 1.0f, 1.0f);

		renderParticle.Radius = 0.75f;

		RenderParticles.push_back(renderParticle);
	}

	size_t count = RenderParticles.size();


	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(RenderParticleData), RenderParticles.data());
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//if (renderMode != 0) return;
	//glBindVertexArray(vao);
	//glDrawArrays(GL_POINTS, 0, (GLsizei)count);
}


void render_water()
{
	if (renderMode == 0) return;

	glm::mat4 projection = camera->GetProjectionMatrix(window->SCR_WIDTH, window->SCR_HEIGHT);
	glm::mat4 view = camera->GetViewMatrix();
	size_t count = RenderParticles.size();
	glm::vec2 screenSize = glm::vec2(window->SCR_WIDTH, window->SCR_HEIGHT);
	glm::vec2 blurDirX = glm::vec2(1.0f / screenSize.x, 0.0f);
	glm::vec2 blurDirY = glm::vec2(0.0f, 1.0f / screenSize.y);
	glm::vec4 color = glm::vec4(0.275f, 0.65f, 0.85f, 0.9f);

	/*
	//----------------------Particle Rendering---------------------
	particleShader->use();
	glBindFramebuffer(GL_FRAMEBUFFER, particleFbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	planeShader->setMat4("projection", projection);
	planeShader->setMat4("view", view);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(RenderParticleData), RenderParticles.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(vao);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glDrawArrays(GL_POINTS, 0, (GLsizei)count);
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

	//glBindVertexArray(vao);
	//glDrawArrays(GL_POINTS, 0, count);
	//glBindVertexArray(0);
	*/
	
	//----------------------Infinite Plane---------------------
	//planeShader->use();
	glBindFramebuffer(GL_FRAMEBUFFER, planeFbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//shaderVaoPlane();

	//planeShader->setMat4("projection", projection);
	//planeShader->setMat4("view", view);

	skybox->draw(window, camera);

	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
	
	//----------------------Particle Depth----------------------
	depthShader->use();
	glBindFramebuffer(GL_FRAMEBUFFER, depthFbo);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glClear(GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(RenderParticleData), RenderParticles.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	depthShader->setMat4("projection", projection);
	depthShader->setMat4("view", view);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	glDrawArrays(GL_POINTS, 0, (GLsizei)count);
	glBindVertexArray(0);

	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

	//--------------------Particle Blur-------------------------
	
	blurShader->use();

	//Vertical blur
	glBindFramebuffer(GL_FRAMEBUFFER, blurFboV);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glClear(GL_DEPTH_BUFFER_BIT);

	shaderVAOQuad();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	blurShader->setInt("depthMap", 0);
	blurShader->setVec2("blurDir", blurDirY);
	blurShader->setMat4("projection", projection);
	blurShader->setMat4("view", view);

	glEnable(GL_DEPTH_TEST);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	
	//Horizontal blur

	glBindFramebuffer(GL_FRAMEBUFFER, blurFboH);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glClear(GL_DEPTH_BUFFER_BIT);
	shaderVAOQuad();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blurMapV);
	blurShader->setInt("depthMap", 0);
	blurShader->setVec2("blurDir", blurDirX);
	blurShader->setMat4("projection", projection);
	blurShader->setMat4("view", view);


	glEnable(GL_DEPTH_TEST);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDisable(GL_DEPTH_TEST);
	
	
	//--------------------Particle Thickness-------------------------
	ticknessShader->use();
	glBindFramebuffer(GL_FRAMEBUFFER, thickFbo);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(RenderParticleData), RenderParticles.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	ticknessShader->setMat4("projection", projection);
	ticknessShader->setMat4("view", view);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);
	glDepthMask(GL_FALSE);
	
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	glDrawArrays(GL_POINTS, 0, (GLsizei)count);
	glBindVertexArray(0);

	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	
	//--------------------Particle fluidFinal-------------------------

	fluidShader->use();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shaderVAOQuad();

	fluidShader->setInt("renderMode", renderMode);
	fluidShader->setMat4("projection", projection);
	fluidShader->setMat4("view", view);
	fluidShader->setVec4("color", color);
	fluidShader->setVec2("invTexScale", glm::vec2(1.0f / screenSize.x, 1.0f / screenSize.y));

	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blurFboH);
	fluidShader->setInt("depthMap", 0);
	
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, thickMap);
	fluidShader->setInt("thicknessMap", 1);
	

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, planeMap);
	fluidShader->setInt("sceneMap", 3);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glDisable(GL_DEPTH_TEST);
	
}

int main(int argc, char **argv)
{
	int success;
	window = new Window(success, 1600, 900);
	if (!success) return -1;

	camera = new Camera();
	window->camera = camera;

	init_sph_system();
	window->solver = solver;

	Frame frame;

	set_shaders();

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);
	glEnable(GL_POINT_SPRITE_ARB);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	init_particles();
	init_water();

	skybox = new Skybox();
	screen = new Screen();

	while (window->continueLoop())
	{
		//Clear buffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera->update();
		solver->update(frame);
		render_particles();
		render_water();		

		timer->update();
		memset(title, 0, 50);
		sprintf(title, "MagicaFilud. FPS: %f", timer->get_fps());
		glfwSetWindowTitle(window->w, title);
		
		window->swapBuffersAndPollEvents();
	}
    return 0;
}
