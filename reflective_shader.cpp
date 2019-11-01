#include "light.h"
#include "reflective_shader.h"
#include "ray.h"
#include "render_world.h"

vec3 Reflective_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
	
	vec3 color(0,0,0);

	color = world.Cast_Ray(ray, recursion_depth);
	
	for (unsigned int i = 0; i < world.lights.size(); i++) { // Iterate through the lights

		const vec3 light_to_object = world.lights[i] -> position - intersection_point; // Calculate a vector to the light from the point

		const vec3 reflection = (-1.0 * light_to_object) + dot(light_to_object, normal) * 2 * normal; // Reflect the light_to_object vector about the normal vector

		// Set the reflection: reflectivity * reflected color + (1- reflectivity) * color
		color = reflectivity * world.Cast_Ray(Ray(intersection_point, reflection.normalized()), recursion_depth++)
				+ (1.0 - reflectivity) * color;

	} 
    //vec3 color = reflectivity * reflected color + (1- reflectivity) * color;
    //TODO; // determine the color
    return color;
}
