
#pragma once

//==============================================================================

#include <unordered_map>
#include <vector>

//==============================================================================

typedef unsigned int uint;

//==============================================================================

class Topology
{
public:
	struct Edge
	{
		uint ind1;
		uint ind2;
		uint ind3;
		uint ind4;
		bool boundary;

		Edge(uint ind1, uint ind2) noexcept :
			ind1(ind1),
			ind2(ind2),
			ind3(0),
			ind4(0),
			boundary(false)
		{
		}
	};

private:
	std::vector<Edge> edges;

	std::vector<std::vector<uint>> vertices_edges;
	std::vector<std::unordered_map<uint, uint>> vertices_vertices_edges;
	std::vector<std::vector<uint>> edges_triangles;

public:
	Topology(uint vertices_size, const std::vector<uint> &indices) noexcept;

	const std::vector<Edge> &GetEdges() const noexcept;
};

//==============================================================================
