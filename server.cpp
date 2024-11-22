// Chat Server
#include "servernetwork.hpp"

int main(int argc, char *argv[])
{
     ServerNetwork server_network(5000);
     server_network.Run();
     return 0;
}
