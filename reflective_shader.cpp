#include "reflective_shader.h"
#include "ray.h"
#include "render_world.h"

vec3 Reflective_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
	vec3 color(0,0,0);

	if (recursion_depth <= 0) {
		return color;
	}

	for (unsigned int i = 0; i < world.lights.size(); i++) {

		const vec3 light_to_object = world.lights[i] -> position - intersection_point;

		const vec3 reflection = (-1.0 * light_to_object) + dot(light_to_object, normal) * 2 * normal; // Reflect the light_to_object vector about the normal vector

		Hit reflection_hit = Closest_Intersection(reflection);

		vec3 reflected_intersection_point = reflection_hit.dist * reflection.normalized();

		if (reflection_hit.object != nullptr) {
			vec3 reflection_normal = reflection_hit.object -> Normal(reflected_intersection_point, reflection_hit.part);
		} else {
			vec3 reflection_normal(0,0,0);
		}

		color = shader -> Shade_Surface(reflection, reflected_intersection_point, reflection_normal, recursion_depth - 1);

	}
    //vec3 color = reflectivity * reflected color + (1- reflectivity) * color;
    //TODO; // determine the color
    return color;
}
