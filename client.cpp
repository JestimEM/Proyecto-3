//Chat Client
#include "clientnetwork.hpp"

int main(int argc, char * argv[]){
     ClientNetwork client_network;
     client_network.Connect("localHost", 5000);
     client_network.Run();
     return 0;
}
