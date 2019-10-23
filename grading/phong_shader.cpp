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
    double specular_ = 0;

    for (unsigned int i = 0; i < world.lights.size(); i++) {

       vec3  df = diffuse(ray, intersection_point, normal, world.lights.at(i));
       double sp = specular(ray, intersection_point, normal, world.lights.at(i));

        diffuse_ += df;
        specular_ += sp;

        if (debug_pixel) { // Debug statement
            std::cout << "Shading for light " << i << ": "
            << "diffuse: "   << df
            << " specular: " << color_specular * sp
            << "\n";
        }
    }

    // Sum the colors and intensities
    color += diffuse_ + (color_specular * specular_);

    if (debug_pixel) {
        std::cout << "Calling Shade_Surface with: ";
        std::cout << "Location: " << intersection_point;
        std::cout << " normal: " << normal << "\n";
        std::cout << "Ambient: " << color_ambient << "\n";
        std::cout << "Final color:\t" << color << "\n";
    }

    return color;
}

double Phong_Shader::specular(const Ray& ray,const vec3& intersection_point,
                              const vec3& normal, const Light* light) const {
    double intensity = 0;

    // Calculate the view direction
    vec3 view_direction = (world.camera.film_position - intersection_point).normalized();

    vec3 light_direction = (light->position - intersection_point).normalized(); // Get its direction

    vec3 reflected_direction = 2 * (dot(normal.normalized(), light_direction)) * (normal.normalized() - light_direction); // Reflect the direction over the normal

    intensity += std::pow(dot(view_direction, reflected_direction), specular_power); // Add the contribution of this light to the overall intensity

    return intensity;
}

vec3 Phong_Shader::diffuse(const Ray& ray,
                             const vec3& intersection_point,
                             const vec3& normal, const Light* light) const {

    vec3 light_direction = light->position - intersection_point;                      // Calculate the direction
    double intensity = std::max(dot(normal, light_direction.normalized()), 0.0);      // Add the intensity of the diffuse effect to the total

    return color_diffuse * light->Emitted_Light(light_direction) * intensity;;
}
