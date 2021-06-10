// implementation defines
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

// include std libraries
#include <chrono>   // for time measurement
#include <cstdio>   // for printf
#include <cinttypes>

// incldue stbmaster
#include <stb/stb_image_write.h>

// include ray tracing
#include "rt_app.h"

int main()
{
    using namespace std::chrono;

    RT_Application app;

    time_point<high_resolution_clock> t0_render = high_resolution_clock::now();     // time before rendering
    app.app_run();
    time_point<high_resolution_clock> t1_render = high_resolution_clock::now();     // time after rendering
    int64_t t_render = duration_cast<milliseconds>(t1_render - t0_render).count();  // get time duration of image rendering operation
    printf("Rendering time: %" PRId64 "ms\n", t_render);

    if(app.fetch_pixels() != nullptr)
    {
        time_point<high_resolution_clock> t0_out = high_resolution_clock::now();    // time before writing
        stbi_write_png("rt_output.png", RT_Application::SCR_WIDTH, RT_Application::SCR_HEIGHT, 3, app.fetch_pixels(), RT_Application::SCR_WIDTH * sizeof(rt::Color3ui8));   // print image
        time_point<high_resolution_clock> t1_out = high_resolution_clock::now();    // time after writing
        int64_t t_out = duration_cast<milliseconds>(t1_out - t0_out).count();       // get time duration of image writing operation
        printf("Writing time: %" PRId64 "\n", t_out);
    }
    
    return (int)t_render;
}