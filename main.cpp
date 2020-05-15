#include "rtrenderer.h"



int main(int argc, char **argv)
{

    try
    {
        RTR::Renderer r( argc, argv);
        
        r.static_display();
        
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    
  



  return EXIT_SUCCESS;
}

