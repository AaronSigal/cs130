#include "driver_state.h"
#include <cstring>
#include <vector>

bool DEBUG_PIXEL = false;
const int DEBUG_X = 160;
const int DEBUG_Y = 120;

void read_data(float * vertex_data, int floats_per_vertex, int index, float* data_out) {
    for (int i = index; i < index + floats_per_vertex; i++) {
        data_out[i - index] = vertex_data[i];
    }
}

// Area = 0.5*((bx*cy - cx*by) - (ax*cy - cx*ay) + (ax*by - bx*ay)) 
float area(vec2 a, vec2 b, vec2 c) {
    return 0.5 * ((b[0] * c[1] - c[0] * b[1]) - (a[0] * c[1] - c[0] * a[1]) + (a[0] * b[1] - b[0] * a[1]));
}


bool intersect_triangle(driver_state& state,data_geometry a, data_geometry b, data_geometry c, vec2 point, float triangle_area, float &alpha, float &beta, float &gamma) { 

    // Barycentric weights
    vec2 A, B, C;

    A = vec2(  ( a.gl_Position[0] / a.gl_Position[3] + 1)  * 0.5 * state.image_width,
               ( a.gl_Position[1] / a.gl_Position[3] + 1) * 0.5 * state.image_height);

    B = vec2(  ( b.gl_Position[0] / b.gl_Position[3] + 1)  * 0.5 * state.image_width,
               ( b.gl_Position[1] / b.gl_Position[3] + 1) * 0.5 * state.image_height);

    C = vec2(  ( c.gl_Position[0] / c.gl_Position[3] + 1)  * 0.5 * state.image_width,
               ( c.gl_Position[1] / c.gl_Position[3] + 1) * 0.5 * state.image_height);

    //std::cout << area(C, B, point);

    // AREA(CBP) / AREA(ABC)
    float inverse_area = 1.0 / area(A, B, C);

    alpha = area(point,B, C) * inverse_area;
    beta  = area(A, point, C) * inverse_area;
    gamma = area(A, B, point) * inverse_area;

    if (DEBUG_PIXEL) std::cout << "alpha: " << alpha << " "
              << "beta:"   << beta  << " "
              << "gamea: " << gamma << "\n";

    if (alpha >= 0 && beta >= 0 && gamma >= 0) {
        if (DEBUG_PIXEL) std::cout << "Point passed!" << std::endl;
        return true;
    }

    if (DEBUG_PIXEL) std::cout << "Point failed!" << std::endl;
    return false;
}

driver_state::driver_state()
{
}

driver_state::~driver_state()
{
    delete [] image_color;
    delete [] image_depth;
}

// This function should allocate and initialize the arrays that store color and
// depth.  This is not done during the constructor since the width and height
// are not known when this class is constructed.
void initialize_render(driver_state& state, int width, int height)
{
    state.image_width=width;
    state.image_height=height;
    state.image_color = new pixel[width * height]; // We have (width * height) number of pixels, and as such, need to store that many pieces of color information
    state.image_depth = new float[width * height];

    for (int i = 0; i < width * height; i++) {
        //delete state.image_color[i];
        state.image_color[i] = pixel(make_pixel(0,0,0));
        state.image_depth[i] = 1;
    }

    //std::cout<<"TODO: allocate and initialize state.image_color and state.image_depth."<<std::endl;
}

