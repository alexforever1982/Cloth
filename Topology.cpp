
#include "Topology.h"

//==============================================================================

uint GetIndex(const std::vector<uint> &indices, uint triangle, uint ind1, uint ind2)
{
	const auto ind = 3 * triangle;

	const auto i1 = indices[ind + 0];
	const auto i2 = indices[ind + 1];
	const auto i3 = indices[ind + 2];

	if ((i1 != ind1) && (i1 != ind2))
	{
		return i1;
	}

	if ((i2 != ind1) && (i2 != ind2))
	{
		return i2;
	}

	return i3;
}

//==============================================================================

Topology::Topology(uint vertices_size, const std::vector<uint> &indices) noexcept
{
	edges.clear();
	vertices_vertices_edges.resize(vertices_size);

	for (uint i = 0; i < indices.size(); i += 3)
	{
		for (uint j = 0; j < 3; j++)
		{
			const auto ind1 = indices[i + j];
			const auto ind2 = indices[i + (j + 1) % 3];

			auto &edges1 = vertices_vertices_edges[ind1];
			auto &edges2 = vertices_vertices_edges[ind2];

			if ((edges1.find(ind2) == edges1.end()) &&
				(edges2.find(ind1) == edges2.end()))
			{
				edges.emplace_back(ind1, ind2);

				const auto edge = static_cast<uint>(edges.size() - 1);
				edges1[ind2] = edge;
				edges2[ind1] = edge;
			}
		}
	}

	edges_triangles.clear();
	edges_triangles.resize(edges.size());

	for (uint i = 0, triangle = 0; i < indices.size(); i += 3, triangle++)
	{
		const auto ind1 = indices[i + 0];
		const auto ind2 = indices[i + 1];
		const auto ind3 = indices[i + 2];

		const auto e1 = vertices_vertices_edges[ind1][ind2];
		const auto e2 = vertices_vertices_edges[ind2][ind3];
		const auto e3 = vertices_vertices_edges[ind3][ind1];

		edges_triangles[e1].push_back(triangle);
		edges_triangles[e2].push_back(triangle);
		edges_triangles[e3].push_back(triangle);
	}

	for (uint i = 0; i < edges.size(); i++)
	{
		auto &edge = edges[i];
		const auto &triangles = edges_triangles[i];
		edge.boundary = (triangles.size() <= 1);
		if (!edge.boundary)
		{
			const auto t1 = triangles[0];
			const auto t2 = triangles[1];

			const auto ind1 = edge.ind1;
			const auto ind2 = edge.ind2;

			edge.ind3 = GetIndex(indices, t1, ind1, ind2);
			edge.ind4 = GetIndex(indices, t2, ind1, ind2);
		}
	}
}

//==============================================================================

const std::vector<Topology::Edge> &Topology::GetEdges() const noexcept
{
	return edges;
}

//==============================================================================
