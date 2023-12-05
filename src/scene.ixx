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
		bool init(int width, int height);
		std::tuple<int, int> GetSize();
	private:
		int m_width;
		int m_height;
	};

	bool Scene::init(int width, int height)
	{
		m_width = width;
		m_height = height;

		return 0;
	}

	std::tuple<int, int> Scene::GetSize()
	{
		return { m_width, m_height };
	}
}