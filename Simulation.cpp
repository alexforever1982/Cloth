
#include "Debug.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Drawable.h"
#include "Physics.h"
#include "Ray.h"
#include "Shader.h"
#include "Texture.h"

#include "Cloth.h"

#include "GLFW/glfw3.h"

//==============================================================================

void OnResize      (GLFWwindow *window, int width, int height);
void OnMouseButton (GLFWwindow *window, int button, int action, int mods);
void OnMouseMotion (GLFWwindow *window, double x, double y);
void OnScroll      (GLFWwindow *window, double dx, double dy);
void OnKey         (GLFWwindow *window, int key, int scancode, int action, int mods);
void ProcessInput  (GLFWwindow *window);

Ray GetMouseRay(float x, float y, const glm::mat4 &model) noexcept;
glm::vec3 GetMouseShift(float x1, float y1, float x2, float y2, const glm::vec3 &P) noexcept;

void Prepare() noexcept;
void Render();

//==============================================================================

const auto width  = 1280u;
const auto height = 720u;

bool wireframe = false;

uint point;
glm::vec3 POINT;

Camera *camera     = nullptr;
Shader *shader     = nullptr;
Texture *texture   = nullptr;
Drawable *drawable = nullptr;
Physics *physics   = nullptr;

//==============================================================================

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	auto window = glfwCreateWindow(width, height, "Cloth", nullptr, nullptr);

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, OnResize);
	glfwSetKeyCallback(window, OnKey);
	glfwSetMouseButtonCallback(window, OnMouseButton);
	glfwSetCursorPosCallback(window, OnMouseMotion);
	glfwSetScrollCallback(window, OnScroll);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	Prepare();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glViewport(0, 0, width, height);

	while (!glfwWindowShouldClose(window))
	{
		ProcessInput(window);

		physics->Simulate();

		Render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	delete camera;
	delete shader;
	delete texture;
	delete drawable;
	delete physics;

	_CrtDumpMemoryLeaks();

	return 0;
}

//==============================================================================

void OnResize(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//==============================================================================

void OnMouseButton(GLFWwindow *window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			double mx, my;
			glfwGetCursorPos(window, &mx, &my);

			const auto x = static_cast<float>(mx);
			const auto y = static_cast<float>(my);
			const glm::mat4 model(1.0f);
			const auto ray = GetMouseRay(x, y, model);

			uint index;
			glm::vec3 P;
			if (physics->Raycast(ray, index, P))
			{
				point = index;
				POINT = P;

				physics->FixClothPoint(index);
			}
		}
		else
		if (action == GLFW_RELEASE)
		{
			physics->FreeClothPoint(point);
		}
	}
}

//==============================================================================

void OnMouseMotion(GLFWwindow *window, double x, double y)
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

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		const auto mx = static_cast<float>(x);
		const auto my = static_cast<float>(y);
		const glm::mat4 model(1.0f);
		const auto ray = GetMouseRay(mx, my, model);

		const auto x1 = static_cast<float>(x) - dx;
		const auto y1 = static_cast<float>(y) + dy;
		const auto x2 = static_cast<float>(x);
		const auto y2 = static_cast<float>(y);

		const auto dP = GetMouseShift(x1, y1, x2, y2, POINT);

		const auto &ex = camera->GetRight();
		const auto &ey = camera->GetUp();

		const auto translation = glm::dot(dP, ex) * ex + glm::dot(dP, ey) * ey;

		physics->MoveClothPoint(point, translation);
	}
	else
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		camera->Rotate(dx, dy);
	}
	else
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		camera->Move(Camera::Direction::LEFT, 0.002f * dx);
	}
}

//==============================================================================

void OnScroll(GLFWwindow *window, double dx, double dy)
{
	camera->Zoom(static_cast<float>(dy));
}

//==============================================================================

void OnKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_F)
	{
		if (action == GLFW_PRESS)
		{
			wireframe = !wireframe;
		}
	}
}

//==============================================================================

void ProcessInput(GLFWwindow *window)
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

Ray GetMouseRay(float x, float y, const glm::mat4 &model) noexcept
{
	const auto w = static_cast<float>(width);
	const auto h = static_cast<float>(height);

	const auto wx = static_cast<float>(x);
	const auto wy = static_cast<float>(height - y);

	const auto aspect = static_cast<float>(width) / static_cast<float>(height);

	const auto &view = camera->GetView();
	const auto &proj = camera->GetProjection(aspect);

	const auto modelview = view * model;

	const auto viewport = glm::vec4(0.0f, 0.0f, w, h);

	const auto p0 = glm::unProject(glm::vec3(wx, wy, 0.0f), modelview, proj, viewport);
	const auto p1 = glm::unProject(glm::vec3(wx, wy, 1.0f), modelview, proj, viewport);

	const glm::vec3 origin(p0.x, p0.y, p0.z);
	const glm::vec3    end(p1.x, p1.y, p1.z);

	return Ray(origin, end);
}

//==============================================================================

glm::vec3 GetMouseShift(float x1, float y1, float x2, float y2, const glm::vec3 &P) noexcept
{
	const glm::mat4 model(1.0f);

	const auto ray1 = GetMouseRay(x1, y1, model);
	const auto ray2 = GetMouseRay(x2, y2, model);

	const auto &A1 = ray1.GetOrigin();
	const auto &B1 = ray1.GetEnd();

	const auto &A2 = ray2.GetOrigin();
	const auto &B2 = ray2.GetEnd();

	const auto N = glm::normalize(camera->GetFront());

	glm::vec3 P1, P2;
	ray1.PlaneIntersection(P, N, P1);
	ray2.PlaneIntersection(P, N, P2);

	return P2 - P1;
}

//==============================================================================

void Prepare() noexcept
{
	camera   = new Camera(glm::vec3(0.0f, 0.45f, 1.5f));
	shader   = new Shader("shaders\\simple.vs", "shaders\\simple.fs");
	texture  = new Texture;
	drawable = new Drawable;
	physics  = new Physics();

	const auto w = 1.0f;
	const auto h = 1.0f;
	const auto s = 0.02f;

	physics->AddCloth(w, h, s);

	const auto nx = static_cast<uint>(w / s);
	const auto ny = static_cast<uint>(h / s);

	physics->FixClothPoint((ny + 1) *       1  - 1);
	physics->FixClothPoint((ny + 1) * (nx + 1) - 1);

	texture->Load("textures\\cloth.png");

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<uint>  indices;

	physics->GetCloth(vertices, normals, indices);
	drawable->SetBuffers(vertices, normals, vertices, indices);
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

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<uint>  indices;

	physics->GetCloth(vertices, normals, indices);
	drawable->UpdateBuffers(vertices, normals, vertices);

	shader->Use();
	shader->SetMat4("model", model);
	shader->SetMat4("view", view);
	shader->SetMat4("projection", projection);
	shader->SetVec3("camera", camera->GetPosition());
	shader->SetVec3("lightPos", glm::vec3(0.0f, 0.0f, 1.0f));
	shader->SetInt ("diffuse_map", 0);

	texture->Bind(0);

	if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	drawable->Draw();

	glad_glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

//==============================================================================
