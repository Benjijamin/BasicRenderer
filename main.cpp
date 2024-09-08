#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <cmath>
#include <date.h>
#include <ppm.h>
#include <vec2.h>
#include <vec3.h>
#include <matrix.h>
#include <tri.h>
#include <camera.h>
#include <glfw3.h>

int main()
{
    //Setup logging
    freopen( "log.txt" , "a", stdout);

    auto now = std::chrono::system_clock::now();
    std::string formatTime = date::format("%T --- %F", now);
    std::cout << "\nLOG --- " << formatTime << '\n';

    printf("\n");
    int imageWidth = 512, imageHeight = 512;

    Camera cam(imageWidth, imageHeight);

    Vec3i *imageBuffer = new Vec3i[imageWidth * imageHeight];
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
        Tri(Vec3f(0, 0, -0.33), Vec3f(1, 0, -3.3), Vec3f(1, 1, -5)),
        Tri(Vec3f(0.8, 1.2, -7), Vec3f(0.01, 0, -0.3), Vec3f(1, 0.1, -3))
    };


    for(Tri tri : tris)
    {
        Vec2f bbmin = INFINITY, bbmax = -INFINITY;
        Vec3f vProj[3];
        int oobV = 0;
        Vec3f vAttr[3] = 
        {
            {1,0,0},
            {0,1,0},
            {0,0,1}
        };
        Vec2f texCoord[3] =
        {
            {0,0},
            {0,1},
            {1,0}
        };

        //Tri bounding box
        for(int i = 0; i < 3; i++)
        {
            vProj[i] = cam.worldToScreen(tri[i]);

            if(vProj[i].x < bbmin.x) { bbmin.x = vProj[i].x; }
            if(vProj[i].y < bbmin.y) { bbmin.y = vProj[i].y; }
            if(vProj[i].x > bbmax.x) { bbmax.x = vProj[i].x; }
            if(vProj[i].y > bbmax.y) { bbmax.y = vProj[i].y; }

            vProj[i].z = 1 / vProj[i].z;
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
                    Vec3f w = barycentricPoint(Vec2f(i,j), vProj[0].xy(), vProj[1].xy(), vProj[2].xy());

                    float z = 1 / (w.x * vProj[0].z + w.y * vProj[1].z + w.z * vProj[2].z);

                    if(z < depthBuffer[j * imageWidth + i])
                    {
                        depthBuffer[j * imageWidth + i] = z;
                        imageBuffer[j * imageWidth + i] = Vec3i(w.x * 255, w.y * 255, w.z * 255);
                    }
                }
            }
        }
    }

    //print final image
    PpmWriter printer = PpmWriter(imageWidth, imageHeight);
    printer.print(imageBuffer);
    printf("image printed to output.ppm\n");

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Test window", nullptr, nullptr);

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}