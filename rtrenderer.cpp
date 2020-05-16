#include "rtrenderer.h"


// Basic methods:в 
///////////////////////////////////////////////////////////////////////////
//  Constructor/Destructor:
//
    RTR::Window::Window(int argc, char** argv)
    {
        char* filename = argv_parse( argc, argv);

        if (filename != nullptr)
            model = new obj_model( filename);


        int ret = SDL_CreateWindowAndRenderer(  WIN_WIDTH,  WIN_HEIGHT, 0,
                                                &window, &renderer);     
        if (ret < 0) throw sdl_error();

        zbuf = new zbuf_depth_t[WIN_WIDTH * WIN_HEIGHT];
        zbuf_clear();          
        
        // clear the screen
        clear_screen();
        
        SDL_RenderPresent(renderer);
        
        return;
    }
    



    RTR::Window::~Window()
    {
        delete model;
        delete [] zbuf;
        
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        
        return;
    }
//
//
///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////
//  Task parser:
//

    char* RTR::Window::argv_parse( int argc, char** argv)
    {
        bool flag0 = false;
        bool flag1 = false;
        bool flag2 = false;
        
        char* filename = nullptr;

        int i = 1;
        while (i < argc)
        {
            if (argv[i][0] == '-') switch( argv[i][1])  
            {
                case 's' :  
                    if( (i + 1 >= argc))    show_usage();
                    if( strcmp( argv[i + 1], "spiral") == 0)
                        mode = NOT_GOLD;
                        
                    else if( strcmp( argv[i + 1], "golden_spiral") == 0)
                        mode = GOLD;
                          
                    else if( strcmp( argv[i + 1], "triangles") == 0)
                        mode = TRIANGLE;
                            
                    else
                        show_usage();
                            
                    flag0 = true;
                    i += 2;
                    break;
                        
                case 'o' :  
                    if( (i + 1 >= argc) or (flag1 == true))
                        show_usage();
                    filename = argv[i + 1];
                    flag1 = true;
                    i += 2;
                    break;
                        
                case 'm' :  
                    if( (i + 1 >= argc) or (flag2 == true))
                        show_usage();
                        
                        
                    if(      strcmp( argv[i + 1], "wire") == 0)
                        mode = WIREFRAME;
                            
                    else if( strcmp( argv[i + 1], "dont_remove") == 0)
                        mode = N_RM_RST;
                        
                    else if( strcmp( argv[i + 1], "rand") == 0)
                        mode = RAND;
                         
                    else if( strcmp( argv[i + 1], "rasterize") == 0)
                        mode = RAST;
                          
                    else if( strcmp( argv[i + 1], "texture") == 0)
                        mode = TEXTURE;
                        
                    else if( strcmp( argv[i + 1], "zbuf") == 0)
                        mode = ZBUF;
                            
                    else
                        show_usage();
                            
                    flag2 = true;
                    i += 2;
                    break;
                    
                case 'h' :  
                    show_usage();
                    break;
                            
                default : 
                    show_usage();
                    break;
            }

            else
                show_usage();
                
        }
        
        
        if ( !flag0 and flag1 and flag2)
            return filename;
            
        if ( flag0 and !flag1 and !flag2)
            return filename;
            
        show_usage();
        return nullptr; // end of non-void function;
    }
    
    
    
    
    
    void RTR::Window::show_usage()
    {
        std::cout << "Usage: [-s <FIGURE>] [-o <FILE>] [-m <MODE>]\n";
        throw bad_input();
    }
