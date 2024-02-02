module;

#include <iostream>
#include <cmath>

export module math;

export namespace math
{
	template <class T, int N>
	struct Vec
	{
		T data[N];
	};

	template <class T>
	struct Vec<T, 2>
	{
		typedef Vec<T, 2> type;
		union
		{
			struct { T u, v; };
			struct { T x, y; };
			T data[2];
		};

		Vec() : u(0), v(0) {}
		Vec(T _u, T _v) : u(_u), v(_v) {}
		type operator+(const type& other) { return type(u + other.u, v + other.v); }
		type operator-(const type& other) { return type(u - other.u, v - other.v); }
		type operator*(float f) { return type(u * f, v * f); }
		template <class> friend std::ostream& operator<<(std::ostream& out, Vec<T, 2>& other);
	};

	template <class T>
	auto operator*(float f, Vec<T, 2>& other)
	{
		return other * f;
	}

	template <class T>
	auto& operator<<(std::ostream& out, Vec<T, 2>& other)
	{
		out << "(" << other.x << ", " << other.y << ")\n";
		return out;
	}


	template <class T>
	struct Vec<T, 3>
	{
		typedef Vec<T, 3> type;
		union
		{
			struct { T x, y, z; };
			struct { T ivert, iuv, inorm; };
			struct { T r, g, b; };
			T data[3];
		};
		Vec() : x(0), y(0), z(0) {}
		Vec(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
		type operator^ (const type& v) const { return type(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
		type operator+ (const type& v) const { return type(x + v.x, y + v.y, z + v.z); }
		type operator- (const type& v) const { return type(x - v.x, y - v.y, z - v.z); }
		type operator* (float       f) const { return type(x * f, y * f, z * f); }
		type operator/ (float       f) const { return type(x / f, y / f, z / f); }
		T    operator* (const type& v) const { return x * v.x + y * v.y + z * v.z; }
		T    operator[](int         i) const { return data[i]; }

		float norm() const { return std::sqrt(x * x + y * y + z * z); }
		type& normalize(T l = 1) { *this = (*this) * (l / norm()); return *this; }
		template <class > friend std::ostream& operator<<(std::ostream& s, Vec<T, 3>& v);
	};

	template <class T>
	auto operator*(float f, Vec<T, 3>& other)
	{
		return other * f;
	}

	template <class T>
	auto& operator<<(std::ostream& out, Vec<T, 3>& other)
	{
		out << "(" << other.x << ", " << other.y << ", " << other.z << ")\n";
		return out;
	}

	template <class T>
	auto CrossProduct(Vec<T, 3> v1, Vec<T, 3> v2)
	{
		return v1 ^ v2;
	}

	template <class T, int M, int N>
	struct Matrix
	{
		T data[M][N];
	};

	template <class T>
	struct Matrix<T, 2, 2>
	{
		typedef Matrix<T, 2, 2> type;

		union
		{
			struct { T a, b, c, d; };
			T data[2][2];
		};

		Matrix(T _a, T _b, T _c, T _d) : a(_a), b(_b), c(_c), d(_d) {};

		type    operator*(const type& other) { return type(a * other.a + b * other.c, a * other.b + b * other.d, 
														c * other.a + d * other.c, c * other.b + d * other.d); };
		Vec<T, 2> operator*(const Vec<T, 2>& v) const { return Vec<T, 2>(a * v.x + b * v.y, c * v.x + d * v.y); };
		type    operator/(float f) const { return type(a / f, b / f, c / f, d / f); };
		
		T    Det()     const { return data[0][0] * data[1][1] - data[0][1] * data[1][0]; };
		type Inverse() const { return type(d, -b, -c, a) / Det(); };

	};


	typedef Vec<int,   2> Vec2i;
	typedef Vec<float, 2> Vec2f;
	typedef Vec<int,   3> Vec3i;
	typedef Vec<float, 3> Vec3f;
	typedef Vec<int,   4> Vec4i;
	typedef Vec<float, 4> Vec4f;

	typedef Vec2i       Point2i;
	typedef Vec2f       Point2f;
	typedef Vec3i       Point3i;
	typedef Vec3f       Point3f;

	typedef Matrix<float, 2, 2> Matrix2f;
	typedef Matrix<float, 3, 3> Matrix3f;

	typedef Point2f Triangle2f[3];
	typedef Point2i Triangle2i[3];
	typedef Point3f Triangle3f[3];
	typedef Point3i Triangle3i[3];

	typedef Vec<uint8_t, 3> Color;

}