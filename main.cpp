#include <iostream>
#include <chrono>
#include <ctime>
#include <cmath>
#include <date.h>
#include <glfw3.h>
#include <vulkanapp.cpp>
#include <rendering.cpp>

void errorLog(int error, const char* description)
{
    printf("Error: %s\n", description);
}

void GLFWTest()
{
    const int WIDTH = 512, HEIGHT = 512;
    //renders scene in a ppm file
    renderScene(WIDTH, HEIGHT);

    glfwInit();
    glfwSetErrorCallback(errorLog);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Test window", nullptr, nullptr);

    glfwMakeContextCurrent(window);

    /*
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0,0,width,height);
    */

    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    Vec3i *imageBuffer = new Vec3i[WIDTH * HEIGHT];
    const char* fileName = "output.ppm";
    readImage(fileName, imageBuffer);

    glfwDestroyWindow(window);

    glfwTerminate();
}

int main()
{
    //Setup logging
    freopen( "log.txt" , "a", stdout);

    auto now = std::chrono::system_clock::now();
    std::string formatTime = date::format("%T --- %F", now);
    std::cout << "\nLOG --- " << formatTime << '\n';

    printf("\n");

    GLFWTest();

    VulkanApp app;

    try
    {
        app.run();
    }
    catch(const std::exception& e)
    {
        printf(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}