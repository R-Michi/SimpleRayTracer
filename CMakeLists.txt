# requiered CMAKE version to build the project
cmake_minimum_required (VERSION 3.8)

# current project
project ("SimpleRayTracer")

# set comiler flags
# -fopenmp does not work with msvc compiler
if(NOT CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif()

# add external include directories
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/glm") 
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/stb_master")

# compile and link the internal ray tracing source files into a seperate library
# can also be used as an external library
add_library(ray_tracing_static STATIC 
			"rt/primitive/primitive.cpp"
			"rt/primitive/sphere.cpp"
			"rt/primitive/distancesphere.cpp"
			"rt/primitive/infplane.cpp"
			
			"rt/image/cubemap.cpp"
			"rt/image/texture.cpp"

			"rt/misc/buffer.cpp"
			"rt/misc/app.cpp")

# compile and link final executable
add_executable(ray_tracer 
			   "${CMAKE_CURRENT_SOURCE_DIR}/rt_app.cpp"
			   "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp" "rt/image/texture.cpp")

# link external libraries to the executable
target_link_libraries(ray_tracer
					  "-fopenmp"
					  "${CMAKE_CURRENT_SOURCE_DIR}/lib/glm/glm/lib/glm_static.lib"
					  "ray_tracing_static")

# additional work
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
