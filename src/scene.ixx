module;

#include <tuple>
import math;

export module scene;

export namespace tr
{
	class Scene
	{
	public:
		Scene() : m_width(0), m_height(0) {}
		Scene(int width, int height, math::Vec3f lightDir) :m_width(height), m_height(height), m_lightDir(lightDir) {}
		void Init(int width, int height, math::Vec3f lightDir);
		void Update(int width, int height);
		std::tuple<int, int> GetSize();
		math::Vec3f GetLight();
	private:
		int m_width;
		int m_height;

		math::Vec3f m_lightDir;
	};

	void Scene::Init(int width, int height, math::Vec3f lightDir)
	{
		m_width = width;
		m_height = height;
		m_lightDir = lightDir;
	}

	void Scene::Update(int width, int height)
	{
		m_width = width;
		m_height = height;
	}

	std::tuple<int, int> Scene::GetSize()
	{
		return { m_width, m_height };
	}
	math::Vec3f Scene::GetLight()
	{
		return m_lightDir;
	}
}