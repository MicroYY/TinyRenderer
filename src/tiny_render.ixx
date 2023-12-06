module;

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
import model;
import scene;
import matrix;

export module renderer;

export namespace tr
{
	class TinyRender
	{
	public:
		bool Init(std::string& modelName);
		bool Render(Scene& scene, unsigned int& texture);
		~TinyRender();

	private:
		void DrawLine(int x0, int y0, int x1, int y1, int width, int height);

		model::Model   m_model;
		unsigned char* m_frameBuffer;

	};

	bool TinyRender::Init(std::string& modelName)
	{
		m_model.LoadModel(modelName);

		return 0;
	}

	bool TinyRender::Render(Scene& scene, unsigned int& texture)
	{
		auto [width, height] = scene.GetSize();
		if (m_frameBuffer)
			m_frameBuffer = (unsigned char*)realloc(m_frameBuffer, width * height * 3);
		else
			m_frameBuffer = (unsigned char*)malloc(width * height * 3);
		memset(m_frameBuffer, 0, width * height * 3);

		for (size_t i = 0; i < m_model.GetFaceNum(); i++)
		{
			matrix::Vec3i face = m_model.GetFaces()[i];
			for (int j = 0; j < 3; j++)
			{
				matrix::Vec3f v0 = m_model.GetVertices()[face[j]];
				matrix::Vec3f v1 = m_model.GetVertices()[face[(j + 1) % 3]];
				int x0 = (v0.x + 1.) * width / 2.;
				int y0 = (v0.y + 1.) * height / 2.;
				int x1 = (v1.x + 1.) * width / 2.;
				int y1 = (v1.y + 1.) * height / 2.;
				DrawLine(x0, y0, x1, y1, width, height);
			}
		}
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_frameBuffer);
		glGenerateMipmap(GL_TEXTURE_2D);

		return true;
	}

	TinyRender::~TinyRender()
	{
		if (m_frameBuffer)
			free(m_frameBuffer);
	}

	void TinyRender::DrawLine(int x0, int y0, int x1, int y1, int width, int height)
	{
		bool steep = false;
		if (std::abs(x0 - x1) < std::abs(y0 - y1))
		{
			std::swap(x0, y0);
			std::swap(x1, y1);
			steep = true;
		}
		if (x0 > x1)
		{
			std::swap(x0, x1);
			std::swap(y0, y1);
		}

		for (int x = x0; x < x1; x++)
		{
			float t = (x - x0) / (float)(x1 - x0);
			int   y = y0 * (1. - t) + y1 * t;
			if (steep)
			{
				int offset = (x * width + y) * 3;
				m_frameBuffer[offset] = 255;
				m_frameBuffer[offset + 1] = 255;
				m_frameBuffer[offset + 2] = 255;
			}
			else
			{
				int offset = (y * width + x) * 3;
				m_frameBuffer[offset] = 255;
				m_frameBuffer[offset + 1] = 255;
				m_frameBuffer[offset + 2] = 255;
			}
		}
	}
}