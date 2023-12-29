module;

import math;
import geo;
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
export module model;

export namespace model
{
	class Model
	{
	public:
		bool LoadModel(std::string& modelName);
		size_t GetFaceNum();
		size_t GetVertNum();
		std::vector<math::Vec3f>& GetVertices();
		std::vector<math::Vec3i>& GetFaces();
		std::vector<geo::Triangle>& GetTriangles();
	private:
		std::vector<math::Vec3f> m_vertices;
		std::vector<math::Vec3i> m_faces;
		std::vector<math::Vec2f> m_texCoords;
		std::vector<math::Vec3f> m_normals;

		std::vector<geo::Triangle> m_tris;
	};

	bool Model::LoadModel(std::string& modelName)
	{
		m_vertices.clear();
		m_faces.clear();

		std::ifstream in;
		in.open(modelName, std::ifstream::in);
		if (in.fail()) return -1;

		std::string line;
		while (!in.eof())
		{
			std::getline(in, line);
			std::istringstream iss(line.c_str());
			char type;
			char slash;
			if (!line.compare(0, 2, "v "))
			{
				iss >> type;
				math::Vec3f v;
				iss >> v.data[0] >> v.data[1] >> v.data[2];
				m_vertices.emplace_back(v);
			}
			else if (!line.compare(0, 4, "vt  "))
			{
				iss >> type >> type;
				float u, v, null;
				iss >> u >> v >> null;
				m_texCoords.emplace_back(math::Vec2f(u, v));
			}
			else if (!line.compare(0, 4, "vn  "))
			{
				iss >> type >> type;
				float x, y, z;
				iss >> x >> y >> z;
				m_normals.emplace_back(x, y, z);
			}
			else if (!line.compare(0, 2, "f "))
			{
				int v0, v1, v2, vt0, vt1, vt2, vn0, vn1, vn2;
				iss >> type;

				iss >> v0 >> slash >> vt0 >> slash >> vn0;
				iss >> v1 >> slash >> vt1 >> slash >> vn1;
				iss >> v2 >> slash >> vt2 >> slash >> vn2;

				m_faces.emplace_back(math::Vec3i(--v0, --v1, --v2));

				geo::Triangle tri;
				tri.vertices[0]  = m_vertices[v0],   tri.vertices[1]  = m_vertices[v1],   tri.vertices[2]  = m_vertices[v2];
				tri.texCoords[0] = m_texCoords[--vt0], tri.texCoords[1] = m_texCoords[--vt1], tri.texCoords[2] = m_texCoords[--vt2];
				tri.normals[0]   = m_normals[--vn0],   tri.normals[1]   = m_normals[--vn1],   tri.normals[2]   = m_normals[--vn2];

				m_tris.emplace_back(tri);
			}
		}
		std::cout << "# v# " << m_vertices.size() << " f# " << m_faces.size() << std::endl;

		return 0;
	}

	size_t Model::GetFaceNum()
	{
		return m_faces.size();
	}


	size_t Model::GetVertNum()
	{
		return m_vertices.size();
	}

	std::vector<math::Vec3f>& Model::GetVertices()
	{
		return m_vertices;
	}

	std::vector<math::Vec3i>& Model::GetFaces()
	{
		return m_faces;
	}
	std::vector<geo::Triangle>& Model::GetTriangles()
	{
		return m_tris;
	}
}