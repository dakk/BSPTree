#pragma once

#include <cmath>
#include <iostream>

template <class T> class Vec3D
{

        public:

                inline Vec3D(void) { p[0] = p[1] = p[2] = 0.0; }

		inline Vec3D (T p0, T p1, T p2)
		{
			p[0] = p0;
			p[1] = p1;
			p[2] = p2;
		}

		inline Vec3D (const Vec3D & v)
		{
			init(v[0], v[1], v[2]);
		}
  
                ~Vec3D() {}

		inline Vec3D (T* pp)
		{
			p[0] = pp[0];
			p[1] = pp[1];
			p[2] = pp[2];
		}


		// *********
		// Operators
		// *********


		inline       T& operator[] (int Index)       { return (p[Index]); }
		inline const T& operator[] (int Index) const { return (p[Index]); }

		inline Vec3D& operator= (const Vec3D & P)
		{
			p[0] = P[0];
			p[1] = P[1];
			p[2] = P[2];
			return (*this);
		}

		  inline Vec3D& operator+= (const Vec3D & P)
		  {
			p[0] += P[0];
			p[1] += P[1];
			p[2] += P[2];
			return (*this);
		  }

		inline Vec3D& operator-= (const Vec3D & P)
		{
			p[0] -= P[0];
			p[1] -= P[1];
			p[2] -= P[2];
			return (*this);
		}

		inline Vec3D& operator*= (const Vec3D & P)
		{
			p[0] *= P[0];
			p[1] *= P[1];
			p[2] *= P[2];
			return (*this);
		}

		inline Vec3D& operator*= (T s)
		{
			p[0] *= s;
			p[1] *= s;
			p[2] *= s;
			return (*this);
		}

		inline Vec3D& operator/= (const Vec3D & P)
		{
			p[0] /= P[0];
			p[1] /= P[1];
			p[2] /= P[2];
			return (*this);
		}

		inline Vec3D& operator/= (T s)
		{
			p[0] /= s;
			p[1] /= s;
			p[2] /= s;
			return (*this);
		}

		inline Vec3D operator+ (const Vec3D & P) const
		{
			Vec3D res;
			res[0] = p[0] + P[0];
			res[1] = p[1] + P[1];
			res[2] = p[2] + P[2];
			return (res);
		}

		inline Vec3D operator- (const Vec3D & P) const
		{
			Vec3D res;
			res[0] = p[0] - P[0];
			res[1] = p[1] - P[1];
			res[2] = p[2] - P[2];
			return (res);
		}

		inline Vec3D operator- () const
		{
			Vec3D res;
			res[0] = -p[0];
			res[1] = -p[1];
			res[2] = -p[2];
			return (res);
		}

		inline Vec3D operator* (const Vec3D & P) const
		{
			Vec3D res;
			res[0] = p[0] * P[0];
			res[1] = p[1] * P[1];
			res[2] = p[2] * P[2];
			return (res);
		}

		inline Vec3D operator* (T s) const
		{
			Vec3D res;
			res[0] = p[0] * s;
			res[1] = p[1] * s;
			res[2] = p[2] * s;
			return (res);
		}

		inline Vec3D operator/ (const Vec3D & P) const
		{
			Vec3D res;
			res[0] = p[0] / P[0];
			res[1] = p[1] / P[1];
			res[2] = p[2] / P[2];
			return (res);
		}

		inline Vec3D operator/ (T s) const
		{
			Vec3D res;
			res[0] = p[0] / s;
			res[1] = p[1] / s;
			res[2] = p[2] / s;
			return (res);
		}
	
		inline bool operator == (const Vec3D & a) const
		{
			return(p[0] == a[0] && p[1] == a[1] && p[2] == a[2]);
		}
	
		inline bool operator != (const Vec3D & a) const
		{
			return(p[0] != a[0] || p[1] != a[1] || p[2] != a[2]);
		}
		
		inline bool operator < (const Vec3D & a) const
		{
			return(p[0] < a[0] && p[1] < a[1] && p[2] < a[2]);
		}
		
		inline bool operator >= (const Vec3D &a) const
		{
			return(p[0] >= a[0] && p[1] >= a[1] && p[2] >= a[2]);
		}

  
                /////////////////////////////////////////////////////////////////


		inline Vec3D & init(T x, T y, T z)
		{
			p[0] = x;
			p[1] = y;
			p[2] = z;
			return (*this);
		}

		inline T squared_length() const
		{
			return (dot_product(*this, *this));
		}
	
		inline T length() const
		{
			return (T)sqrt (squared_length());
		}

                /// Return length after normalization
                inline T normalize(void)
                {
                        T length = this->length();
                        if (length == 0.0f) return 0;

                        T rez_length = 1.0f / length;
                        p[0] *= rez_length;
                        p[1] *= rez_length;
                        p[2] *= rez_length;
                        return length;
                }

		inline void from_to(const Vec3D & P1, const Vec3D & P2)
		{
			p[0] = P2[0] - P1[0];
			p[1] = P2[1] - P1[1];
			p[2] = P2[2] - P1[2];
		}

		inline void get_two_orthogonals(Vec3D & u, Vec3D & v) const
		{
			if (fabs(p[0]) < fabs(p[1]))
			{
				if (fabs(p[0]) < fabs(p[2]))
				{
					// p[0] is minimum
					u = Vec3D (0, -p[2], p[1]);
				}
				else
				{
					// p[2] is mimimum
					u = Vec3D (-p[1], p[0], 0);
				}
			}
			else
			{
				if (fabs(p[1]) < fabs(p[2]))
				{
					// p[1] is minimum
					u = Vec3D (p[2], 0, -p[0]);
				}
				else
				{
					// p[2] is mimimum
					u = Vec3D(-p[1], p[0], 0);
				}
			}
			v = cross_product(*this, u);
		}
		
  
                inline Vec3D project_on(const Vec3D & N, const Vec3D & P) const
                {
                        T w = dot_product(((*this) - P), N);
                        return (*this) - (N * w);
                }


		static inline Vec3D segment(const Vec3D & a, const Vec3D & b)
		{

			Vec3D r;
			r[0] = b[0] - a[0];
			r[1] = b[1] - a[1];
			r[2] = b[2] - a[2];
			return r;
		}


		static inline Vec3D cross_product(const Vec3D & a, const Vec3D & b)
		{
			Vec3D result;

			result[0] = a[1] * b[2] - a[2] * b[1];
			result[1] = a[2] * b[0] - a[0] * b[2];
			result[2] = a[0] * b[1] - a[1] * b[0];

			return(result);
		}


		static inline T dot_product(const Vec3D & a, const Vec3D & b)
		{
			return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
		}
		
	
		static inline T squared_distance(const Vec3D &v1, const Vec3D &v2)
		{
			Vec3D tmp = v1 - v2;
			return (tmp.squared_length());
		}
	
	
		static inline T distance(const Vec3D &v1, const Vec3D &v2)
		{
			Vec3D tmp = v1 - v2;
			return (tmp.length());
		}
	
  
                static inline Vec3D interpolate(const Vec3D & u, const Vec3D & v, float alpha = 0.5)
                {
                        return (u * (1.0 - alpha) + v * alpha);
                }
	
		// cartesion to polar coordinates
		// result:
		// [0] = length
		// [1] = angle with z-axis
		// [2] = angle of projection into x,y, plane with x-axis
		//
		static inline Vec3D cartesian_to_polar(const Vec3D &v)
		{
			Vec3D polar;
			polar[0] = v.length();
		
			if (v[2] > 0.0f)
			{
				polar[1] = (T) atan (sqrt (v[0] * v[0] + v[1] * v[1]) / v[2]);
			}
			else
			{
				if (v[2] < 0.0f)
				{
					polar[1] = (T) atan (sqrt (v[0] * v[0] + v[1] * v[1]) / v[2]) + M_PI;
				}
				else
				{
					polar[1] = M_PI * 0.5f;
				}
			}

			if (v[0] > 0.0f)
			{
				polar[2] = (T) atan (v[1] / v[0]);
			}
			else
			{
				if (v[0] < 0.0f)
				{
					polar[2] = (T) atan (v[1] / v[0]) + M_PI;
				}
				else
				{
					if (v[1] > 0)
					{
						polar[2] = M_PI * 0.5f;
					}
					else
					{
						polar[2] = -M_PI * 0.5;
					}
				}
			}

			return polar;
		}
	
		// polar to cartesion coordinates
		// input:
		// [0] = length
		// [1] = angle with z-axis
		// [2] = angle of projection into x,y, plane with x-axis
		//
		static inline Vec3D polar_to_cartesian(const Vec3D & v)
		{
			Vec3D cart;
			cart[0] = v[0] * (T) sin (v[1]) * (T) cos (v[2]);
			cart[1] = v[0] * (T) sin (v[1]) * (T) sin (v[2]);
			cart[2] = v[0] * (T) cos (v[1]);
			return cart;
		}

		static inline Vec3D project_onto_vector(const Vec3D & v1, const Vec3D & v2)
		{
			return v2 * dot_product(v1, v2);
		}


        protected:

                T p[3];
};


////////////////////////////////////////////////////////////////////////////
template <class T> Vec3D<T> operator * (const T &s, const Vec3D<T> &P)
{
   return (P * s);
}

template <class T> std::ostream & operator<< (std::ostream & output, const Vec3D<T> & v)
{
        output << v[0] << " " << v[1] << " " << v[2];
        return output;
}

template <class T> std::istream & operator>> (std::istream & input, Vec3D<T> & v)
{
        input >> v[0] >> v[1] >> v[2];
        return input;
}

/////////////////////////////////////////////////////////////////////////////

typedef Vec3D<float>    Vec3Df;
typedef Vec3D<double>   Vec3Dd;
typedef Vec3D<int>      Vec3Di;
