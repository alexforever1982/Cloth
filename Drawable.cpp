
#include "Drawable.h"

#include "GLAD/glad.h"

//==============================================================================

Drawable::Drawable() noexcept :
	VAO(0),
	VBO(0),
	EBO(0),
	count(0),
	model(1.0f)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
}

//==============================================================================

Drawable::~Drawable() noexcept
{
	Clear();
}

//==============================================================================

void Drawable::Draw() const noexcept
{
	if (!VAO || !count)
	{
		return;
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(0));
	glBindVertexArray(0);
}

//==============================================================================

const glm::mat4 &Drawable::GetModel() const noexcept
{
	return model;
}

//==============================================================================

void Drawable::SetModel(const glm::mat4 &model) noexcept
{
	this->model = model;
}

//==============================================================================

void Drawable::SetBuffers(const std::vector<float> &vertices,
	                      const std::vector<float> &normals,
	                      const std::vector<float> &uvs,
	                      const std::vector<uint> &indices) noexcept
{
	Clear();

	count = static_cast<unsigned int>(indices.size());

	const auto vertices_size = vertices.size() * sizeof(float);
	const auto normals_size  = normals.size()  * sizeof(float);
	const auto uvs_size      = uvs.size()      * sizeof(float);
	const auto indices_size  = indices.size()  * sizeof(uint);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices_size + normals_size + uvs_size, nullptr, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_size, &vertices[0]);                  // {x, y, z}
	glBufferSubData(GL_ARRAY_BUFFER, vertices_size, normals_size, &normals[0]);        // {nx, ny, nz}
	glBufferSubData(GL_ARRAY_BUFFER, vertices_size + normals_size, uvs_size, &uvs[0]); // {u, v}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, &indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(vertices_size));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(vertices_size + normals_size));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//==============================================================================

void Drawable::UpdateBuffers(const std::vector<float> &vertices,
	                         const std::vector<float> &normals,
	                         const std::vector<float> &uvs) noexcept
{
	const auto vertices_size = vertices.size() * sizeof(GLfloat);
	const auto normals_size  = normals.size()  * sizeof(GLfloat);
	const auto uvs_size      = uvs.size()      * sizeof(GLfloat);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_size, &vertices[0]);                  // {x, y, z}
	glBufferSubData(GL_ARRAY_BUFFER, vertices_size, normals_size, &normals[0]);        // {nx, ny, nz}
	glBufferSubData(GL_ARRAY_BUFFER, vertices_size + normals_size, uvs_size, &uvs[0]); // {u, v}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(vertices_size));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(vertices_size + normals_size));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//==============================================================================

void Drawable::Clear() noexcept
{
	if (VAO)
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);

		VAO = 0;
		VBO = 0;
		EBO = 0;

		count = 0;
	}
}

//==============================================================================
