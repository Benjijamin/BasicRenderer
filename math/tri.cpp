#include <tri.h>

Tri::Tri() {}
Tri::Tri(Vec3f p1, Vec3f p2, Vec3f p3)
{
    tri[0] = p1;
    tri[1] = p2;
    tri[2] = p3;
}

const Vec3f& Tri::operator [] (int i) const { return tri[i]; }
Vec3f Tri::operator [] (int i) { return tri[i]; } 

bool Tri::isPointInTri(const Vec2f &p) const
{
    float e1 = edge(p, tri[1].xy(), tri[2].xy());
    float e2 = edge(p, tri[2].xy(), tri[0].xy());
    float e3 = edge(p, tri[0].xy(), tri[1].xy());

    bool neg = (e1 < 0) || (e2 < 0) || (e3 < 0);
    bool pos = (e1 > 0) || (e2 > 0) || (e3 > 0);

    return !( neg && pos );
}

float edge(const Vec2f &p1, const Vec2f &p2, const Vec2f &p3)
{
    return (p3.x - p1.x) * (p2.y - p1.y) - (p3.y - p1.y) * (p2.x - p1.x);
}

bool isPointInTri(const Vec2f &p, const Vec2f &v1, const Vec2f &v2, const Vec2f &v3)
{
    float e1 = edge(p, v2, v3);
    float e2 = edge(p, v3, v1);
    float e3 = edge(p, v1, v2);

    bool neg = (e1 < 0) || (e2 < 0) || (e3 < 0);
    bool pos = (e1 > 0) || (e2 > 0) || (e3 > 0);

    return !( neg && pos );
}

Vec3f barycentricPoint(const Vec2f &p, const Vec2f &v1, const Vec2f &v2, const Vec2f &v3)
{
    float e1 = edge(p, v2, v3);
    float e2 = edge(p, v3, v1);
    float e3 = edge(p, v1, v2);

    float area = edge(v1,v2,v3);
    float w1 = e1 / area;
    float w2 = e2 / area;
    float w3 = e3 / area;

    return Vec3f(w1,w2,w3);
}