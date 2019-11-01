#include "light.h"
#include "reflective_shader.h"
#include "ray.h"
#include "render_world.h"

vec3 Reflective_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
	
	//std::cout << "Setting initial color in reflection shader\n";
	vec3 color = world.Cast_Ray(ray, recursion_depth + 1);

	//vec3 color(0,0,0);
	
	for (unsigned int i = 0; i < world.lights.size(); i++) { // Iterate through the lights

		const vec3 light_to_object = world.lights[i] -> position - intersection_point; // Calculate a vector to the light from the point

		const vec3 reflection = (-1.0 * light_to_object) + dot(light_to_object, normal) * 2 * normal; // Reflect the light_to_object vector about the normal vector

		Hit reflection_hit = world.Closest_Intersection(Ray(intersection_point, reflection.normalized())); // Get a hit from reflected ray

		vec3 reflected_intersection_point = reflection_hit.dist * reflection.normalized(); // Get the intersection point on the reflection

		vec3 reflection_normal;

		if (reflection_hit.object != nullptr) { // If the object exists, get its normal
			reflection_normal = reflection_hit.object -> Normal(reflected_intersection_point, reflection_hit.part);
		} else { // If it doesn't, default to an empty normal
			reflection_normal = vec3(0,0,0);
		}

		// Set the reflection: reflectivity * reflected color + (1- reflectivity) * color
		color = reflectivity * shader -> Shade_Surface(Ray(intersection_point, reflection.normalized()), reflected_intersection_point, reflection_normal, recursion_depth + 1)
				+ (1.0 - reflectivity) * color;
	}
    //vec3 color = reflectivity * reflected color + (1- reflectivity) * color;
    //TODO; // determine the color
    return color;
}
