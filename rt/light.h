#ifndef __ray_tracer_light__
#define __ray_tracer_light__

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

#include "rt_types.h"

namespace rt
{
    namespace __rt_internal
    {
        /**
         *  @LearnOpenGL: 
         *  Normal distribution function: approximates the amount the surface's microfacets are aligned to the halfway vector, 
         *  influenced by the roughness of the surface. 
         *  This is the primary function that approximates the roughness of a surface.
         * 
         *  @param N -> The surface's normal vector.
         *  @param H -> The halfway vector between the view and the normal vector.
         *  @param r -> A scalar for the surface's roughness ranging fom 0.0 to 1.0.
         */
        float distribution_GGX(const glm::vec3& N, const glm::vec3& H, float r) noexcept;

        /**
         *  @LearnOpenGL:
         *  Geometry function: describes the self-shadowing property of the microfacets. 
         *  When a surface is relatively rough, the surface's microfacets can overshadow 
         *  other microfacets reducing the light the surface reflects.
         * 
         *  @param NdotV -> Dot product between the normal and the view vector.
         *  @param r -> A scalar for the surface's roughness ranging fom 0.0 to 1.0.
         */
        float geometry_schlick_GGX(float NdotV, float r) noexcept;

        /**
         *  @param N -> The surface's normal vector.
         *  @param V -> View vector from the camera to the current point on the goemetry.
         *  @param L -> The light direction.
         *  @param r -> A scalar for the surface's roughness ranging fom 0.0 to 1.0.
         */
        float geometry_smith(const glm::vec3& N, const glm::vec3& V, const glm::vec3& L, float r) noexcept;

        /**
         *  @LearnOpenGL
         *  Fresnel equation: The Fresnel equation describes the ratio of surface reflection at different surface angles.
         * 
         *  @param H -> The halfway vector between the view and the normal vector.
         *  @param V -> View vector from the camera to the current point on the goemetry.
         *  @param F0 -> Represents the base reflectivity of the surface.
         */
        glm::vec3 frensel_schlick(const glm::vec3& H, const glm::vec3& V, const glm::vec3& F0) noexcept;
    }

    /**
     *  Calculates the light intensity of the surface.
     *  @param L -> Light source.
     *  @param M -> The surface's material parameter.
     *  @param V -> View vector from the camera to the current point on the goemetry.
     *  @param N -> The surface's normal vector.
     *  @return -> Light intensity of the surface.
     */
    glm::vec3 light(const Light& L, const Material& M, const glm::vec3& V, const glm::vec3 N) noexcept;
};
#endif // __ray_tracer_light__