#include "rtrenderer.h"



int main(int argc, char **argv)
{

    try
    {
        RTR::Window r( argc, argv);
        r.xy_move_display();   
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    catch(...)
    {
        std::cout << "unknown exception" << std::endl;
    }
  



  return EXIT_SUCCESS;
}

