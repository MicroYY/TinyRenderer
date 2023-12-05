module;

import matrix;
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
		std::vector<matrix::Vec3f>& GetVertices();
		std::vector<std::vector<int>>& GetFaces();
	private:
		std::vector<matrix::Vec3f>    m_vertices;
		std::vector<std::vector<int>> m_faces;
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
			char trash;
			if (!line.compare(0, 2, "v "))
			{
				iss >> trash;
				matrix::Vec3f v;
				for (int i = 0; i < 3; i++)
				{
					iss >> v.data[i];
				}
				m_vertices.emplace_back(v);
			}
			else if (!line.compare(0, 2, "f "))
			{
				std::vector<int> f;
				int itrash, idx;
				iss >> trash;
				while (iss >> idx >> trash >> itrash >> trash >> itrash)
				{
					idx--;
					f.push_back(idx);
				}
				m_faces.emplace_back(f);
			}
		}
		std::cerr << "# v# " << m_vertices.size() << " f# " << m_faces.size() << std::endl;

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

	std::vector<matrix::Vec3f>& Model::GetVertices()
	{
		return m_vertices;
	}

	std::vector<std::vector<int>>& Model::GetFaces()
	{
		return m_faces;
	}
}