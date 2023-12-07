module;

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
import model;
import scene;
import math;

export module renderer;

export namespace tr
{
	class TinyRender
	{
	public:
		bool Init(std::string& modelName, unsigned int& texture);
		bool Render(Scene& scene);
		~TinyRender();

	private:
		void DrawLine(math::Point2i& p0, math::Point2i& p1, int width, int height, math::Color&& color);
		void DrawTriangle(math::Triangle2i& tri, int width, int height, math::Color& color);

		model::Model   m_model;
		unsigned char* m_frameBuffer;

		unsigned int   m_texture;
	};

	bool TinyRender::Init(std::string& modelName, unsigned int& texture)
	{
		m_model.LoadModel(modelName);

		glGenTextures(1, &texture);

		m_texture = texture;

		return 0;
	}

	bool TinyRender::Render(Scene& scene)
	{
		auto [width, height] = scene.GetSize();
		if (m_frameBuffer)
			m_frameBuffer = (unsigned char*)realloc(m_frameBuffer, width * height * 3);
		else
			m_frameBuffer = (unsigned char*)malloc(width * height * 3);
		memset(m_frameBuffer, 0, width * height * 3);

		for (size_t i = 0; i < m_model.GetFaceNum(); i++)
		{
			math::Vec3i face = m_model.GetFaces()[i];
			for (int j = 0; j < 3; j++)
			{
				math::Vec3f v0 = m_model.GetVertices()[face[j]];
				math::Vec3f v1 = m_model.GetVertices()[face[(j + 1) % 3]];
				math::Point2i p0((v0.x + 1.) * width / 2., (v0.y + 1.) * height / 2);
				math::Point2i p1((v1.x + 1.) * width / 2., (v1.y + 1.) * height / 2.);

				DrawLine(p0, p1, width, height, math::Color(255, 255, 255));
			}
		}
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_frameBuffer);
		glGenerateMipmap(GL_TEXTURE_2D);

		return true;
	}

	TinyRender::~TinyRender()
	{
		if (m_frameBuffer)
			free(m_frameBuffer);
	}

	void TinyRender::DrawLine(math::Point2i& p0, math::Point2i& p1, int width, int height, math::Color&& color)
	{
		auto x0 = p0.x, y0 = p0.y;
		auto x1 = p1.x, y1 = p1.y;
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
				m_frameBuffer[offset] = color.r;
				m_frameBuffer[offset + 1] = color.g;
				m_frameBuffer[offset + 2] = color.b;
			}
			else
			{
				int offset = (y * width + x) * 3;
				m_frameBuffer[offset] = color.r;
				m_frameBuffer[offset + 1] = color.g;
				m_frameBuffer[offset + 2] = color.b;
			}
		}
	}

	void TinyRender::DrawTriangle(math::Triangle2i& tri, int width, int height, math::Color& color)
	{
		DrawLine(tri[0], tri[1], width, height, math::Color(255, 255, 255));
	}
}