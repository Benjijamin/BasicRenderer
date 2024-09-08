#ifndef VEC2_H
#define VEC2_H

template<typename T>
class Vec2
{
    public:
        T x,y;
        Vec2() : x(T(0)), y(T(0)){}
        Vec2(const T &val) : x(val), y(val){}
        Vec2(T xx, T yy) : x(xx), y(yy){}

        T length() const
        {
            return sqrt(x*x + y*y);
        }

        Vec2<T>& normalize()
        {
            T l = length();
            if(l > 0)
            {
                T fl = 1 / l;
                x *= fl, y *= fl;
            }

            return *this;
        }

        T dot(const Vec2<T> &vec) const
        {
            return x*vec.x + y*vec.y;
        }

        Vec2<T> operator + (const Vec2<T> &vec) const
        { return Vec2<T>(x+vec.x, y+vec.y); }
        Vec2<T> operator += (const Vec2<T> &vec)
        { x += vec.x, y += vec.y; return *this; }
        Vec2<T> operator - (const Vec2<T> &vec) const
        { return Vec2<T>(x-vec.x, y-vec.y); }
        Vec2<T> operator -= (const Vec2<T> &vec)
        { x -= vec.x, y -= vec.y; return *this; }
        Vec2<T> operator * (const T &mul) const
        { return Vec2<T>(x*mul, y*mul); }
        Vec2<T> operator *= (const T &mul)
        { x *= mul, y *= mul; return *this; }
};

template<typename T>
T length(const Vec2<T> &vec)
{
    return sqrt(vec.x*vec.x + vec.y*vec.y);
}

template<typename T>
T dot(const Vec2<T> &a, const Vec2<T> &b)
{
    return a.x*b.x + a.y*b.y;
}

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;

#endif