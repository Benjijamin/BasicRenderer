#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <cmath>
#include "date.h"
#include "vec3.h"
#include "vec2.h"
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

    Tri tris[1] = 
    {
        Tri(Vec3f(-0.01,-0.01,-10), Vec3f(-2,-0.01,-10), Vec3f(-0.2,0.3,-1.5)) 
    };

    //pour chaque triangle
    //projeter point
    for(Tri tri : tris)
    {
        Vec2f bbmin = INFINITY, bbmax = -INFINITY;
        Vec3f vProj[3];

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

        printf("xmin = %i, ymin = %i, xmax = %i, ymax = %i \n",xmin,ymin,xmax,ymax);

        for(int j = ymin; j <= ymax; j++)
        {
            for(int i = xmin; i <= xmax; i++)
            {
                image[i][j] = Vec3f(126,126,126);

                if(pixelInTri(Vec2f(i,j), vProj[0].xy(), vProj[1].xy(), vProj[2].xy()))
                {
                    image[i][j] = Vec3f(255,255,255);
                }
            }
        }

        image[vProj[0].x][vProj[0].y] = Vec3f(255,0,255);
        image[vProj[1].x][vProj[1].y] = Vec3f(255,0,255);
        image[vProj[2].x][vProj[2].y] = Vec3f(255,0,255);
    }

    //print final image
    PpmWriter printer = PpmWriter(imageWidth, imageHeight);
    printer.print(image);

    printf("image printed to output.ppm\n");

    return 0;
}