//
//
///////////////////////////////////////////////////////////////////////////    
    
   
   
 
///////////////////////////////////////////////////////////////////////////
// Supported window modes:    
//  
    void RTR::Window::static_display()
    {
            draw_target (mode);


            SDL_Event event;
            for(;;)
            {
                if (SDL_WaitEvent( &event) == 0) throw sdl_error();
                   
                if ((event.type == SDL_QUIT) ||
                                    (event.type == SDL_KEYDOWN))
                return;
            }
        
    }
    
    void RTR::Window::xy_move_display()
    {
        SDL_Event event;
        
        draw_target( mode);

        while ( SDL_WaitEvent( &event))
        {
            switch(event.type)
            {
                case SDL_QUIT : return;
            
                case SDL_KEYDOWN : switch( event.key.keysym.scancode)
                {
                    case SDL_SCANCODE_W:
                                    Y_SPEED =  Y_SHIFT_SPEED_DEFAULT;
                                    break;

                    case SDL_SCANCODE_A:
                                    X_SPEED = -X_SHIFT_SPEED_DEFAULT;
                                    break;
                                    
                    case SDL_SCANCODE_S:
                                    Y_SPEED = -Y_SHIFT_SPEED_DEFAULT;
                                    break;
                                    
                    case SDL_SCANCODE_D: 
                                    X_SPEED =  X_SHIFT_SPEED_DEFAULT;
                                    break;
                    default : break;
                }
                break;
                
                case SDL_KEYUP : switch( event.key.keysym.scancode)
                {
                    case SDL_SCANCODE_W:
                                    if (Y_SPEED > 0) Y_SPEED = 0;
                                    break;

                    case SDL_SCANCODE_A:
                                    if (X_SPEED < 0) X_SPEED = 0;
                                    break;
                                    
                    case SDL_SCANCODE_S:
                                    if (Y_SPEED < 0) Y_SPEED = 0;
                                    break;
                                    
                    case SDL_SCANCODE_D: 
                                    if (X_SPEED > 0) X_SPEED = 0;
                                    break;
                    default : break;
                }
                
                default : break;
            }
            
            H_SHIFT += Y_SPEED;
            W_SHIFT += X_SPEED;
            
            draw_target( mode);
        }
        
        throw sdl_error();
    }
//
//
///////////////////////////////////////////////////////////////////////////    




///////////////////////////////////////////////////////////////////////////
//  Rendering stuff:
//
    void RTR::Window::draw_target(mode_t m)
    {
        clear_screen();
        switch(m)
        {
            case WIREFRAME: render_wireframe();
                            break;
                
            case RAND:      render_randcolor();
                            break;
                            
            case N_RM_RST:  render_dnt_remove();
                            break;
                            
            case ZBUF:      render_z_buffer();
                            break;
                            
            case RAST:      render_rasterization();
                            break;
                            
            case TEXTURE:   render_texture();
                            break;
        
            case TRIANGLE:  render_triangles();
                            break;
                            
            case GOLD:      render_lines();
                            break;
                            
            case NOT_GOLD:  render_lines();
                            break;

            default:        throw bad_mode();
        }
          
        SDL_RenderPresent( renderer);
            
        return;
  }







void RTR::Window::render_wireframe()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for(size_t i = 0; i < model->nfaces(); ++i)
    {
        auto face = model->face(i);
        for(size_t j = 0; j < 3; ++j)
        {
            vec3d v1 = model->vertice(face[j]);
            vec3d v2 = model->vertice(face[(j + 1) % 3]);
            
            int x1 = ( model->xshift() - v1.x + W_SHIFT) * OBJ_SCALE
                                                + WIN_WIDTH / 2.0;
                                                    
            int y1 = ( model->yshift() - v1.y - H_SHIFT) * OBJ_SCALE 
                                                + WIN_HEIGHT / 2.0;
                                            
            int x2 = ( model->xshift() - v2.x + W_SHIFT) * OBJ_SCALE
                                                + WIN_WIDTH / 2.0;
                                                    
            int y2 = ( model->yshift() - v2.y - H_SHIFT) * OBJ_SCALE
                                                + WIN_HEIGHT / 2.0;

            draw_line( x1, y1, x2, y2);
        }
    }
    
    return;
}





void RTR::Window::render_randcolor()
{
    for(size_t i = 0; i < model->nfaces(); ++i)
    {
        auto face = model->face(i);
        vec2i coords[3];
        for(size_t j = 0; j < 3; ++j)
        {
            auto v = model->vertice(face[j]);
            int  x = ( model->xshift() - v.x + W_SHIFT) * OBJ_SCALE
                                                + WIN_WIDTH / 2.0;
                                                    
            int  y = ( model->yshift() - v.y - H_SHIFT) * OBJ_SCALE
                                                + WIN_HEIGHT / 2.0;
                                                    
            coords[j] = vec2i(x, y);
    }
    
    int r = std::rand();
    char red = r % 256;
    char green = (r >> 8)  % 256;
    char blue = (r >> 16) % 256;
    char alpha = (r >> 24) % 256;
    SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
    draw_triangle( coords[0], coords[1], coords[2]);
  }
}




