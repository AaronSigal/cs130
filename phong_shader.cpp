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

    color += (color_diffuse * diffuse(ray, intersection_point, normal)) + (color_specular * pow(specular(ray, intersection_point, normal), specular_power));
    return color;
}

double Phong_Shader::specular(const Ray& ray,const vec3& intersection_point,
                              const vec3& normal) const {
    double intensity = 1;

    vec3 view_direction = (world.camera.film_position - intersection_point).normalized();

    return intensity;
}

double Phong_Shader::diffuse(const Ray& ray,
                             const vec3& intersection_point,
                             const vec3& normal) const {
    double intensity = dot (ray.direction.normalized(), normal.normalized());

    return intensity;
}
