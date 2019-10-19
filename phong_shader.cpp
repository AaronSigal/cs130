#include "light.h"
#include "phong_shader.h"
#include "ray.h"
#include "render_world.h"
#include "object.h"

#include <cmath>

vec3 Phong_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
    vec3 color = color_ambient;

    // Sum the colors and intensities
    color += color_diffuse * diffuse(ray, intersection_point, normal) + color_specular * specular(ray, intersection_point, normal);

    if (debug_pixel) {
        std::cout << "Shade normal:\t" << normal << "\n";
        std::cout << "Shading at:\t" << intersection_point << "\n";
        std::cout << "Diffuse:\t" << color_diffuse * diffuse(ray, intersection_point, normal) << "\n";
        std::cout << "Specular:\t" << color_specular * specular(ray, intersection_point, normal) << "\n";
    }

    return color;
}

double Phong_Shader::specular(const Ray& ray,const vec3& intersection_point,
                              const vec3& normal) const {
    double intensity = 0;

    // Calculate the view direction
    vec3 view_direction = (world.camera.film_position - intersection_point).normalized();

    // For each light
    for (const auto& light : world.lights) {
      vec3 light_direction = (light->position - intersection_point).normalized(); // Get its direction

      vec3 reflected_direction = 2 * (dot(normal.normalized(), light_direction)) * (normal.normalized() - light_direction); // Reflect the direction over the normal

      intensity += std::pow(dot(view_direction, reflected_direction), specular_power); // Add the contribution of this light to the overall intensity

    }

    return intensity;
}

double Phong_Shader::diffuse(const Ray& ray,
                             const vec3& intersection_point,
                             const vec3& normal) const {
    double intensity = 0;

    for (const auto &light : world.lights) { // For each light
        vec3 light_direction = light->position - intersection_point; // Calculate the direction
        intensity += dot(normal.normalized(), light_direction.normalized());      // Add the intensity of the diffuse effect to the total
    }



    return intensity;
}
