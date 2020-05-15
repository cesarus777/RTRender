#include "rtrenderer.h"

//namespace RTR::Renderer
//{

    void RTR::Renderer::argv_parse( int argc, char** argv)
    {
        bool flag0 = false;
        bool flag1 = false;
        bool flag2 = false;

        int i = 1;
        while (i < argc)
        {
            if (argv[i][0] == '-') switch(argv[i][1])  
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
                    model = new obj_model( argv[i + 1]);
                    flag1 = true;
                    i += 2;
                    break;
                        
                case 'm' :  
                    if( (i + 1 >= argc) or (flag2 == true))
                        show_usage();
                    if( strcmp( argv[i + 1], "wire") == 0)
                        mode = WIREFRAME;
                            
                    else if( strcmp( argv[i + 1], "rasterize") == 0)
                        mode = RAST;
                        
                    else if( strcmp( argv[i + 1], "rand") == 0)
                        mode = RAND;
                         
                    else if( strcmp( argv[i + 1], "remove") == 0)
                        mode = REMOV;
                          
                    else if( strcmp( argv[i + 1], "color") == 0)
                        mode = COLOR;
                            
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
            
        if ( flag0 and !flag1 and !flag2)
            return;
            
        show_usage();
    }
    
    void RTR::Renderer::show_usage()
    {
        std::cout << "Usage: [-s <FIGURE>] [-o <FILE>] [-m <MODE>]\n";
        throw bad_input();
    }
    
    
    
    void RTR::Renderer::static_display()
    {
        draw_target();
        
        SDL_Event event;
        for(;;)
        {
            if (SDL_WaitEvent( &event) == 0) throw sdl_error();
                   
            if ((event.type == SDL_QUIT) ||
                                    (event.type == SDL_KEYDOWN))
                return;
        }
    }

    void RTR::Renderer::draw_target()
    {
        //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        
        switch(mode)
        {
            case WIREFRAME: render_wireframe();
                            break;
                
            case RAND:      render_randcolor();
                            break;
                            
            case RAST:      render_rasterisator();
                            break;
                            
            case REMOV:     render_removal_rasterisator();
                            break;
                            
            case COLOR:     render_color_removal();
                            break;
        
            case TRIANGLE:  render_triangles();
                            break;
                            
            case GOLD:      render_lines();
                            break;
                            
            case NOT_GOLD:  render_lines();
                            break;

            default:        throw bad_mode();
        }
        
        SDL_RenderPresent(renderer);

        return;
  }






void RTR::Renderer::render_wireframe()
{
  for(size_t i = 0; i < model->nfaces(); ++i)
  {
    auto face = model->face(i);
    for(size_t j = 0; j < 3; ++j)
    {
      vec3d v1 = model->vertice(face[j]);
      vec3d v2 = model->vertice(face[(j + 1) % 3]);
      int x1 = (v1.x + MAX) * WIDTH / (2.0 * MAX);
      int y1 = (MAX - v1.y) * HEIGHT / (2.0 * MAX);
      int x2 = (v2.x + MAX) * WIDTH / (2.0 * MAX);
      int y2 = (MAX - v2.y) * HEIGHT / (2.0 * MAX);
      draw_line(renderer, x1, y1, x2, y2);
    }
  }
}

void RTR::Renderer::render_randcolor()
{
  for(size_t i = 0; i < model->nfaces(); ++i)
  {
    auto face = model->face(i);
    vec2i coords[3];
    for(size_t j = 0; j < 3; ++j)
    {
      auto v = model->vertice(face[j]);
      double x = (v.x + MAX) * WIDTH / (2 * MAX) + 0.5;
      double y = (MAX - v.y) * HEIGHT / (2 * MAX) + 0.5;
      coords[j] = vec2i(x, y);
    }
    int r = std::rand();
    char red = r % 256;
    char green = (r >> 8)  % 256;
    char blue = (r >> 16) % 256;
    char alpha = (r >> 24) % 256;
    SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
    draw_triangle(renderer, coords[0], coords[1], coords[2]);
  }
}

void RTR::Renderer::render_rasterisator()
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
      int x = (v.x + MAX) * WIDTH / (2 * MAX);
      int y = (MAX - v.y) * HEIGHT / (2 * MAX);
      coords[j] = vec2i(x, y);
      world[j] = v;
    }
    vec3d n = (world[2] - world[0]) ^ (world[1] - world[0]);
    n.normalize();
    double intensity = n * light;
    if(intensity > 0.0)
    {
      char red;
      char green;
      char blue;
      red = green = blue = intensity * 255;
      char alpha = static_cast<char>(0);
      SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
      draw_triangle(renderer, coords[0], coords[1], coords[2]);
    }
  }
}