void RTR::Window::render_dnt_remove()
{
    vec3d light(.0, .0, -1.0);
    for(size_t i = 0; i < model->nfaces(); ++i)
    {
        auto face = model->face(i);
        vec2i coords[3];
        vec3d world[3];
        for(size_t j = 0; j < 3; ++j)
        {
            auto v = model->vertice(face[j]);
            int  x = ( model->xshift() - v.x + W_SHIFT) * OBJ_SCALE
                                                + WIN_WIDTH / 2.0;
                                                    
            int  y = ( model->yshift() - v.y - H_SHIFT) * OBJ_SCALE
                                                + WIN_HEIGHT / 2.0;
            coords[j] = vec2i(x, y);
            world[j] = v;
        }
        vec3d n = (world[2] - world[0]) ^ (world[1] - world[0]);
        n.normalize();
        double intensity = n * light;
        if(intensity > 0.0)
        {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            red = green = blue = intensity * 255;
            uint8_t alpha = 0;
            SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
            draw_triangle( coords[0], coords[1], coords[2]);
        }
    }
}






void RTR::Window::render_rasterization()
{
    zbuf_clear();
    vec3d light(1.0, .0, -1.0);
  
    for(size_t i = 0; i < model->nfaces(); ++i)
    {
    //    vec2 pxl = project_face( i);
        auto face = model->face(i);
        vec3i coords[3];
        vec3d world[3];
        for(size_t j = 0; j < 3; ++j)
        {
            auto v = model->vertice(face[j]);
                  
            int  x = ( model->xshift() - v.x + W_SHIFT) * OBJ_SCALE
                                                + WIN_WIDTH / 2.0;
                                                    
            int  y = ( model->yshift() - v.y - H_SHIFT) * OBJ_SCALE
                                                + WIN_HEIGHT / 2.0;
                                            
            coords[j] = vec3i( x, y, (v.z * ZBUF_SCALE) );
            world[j] = v;
        }
        
        vec3d n = (world[2] - world[0]) ^ (world[1] - world[0]);
        n.normalize();
        light.normalize();
        double intensity =  n * light;
   
   
        if(intensity > 0.0)
        {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            red = green = blue = intensity * 255;
            uint8_t alpha = 0;
            SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
            draw_triangle( coords[0], coords[1], coords[2]);
        }
    }

    return;
}







void RTR::Window::render_z_buffer()
{
    zbuf_clear();
    vec3d light(.0, .0, -1.0);
  
    for(size_t i = 0; i < model->nfaces(); ++i)
    {
        auto face = model->face(i);
        vec3i coords[3];
        vec3d world[3];
        for(size_t j = 0; j < 3; ++j)
        {
            auto v = model->vertice(face[j]);
            int  x = ( model->xshift() - v.x + W_SHIFT) * OBJ_SCALE
                                                + WIN_WIDTH / 2.0;
                                                    
            int  y = ( model->yshift() - v.y - H_SHIFT) * OBJ_SCALE
                                                + WIN_HEIGHT / 2.0;
            coords[j] = vec3i( x, y, static_cast<int>(v.z * ZBUF_SCALE) );
            world[j] = v;
        }
    
        vec3d n = (world[2] - world[0]) ^ (world[1] - world[0]);
        n.normalize();
        
        double intensity = n * light;
        if( intensity > 0.0)
        {
            uint8_t red;
            uint8_t green;
            uint8_t blue;
            red = green = blue = intensity * 255;
            uint8_t alpha = 255u;
      
            SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);

            draw_triangle( coords[0], coords[1], coords[2]);
        }
    }
  
    for (int y = 0; y < WIN_HEIGHT; y++)   
        for (int x = 0; x < WIN_WIDTH; x++)
        {
            int color = 255 * (1 - ZBUF_SHARPNESS) * zbuf_max /
            (zbuf[x + y * WIN_WIDTH] - ZBUF_SHARPNESS * zbuf_max);
                 
            SDL_SetRenderDrawColor(renderer, color, color, color, 255);
            SDL_RenderDrawPoint(renderer, x, y);
        }

    return;
}














void RTR::Window::render_texture()
{

    zbuf_clear();
    vec3d light(-1.0, .0, -1.0);


    for(size_t i = 0; i < model->nfaces(); ++i)
    {
        auto face = model->face(i);
        vec3i coords[3];
        vec3d world[3];
        for(size_t j = 0; j < 3; ++j)
        {
            auto v = model->vertice(face[j]);
            int  x = ( model->xshift() - v.x + W_SHIFT) * OBJ_SCALE
                                                + WIN_WIDTH / 2.0;
                                                    
            int  y = ( model->yshift() - v.y - H_SHIFT) * OBJ_SCALE
                                                + WIN_HEIGHT / 2.0;
            coords[j] = vec3i(x, y, (v.z* ZBUF_SCALE));
            world[j] = v;
        }
    
        vec3d n = (world[2] - world[0]) ^ (world[1] - world[0]);
    
        n.normalize();
        light.normalize();
        double intensity = n * light;
    
        if(intensity > 0.0)
        {
            intensity *= intensity;
            vec2i tv[3];
            for( size_t k = 0; k < 3; ++k)
            tv[k] = model->tv(i, k);
            draw_triangle(  coords[0],  coords[1],  coords[2],
                            tv[0],      tv[1],      tv[2], 
                            intensity);
        }
    }
    
    return;
}





