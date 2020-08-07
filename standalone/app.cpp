#include "rtrenderer.hpp"



int main(int argc, char **argv)
{
    try
    {
        char* filename = RTR::Window::argv_parse1( argc, argv);
        RTR::Window r( argc, argv, filename);
        r.do_task();
    }
    
    catch(std::exception& e)
    {
        std::cout<< "\nException catched in main:\t" << e.what() << std::endl;
    }
    catch(...)
    {
        std::cout << "unknown exception" << std::endl;
    }
 
    return EXIT_SUCCESS;
}

