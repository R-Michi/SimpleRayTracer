# description
This library implements a simple ray-tracing pipeline.

#details
Version: 1.1.0
Release date: 2021-08-24

#requierements
1) stb_image is requiered to load textures: https://github.com/nothings/stb
2) glm for vector and matrix mathematics: https://github.com/g-truc/glm

#contact
If you notice any bugs or want to give feedback, write to: 
https://github.com/R-Michi/SimpleRayTracer/issues

#version 1.0.1 | 2021-08-05
- made marked class methods as const
- added noexcept

#version 1.0.2 | 2021-08-05
- primitive attributes are now copied into the primitive class

#version 1.0.3 | 2021-08-06
- fixed bug: unused color channels remained undefined

#version 1.1.0 | 2021-08-24
- added hit information for spheres
- added cull masking for spheres