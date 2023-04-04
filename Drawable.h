
#pragma once

//==============================================================================

#include <vector>

#include <glm/glm.hpp>

//==============================================================================

typedef unsigned int uint;

//==============================================================================

class Drawable
{
protected:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	unsigned int count;
	glm::mat4 model;

public:
	Drawable() noexcept;
	Drawable(const Drawable &) = delete;
	virtual ~Drawable() noexcept;

	void Draw() const noexcept;

	const glm::mat4 &GetModel() const     noexcept;
	void SetModel(const glm::mat4 &model) noexcept;

	void SetBuffers(const std::vector<float> &vertices,
		            const std::vector<float> &normals,
		            const std::vector<float> &uvs,
		            const std::vector<uint>  &indices) noexcept;

	void UpdateBuffers(const std::vector<float> &vertices,
		               const std::vector<float> &normals,
		               const std::vector<float> &uvs)  noexcept;

	void Clear() noexcept;
};

//==============================================================================
