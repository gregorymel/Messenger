#include <Server.hpp>

int main( int argc, char** argv )
{
    Server server( 8001 );
    server.start();
    server.service().run();

    return 0;
}
