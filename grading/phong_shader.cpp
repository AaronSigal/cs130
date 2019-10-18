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

    color += color_diffuse * diffuse(ray, intersection_point, normal) + color_specular * std::pow(specular(ray, intersection_point, normal), specular_power);

    return color;
}

double Phong_Shader::specular(const Ray& ray,const vec3& intersection_point,
                              const vec3& normal) const {
    double intensity = 0;

    vec3 view_direction = (world.camera.film_position - intersection_point).normalized();

    for (const auto& light : world.lights) {
      vec3 light_direction = (light->position - intersection_point).normalized();

      vec3 reflected_direction = 2 * (dot(normal.normalized(), light_direction)) * (normal.normalized() - light_direction);

      intensity += dot(view_direction, reflected_direction);

    }

    return intensity;
}

double Phong_Shader::diffuse(const Ray& ray,
                             const vec3& intersection_point,
                             const vec3& normal) const {
    double intensity = 0;

    for (const auto &light : world.lights) {
        vec3 light_direction = light->position - intersection_point;
        intensity += dot(normal, light_direction.normalized());
    }

    return intensity;
}
