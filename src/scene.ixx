module;

#include <tuple>

export module scene;

export namespace tr
{
	class Scene
	{
	public:
		Scene() : m_width(0), m_height(0) {}
		Scene(int width, int height) :m_width(height), m_height(height) {}
		void Init(int width, int height);
		void Update(int width, int height);
		std::tuple<int, int> GetSize();
	private:
		int m_width;
		int m_height;
	};

	void Scene::Init(int width, int height)
	{
		m_width = width;
		m_height = height;
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
}