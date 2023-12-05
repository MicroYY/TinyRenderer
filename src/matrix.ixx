module;

#include <iostream>
#include <cmath>

export module matrix;

export namespace matrix
{
	template <class T>
	struct Vec2
	{
		typedef Vec2<T> type;
		union
		{
			struct { T u, v; };
			struct { T x, y; };
			T data[2];
		};

		Vec2() : u(0), v(0) {}
		Vec2(T _u, T _v) : u(_u), v(_v) {}
		type operator+(const type& other) { return type(u + other.u, v + other.v); }
		type operator-(const type& other) { return type(u - other.u, v - other.v); }
		type operator*(float f) { return type(u * f, v * f); }
		template <class> friend std::ostream& operator<<(std::ostream& out, type& other);
	};

	template <class T>
	auto operator*(float f, Vec2<T>& other)
	{
		return other * f;
	}

	template <class T>
	auto& operator<<(std::ostream& out, Vec2<T>& other)
	{
		out << "(" << other.x << ", " << other.y << ")\n";
		return out;
	}
	

	template <class T>
	struct Vec3
	{
		typedef Vec3<T> type;
		union
		{
			struct { T x, y, z; };
			struct { T ivert, iuv, inorm; };
			T data[3];
		};
		Vec3() : x(0), y(0), z(0) {}
		Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
		type operator ^(const type& v) const { return type(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
		type operator +(const type& v) const { return type(x + v.x, y + v.y, z + v.z); }
		type operator -(const type& v) const { return type(x - v.x, y - v.y, z - v.z); }
		type operator *(float f)          const { return type(x * f, y * f, z * f); }
		T       operator *(const type& v) const { return x * v.x + y * v.y + z * v.z; }
		float norm() const { return std::sqrt(x * x + y * y + z * z); }
		type& normalize(T l = 1) { *this = (*this) * (l / norm()); return *this; }
		template <class > friend std::ostream& operator<<(std::ostream& s, type& v);
	};

	template <class T>
	auto operator*(float f, Vec3<T>& other)
	{
		return other * f;
	}

	template <class T>
	auto& operator<<(std::ostream& out, Vec3<T>& other)
	{
		out << "(" << other.x << ", " << other.y << ", " << other.z << ")\n";
		return out;
	}


	typedef Vec2<int>   Vec2i;
	typedef Vec2<float> Vec2f;
	typedef Vec2<int>   Vec2i;
	typedef Vec3<float> Vec3f;

}