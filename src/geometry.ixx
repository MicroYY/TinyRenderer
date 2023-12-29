module;

import math;

export module geo;

export namespace geo
{
	struct Vertex
	{
		math::Point3f v;
		math::Vec2f   texCoord;
		math::Vec3f   normal;
	};

	struct Triangle
	{
		math::Triangle3f vertices;
		math::Vec2f      texCoords[3];
		math::Vec3f      normals[3];
	};
}