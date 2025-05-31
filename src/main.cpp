#include "../include/RedisServer.h"
#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char* argv[]){

    int port = 6379; //default port

    if(argc >= 2) port = std::stoi(argv[1]);//checking if the user wants to specific port
                                            //if not then the default port will be used 
    RedisServer server(port);
   
    // Background persistence: dump the database every 300 seconds. 5 * 60 Save database
    std::thread persistanceThread([](){
        while (true){
            std::this_thread::sleep_for(std::chrono::seconds(300));
            // dupmp the database 
        }
    });
    persistanceThread.detach();

    server.run();
    return 0;
}