void RTR::Window::render_lines()
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawPoint(renderer, WIN_WIDTH / 2, WIN_HEIGHT / 2);

    bool ok = true;
    int fib = 1, prev_fib = 0;
    int new_x = WIN_WIDTH / 2, new_y = WIN_HEIGHT / 2,
        prev_x = WIN_WIDTH / 2, prev_y = WIN_HEIGHT / 2;
    bool gold = (mode == GOLD);

    while(ok)
    {
        draw_line( prev_x, prev_y, new_x, new_y);

        if(gold)
        {
            fib += prev_fib;
            prev_fib = fib;
        }
        else
        {
            prev_fib = fib++;
        }
        prev_x = new_x;
        prev_y = new_y;
        static int i = 0;
        switch(i++)
        {
            case 0:
                new_x = prev_x + fib;
                new_y = prev_y - fib;
                break;
            case 1:
                new_x = prev_x + fib;
                new_y = prev_y + fib;
                break;
            case 2:
                new_x = prev_x - fib;
                new_y = prev_y + fib;
                break;
            case 3:
                new_x = prev_x - fib;
                new_y = prev_y - fib;
                break;
        }

        i %= 4;

        if (( (new_x < 0) || (new_x > WIN_WIDTH) ) 
                        || ( (new_y < 0) || (new_y > WIN_HEIGHT)))
        {
            if(new_x < 0)
            {
                new_y = new_y + new_x * (new_y - prev_y) 
                                        / (double) (new_x - prev_x);
                new_x = 0;
            }
            if(new_x > WIN_WIDTH)
            {
                new_y = new_y - (new_x - WIN_WIDTH) * 
                                (new_y - prev_y) /
                                (double) (new_x - prev_x);
                new_x = WIN_WIDTH;
            }
            
            if(new_y < 0)
            {
                new_x = new_x + new_y *
                                (new_x - prev_x) /
                                (double) (new_y - prev_y);
                new_y = 0;
            }
            if(new_y > WIN_HEIGHT)
            {
                new_x = new_x - (new_y - WIN_HEIGHT) *
                                (new_x - prev_x) /
                                (double) (new_y - prev_y);
                new_y = WIN_HEIGHT;
            }

            draw_line( prev_x, prev_y, new_x, new_y);

            ok = false;
        }
    }
    
    return;
}



void RTR::Window::render_triangles()
{
    vec3i v1(100, 400, -10);
    vec3i v2(700, 250, -1);
    vec3i v3(700, 550, -1);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    draw_triangle( v1, v2, v3);
    
    v1 = vec3i(300, 100, -5);
    v2 = vec3i(300, 700, -5);
    v3 = vec3i(525, 400, -1);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    draw_triangle( v1, v2, v3);
    
    v1 = vec3i(600, 50, -5);
    v2 = vec3i(600, 750, -5);
    v3 = vec3i(475, 400, -1);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    draw_triangle( v1, v2, v3);

    return;
}
//
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//MISC
//
void RTR::Window::zbuf_clear()
{
    assert( zbuf != nullptr);
    
    #ifdef USE_MEMSET
    memset( zbuf, -127, WIN_HEIGHT * WIN_WIDTH * sizeof(zbuf_depth_t));
    #else
    for(int i = 0; i < WIN_HEIGHT; ++i)
        for(int j = 0; j < WIN_WIDTH; ++j)
               zbuf[j + i * WIN_WIDTH] = 
                        std::numeric_limits<zbuf_depth_t>::min();
    #endif     
    return;
}



void RTR::Window::clear_screen()
{
    if( SDL_SetRenderDrawColor(renderer, R_BGR,
                                         G_BGR,
                                         B_BGR,
                                         A_BGR) < 0 )
    throw sdl_error();
             
    if ( SDL_RenderClear(renderer) < 0)
         throw sdl_error();

}
//
//
///////////////////////////////////////////////////////////////////////////
