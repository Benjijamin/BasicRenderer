#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <cmath>
#include "date.h"
#include "vec2.h"
#include "vec3.h"
#include "matrix.h"
#include "tri.h"
#include "ppm.h"
#include "camera.h"

int main()
{   
    //Setup logging
    freopen( "log.txt" , "a", stdout);

    auto now = std::chrono::system_clock::now();
    std::string formatTime = date::format("%T --- %F", now);
    std::cout << "\nLOG --- " << formatTime << '\n';

    printf("\n");
    int imageWidth = 512, imageHeight = 512;

    //create camera
    Camera cam;
    
    std::vector<std::vector<Vec3f>> image(imageWidth, std::vector<Vec3f>(imageHeight));

    Vec3f *imageBuffer = new Vec3f[imageWidth * imageHeight];
    float *depthBuffer = new float[imageWidth * imageHeight];

    for(int j = 0; j < imageHeight; j++)
    {
        for(int i = 0; i < imageWidth; i++)
        {
            depthBuffer[j * imageWidth + i] = INFINITY;
        }
    }

    Tri tris[2] = 
    {
        Tri(Vec3f(-3,-0.5,10), Vec3f(-1,-1,10), Vec3f(-1.5,-2,10)),
        Tri(Vec3f(-1.5,-2,11), Vec3f(-3,-0.5,8), Vec3f(-1,-1,8))
    };


    for(Tri tri : tris)
    {
        Vec2f bbmin = INFINITY, bbmax = -INFINITY;
        Vec3f vProj[3];

        //Tri bounding box
        for(int i = 0; i < 3; i++)
        {
            vProj[i] = cam.worldToScreen(tri[i]);

            if(vProj[i].x < bbmin.x) { bbmin.x = vProj[i].x; }
            if(vProj[i].y < bbmin.y) { bbmin.y = vProj[i].y; }
            if(vProj[i].x > bbmax.x) { bbmax.x = vProj[i].x; }
            if(vProj[i].y > bbmax.y) { bbmax.y = vProj[i].y; }
        }

        int xmin = std::max(0, std::min(imageWidth - 1, (int)bbmin.x));
        int ymin = std::max(0, std::min(imageHeight - 1, (int)bbmin.y));
        int xmax = std::max(0, std::min(imageWidth - 1, (int)bbmax.x));
        int ymax = std::max(0, std::min(imageHeight - 1, (int)bbmax.y));

        for(int j = ymin; j <= ymax; j++)
        {
            for(int i = xmin; i <= xmax; i++)
            {
                if(isPointInTri(Vec2f(i,j), vProj[0].xy(), vProj[1].xy(), vProj[2].xy()))
                {
                    Vec3f rgb = barycentricPoint(Vec2f(i,j), vProj[0].xy(), vProj[1].xy(), vProj[2].xy());
                    rgb *= 255;

                    imageBuffer[j * imageWidth + i] = Vec3f(rgb.x, rgb.y, rgb.z);
                }
            }
        }
    }

    //print final image
    PpmWriter printer = PpmWriter(imageWidth, imageHeight);
    printer.print(imageBuffer);
    printf("image printed to output.ppm\n");

    return 0;
}