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
    vec3 color = color_ambient * world.ambient_color * world.ambient_intensity;

    vec3 diffuse_( 0,0,0);
    vec3 specular_(0,0,0);

    for (unsigned int i = 0; i < world.lights.size(); i++) {

       vec3 df  = diffuse (ray, intersection_point, normal, world.lights.at(i));
       vec3 sp  = specular(ray, intersection_point, normal, world.lights.at(i));

        diffuse_  += df;
        specular_ += sp;

        if (debug_pixel) { // Debug statement
            std::cout << "Shading for light " << i << ": "
            << "diffuse: "   << df
            << " specular: " << color_specular * sp
            << "\n";
        }
    }

    // Sum the colors and intensities
    color += diffuse_ + specular_;

    if (debug_pixel) {
        std::cout << "Calling Shade_Surface with: ";
        std::cout << "Location: " << intersection_point;
        std::cout << " normal: " << normal << "\n";
        std::cout << "Ambient: " << color_ambient << "\n";
        std::cout << "Final color:\t" << color << "\n";
    }

    return color;
}

vec3 Phong_Shader::specular(const Ray& ray,const vec3& intersection_point,
                              const vec3& normal, const Light* light) const {
    
    const vec3 light_to_object = light->position - intersection_point; // Calculate the light direction
    const vec3 reflection = (-1.0 * light_to_object) + dot(light_to_object, normal) * 2 * normal; // Reflect the light_to_object vector about the normal vector
    const double intensity = std::pow(std::max(dot(-1.0 * ray.direction, reflection.normalized()), 0.0), specular_power);



    return light->Emitted_Light(light_to_object) * color_specular * intensity;
}

vec3 Phong_Shader::diffuse(const Ray& ray,
                             const vec3& intersection_point,
                             const vec3& normal, const Light* light) const {

    vec3 light_direction = light->position - intersection_point;                      // Calculate the direction
    double intensity = std::max(dot(normal, light_direction.normalized()), 0.0);      // Add the intensity of the diffuse effect to the total

    return color_diffuse * light->Emitted_Light(light_direction) * intensity;;
}
