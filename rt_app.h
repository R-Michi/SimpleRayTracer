/**
* @file     rt_app.h
* @brief    Header file of the ray_tracing test application.
* @author   Michael Reim / Github: R-Michi
* Copyright (c) 2021 by Michael Reim
*
* This code is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/ 

#pragma once

#include "rt/ray_tracing.h"
#include <cmath>

class Material : public rt::PrimitiveAttribute
{
private:
    glm::vec3 _albedo;
    float _roughness;
    float _metallic;
    float _opacity;

public:
    explicit Material(const glm::vec3& albedo = glm::vec3(0.0f), float roughness = 0.0f, float metallic = 0.0f, float opacity = 1.0f)
    { 
        this->_albedo = albedo;
        this->_roughness = roughness;
        this->_metallic = metallic;
        this->_opacity = opacity;
    }

    Material(const Material& mtl)
    {
        *this = mtl;
    }

    Material& operator= (const Material& mtl)
    {
        this->_albedo = mtl._albedo;
        this->_roughness = mtl._roughness;
        this->_metallic = mtl._metallic;
        this->_opacity = mtl._opacity;
        return *this;
    }

    virtual ~Material(void) {}
    virtual rt::PrimitiveAttribute* clone_dynamic(void) const
    { return new Material(this->_albedo, this->_roughness, this->_metallic, this->_opacity); }

    glm::vec3&          albedo(void)    noexcept        { return this->_albedo; }
    const glm::vec3&    albedo(void)    const noexcept  { return this->_albedo; }
    float&              roughness(void) noexcept        { return this->_roughness; }
    float               roughness(void) const noexcept  { return this->_roughness; }
    float&              metallic(void)  noexcept        { return this->_metallic; }
    float               metallic(void)  const noexcept  { return this->_metallic; }
    float&              opacity(void)   noexcept        { return this->_opacity; }
    float               opacity(void)   const noexcept  { return this->_opacity; }

};

struct Light
{
    glm::vec3 direction;
    glm::vec3 intensity;
};

class RT_Application : protected rt::RayTracer
{
private:
    Light light;
    rt::Texture2D<uint8_t, float> tex;
    rt::SphericalMap<float, float> spherical_env;
    rt::Cubemap<uint8_t, float> cubemap;

    /**
     *  Calculates the distance to the closest sphere (primitive / object) from a given point P.
     *  @param prim -> Primitives to be checked for intersection.
     *  @param n_spheres -> The number of the spheres.
     *  @param p -> The point from where the closest distance is calculated.
     *  @param t_max -> The maximum length of the ray.
     *  @return @param hit_prim -> The primitive that the ray intersected with.
     *  @return -> Distance to the closest sphere (primitive / object). 
     */
    float sdf(const glm::vec3& p, float t_max, const rt::Primitive** hit_prim);

    /**
     *  Combutes how much an object is in shadow.
     *  @param prim -> Primitives to be checked for intersection.
     *  @param n_spheres -> The number of the spheres.
     *  @param shadow_ray -> The ray that is traced from a point in the space to  the light source.
     *  @param t_max -> The maximum length of the ray.
     *  @param softness -> Defines how soft is the shadow (size of the penumbra), whereby 0 is infinite soft.
     *  @return -> A value between 0 and 1 that determines how much the point is in shadow.
     */
    float shadow(const rt::ray_t& shadow_ray, float t_max, float softness);

protected:
    glm::vec3 ray_generation_shader(uint32_t x, uint32_t y);
    void closest_hit_shader(const rt::ray_t& ray, int recursion, float t, float t_max, const rt::Primitive* hit, void* ray_payload);
    void miss_shader(const rt::ray_t& ray, int recursuon, float t_max, void* ray_payload);

public:
    // constants
    static constexpr int32_t SCR_WIDTH      = 960 * 4;
    static constexpr int32_t SCR_HEIGHT     = 540 * 4;
    static constexpr size_t PRIM_COUNT      = 3;
    static constexpr size_t RT_RECURSIONS   = 5;

    RT_Application(void);
    virtual ~RT_Application(void);

    void app_run(void);
    const uint8_t* fetch_pixels(void) noexcept;
};