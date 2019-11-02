#include "light.h"
#include "phong_shader.h"
#include "ray.h"
#include "render_world.h"
#include "object.h"
#include "util.h"

#include <cmath>

vec3 Phong_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
    vec3 color = color_ambient * world.ambient_color * world.ambient_intensity;

    vec3 diffuse_( 0,0,0);
    vec3 specular_(0,0,0);

    if (world.enable_shadows) { // Takes into account shadows

      vec3 shadow;

      for (unsigned int i = 0; i < world.lights.size(); i++) {


        const vec3 light_to_object = world.lights.at(i) -> position - intersection_point; // Calculate the light direction

        Ray shadow_ray (intersection_point, light_to_object); // A ray that traverses the line between the point on the object and the light source we are iterating past

        const Hit light_obstruction = world.Closest_Intersection(shadow_ray); // Find the closest obstruction between the light and the point (if any);

        if (light_obstruction.dist > 0) { // If the obstruction exists
          shadow = shadow_ray.Point(light_obstruction.dist) - intersection_point;      // Calculate the shadow
        }

        if (shadow.magnitude() > light_to_object.magnitude()) {// If the shadow doesn't actually exists, shade the point. If the shadow doesn't exist, then we don't shade the point to simulate the shadow.
          vec3 df  = diffuse (ray, intersection_point, normal, world.lights.at(i), light_to_object);
          vec3 sp  = specular(ray, intersection_point, normal, world.lights.at(i), light_to_object);

          diffuse_  += df;
          specular_ += sp;

          if (debug_pixel) { // Debug statement
              std::cout << "Shading for light " << i << ": "
              << "diffuse: "   << df
              << " specular: " << color_specular * sp
              << "\n";
          }
        }
      }

    } else { // Ignores shadows

      for (unsigned int i = 0; i < world.lights.size(); i++) {

        const vec3 light_to_object = world.lights.at(i) -> position - intersection_point; // Calculate the light direction

        vec3 df  = diffuse (ray, intersection_point, normal, world.lights.at(i), light_to_object);
        vec3 sp  = specular(ray, intersection_point, normal, world.lights.at(i), light_to_object);

        diffuse_  += df;
        specular_ += sp;

        if (debug_pixel) { // Debug statement
            std::cout << "Shading for light " << i << ": "
            << "diffuse = "   << df
            << " specular = " << color_specular * sp
            << "\n";
        }
      } 
    }

    

    // Sum the colors and intensities
    color += diffuse_ + specular_;

    if (debug_pixel) {
        std::cout << "call Shade_Surface with: location = ";
        std::cout << intersection_point;
        std::cout << "; normal = " << normal << "\n";
        std::cout << "Ambient = " << color_ambient << "\n";
        std::cout << "Final color = " << color << "\n";
    }

    return color;
}

vec3 Phong_Shader::specular(const Ray& ray,
                            const vec3& intersection_point,
                            const vec3& normal, 
                            const Light* light, 
                            const vec3& light_to_object_) const {
    
    const vec3 light_to_object = light_to_object_; // Calculate the light direction
    
    const vec3 reflection = (-1.0 * light_to_object) + dot(light_to_object, normal) * 2 * normal; // Reflect the light_to_object vector about the normal vector
                                                                                                  // Calculates the intensity of the specular lighting
    const double intensity = std::pow(
                             std::max(
                             dot(-1.0 * ray.direction, reflection.normalized()), 
                             0.0), 
                             specular_power); 

    return light->Emitted_Light(light_to_object) * color_specular * intensity; // Return the intensity of the specular light multiplied by the specular color and light color
}

vec3 Phong_Shader::diffuse(const Ray& ray,
                           const vec3& intersection_point,
                           const vec3& normal,
                           const Light* light,
                           const vec3& light_to_object_) const {

    vec3 light_direction = light_to_object_;                      // Calculate the direction

    double intensity = std::max(dot(normal, light_direction.normalized()), 0.0);      // Add the intensity of the diffuse effect to the total

    return color_diffuse * light->Emitted_Light(light_direction) * intensity;
}
