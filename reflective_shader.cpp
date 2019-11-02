#include "light.h"
#include "reflective_shader.h"
#include "ray.h"
#include "render_world.h"

vec3 Reflective_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
	
	vec3 color(0,0,0); // Initialize to black
    // TODO: determine the color

    vec3 reflection = (ray.direction + dot(ray.direction, normal) * 2 * normal * -1.0 ).normalized(); // Reflect the light_to_object vector about the normal vector
 
 	Ray reflected_ray(intersection_point, reflection);

    vec3 reflected_color = world.Cast_Ray(reflected_ray, ++recursion_depth);
    
    color = shader->Shade_Surface(ray, intersection_point, normal, recursion_depth);
    
    color = (reflectivity * reflected_color + (1 - reflectivity) * color);
    return color;
}
