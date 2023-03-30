
#include "Debug.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Shader.h"
#include "Texture.h"

#include "GLFW/glfw3.h"

//==============================================================================

void OnResize (GLFWwindow *window, int width, int height);
void OnMouse  (GLFWwindow *window, double x, double y);
void OnScroll (GLFWwindow *window, double dx, double dy);

void ProcessInput(GLFWwindow *window);

void Prepare() noexcept;
void Render();

//==============================================================================

const auto width  = 1280u;
const auto height = 720u;

Camera *camera = nullptr;
Shader *shader = nullptr;

//==============================================================================

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	auto window = glfwCreateWindow(width, height, "PBR", nullptr, nullptr);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, OnResize);
	glfwSetCursorPosCallback(window, OnMouse);
	glfwSetScrollCallback(window, OnScroll);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	camera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
	shader = new Shader("shaders\\simple.vs", "shaders\\simple.fs");

	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);
		Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	delete camera;
	delete shader;

	_CrtDumpMemoryLeaks();

	return 0;
}

//==============================================================================

void ProcessInput(GLFWwindow* window)
{
	const auto esc = glfwGetKey(window, GLFW_KEY_ESCAPE);
	if (esc == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	static auto delta_time = 0.0f;
	static auto last_time  = 0.0f;

	const auto current_time = static_cast<float>(glfwGetTime());
	delta_time = current_time - last_time;
	last_time = current_time;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera->Move(Camera::Direction::FORWARD, delta_time);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera->Move(Camera::Direction::BACKWARD, delta_time);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera->Move(Camera::Direction::LEFT, delta_time);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera->Move(Camera::Direction::RIGHT, delta_time);
	}
}

//==============================================================================

void OnResize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//==============================================================================

void OnMouse(GLFWwindow* window, double x, double y)
{
	static auto start = false;
	static auto last_x = static_cast<float>(width) / 2.0f;
	static auto last_y = static_cast<float>(height) / 2.0f;

	if (start)
	{
		last_x = static_cast<float>(x);
		last_y = static_cast<float>(y);
		start = false;
	}

	const auto dx = static_cast<float>(x) - last_x;
	const auto dy = last_y - static_cast<float>(y);

	last_x = static_cast<float>(x);
	last_y = static_cast<float>(y);

	camera->Rotate(dx, dy);
}

//==============================================================================

void OnScroll(GLFWwindow *window, double dx, double dy)
{
	camera->Zoom(static_cast<float>(dy));
}

//==============================================================================

void Prepare() noexcept
{

}

//==============================================================================

void Render()
{
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const auto aspect = static_cast<float>(width) / static_cast<float>(height);

	const auto model = glm::mat4(1.0f);
	const auto view = camera->GetView();
	const auto projection = camera->GetProjection(aspect);

	shader->Use();
	shader->SetMat4("model", model);
	shader->SetMat4("view", view);
	shader->SetMat4("projection", projection);
	shader->SetVec3("camera", camera->GetPosition());
}

//==============================================================================
