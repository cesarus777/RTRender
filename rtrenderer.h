#ifndef RTRENDERER_H_INCLUDDED
#define RTRENDERER_H_INCLUDDED

#include "obj_parser.hpp"
#include "primitives.hpp"

#include <SDL.h>

#include <exception>
#include <string>
#include <iostream>


namespace RTR
{
    // Window size
    const int WIDTH = 800;
    const int HEIGHT = 800;
    const int MAX = 1;

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
            REMOV,
            COLOR,
            
            null
        };
    
    
    // The main class:
    class Renderer
    {
        obj_model*      model   = nullptr;
        mode_t          mode    = null;
        
        SDL_Renderer*   renderer = nullptr;
        SDL_Window*     window   = nullptr;


        private:
            /* draw the <mode_t> target */
            void draw_target();
            void render_wireframe();
            void render_randcolor();
            void render_rasterisator();
            void render_removal_rasterisator();
            void render_color_removal();
            void render_lines();
            void render_triangles();
            
        public:
            Renderer( int argc, char** argv);
            ~Renderer();
            
            /* configure the class */
            void argv_parse( int argc, char** argv);
            void show_usage();
                        
            /* draw the model and wait for an event */
            void static_display();          
            
            
            
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
