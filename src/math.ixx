module;

#include <iostream>
#include <cmath>

export module math;

export namespace math
{

	// vec
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




	// matrix
	template <class T, int M, int N>
	struct Matrix
	{
		Matrix() {};
		T data[M][N];
	};

	// matrix 2x2
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

	// matrix3x3
	template <class T>
	struct Matrix<T, 3, 3>
	{
		typedef Matrix<T, 3, 3> type;

		Matrix(T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8, T v9) 
		{
			data[0][0] = v1, data[0][1] = v2, data[0][2] = v3;
			data[1][0] = v4, data[1][1] = v5, data[1][2] = v6;
			data[2][0] = v7, data[2][1] = v8, data[2][2] = v9;
		};

		union
		{
			//struct { T a, b, c, d; };
			//T data[9];
			T data[3][3];
		};

		template <class> friend std::ostream& operator<<(std::ostream&, const Matrix<T, 3, 3>&);
		template <class> friend std::istream& operator>>(std::istream&, const Matrix<T, 3, 3>&);

		static constexpr type Indentity()
		{
			return type(
				1, 0, 0,
				0, 1, 0,
				0, 0, 1);
		}
	};

	template<class T>
	std::ostream& operator<<(std::ostream& out, const Matrix<T, 3, 3>& m)
	{
		// TODO: insert return statement here
	}

	template<class T >
	std::istream& operator>>(std::istream& in, const Matrix<T, 3, 3>& m)
	{
		for (size_t i = 0; i < 3; i++)
		{
			for (size_t j = 0; j < 3; j++) 
			{
				in >> m.data[i][j];
			}
		}
	}

	// matrix4x4
	template <class T>
	struct Matrix<T, 4, 4>
	{
		typedef Matrix<T, 4, 4> type;

		Matrix() {}

		Matrix(
			T v1,  T v2,  T v3,  T v4,
			T v5,  T v6,  T v7,  T v8,
			T v9,  T v10, T v11, T v12,
			T v13, T v14, T v15, T v16)
		{
			data[0][0] = v1,  data[0][1] = v2,  data[0][2] = v3,  data[0][3] = v4;
			data[1][0] = v5,  data[1][1] = v6,  data[1][2] = v7,  data[1][3] = v8;
			data[2][0] = v9,  data[2][1] = v10, data[2][2] = v11, data[2][3] = v12;
			data[3][0] = v13, data[3][1] = v14, data[3][2] = v15, data[3][3] = v16;
		};

		union
		{
			//struct { T a, b, c, d; };
			//T data[9];
			T data[4][4];
		};

		type operator*(const type& other)
		{
			T trans;
			type temp;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					trans = 0;
					for (int k = 0; k < 4; ++k)
						trans += data[i][k] * other.data[k][j];
					temp.data[i][j] = trans;
				}
			}
			return temp;
		}

		Vec<T, 4> operator*(const Vec<T, 4>& vec)
		{
			Vec<T, 4> ret;

			for (int i = 0; i < 4; ++i)
			{
				T acc = 0;
				for (int j = 0; j < 4; ++j)
				{
					acc += data[i][j] * vec.data[j];
				}
				ret.data[i] = acc;
				
			}
			return ret;
		}

		template <class> friend std::ostream& operator<<(std::ostream&, const Matrix<T, 4, 4>&);
		template <class> friend std::istream& operator>>(std::istream&, const Matrix<T, 4, 4>&);

		static constexpr type Indentity()
		{
			return type(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1);
		}
	};

	template<class T>
	std::ostream& operator<<(std::ostream& out, const Matrix<T, 4, 4>& m)
	{
		out << "[" << 
			m.data[0][0] << ", " << m.data[0][1] << ", " << m.data[0][2] << ", " << m.data[0][3] << "\n " <<
			m.data[1][0] << ", " << m.data[1][1] << ", " << m.data[1][2] << ", " << m.data[1][3] << "\n " <<
			m.data[2][0] << ", " << m.data[2][1] << ", " << m.data[2][2] << ", " << m.data[2][3] << "\n " <<
			m.data[3][0] << ", " << m.data[3][1] << ", " << m.data[3][2] << ", " << m.data[3][3] <<
			"]\n";
		return out;
	}

	template<class T >
	std::istream& operator>>(std::istream& in, const Matrix<T, 4, 4>& m)
	{
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = 0; j < 4; j++)
			{
				in >> m.data[i][j];
			}
		}
	}



	// defs
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
	typedef Matrix<float, 4, 4> Matrix4f;

	typedef Point2f Triangle2f[3];
	typedef Point2i Triangle2i[3];
	typedef Point3f Triangle3f[3];
	typedef Point3i Triangle3i[3];

	typedef Vec<uint8_t, 3> Color;

}