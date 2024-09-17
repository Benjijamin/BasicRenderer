#include <iostream>
#include <chrono>
#include <ctime>
#include <cmath>
#include <date.h>
#include <vulkanapp.cpp>
#include <rendering.cpp>

int main()
{
    //Setup logging
    freopen( "log.txt" , "a", stdout);

    auto now = std::chrono::system_clock::now();
    std::string formatTime = date::format("%T --- %F", now);
    std::cout << "\nLOG --- " << formatTime << '\n';

    printf("\n");

    const int WIDTH = 512, HEIGHT = 512;
    //renders scene in a ppm file
    renderScene(WIDTH, HEIGHT);

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