void RTR::Renderer::render_removal_rasterisator()
{
  vec3d light(.0, .0, -1.0);
  const size_t w = WIDTH;
  const size_t h = HEIGHT;
  double *zbuf = new double[w * h];
  for(size_t i = 0; i < h; ++i)
    for(size_t j = 0; j < w; ++j)
      zbuf[j + i * w] = -std::numeric_limits<double>::max();

  for(size_t i = 0; i < model->nfaces(); ++i)
  {
    auto face = model->face(i);
    vec3i coords[3];
    vec3d world[3];
    for(size_t j = 0; j < 3; ++j)
    {
      auto v = model->vertice(face[j]);
      int x = (v.x + MAX) * WIDTH / (2 * MAX);
      int y = (MAX - v.y) * HEIGHT / (2 * MAX);
      coords[j] = vec3i(x, y, v.z);
      world[j] = v;
    }
    vec3d n = (world[2] - world[0]) ^ (world[1] - world[0]);
    n.normalize();
    double intensity = n * light;
    if(intensity > 0.0)
    {
      char red;
      char green;
      char blue;
      red = green = blue = intensity * 255;
      char alpha = static_cast<char>(0);
      SDL_SetRenderDrawColor(renderer, red, green, blue, alpha);
      draw_triangle(renderer, coords[0], coords[1], coords[2], zbuf, w, h);
    }
  }
}

void RTR::Renderer::render_color_removal()
{
  vec3d light(0.0, 0.0, -1.0);
  const size_t w = WIDTH;
  const size_t h = HEIGHT;
  double *zbuf = new double[w * h];
  for(size_t i = 0; i < h; ++i)
    for(size_t j = 0; j < w; ++j)
      zbuf[j + i * w] = -std::numeric_limits<double>::max();

  for(size_t i = 0; i < model->nfaces(); ++i)
  {
    auto face = model->face(i);
    vec3i coords[3];
    vec3d world[3];
    for(size_t j = 0; j < 3; ++j)
    {
      auto v = model->vertice(face[j]);
      int x = (model->max() + model->xshift() + v.x)
        * WIDTH / (2 * model->max());
      int y = (model->max() + model->yshift() - v.y)
        * HEIGHT / (2 * model->max());
      coords[j] = vec3i(x, y, v.z);
      world[j] = v;
    }
    vec3d n = (world[2] - world[0]) ^ (world[1] - world[0]);
    n.normalize();
    double intensity = n * light;
    if(intensity > 0.0)
    {
      vec2i tv[3];
      for(size_t k = 0; k < 3; ++k)
        tv[k] = model->tv(i, k);
      draw_triangle(renderer, model, coords[0], coords[1], coords[2],
          tv[0], tv[1], tv[2], intensity, zbuf, w, h);
    }
  }
}





void RTR::Renderer::render_lines()
{
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderDrawPoint(renderer, WIDTH / 2, HEIGHT / 2);

  bool ok = true;
  int fib = 1, prev_fib = 0;
  int new_x = WIDTH / 2, new_y = HEIGHT / 2,
      prev_x = WIDTH / 2, prev_y = HEIGHT / 2;
  bool gold = (mode == GOLD);

  while(ok)
  {
    draw_line(renderer, prev_x, prev_y, new_x, new_y);

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

    if(((new_x < 0) || (new_x > WIDTH)) || ((new_y < 0) || (new_y > HEIGHT)))
    {
      if(new_x < 0)
      {
        new_y = new_y + new_x * (new_y - prev_y) / (double) (new_x - prev_x);
        new_x = 0;
      }
      if(new_x > WIDTH)
      {
        new_y = new_y - (new_x - WIDTH) * (new_y - prev_y) / (double) (new_x - prev_x);
        new_x = WIDTH;
      }
      if(new_y < 0)
      {
        new_x = new_x + new_y * (new_x - prev_x) / (double) (new_y - prev_y);
        new_y = 0;
      }
      if(new_y > HEIGHT)
      {
        new_x = new_x - (new_y - HEIGHT) * (new_x - prev_x) / (double) (new_y - prev_y);
        new_y = HEIGHT;
      }

      draw_line(renderer, prev_x, prev_y, new_x, new_y);

      ok = false;
    }
  }

}

void RTR::Renderer::render_triangles()
{
  const size_t w = WIDTH;
  const size_t h = HEIGHT;
  double *zbuf = new double[w * h];
  for(size_t i = 0; i < w * h; ++i)
    zbuf[i] = -std::numeric_limits<double>::max();

  vec3i v1(100, 400, -10);
  vec3i v2(700, 250, -1);
  vec3i v3(700, 550, -1);
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  draw_triangle(renderer, v1, v2, v3, zbuf, w, h);
  v1 = vec3i(300, 100, -5);
  v2 = vec3i(300, 700, -5);
  v3 = vec3i(525, 400, -1);
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  draw_triangle(renderer, v1, v2, v3, zbuf, w, h);
  v1 = vec3i(600, 50, -5);
  v2 = vec3i(600, 750, -5);
  v3 = vec3i(475, 400, -1);
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  draw_triangle(renderer, v1, v2, v3, zbuf, w, h);
  delete [] zbuf;
}


    RTR::Renderer::Renderer(int argc, char** argv)
    {
        argv_parse( argc, argv);
        int r;        
        r = SDL_CreateWindowAndRenderer( 
                                            WIDTH,
                                            HEIGHT,
                                            0,
                                            &window,
                                            &renderer     
                                        );
                                                
        if (r < 0) throw sdl_error();

        if( SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255) < 0 )
            throw sdl_error();
             
        if ( SDL_RenderClear(renderer) < 0)
            throw sdl_error();
        
        SDL_RenderPresent(renderer);
        return;
    }
    

    RTR::Renderer::~Renderer()
    {
        delete model;
        
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        
        return;
    }