// This function will be called to render the data that has been stored in this class.
// Valid values of type are:
//   render_type::triangle - Each group of three vertices corresponds to a triangle.
//   render_type::indexed -  Each group of three indices in index_data corresponds
//                           to a triangle.  These numbers are indices into vertex_data.
//   render_type::fan -      The vertices are to be interpreted as a triangle fan.
//   render_type::strip -    The vertices are to be interpreted as a triangle strip.
void render(driver_state& state, render_type type) {
    //std::cout<<"TODO: implement rendering."<<std::endl;

    data_geometry dg[3]; // Create an array of size 3
    
    const data_geometry * dg_ptr[3] = {&dg[0], &dg[1], &dg[2]};

        if (type == render_type::triangle) {

            data_vertex dv;      // Used to vertex shade the above data_geometry

            for (int i = 0; i < state.num_vertices * state.floats_per_vertex; i += 3 * state.floats_per_vertex) { // Tracks the starting point of each group of three. So i = the position of the first float in every group of three vertices.
                
                for (int j = 0; j < 3; j++) {
                    dg[j].data = state.vertex_data + i + j * state.floats_per_vertex; // Set the contents of the data of the data_geometry currently being iterated over
                    dv.data = dg[j].data;                                             // Extract the data into a data_vertex object
                    state.vertex_shader(dv, dg[j], state.uniform_data);               // Shade the data_geometry using the data_vertex
                }

                clip_triangle(state, dg_ptr, 0); // TODO: Update when clipping is ready

               if (DEBUG) {
                   std::cout << "vertex_data:\n ";
                   for (int j = 0; j < state.floats_per_vertex * state.num_vertices; j++)
                       std::cout << state.vertex_data[j] << " ";
                   std::cout << "\n";

                   std::cout << "data_geometry data:\n";
                   for (int j = 0; j < 3; j++) {
                       for (int k = 0; k < state.floats_per_vertex; k++) {
                           std::cout << dg[j].data[k] << " ";
                       }
                       std::cout << "\n";
                   }
               }
            }
        return;
    }

    if (type == render_type::indexed) {

        data_vertex dv[3]; // We need more data vertex's for this one, so we have our own array in this case
    
        for (int i = 0; i < 3 * state.num_triangles; i += 3) { // Tracks the starting point of each group of three. So i = the position of the first float in every group of three vertices.    
            for (int j = 0; j < 3; j++) {
            
                int index = state.index_data[i +j]; // Create an index that will be scaled later on

                dv[j].data = state.vertex_data + (index * state.floats_per_vertex); // Set the data vertex's data to our scaled index
                dg[j].data = dv[j].data;                                            // Set the data geomentry's data to our data vertex's data

                // Shade our veticies
                state.vertex_shader(dv[j], dg[j], state.uniform_data); // Call the vertex shader on the specific vertex data we are working on

            }
            clip_triangle(state, dg_ptr, 0);

        }


        return;
    }

    if (type == render_type::fan) {

        data_vertex dv[3]; // We need more data vertex's for this one, so we have our own array in this case

            for(int i = 0; i < state.num_vertices; i++) {
                for(int j = 0; j < 3; j++) {
                    
                    if (j != 0) dv[j].data = state.vertex_data + (j * state.floats_per_vertex) + (i * state.floats_per_vertex);
                    else dv[j].data = state.vertex_data;

                    dg[j].data = dv[j].data;
                    
                    state.vertex_shader(dv[j], dg[j], state.uniform_data); // Call the vertex shader on the specific data vertex we just created
    
                    
                }
                clip_triangle(state, dg_ptr, 0);
             
             }
        
        return;
    }

    if (type == render_type::strip) {

        data_vertex dv[3]; // We need more data vertex's for this one, so we have our own array in this case
        
        int index = 0;     // The math is too complicated to do a simplified math-based iteration, so I'm being lazy and using an external index
        
         for(int i = 0; i < state.num_vertices - 2; i++) {

            if(index != 0) { // We shouldn't do this when the index hits zero (that would cause an out of bounds exception)

                index -= (2 * state.floats_per_vertex); // Move the index back by two vertex's worth

            }

            for(int j = 0; j < 3; j++) {

                dv[j].data = state.vertex_data + index; // Grab the verticies at the current index in vertex_data
                dg[j].data = dv[j].data;                // Copy the data over

                index += state.floats_per_vertex; // Increase the index by one vertex's worth

                state.vertex_shader(dv[j], dg[j], state.uniform_data); // Call the vertex shader
            }


             clip_triangle(state, dg_ptr, 0);                          // Clip and render
        }
    }

}


// This function clips a triangle (defined by the three vertices in the "in" array).
// It will be called recursively, once for each clipping face (face=0, 1, ..., 5) to
// clip against each of the clipping faces in turn.  When face=6, clip_triangle should
// simply pass the call on to rasterize_triangle.
void clip_triangle(driver_state& state, const data_geometry* in[3],int face)
{


    if(face==6)
    {
        rasterize_triangle(state, in);
        return;
    }
    //std::cout<<"TODO: implement clipping. (The current code passes the triangle through without clipping them.)"<<std::endl;
    clip_triangle(state,in,face+1);
}

