#ifndef RTRENDERER_H_INCLUDDED
#define RTRENDERER_H_INCLUDDED

#include "obj_parser.hpp"
//#include "primitives.hpp"
#include "geometry.hpp"

#include <SDL.h>

#include <exception>
#include <string>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cassert>


#define USE_MEMSET



namespace RTR
{
    // Window size
    const int       WIN_WIDTH_DEFAULT     = 1000;
    const int       WIN_HEIGHT_DEFAULT    = 800;

   
    
    const uint8_t R_BGR = 0;
    const uint8_t G_BGR = 0;
    const uint8_t B_BGR = 0;
    const uint8_t A_BGR = 255;
    
    const double W_SHIFT_DEFAULT    = 0.1;  // determines .obj position 
    const double H_SHIFT_DEFAULT    = 0.;   // on the screen
    const double OBJ_SCALE_DEFAULT  = 400;  // determines size of the model on the screen
    const double Y_SHIFT_SPEED_DEFAULT    = 0.03;  // WASD speed
    const double X_SHIFT_SPEED_DEFAULT    = 0.03;  //
    
    
    using zbuf_depth_t              = int32_t;
    const int ZBUF_SCALE            = 1000000000;  // 
    const double ZBUF_SHARPNESS     = 1.25;  // should be more 1.0 
   
    
//    const char* usage_info = "Usage: [-s <FIGURE>] [-o <FILE>] [-m <MODE>]\n";
    // Supported modes:
        enum mode_t 
        { 
            GOLD,
            NOT_GOLD,
            TRIANGLE,
            WIREFRAME,
            RAND,
            RAST,
            N_RM_RST,
            TEXTURE,
            ZBUF,
            
            null
        };
    
    
    // The main class:
    class Window
    {
        obj_model*  model   = nullptr;
        mode_t      mode    = null;

        double W_SHIFT    = W_SHIFT_DEFAULT;   // determine .obj 
        double H_SHIFT    = H_SHIFT_DEFAULT;   // position 
        double OBJ_SCALE  = OBJ_SCALE_DEFAULT; // on the screen
        
        
        zbuf_depth_t*   zbuf     = nullptr;
        zbuf_depth_t    zbuf_min =          
                            std::numeric_limits<zbuf_depth_t>::max();
        zbuf_depth_t    zbuf_max =
                            std::numeric_limits<zbuf_depth_t>::min();
        
        SDL_Renderer*   renderer = nullptr;
        SDL_Window*     window   = nullptr;
        int         WIN_WIDTH   = WIN_WIDTH_DEFAULT;
        int         WIN_HEIGHT  = WIN_HEIGHT_DEFAULT;
        
        double Y_SPEED    = 0;  //
        double X_SPEED    = 0;  //
        

        private:
            /* draw the <mode_t> target */
            void draw_target(mode_t);
            
            void render_wireframe();
            void render_randcolor();
            void render_rasterization();
            void render_dnt_remove();
            void render_texture();
            void render_lines();
            void render_triangles();
            void render_z_buffer();
            void clear_screen();
            
            
            // Pimitives:
            void draw_line( int x1, int y1, int x2, int y2);
            void draw_line( vec2i v1, vec2i v2);
            
            void draw_triangle( vec2i v1, vec2i v2, vec2i v3);  // no zbuf
            
            void draw_triangle( vec3i v1, vec3i v2, vec3i v3);
            void only_fill_zbuf( vec3i v1, vec3i v2, vec3i v3);
            
            void draw_triangle( vec3i v1, vec3i v2, vec3i v3,
                                vec2i t1, vec2i t2, vec2i t3,
                                double intensity);
                                
                                
             /* zbuf */
             void zbuf_clear();
            
        public:
            Window( int argc, char** argv);
            ~Window();
            
            /* configure the class */
            char* argv_parse( int argc, char** argv);
            void  show_usage();
                        
            /* draw the model and wait for an event */
            void static_display();  
            void xy_move_display();
            
                
            
            
            
    };
    
    
    

    
    
    // Exceptions:
        class bad_mode : public std::exception
        {
            public:
                virtual const char* what() const noexcept override
                { return "Incorrect rendering mode provided"; }
        };
        
        
        
        class bad_input : public std::exception
        {
            public:
                virtual const char* what() const noexcept override
                { return "Incorrect input"; }
        };
    
        
        class sdl_error : public std::exception
        {
            const char* msg;
            
            public:
                sdl_error()
                { msg = SDL_GetError(); }
                
                virtual const char* what() const noexcept override
                { return msg; }
        };

}

#endif
