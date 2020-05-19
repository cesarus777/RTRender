//#include "primitives.hpp"
#include "rtrenderer.h"
void RTR::Window::draw_line( int x1, int y1, int x2, int y2)
{
    #ifdef USING_SDL
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    #else
    if(x1 == x2)
    {
        if(y1 > y2)
            std::swap(y1, y2);
            
        for(int y = y1; y <= y2; ++y)
            SDL_RenderDrawPoint(renderer, x1, y);
        return;
    }

    bool transposed = (std::abs(y2 - y1) > std::abs(x2 - x1));
    if(transposed)
    {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }

    if(x2 < x1)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    int dx = x2 - x1;
    int dy = y2 - y1;
    int derror = std::abs(dy) * 2;
    int error = 0;
    int y = y1;
    for(int x = x1; x <= x2; ++x)
    {
        transposed
            ? SDL_RenderDrawPoint(renderer, y, x)
            : SDL_RenderDrawPoint(renderer, x, y);

        error += derror;
        if(error > dx)
        {
            y += (dy > 0) ? 1 : -1;
            error -= dx * 2;
        }
    }
    #endif
}




void RTR::Window::draw_line( vec2i v1, vec2i v2)
{
    draw_line( v1.x, v1.y, v2.x, v2.y);
}



// !!NO ZBUF!!
void RTR::Window::draw_triangle( vec2i v1, vec2i v2, vec2i v3)
{
    if((v1.y == v2.y) && (v1.y == v3.y)) return;

    if(v1.y > v2.y)
        std::swap(v1, v2);
    if(v1.y > v3.y)
        std::swap(v1, v3);
    if(v2.y > v3.y)
        std::swap(v2, v3);

    for(int y = v1.y; y < v3.y; ++y)
    {
        int x1 = v1.x + (y - v1.y) * (v3.x - v1.x) / (double) (v3.y - v1.y);
        int x2 = (y < v2.y)
            ? v1.x + (y - v1.y) * (v2.x - v1.x) / (double) (v2.y - v1.y)
            : v2.x + (y - v2.y) * (v3.x - v2.x) / (double) (v3.y - v2.y);
        draw_line( x1, y, x2, y);
    }
}





void RTR::Window::draw_triangle( vec3i v1, vec3i v2, vec3i v3)
{
    if(v1.y > v2.y)        std::swap(v1, v2);
    if(v1.y > v3.y)        std::swap(v1, v3);
    if(v2.y > v3.y)        std::swap(v2, v3);

    for( int y = v1.y; y < v3.y; ++y)
    {
        bool second = (y >= v2.y);
        double k1 = (y - v1.y) / (double) (v3.y - v1.y);
        double k2 = (second)
            ? (y - v2.y) / (double) (v3.y - v2.y)
            : (y - v1.y) / (double) (v2.y - v1.y);


        //  whole i.e. non-sliced side (v1v3)
        vec3d whole = (vec3d(v1) + vec3d(v3 - v1) * k1);
        
        // compound i.e. consisting of two vectors (v1v2 and v2v3)
        vec3d compound = second
            ? (vec3d(v2) + vec3d(v3 - v2) * k2)
            : (vec3d(v1) + vec3d(v2 - v1) * k2);

        if (whole.x > compound.x)
            std::swap(whole, compound);
            

        // needed for z buffer interpolation 
        double phi;
        if (compound.x - whole.x)
            phi = (compound.z - whole.z) /
                                (double) (compound.x - whole.x);

 
        for (int x = (int) whole.x; x <= (int) compound.x; ++x)
        {
            zbuf_depth_t z = static_cast<zbuf_depth_t>(
                                whole.z + phi * (x - whole.x));
                                
            size_t i = x + y * WIN_WIDTH;

            if ((0 < x) and (x < WIN_WIDTH) and (0 < y) and (y < WIN_HEIGHT))
            {
                if (zbuf_min > z) zbuf_min = z;
                if (zbuf_max < z) zbuf_max = z;
                if( zbuf[i] < z)  
                {
                    zbuf[i] = z;
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            }
        }
    }
}










// textures the triangle
void RTR::Window::draw_triangle(    vec3i v1, vec3i v2, vec3i v3,
                                    vec2i t1, vec2i t2, vec2i t3, 
                                    double intensity)
{
    if(v1.y > v2.y)
    {
        std::swap(v1, v2);
        std::swap(t1, t2);
    }
    if(v1.y > v3.y)
    {
        std::swap(v1, v3);
        std::swap(t1, t3);
    }
    if(v2.y > v3.y)
    {
        std::swap(v2, v3);
        std::swap(t2, t3);
    }


    for(int y = v1.y; y < v3.y; ++y)
    {
        bool second = (y >= v2.y);
        double k1 = (y - v1.y) / (double) (v3.y - v1.y);
        double k2 = (second)
            ? (y - v2.y) / (double) (v3.y - v2.y)
            : (y - v1.y) / (double) (v2.y - v1.y);

        vec3d whole = vec3d(v1) + vec3d(v3 - v1) * k1;
        vec3d compound = second
                                ? vec3d(v2) + vec3d(v3 - v2) * k2
                                : vec3d(v1) + vec3d(v2 - v1) * k2;


        vec2d t_whole = vec2d(t1) + vec2d(t3 - t1) * k1;
        vec2d t_compound = second
                                ? vec2d(t2) + vec2d(t3 - t2) * k2
                                : vec2d(t1) + vec2d(t2 - t1) * k2;

        if (whole.x > compound.x)
        {
            std::swap(whole, compound);
            std::swap(t_whole, t_compound);
        }



        double phi1, phi2, phi3;
        // avoiding deletion on zero after (double) cast
        if ((compound.x - whole.x) >= 1)
        {
                phi1 = (compound.z - whole.z) / 
                                (double) (compound.x - whole.x);
                            
                phi2 = (t_compound[0] - t_whole[0]) /
                                (double) (compound.x - whole.x);
                            
                phi3 = (t_compound[1] - t_whole[1]) /
                                (double) (compound.x - whole.x);
        }

        for(int x = whole.x; x <= compound.x; ++x)
        {
            zbuf_depth_t z = static_cast<zbuf_depth_t>(
                                whole.z + phi1 * (int) (x - whole.x));

            int t_1 = t_whole[0] + phi2 * (int) (x - whole.x);
            int t_2 = t_whole[1] + phi3 * (int) (x - whole.x);

            size_t i = x + y * WIN_WIDTH;

            if (i < static_cast<size_t>(WIN_WIDTH * WIN_HEIGHT))
            {
                if (zbuf_min > z) zbuf_min = z;
                if (zbuf_max < z) zbuf_max = z;
                if (zbuf[i]  < z)
                {
                    zbuf[i] = z;
                    
                    SDL_Color clr = model.tv_clr( t_1, t_2);

                    uint8_t r = clr.r * intensity;
                    uint8_t g = clr.g * intensity;
                    uint8_t b = clr.b * intensity;
                    uint8_t a = clr.a * intensity;

                    SDL_SetRenderDrawColor( renderer, r, g, b, a);
                        
                    SDL_RenderDrawPoint(renderer, x, y);


                }
            }
        }
    }
}











