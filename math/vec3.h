#ifndef VEC3_H
#define VEC3_H

#include <vec2.h>

template<typename T>
class Vec3
{
    public:
        T x,y,z;
        Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
        Vec3(const T &val) : x(val), y(val), z(val){}
        Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz){}

        Vec2<T> xy() const
        {
            return Vec2<T>(x, y);
        }

        T length() const
        {
            return sqrt(x*x + y*y + z*z);
        }

        Vec3<T>& normalize()
        {
            T l = length();
            if(l > 0)
            {
                T fl = 1 / l;
                x *= fl, y *= fl, z *= fl;
            }

            return *this;
        }

        T dot(const Vec3<T> &vec) const
        {
            return x*vec.x + y*vec.y + z*vec.z;
        }

        Vec3<T> cross(const Vec3<T> &vec) const
        {
            return Vec3<T>
            (
                y * vec.z - vec.y * z,
                z * vec.x - vec.z * x,
                x * vec.y - vec.x * y
            );
        }

        Vec3<T> operator + (const Vec3<T> &vec) const
        { return Vec3<T>(x+vec.x, y+vec.y, z+vec.z); }
        Vec3<T> operator += (const Vec3<T> &vec)
        { x += vec.x, y += vec.y, z += vec.z; return *this; }
        Vec3<T> operator - (const Vec3<T> &vec) const
        { return Vec3<T>(x-vec.x, y-vec.y, z-vec.z); }
        Vec3<T> operator -= (const Vec3<T> &vec)
        { x -= vec.x, y -= vec.y, z -= vec.z; return *this; }
        Vec3<T> operator * (const T &mul) const
        { return Vec3<T>(x*mul, y*mul, z*mul); }
        Vec3<T> operator *= (const T &mul)
        { x *= mul, y *= mul, z *= mul; return *this; }
};

template<typename T>
T length(const Vec3<T> &vec)
{
    return sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
}

template<typename T>
T dot(const Vec3<T> &a, const Vec3<T> &b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

template<typename T>
Vec3<T> cross(const Vec3<T> &a, const Vec3<T> &b)
{
    return Vec3<T>
    (
        a.y * b.z - b.y * a.z,
        a.z * b.x - b.z * a.x,
        a.x * b.y - b.x * a.y
    );
}

typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;

#endif