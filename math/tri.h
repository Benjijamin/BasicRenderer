#ifndef TRI_H
#define TRI_H

#include <vec2.h>
#include <vec3.h>

class Tri
{
    public:
        Tri();
        Tri(Vec3f p1, Vec3f p2, Vec3f p3);

        bool isPointInTri(const Vec2f &p) const;
        
        const Vec3f& operator [] (int i) const;
        Vec3f operator [] (int i);

    private:
        Vec3f tri[3] = {Vec3f(),Vec3f(),Vec3f()};
};

float edge(const Vec2f &p1, const Vec2f &p2, const Vec2f &p3);
bool isPointInTri(const Vec2f &p, const Vec2f &v1, const Vec2f &v2, const Vec2f &v3);
Vec3f barycentricPoint(const Vec2f &p, const Vec2f &v1, const Vec2f &v2, const Vec2f &v3);

#endif