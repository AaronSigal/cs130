#include "sphere.h"
#include "ray.h"

// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
	Hit hit;
	hit.object = this;
	hit.part = part;
	hit.dist = 1;

    TODO;
    return hit;
}

vec3 Sphere::Normal(const vec3& point, int part) const
{
    vec3 normal = (point - center).normalized();
    //TODO; // compute the normal direction
    return normal;
}

Box Sphere::Bounding_Box(int part) const
{
    Box box;
    TODO; // calculate bounding box
    return box;
}
