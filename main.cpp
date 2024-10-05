#include <iostream>
#include <chrono>
#include <ctime>
#include <cmath>
#include <date.h>
#include <vulkanapp.cpp>

int main()
{
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