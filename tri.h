class Tri
{
    public:
        Tri() {}
        Tri(Vec3f p1, Vec3f p2, Vec3f p3)
        {
            tri[0] = p1;
            tri[1] = p2;
            tri[2] = p3;
        }

        Vec3f tri[3] = {Vec3f(),Vec3f(),Vec3f()};

        const Vec3f& operator [] (int i) const { return tri[i]; }
        Vec3f operator [] (int i) { return tri[i]; } 
};

float edge(const Vec2f p1, const Vec2f p2, const Vec2f p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool pixelInTri(const Vec2f p, const Vec2f v1, const Vec2f v2, const Vec2f v3)
{
    float e1 = edge(p, v1, v2);
    float e2 = edge(p, v2, v3);
    float e3 = edge(p, v3, v1);

    bool neg = (e1 < 0) || (e2 < 0) || (e3 < 0);
    bool pos = (e1 > 0) || (e2 > 0) || (e3 > 0);

    return !( neg && pos );
}