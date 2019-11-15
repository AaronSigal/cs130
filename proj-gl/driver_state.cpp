#include "driver_state.h"
#include <cstring>
#include <vector>

bool intersect_triangle(data_geometry a, data_geometry b, data_geometry c, vec2 point) {


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
    state.image_depth=0;

    for (int i = 0; i < width * height; i++) {
        //delete state.image_color[i];
        state.image_color[i] = pixel(make_pixel(0,0,0));
    }

    std::cout<<"TODO: allocate and initialize state.image_color and state.image_depth."<<std::endl;
}

// This function will be called to render the data that has been stored in this class.
// Valid values of type are:
//   render_type::triangle - Each group of three vertices corresponds to a triangle.
//   render_type::indexed -  Each group of three indices in index_data corresponds
//                           to a triangle.  These numbers are indices into vertex_data.
//   render_type::fan -      The vertices are to be interpreted as a triangle fan.
//   render_type::strip -    The vertices are to be interpreted as a triangle strip.
void render(driver_state& state, render_type type)
{
    std::cout<<"TODO: implement rendering."<<std::endl;

        if (type == render_type::triangle) {

            for (int i = 0; i < (state.num_vertices * state.floats_per_vertex); i = i + (3 * state.floats_per_vertex)) { // Tracks the starting point of eachgroup of three. So i = the position of the first float in every group of three vertices. 

                data_geometry dg[3]; // Create an array of size 3
                const data_geometry * dg_ptr[3] = {&dg[0], &dg[1], &dg[2]};
                
                for (int j = 0; j < 3; j++)
                    dg[j].data = new float[MAX_FLOATS_PER_VERTEX]; // Initialize the data array of each data_geometry to the limit


                for (int j = 0; j < 3; j++) {

                    // Inserts a single vertex into dg[j]'s data. It uses j to track which one of the three vertices it is currently working on.
                    read_data(state.vertex_data, state.floats_per_vertex, i + j * state.floats_per_vertex, dg[j].data);

                    rasterize_triangle(state, dg_ptr);
                }

                /* // Debugging outputs
                std::cout << "vertex_data:\n ";
                for (int j = 0; j < state.floats_per_vertex * state.num_vertices; j++)
                    std::cout << state.vertex_data[j] << " ";
                std::cout << "\n";

                std::cout << "data_geometry data:\n";
                for(int j = 0; j < 3; j++) {
                    for (int k = 0; k < state.floats_per_vertex; k++) {
                        std::cout << dg[j].data[k] << " ";
                    }
                    std::cout << "\n";
                }
                */

                for (int j = 0; j < 3; j++)
                    delete dg[j].data; // Free the memeory we allocated earlier

                }
            return;

        }

    if (type == render_type::indexed) {
    
        return;
    }

    if (type == render_type::fan) {
    
        return;
    }

    if (type == render_type::strip) {
    
        return;
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
    std::cout<<"TODO: implement clipping. (The current code passes the triangle through without clipping them.)"<<std::endl;
    clip_triangle(state,in,face+1);
}

// Rasterize the triangle defined by the three vertices in the "in" array.  This
// function is responsible for rasterization, interpolation of data to
// fragments, calling the fragment shader, and z-buffering.
void rasterize_triangle(driver_state& state, const data_geometry* in[3])
{
    const data_vertex dv_0{in[0]->data};
    const data_vertex dv_1{in[1]->data};
    const data_vertex dv_2{in[2]->data};

    std::vector<data_geometry> dg_outs;

    for (int i = 0; i < 3; i++) {
        data_geometry dg_out;

        (*state.vertex_shader)(dv_0, dg_out, state.uniform_data);

        dg_outs.push_back(dg_out);

    }

    int lower_x_bound, upper_x_bound, lower_y_bound, upper_y_bound; // Bounds for iterating over the image.

    // The bounds are going to be naievely set for now. They will be optimized by a function that will be written later.
    lower_x_bound = lower_y_bound = 0;
    upper_x_bound = state.image_width;
    upper_y_bound = state.image_height;

    for (int i = lower_x_bound; i < upper_x_bound; i++) {
        for (int j = lower_y_bound; j < upper_y_bound; j++) {

            if (intersect_triangle(dg_outs[0], dg_outs[1], dg_outs[2], vec2(i,j))) {
                state.image_color[((j) * state.image_height) + i] = make_pixel(255,0,0);
            }
        }
    }




    std::cout<<"TODO: implement rasterization"<<std::endl;
}



