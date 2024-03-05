module;

#include <tuple>
import math;

export module scene;

export namespace tr
{
	struct Camera
	{
		math::Vec3f pos;
		math::Vec3f rot;
		math::Vec3f dir;
	};
	class Scene
	{
	public:
		Scene() : m_width(0), m_height(0) {}
		Scene(int width, int height, math::Vec3f lightDir) :m_width(height), m_height(height), m_lightDir(lightDir) {}
		void Init(int width, int height, math::Vec3f lightDir);
		void Update(int width, int height, Camera& camera);
		std::tuple<int, int> GetWindowSize();
		math::Vec3f GetLight();
		math::Vec3f	GetPos();
		math::Vec3f GetRot();
		math::Vec3f GetDir();
	private:
		int m_width;
		int m_height;

		math::Vec3f m_lightDir;

		Camera m_camera;
	};

	void Scene::Init(int width, int height, math::Vec3f lightDir)
	{
		m_width = width;
		m_height = height;
		m_lightDir = lightDir;
	}

	void Scene::Update(int width, int height, Camera& camera)
	{
		m_width = width;
		m_height = height;
		m_camera = camera;
	}

	std::tuple<int, int> Scene::GetWindowSize()
	{
		return { m_width, m_height };
	}
	math::Vec3f Scene::GetLight()
	{
		return m_lightDir;
	}
	math::Vec3f Scene::GetPos()
	{
		return m_camera.pos;
	}
	math::Vec3f Scene::GetRot()
	{
		return m_camera.rot;
	}
	math::Vec3f Scene::GetDir()
	{
		return m_camera.dir;
	}
}