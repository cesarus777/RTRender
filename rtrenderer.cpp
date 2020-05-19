#include "rtrenderer.h"


// Basic methods:
///////////////////////////////////////////////////////////////////////////
//  Constructor/Destructor:
//
    RTR::Window::Window(int argc, char** argv, char* filename)
    : model( filename)
    {
        argv_parse2( argc, argv);
        
        int ret = SDL_CreateWindowAndRenderer(  WIN_WIDTH,  WIN_HEIGHT, 0,
                                                &window, &renderer);
        if (ret < 0) throw sdl_error();
        
        clear_screen();
        
        SDL_RenderPresent(renderer);

        pool = new std::thread[ N_MACHINES];

        zbuf = new zbuf_depth_t[WIN_WIDTH * WIN_HEIGHT];
            zbuf_clear();

        return;
    }




    RTR::Window::~Window()
    {
        delete [] zbuf;
        delete [] pool;

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
    char* RTR::Window::argv_parse1( int argc, char** argv)
    {
        int i = 1;
        while (i < argc)
        {
            if ((argv[i][0] == '-') and (argv[i][1] == 'o') )
            {
                if( (i + 1) >= argc)
                        show_usage();
                        
                return argv[i + 1];
            }              

            i++;
        }

        return nullptr; // end of non-void function;
    }



    void RTR::Window::argv_parse2( int argc, char** argv)
    {
        bool flag0 = false;
        bool flag1 = false;
        bool flag2 = false;

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
            return;

        if ( !flag0 and flag1 and !flag2)
        {
            mode = RAST;
            return;
        }
        
        if ( flag0 and !flag1 and !flag2)
            return;

        show_usage();
        return; // end of non-void function;
    }




    void RTR::Window::show_usage()
    {
        std::cout << usage_info;
        throw bad_input();
    }
//
//
///////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////
// Supported window modes:
//

    void RTR::Window::do_task()
    {
        switch( mode)
        {
            case N_RM_RST: 
            case WIREFRAME:
            case ZBUF:
            case RAST:
            case RAND:
            case TEXTURE:   
                            dynamic_display();
                            break;
                            
                            
            case TRIANGLE:  
            case GOLD:      
            case NOT_GOLD:  static_display();
                            break;


            default:        throw bad_mode();
        }
        
        return;
    }
    
    
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
    
    
    

    void RTR::Window::dynamic_display()
    {
        SDL_Event event;

        draw_target( mode);
        
        mode_t savedMode = mode;

        while ( SDL_WaitEvent( &event))
        {
            switch(event.type)
            {
                case SDL_QUIT : return;

                case SDL_KEYDOWN : switch( event.key.keysym.scancode)
                {
                    /* Translational motion */
                    case SDL_SCANCODE_W :
                                    Y_SPEED = -Y_SHIFT_SPEED_DEFAULT;
                                    break;
                
                    case SDL_SCANCODE_A :
                                    X_SPEED = -X_SHIFT_SPEED_DEFAULT;
                                    break;

                    case SDL_SCANCODE_S :
                                    Y_SPEED =  Y_SHIFT_SPEED_DEFAULT;
                                    break;

                    case SDL_SCANCODE_D :
                                    X_SPEED =  X_SHIFT_SPEED_DEFAULT;
                                    break;

                    case SDL_SCANCODE_I :
                                    Z_SPEED =  Z_SHIFT_SPEED_DEFAULT;
                                    break;

                    case SDL_SCANCODE_O :
                                    Z_SPEED =  -Z_SHIFT_SPEED_DEFAULT;
                                    break;
                                   
                                   
                                   
                    /* Rotation: */          
                    case SDL_SCANCODE_5 :
                                    orientation = 
                                        orientation * X_ROT_SPEED;
                                    break;
                                    
                    case SDL_SCANCODE_6 :
                                    orientation = 
                                        orientation * 
                                            X_ROT_SPEED.get_reverse();
                                    break;
                                    
                    case SDL_SCANCODE_1 :
                                    orientation = 
                                        orientation * Y_ROT_SPEED;
                                    break;
                                    
                    case SDL_SCANCODE_2 :
                                    orientation = 
                                        orientation * 
                                            Y_ROT_SPEED.get_reverse();
                                    break;
                                    
                    case SDL_SCANCODE_4 :
                                    orientation = 
                                        orientation * Z_ROT_SPEED;
                                    break;
                                    
                    case SDL_SCANCODE_3 :
                                    orientation = 
                                        orientation * 
                                            Z_ROT_SPEED.get_reverse();
                                    break;
                                    
                                    
                                    
                                    
                    /* Rendering mode selection: */            
                    case SDL_SCANCODE_Z : 
                                    if (mode != ZBUF)
                                    {
                                        savedMode   = mode;
                                        mode        = ZBUF;
                                    }
                                    
                                    else mode = savedMode;
                                    break;
                                    
                    case SDL_SCANCODE_X :
                                    if (mode != WIREFRAME)
                                    {
                                        savedMode   = mode;
                                        mode        = WIREFRAME;
                                    }
                                    
                                    else mode = savedMode;
                                    break;
                                    
                    
                    case SDL_SCANCODE_C :
                                    if (mode != RAST)
                                    {
                                        savedMode   = mode;
                                        mode        = RAST;
                                    }
                                    
                                    else mode = savedMode;
                                    break;
                                    
                    
                    case SDL_SCANCODE_V :
                                    if (mode != TEXTURE)
                                    {
                                        savedMode   = mode;
                                        mode        = TEXTURE;
                                    }
                                    else mode = savedMode;
                                    break;
                               
                    case SDL_SCANCODE_B:
                                    if (mode != RAND)
                                    {
                                        savedMode   = mode;
                                        mode        = RAND;
                                    }
                                    else mode = savedMode;
                                    break;

                    case SDL_SCANCODE_N:
                                    if (mode != N_RM_RST)
                                    {
                                        savedMode   = mode;
                                        mode        = N_RM_RST;
                                    }
                                    else mode = savedMode;
                                    break;
                    default : break;
                }
                break;


                case SDL_KEYUP :
                    W_SHIFT += X_SPEED;
                    H_SHIFT += Y_SPEED;
                    D_SHIFT += Z_SPEED;
                    
                    
                    draw_target( mode);
                    switch( event.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_W:
                                        if (Y_SPEED < 0) Y_SPEED = 0;
                                        break;
                                        
                        case SDL_SCANCODE_A:
                                        if (X_SPEED < 0) X_SPEED = 0;
                                        break;
                                        
                        case SDL_SCANCODE_S:
                                        if (Y_SPEED > 0) Y_SPEED = 0;
                                        break;
                        

                        case SDL_SCANCODE_D:
                                        if (X_SPEED > 0) X_SPEED = 0;
                                        break;
                                        
                        case SDL_SCANCODE_I:
                                        if (Z_SPEED > 0) Z_SPEED = 0;
                                        break;

                        case SDL_SCANCODE_O:
                                        if (Z_SPEED < 0) Z_SPEED = 0;
                                        break;
                                         
                                    

                        default : break;
                    }
                    break;

                default : break;
            }






        }

        throw sdl_error();
    }
//
//
///////////////////////////////////////////////////////////////////////////



//  Rendering stuff:
///////////////////////////////////////////////////////////////////////////
//  Switch:
//
    void RTR::Window::draw_target(mode_t m)
    {
        clear_screen();
        switch(m)
        {
            case N_RM_RST: 
            case WIREFRAME:
            case ZBUF:
            case RAST:
            case RAND:
            case TEXTURE:   
                            render_mode_threaded();
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


// Object display mode handler
// Supports all the modes
void RTR::Window::render_mode_threaded()
{

    zbuf_clear();
    vec3d light(-1.0, .0, -1.0);
    light.normalize();

    int r = 0;
    uint8_t red     = 0;
    uint8_t green   = 0;
    uint8_t blue    = 0;
    uint8_t alpha   = 0;

    size_t nfaces = model.nfaces();
    auto retval = new tuple_triangleI_double_bool[N_MACHINES];

    for (size_t i = 0; i < model.nfaces(); i += N_MACHINES)
    {
        // Starting threads:
        for (size_t j = 0; j < N_MACHINES; j++)
        {
            if ((i + j) < nfaces)
                pool[j] = std::thread( &RTR::Window::project_face,
                                        this,
                                        std::ref( retval),
                                        j,
                                        i + j,
                                        light);
        }



        // Picking the result:
        // (faces should be rendered in a single thread
        //  because back-end may fail in another way)
        for (size_t j = 0; j < N_MACHINES; j++)
        {
            if (pool[j].joinable())
            {
                pool[j].join();

                const triangleI&    tr          = std::get<0>( retval[j]);
                double              intensity   = std::get<1>( retval[j]);
                bool                isOnScreen  = std::get<2>( retval[j]);

                intensity *= intensity;
                if ( isOnScreen ) try
                {
                    switch( mode)
                    {
                        case TEXTURE :
                            if (intensity >= 0)
                            {
                                vec2i tv[3];
                                for( size_t k = 0; k < 3; ++k)
                                    tv[k] = model.tv(i + j, k);

                                draw_triangle( tr[0], tr[1], tr[2],
                                               tv[0], tv[1], tv[2],
                                               intensity);
                            }
                            break;



                        case ZBUF :
                                SDL_SetRenderDrawColor( renderer,
                                                        0, 0,
                                                        0, 0);
                                draw_triangle(  tr[0],  tr[1],  tr[2]); // filling zbuf;
                                break;
                                
                                

                        case RAST :
                            if (intensity >= 0)
                            {
                                red = green = blue = alpha = intensity * 255u;
                                SDL_SetRenderDrawColor( renderer,
                                                        red, green,
                                                        blue, alpha);
                                draw_triangle(  tr[0],  tr[1],  tr[2]);
                            }
                            break;
                            
                            
                            
                        case RAND : 
                            {
                                r       = std::rand();
                                red     = r % 256;
                                green   = (r >> 8)  % 256;
                                blue    = (r >> 16) % 256;
                                alpha   = (r >> 24) % 256;
                                SDL_SetRenderDrawColor( renderer,
                                                        red, green,
                                                        blue, alpha);
                                draw_triangle(  tr[0],  tr[1],  tr[2]);
                            }
                            break;
                            
                            
                            
                        case WIREFRAME :
                            SDL_SetRenderDrawColor( renderer, 255u, 255u, 255u, 255u);
                            draw_line( tr[0].x, tr[0].y, tr[1].x, tr[1].y);
                            draw_line( tr[0].x, tr[0].y, tr[2].x, tr[2].y);
                            draw_line( tr[2].x, tr[2].y, tr[1].x, tr[1].y);
                            break;
                            
                            
                            
                        case N_RM_RST :
                            if (intensity >= 0)
                            {
                                red = green = blue = alpha = intensity * 255u;
                                SDL_SetRenderDrawColor( renderer,
                                                        red, green,
                                                        blue, alpha);
                                draw_triangle( vec2i(tr[0].x, tr[0].y), 
                                            vec2i(tr[1].x, tr[1].y),
                                            vec2i(tr[2].x, tr[2].y));
                            }
                             
                             break;
                      default : break;
                    }
                }

                catch( std::exception& e)
                {
                    for ( ; j < N_MACHINES; j++)
                        if (pool[j].joinable()) pool[j].join();
                    throw e;
                }

                catch(...)
                {
                    for ( ; j < N_MACHINES; j++)
                        if (pool[j].joinable()) pool[j].join();

                    throw std::runtime_error(
                                    "draw_triangle() failed");
                }
            }
            
        }

    }

    if ( mode == ZBUF)
        display_zbuf();

    delete [] retval;
    return;
}


  //  std::cout<<"z_"<<z<<std::endl;



// Thread routine:
// using macro because of the need of founding xmin, xmax, ...
#define project_vertice(/* vec3d world[j] */)\
{               \
    /* Rotate: */                                                        \
    orientation.rotate( world[j]);\
                                                                        \
    /* Shift: */                                                \
    world[j].x = model.xshift() - world[j].x + W_SHIFT;         \
    world[j].y = model.yshift() - world[j].y + H_SHIFT;         \
    world[j].z = model.zshift() - world[j].z + D_SHIFT;         \
                                                                    \
    /* Perspective: */                                              \
    double div = PERSPECTIVE_FOCUS * world[j].z + 1;                \
    if (div < 0.1)                                                  \
        div = 0.1;                                                  \
                                                                    \
    world[j].x /= div;                                              \
    world[j].y /= div;                                              \
                                                                    \
    x = ( world[j].x) * OBJ_SCALE                                   \
                                                + WIN_WIDTH / 2.0;  \
                                                                    \
    y = ( world[j].y) * OBJ_SCALE                                   \
                                                + WIN_HEIGHT / 2.0; \
                                                                    \
    double tempz =  (  world[j].z)  * ZBUF_SCALE;                   \
    if ( tempz >= std::numeric_limits<zbuf_depth_t>::max() )        \
        z = std::numeric_limits<zbuf_depth_t>::max();               \
                                                                    \
    if ( tempz <= std::numeric_limits<zbuf_depth_t>::min() )        \
        z = std::numeric_limits<zbuf_depth_t>::min();               \
                                                                    \
    else                                                            \
        z = tempz;                                                  \
                                                                    \
}
 


// (supports parallelization)
void RTR::Window::project_face( tuple_triangleI_double_bool*& info,
                                size_t infoIDX,
                                size_t i,
                                const vec3d& light)
{

    auto face =  model.face(i);

    triangleI projection;

    bool    isOnScreen  = true;
    int     xmin        = WIN_WIDTH;
    int     xmax        = 0;
    int     ymin        = WIN_HEIGHT;
    int     ymax        = 0;


    vec3d world[3];

    for(size_t j = 0; j < 3; ++j)
    {
        world[j] = model.vertice(face[j]);
        int     x, y, z;

        project_vertice();       // "fills" x, y, z;

        if (xmin > x) xmin = x;
        if (xmax < x) xmax = x;
        if (ymin > y) ymin = y;
        if (ymax < y) ymax = y;

        projection[j] = vec3i( x, y, z);
    }

    vec3d n = (world[2] - world[0]) ^ (world[1] - world[0]);
    n.normalize();
    double intensity = n * light;

    assert( intensity <= 1);


    if ( (xmin > WIN_WIDTH) or (xmax < 0))
        isOnScreen = false;

    if ( (ymin > WIN_HEIGHT) or (ymax < 0))
        isOnScreen = false;

    info[ infoIDX] = std::make_tuple( projection, intensity, isOnScreen);

    return;

}

#undef project_vertice
//
//
///////////////////////////////////////////////////////////////////////////






///////////////////////////////////////////////////////////////////////////
// Render samples:
//
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
// MISC
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


void RTR::Window::display_zbuf()
{
    if (zbuf != nullptr )
    {
        long long max_distance  = zbuf_max - zbuf_min;
        for (int y = 0; y < WIN_HEIGHT; y++)
            for (int x = 0; x < WIN_WIDTH; x++)
            {

                int color;
                switch( zbuf[x + y * WIN_WIDTH])
                {
                    case std::numeric_limits<zbuf_depth_t>::min() : 
                        color = 0;
                        break;
                        
                    case std::numeric_limits<zbuf_depth_t>::max() :
                        color = 255;
                        break;
                        
                    default :
                    {
                        long long distance = ( ((long long)zbuf[x + y * WIN_WIDTH]) - zbuf_min);
                        double k = 255 / (double) max_distance;

                        color =  distance * k;
                        break;
                    }
                }
                             
            
                assert(color <= 255);

                SDL_SetRenderDrawColor( renderer, color, color, color, color);
                SDL_RenderDrawPoint( renderer, x, y);
            }
    }
    
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
