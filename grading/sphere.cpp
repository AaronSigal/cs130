#include "sphere.h"
#include "ray.h"
#include "util.h"
#include <cmath> // for pow

// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
    //Hit hit;

    vec3 v = ray.endpoint - center;

    double a, b, c, d; // a, b, and c are the members of the quadratic equation. d is the result for the part within the sqrt

    a = dot(ray.direction, ray.direction); // Direction^2
    b = dot (v, ray.direction) * 2;
    c = dot (v, v) - std::pow(radius, 2);
    d = std::pow(b, 2) - (4 * a * c); // as per notes on sphere intersection

    if (d < 0) { // If the intersection was behind the camera (not relevent)

        return Hit(nullptr, std::numeric_limits<double>::infinity(), -1); // Report that there was no valid intersection

    } else { 									 // If the intersection is in front of the camera
        float precursor = -b - std::pow(d, 0.5); // Checking the -b - 2a scenario in the quadratic equation

        if (precursor > 0) {

            return Hit(this, precursor / (2.0* a), part); // Return the top of the quadratic equation divided by 2a (which is the bottom)
        }

        precursor = -b + std::pow(d, 0.5); 		 // Checking the -b + 2a scenario

        if (precursor > 0) {

            return Hit(this, precursor / (2.0* a), part);

        } else {

            return NO_INTERSECTION;

        }
    }
}

vec3 Sphere::Normal(const vec3& point, int part) const
{
    vec3 normal = (point - center).normalized();
    return normal;
}

Box Sphere::Bounding_Box(int part) const
{
    Box box;
    TODO; // calculate bounding box
    return box;
}
