#ifndef PPM_H
#define PPM_H

#include <iostream>
#include <fstream>
#include <vector>
#include <vec3.h>

class PpmWriter
{
    public:
        PpmWriter(int width, int height) : w(width), h(height){}
        int w,h;

        void print(const Vec3i *imageBuffer) const;
};

void readImage(const char *fileName, Vec3i *imageBuffer);

#endif