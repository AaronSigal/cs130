#include "render_world.h"
#include "flat_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"
#include "util.h" // For NO_INTERSECTION

extern bool disable_hierarchy;



Render_World::Render_World()
    :background_shader(0),ambient_intensity(0),enable_shadows(true),
    recursion_depth_limit(3)
{}

Render_World::~Render_World()
{
    delete background_shader;
    for(size_t i=0;i<objects.size();i++) delete objects[i];
    for(size_t i=0;i<lights.size();i++) delete lights[i];
}

// Find and return the Hit structure for the closest intersection.  Be careful
// to ensure that hit.dist>=small_t.
Hit Render_World::Closest_Intersection(const Ray& ray)
{
    int obj_index = -1;
    int index = -1;
    //std::cout << "Taking intersection\n"; // TODO: Remove debugging

    double min_t = std::numeric_limits<double>::max(); // Set min_t to the largest possible double value

    Hit closest_hit = NO_INTERSECTION; // Default to no intersection. If any other hit is found, it is reported instead of this one.

    for (auto& obj : objects) { // For each object in the world
        index++;
        for (int i = 0; i < obj->number_parts; i++) { // For each part on a given object

            //std::cout << "Checking collision...\n";

            Hit collision = obj -> Intersection(ray, i); // Get a hit

            //std::cout << "Done calculating collision\n";

            if (collision.dist < min_t && collision.dist > small_t) { // Checks if the hit is closer than the previous-closest hit. Ignores if the hit is itself
                
                //std::cout << "Found new hit!\n";
                min_t = collision.dist;
                closest_hit = collision;

                if (debug_pixel) {
                    obj_index = index;
                }

            }

            //std::cout << "Done assigning new hit...\n";

        }
    }

    if (debug_pixel) {
        std::cout << "Hit distance: " << closest_hit.dist << "\n";
        std::cout << "Obj[" << obj_index << "]\n";
    }

    return closest_hit;
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{

    vec3 p2 = camera.World_Position(pixel_index);

    vec3 direction_raw;
    
    // Build the direction vector from p1 and p2;
    for (int i = 0; i < 3; i++)
        direction_raw[i] = p2[i] - camera.position[i];
    
    vec3 direction = direction_raw.normalized();

    Ray ray(camera.position, direction); // Initialize the ray using the camera position as the end point and the direction vector calculated earlier.
    vec3 color=Cast_Ray(ray,1);
    camera.Set_Pixel(pixel_index,Pixel_Color(color));
}

void Render_World::Render()
{
    if(!disable_hierarchy)
        Initialize_Hierarchy();

    for(int j=0;j<camera.number_pixels[1];j++)
        for(int i=0;i<camera.number_pixels[0];i++)
            Render_Pixel(ivec2(i,j));
}

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray& ray,int recursion_depth)
{

    //std::cout << "Casting ray\n"; // TODO: Remove debugging

    vec3 color(0,0,0);

    if (recursion_depth > recursion_depth_limit) {
        std::cout << "Exceeded depth limit\n";
        return color;
    }

    Hit ht = Closest_Intersection(ray);

    //std::cout << "Closest hit found...\n";

    //TODO; // Confirm that this is the correct way to check
    if (ht.object != nullptr) { // Checks if there was a valid intersection.

        //std::cout << "Within depth limit\n";

        //std::cout << "Shading by material...\n";
        //TODO; // Get the color from the material shader of the intersected object
        color = ht.object -> material_shader -> Shade_Surface(ray, 
                                                                 ray.endpoint + (ray.direction) * ht.dist,
                                                                 ht.object -> Normal( ray.endpoint + (ray.direction) * ht.dist, ht.part),
                                                                 recursion_depth);

    } else {

        std::cout << "Shading by background...\n";
        vec3 zero_vector(0,0,0);
        color = background_shader->Shade_Surface(ray, 
                                                 zero_vector, 
                                                 zero_vector,
                                                 recursion_depth);
    
    }

    //std::cout << "Returning color...\n";

    if (debug_pixel) {
        std::cout << " Cast Ray: Endpoint: " << ray.endpoint << "Direction: " << ray.direction << " ";
        std::cout << "\n";
    }
    return color;
}

void Render_World::Initialize_Hierarchy()
{
    TODO; // Fill in hierarchy.entries; there should be one entry for
    // each part of each object.

    hierarchy.Reorder_Entries();
    hierarchy.Build_Tree();
}
