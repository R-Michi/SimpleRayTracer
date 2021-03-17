// include std libraries
#include <chrono>   // for time measurement
#include <cstdio>   // for printf
#include <cinttypes>

// incldue stbmaster
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

// include ray tracing
#include "rt_app.h"

int main()
{
    RT_Application app;

    std::chrono::time_point<std::chrono::system_clock> t0_render = std::chrono::high_resolution_clock::now();   // time before rendering
    app.app_run();
    std::chrono::time_point<std::chrono::system_clock> t1_render = std::chrono::high_resolution_clock::now();   // time after rendering
    int64_t t_render = std::chrono::duration_cast<std::chrono::milliseconds>(t1_render - t0_render).count();    // get time duration of image rendering operation
    printf("Rendering time: %" PRId64 "ms\n", t_render);

    if(app.fetch_pixels() != nullptr)
    {
         std::chrono::time_point<std::chrono::system_clock> t0_out = std::chrono::high_resolution_clock::now();                      // time before writing
        stbi_write_png("rt_output.png", RT_Application::SCR_WIDTH, RT_Application::SCR_HEIGHT, 3, app.fetch_pixels(), RT_Application::SCR_WIDTH * sizeof(rt::Color3ui8));   // print image
        std::chrono::time_point<std::chrono::system_clock> t1_out = std::chrono::high_resolution_clock::now();                      // time after writing
        int64_t t_out = std::chrono::duration_cast<std::chrono::milliseconds>(t1_out - t0_out).count();                             // get time duration of image writing operation
        printf("Writing time: %" PRId64 "\n", t_out);
    }
    
    return 0;
}