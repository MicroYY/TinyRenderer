module;

#include <string>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
import model;
import scene;
import math;
import io;

export module renderer;

export namespace tr
{
	class TinyRender
	{
	public:
		bool Init(std::string& modelName, unsigned int& texture);
		bool Render(Scene& scene);
		bool LoadTexture(std::string& textureName);
		~TinyRender();

	private:
		void DrawLine(math::Point2i& p0, math::Point2i& p1, int width, int height, math::Color&& color);
		void DrawTriangle(math::Triangle2i& tri, int width, int height, math::Color& color);
		void DrawTriangle(math::Triangle3f& tri, int width, int height, math::Color&& color);
		void DrawTriangleWithTexture(math::Triangle3f& tri, int width, int height, math::Vec2f*);
		bool InsideTriangle(math::Triangle3f& tri, int x, int y) const;
		std::tuple<float, float, float> ComputeBarycentric(math::Triangle3f& tri, int x, int y);

		model::Model   m_model;
		unsigned char* m_frameBuffer;
		float*         m_zBuffer;

		unsigned int   m_gpuTextureHandle;

		io::TgaImage   m_texture;

		bool m_lineMode;
	};

	bool TinyRender::Init(std::string& modelName, unsigned int& textureHandle)
	{
		m_model.LoadModel(modelName);

		glGenTextures(1, &textureHandle);

		m_gpuTextureHandle = textureHandle;

		return 0;
	}

	bool TinyRender::Render(Scene& scene)
	{
		auto [width, height] = scene.GetSize();
		auto lightDir = scene.GetLight();
		if (m_frameBuffer)
			m_frameBuffer = (unsigned char*)realloc(m_frameBuffer, width * height * 3);
		else
			m_frameBuffer = (unsigned char*)malloc(width * height * 3);
		memset(m_frameBuffer, 0, width * height * 3);

		if (m_zBuffer)
			m_zBuffer = (float*)realloc(m_zBuffer, width * height * sizeof(float));
		else
			m_zBuffer = (float*)malloc(width * height * sizeof(float));
		for (int i = 0; i < width * height; i++)
		{
			m_zBuffer[i] = -std::numeric_limits<double>::infinity();
		}

		for (size_t i = 0; i < m_model.GetFaceNum(); i++)
		{
			if (m_lineMode)
			{
				math::Vec3i face = m_model.GetFaces()[i];
				for (int j = 0; j < 3; j++)
				{
					math::Vec3f v0 = m_model.GetVertices()[face[j]];
					math::Vec3f v1 = m_model.GetVertices()[face[(j + 1) % 3]];
					math::Point2i p0((v0.x + 1.) * width / 2., (v0.y + 1.) * height / 2.);
					math::Point2i p1((v1.x + 1.) * width / 2., (v1.y + 1.) * height / 2.);

					DrawLine(p0, p1, width, height, math::Color(255, 255, 255));
				}
			}
			else
			{
				auto face = m_model.GetTriangles()[i];
				auto v0 = face.vertices[0];
				auto v1 = face.vertices[1];
				auto v2 = face.vertices[2];

				math::Point3f p0((v0.x + 1.) * width / 2., (v0.y + 1.) * height / 2., v0.z);
				math::Point3f p1((v1.x + 1.) * width / 2., (v1.y + 1.) * height / 2., v1.z);
				math::Point3f p2((v2.x + 1.) * width / 2., (v2.y + 1.) * height / 2., v2.z);

				math::Triangle3f tri   = { p0, p1, p2 };
				math::Vec2f      vt[3] = { face.texCoords[0], face.texCoords[1], face.texCoords[2] };

				// Cross product to get normal
				math::Vec3f normal = ((v2 - v0) ^ (v1 - v0)).normalize();
				auto intensity = normal * lightDir;
				// Back-face culling
				if (intensity > 0)
				{
					if (m_texture.Buffer())
					{
						DrawTriangleWithTexture(tri, width, height, vt);
					}
					else
					{
						DrawTriangle(tri, width, height, math::Color(intensity * 255, intensity * 255, intensity * 255));
					}
				}
			}
		}
		glBindTexture(GL_TEXTURE_2D, m_gpuTextureHandle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_frameBuffer);
		glGenerateMipmap(GL_TEXTURE_2D);

		return true;
	}

	bool TinyRender::LoadTexture(std::string& textureName)
	{
		return m_texture.ReadTgaFile(textureName.c_str());
	}

