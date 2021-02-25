mkdir obj
g++ -O3 -Wall -std=c++17 -I./lib/glm -I./lib/stb_master -I./lib/glc-1.0.0 -c main.cpp -o obj/main.o -fopenmp
g++ -L./lib/glm/lib -o ray_tracing.exe obj/main.o -lglm_static -fopenmp