// Rasterize the triangle defined by the three vertices in the "in" array.  This
// function is responsible for rasterization, interpolation of data to
// fragments, calling the fragment shader, and z-buffering.
void rasterize_triangle(driver_state& state, const data_geometry* in[3])
{

    int lower_x_bound, upper_x_bound, lower_y_bound, upper_y_bound; // Bounds for iterating over the image.

    // The bounds are going to be naievely set for now. They will be optimized by a function that will be written later.
    lower_x_bound = lower_y_bound = 0;
    upper_x_bound = state.image_width;
    upper_y_bound = state.image_height;

    data_fragment d_f;
    data_output d_o;

    //std::cout << "lx " << lower_x_bound << " ux " << upper_x_bound << " ly " << lower_y_bound << " uy " << upper_y_bound << std::endl;
    for (int i = lower_x_bound; i < upper_x_bound; i++) {
        for (int j = lower_y_bound; j < upper_y_bound; j++) {

            if (DEBUG && i == DEBUG_X && j == DEBUG_Y) DEBUG_PIXEL = true;
            else DEBUG_PIXEL = false;
        
            //std::cout << "Checking " << i << ", " << j << std::endl;
            float triangle_area = -1; // To be calculated via intersect trinagle using pass-by-reference

            float alpha = 0, beta = 0, gamma = 0; // To be calculated via intersect triangle using pass-by-reference

            if (intersect_triangle(state, *in[0], *in[1], *in[2], vec2(float(i) + 0.5, float(j) + 0.5), triangle_area, alpha, beta, gamma)) {

                if (DEBUG_PIXEL) std::cout << "Alpha: " << alpha << " Beta: " << beta << " Gamma: " << gamma << std::endl;

                float depth = (alpha * in[0] -> gl_Position[2] / in[0] -> gl_Position[3]) + (beta * in[1] -> gl_Position[2] / in[1] -> gl_Position[3]) + (gamma * in[2] -> gl_Position[2] / in[2] -> gl_Position[3]);

                if (DEBUG_PIXEL) std::cout << "Depth: " << depth << std::endl;

                if (depth < state.image_depth[(j * state.image_width) + i]) {

                    d_f.data = new float[state.floats_per_vertex];

                    for (int k = 0; k < state.floats_per_vertex; k++) {
                        
                        if (state.interp_rules[k] == interp_type::noperspective) { // Simple interpolation case. Handle first.
                            d_f.data[k] = (alpha * in[0] -> data[k]) + (beta * in[1] -> data[k]) + (gamma * in[2] -> data[k]); // Perform simple interpolation.
                            
                            if (DEBUG_PIXEL) std::cout << "interpolation type: noperspective" << std::endl;

                        } else if (state.interp_rules[k] == interp_type::smooth) {
                            float delta = (alpha / in[0] -> gl_Position[3]) + (beta / in[1] -> gl_Position[3]) + (gamma / in[2] -> gl_Position[3]); // Caluclate the final term

                            d_f.data[k] = (alpha / in[0] -> gl_Position[3] / delta * in[0] -> data[k]) + (beta / in[1] -> gl_Position[3] / delta * in[1] -> data[k]) + (gamma / in[2]->gl_Position[3] / delta * in[2] -> data[k]); // Perform the interpolation on data[k]
                        
                            if (DEBUG_PIXEL) std::cout << "interpolation type: smooth" << std::endl;
                        } else if (state.interp_rules[k] == interp_type::flat) { // Trivial case. No interpolation performed
                            d_f.data[k] = in[0] -> data[k]; // Pass the data through, unmolested.
                        
                            if (DEBUG_PIXEL) std::cout << "interpolation type: flat" << std::endl;
                        }
                    }
                    
                    state.image_color[(j * state.image_width) + i] = make_pixel(d_o.output_color[0] * 255, d_o.output_color[1] * 255, d_o.output_color[2] * 255);// If the intersection routine comes back true, color the current pixel. //TODO: set color correctly
                    state.image_depth[(j * state.image_width) + i] = depth;
                    state.fragment_shader(d_f, d_o, state.uniform_data);
                }
            }
        }
    }
}



