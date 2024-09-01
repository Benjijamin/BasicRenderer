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
    cam.rescaleAspectRatio();
    
    std::vector<std::vector<Vec3f>> image(imageWidth, std::vector<Vec3f>(imageHeight));

    Vec3i *imageBuffer = new Vec3i[imageWidth * imageHeight];
    float *depthBuffer = new float[imageWidth * imageHeight];

    for(int j = 0; j < imageHeight; j++)
    {
        for(int i = 0; i < imageWidth; i++)
        {
            depthBuffer[j * imageWidth + i] = INFINITY;
        }
    }

    Tri tris[1] = 
    {
        //Tri(Vec3f(-3,-0.5,10), Vec3f(-1,-1,10), Vec3f(-1.5,-2,10)),
        Tri(Vec3f(0, 0, -5), Vec3f(1, 0, -5), Vec3f(.5, 1, -10))
    };


    for(Tri tri : tris)
    {
        Vec2f bbmin = INFINITY, bbmax = -INFINITY;
        Vec3f vProj[3];
        Vec3f vAttr[3] = 
        {
            {1,0,0},
            {0,1,0},
            {0,0,1}
        };

        //Tri bounding box
        for(int i = 0; i < 3; i++)
        {
            vProj[i] = cam.worldToScreen(tri[i]);

            if(vProj[i].x < bbmin.x) { bbmin.x = vProj[i].x; }
            if(vProj[i].y < bbmin.y) { bbmin.y = vProj[i].y; }
            if(vProj[i].x > bbmax.x) { bbmax.x = vProj[i].x; }
            if(vProj[i].y > bbmax.y) { bbmax.y = vProj[i].y; }

            // Dark Ritual B
            vAttr[i].x = vAttr[i].x / vProj[i].z;
            vAttr[i].y = vAttr[i].y / vProj[i].z;
            vAttr[i].z = vAttr[i].z / vProj[i].z;

            //Instant
            vProj[i].z = 1 / vProj[i].z;
        }

        int xmin = std::max(0, std::min(imageWidth - 1, (int)bbmin.x));
        printf("xmin: %i, bbmin.x: %f \n",xmin,bbmin.x);
        int ymin = std::max(0, std::min(imageHeight - 1, (int)bbmin.y));
        printf("ymin: %i, bbmin.y: %f \n",ymin,bbmin.y);
        int xmax = std::max(0, std::min(imageWidth - 1, (int)bbmax.x));
        printf("xmax: %i, bbmax.x: %f \n",xmax,bbmax.x);
        int ymax = std::max(0, std::min(imageHeight - 1, (int)bbmax.y));
        printf("ymax: %i, bbmax.y: %f \n",ymax,bbmax.y);

        for(int j = ymin; j <= ymax; j++)
        {
            for(int i = xmin; i <= xmax; i++)
            {
                imageBuffer[j * imageWidth + i] = Vec3i(122, 122, 0);

                if(isPointInTri(Vec2f(i,j), vProj[0].xy(), vProj[1].xy(), vProj[2].xy()))
                {
                    Vec3f w = barycentricPoint(Vec2f(i,j), vProj[0].xy(), vProj[1].xy(), vProj[2].xy());

                    //Add BBB to your mana pool
                    float z = 1 / (w.x * vProj[0].z + w.y * vProj[1].z + w.z * vProj[2].z);

                    if(z < depthBuffer[j * imageWidth + i])
                    {
                        depthBuffer[j * imageWidth + i] = z;
                    }

                    float r = w.x * vAttr[0].x + w.y * vAttr[1].x + w.z * vAttr[2].x;
                    float g = w.x * vAttr[0].y + w.y * vAttr[1].y + w.z * vAttr[2].y;
                    float b = w.x * vAttr[0].z + w.y * vAttr[1].z + w.z * vAttr[2].z; 

                    r*=z,g*=z,b*=z;

                    imageBuffer[j * imageWidth + i] = Vec3i(r * 255, g * 255, b * 255);
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