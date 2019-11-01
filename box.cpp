#include <limits>
#include "box.h"
#include "util.h"

template <class T>
void swap_element (T &a, T &b) {
	T temp = a;
	a = b;
	b = temp;
}

// Return whether the ray intersects this box.
bool Box::Intersection(const Ray& ray) const
{


	TODO; // Remove last two checks if they aren't needed

	// Declarations
	double lowest_x, lowest_y, lowest_z;
	double highest_x, highest_y, highest_z;

	// Initial value calculation of the bounding lines
    lowest_x = (lo[0]- ray.endpoint[0]) / ray.direction[0]; 
	highest_x = (hi[0] - ray.endpoint[0]) / ray.direction[0]; 

	lowest_y = (lo[1] - ray.endpoint[1]) / ray.direction[1]; 
	highest_y = (hi[1] - ray.endpoint[1]) / ray.direction[1]; 

	// Corrections
	if (lowest_x > highest_x) 
		swap_element(lowest_x, highest_x); 

	if (lowest_y > highest_y) 
		swap_element(lowest_y, highest_y); 

	// Check for nonsense. If we get nonsense, that means there's no hit and we can stop (avoids doing extra work)
	if ((lowest_x > highest_y) || (lowest_y > highest_x)) 
		return false; 

	if (lowest_y > lowest_x) 
		lowest_x = lowest_y; 

	if (highest_x > highest_y) 
		highest_x = highest_y; 

	// Initial Calculations for just Z
	lowest_z = (lo[2] - ray.endpoint[2]) / ray.direction[2]; 
	highest_z = (hi[2]- ray.endpoint[2]) / ray.direction[2]; 

	// Correct Z
	if (lowest_z > highest_z) 
		swap_element(lowest_z, highest_z); 

	// Check for nonsense
	if ((lowest_x > highest_z) || (lowest_z > highest_x)) 
		return false; 

	if (lowest_z > lowest_x) 
		lowest_x = lowest_z; 

	if (highest_z < highest_x) 
		highest_x = highest_z; 

	// Return true if we never ran into nonsense
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

