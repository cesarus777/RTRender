#include "rtrenderer.h"



int main(int argc, char **argv)
{

    try
    {
 
        RTR::Window r( argc, argv);
//        std::cout << "2 " << std::endl;
        r.static_display();
        
  //      std::cout << " 3" << std::endl;
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

