#include <limits>
#include "box.h"

// Return whether the ray intersects this box.
bool Box::Intersection(const Ray& ray) const
{
    TODO;
    return true;
}

// Compute the smallest box that contains both *this and bb.
Box Box::Union(const Box& bb) const
{
    Box box;
    box.Make_Empty();
    box.Include_Point(bb.hi);
	box.Include_Point(bb.lo);
	box.Include_Point(hi);
	box.Include_Point(lo);
    return box;
}

// Enlarge this box (if necessary) so that pt also lies inside it.
void Box::Include_Point(const vec3& pt)
{
    for (int i = 0; i < 3; i++) {
    	if (pt[i] < lo[i])
    		lo[i] = pt[i];

    	if (pt[i] > hi[i])
    		hi[i] = pt[i];
    }
}

// Create a box to which points can be correctly added using Include_Point.
void Box::Make_Empty()
{
    lo.fill(std::numeric_limits<double>::infinity());
    hi=-lo;
}