	TinyRender::~TinyRender()
	{
		if (m_frameBuffer)
			free(m_frameBuffer);
		if (m_zBuffer)
			free(m_zBuffer);
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
				if (offset >= width * height * 3)
					continue;
				m_frameBuffer[offset] = color.r;
				m_frameBuffer[offset + 1] = color.g;
				m_frameBuffer[offset + 2] = color.b;
			}
			else
			{
				int offset = (y * width + x) * 3;
				if (offset >= width * height * 3)
					continue;
				m_frameBuffer[offset] = color.r;
				m_frameBuffer[offset + 1] = color.g;
				m_frameBuffer[offset + 2] = color.b;
			}
		}
	}

	void tr::TinyRender::DrawTriangleWithTexture(math::Triangle3f& tri, int width, int height, math::Vec2f* vt)
	{
		math::Vec2f bounding_min;
		math::Vec2f	bounding_max;
		bounding_min.x = std::min(tri[0].x, std::min(tri[1].x, tri[2].x));
		bounding_min.y = std::min(tri[0].y, std::min(tri[1].y, tri[2].y));
		bounding_max.x = std::max(tri[0].x, std::max(tri[1].x, tri[2].x));
		bounding_max.y = std::max(tri[0].y, std::max(tri[1].y, tri[2].y));

		for (int x = std::floor(bounding_min.x); x <= std::ceil(bounding_max.x); x++)
		{
			for (int y = std::floor(bounding_min.y); y <= std::ceil(bounding_max.y); y++)
			{
				if (InsideTriangle(tri, x, y))
				{
					// Get barycentric to interpolate depth z
					auto [alpha, beta, gamma] = ComputeBarycentric(tri, x, y);
					auto inteZ = alpha * tri[0].z + beta * tri[1].z + gamma * tri[2].z;
					int offset = y * width + x;
					if (m_zBuffer[offset] < inteZ)
					{
						m_zBuffer[y * width + x] = inteZ;
						offset *= 3;
						int u = (alpha * vt[0].u + beta * vt[1].u + gamma * vt[2].u) * m_texture.GetWidth();
						int v = (alpha * vt[0].v + beta * vt[1].v + gamma * vt[2].v) * m_texture.GetHeight();
						int idx = (v * m_texture.GetWidth() + u) * m_texture.GetBytespp();
						m_frameBuffer[offset]     = m_texture.Buffer()[idx + 2];
						m_frameBuffer[offset + 1] = m_texture.Buffer()[idx + 1];
						m_frameBuffer[offset + 2] = m_texture.Buffer()[idx + 0];
					}
				}
			}
		}
	}

	void TinyRender::DrawTriangle(math::Triangle2i& tri, int width, int height, math::Color& color)
	{
		if (true)
		{

		}
		else
		{
			DrawLine(tri[0], tri[1], width, height, math::Color(255, 255, 255));
			DrawLine(tri[1], tri[2], width, height, math::Color(255, 255, 255));
			DrawLine(tri[2], tri[0], width, height, math::Color(255, 255, 255));
		}
	}
	
	void TinyRender::DrawTriangle(math::Triangle3f& tri, int width, int height, math::Color&& color)
	{
		math::Vec2f bounding_min;
		math::Vec2f	bounding_max;
		bounding_min.x = std::min(tri[0].x, std::min(tri[1].x, tri[2].x));
		bounding_min.y = std::min(tri[0].y, std::min(tri[1].y, tri[2].y));
		bounding_max.x = std::max(tri[0].x, std::max(tri[1].x, tri[2].x));
		bounding_max.y = std::max(tri[0].y, std::max(tri[1].y, tri[2].y));

		for (int x = std::floor(bounding_min.x); x <= std::ceil(bounding_max.x); x++)
		{
			for (int y = std::floor(bounding_min.y); y <= std::ceil(bounding_max.y); y++)
			{
				if (InsideTriangle(tri, x, y))
				{
					// Get barycentric to interpolate depth z
					auto [alpha, beta, gamma] = ComputeBarycentric(tri, x, y);
					auto inteZ = alpha * tri[0].z + beta * tri[1].z + gamma * tri[2].z;
					int offset = y * width + x;
					if (m_zBuffer[offset] < inteZ)
					{
						m_zBuffer[y * width + x] = inteZ;
						offset *= 3;
						m_frameBuffer[offset] = color.r;
						m_frameBuffer[offset + 1] = color.g;
						m_frameBuffer[offset + 2] = color.b;
					}
				}
			}
		}
	}

	bool TinyRender::InsideTriangle(math::Triangle3f& tri, int x, int y) const
	{
		math::Point3f p(x, y, 1);
		math::Point3f a(tri[0]);
		math::Point3f b(tri[1]);
		math::Point3f c(tri[2]);

		auto pa = p - a;
		auto pb = p - b;
		auto pc = p - c;

		auto z1 = math::CrossProduct(pa, pb).z;
		auto z2 = math::CrossProduct(pb, pc).z;
		auto z3 = math::CrossProduct(pc, pa).z;

		return z1 * z2 > 0 && z2 * z3 > 0;
	}

	std::tuple<float, float, float> TinyRender::ComputeBarycentric(math::Triangle3f& tri, int x, int y)
	{
		math::Vec2f CA(tri[0].x - tri[2].x, tri[0].y - tri[2].y);
		math::Vec2f CB(tri[1].x - tri[2].x, tri[1].y - tri[2].y);
		math::Vec2f CP(x - tri[2].x, y - tri[2].y);
		math::Matrix2f m(CA.x, CA.y, CB.x, CB.y);
		auto inv = m.Inverse();
		float u = CP.x * inv.a + CP.y * inv.c;
		float v = CP.x * inv.b + CP.y * inv.d;
		return { u, v, 1 - u - v };
	}
}