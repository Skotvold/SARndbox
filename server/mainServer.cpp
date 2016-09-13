//
//  mainServer.cpp
//  tcp_ip_cpp
//
//  Created by Eskil André Skotvold on 08/09/2016.
//
//


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <thread>
#include <chrono>
#include "tcpAcceptor.hpp"






void serverThreadFunc(int argc, char** argv)
{

    // Server has started
    auto start = std::chrono::high_resolution_clock::now();


    // Declare stram and acceptor
    bool running {true};
    tcp_stream* stream {nullptr};
    TCPAcceptor* acceptor {nullptr};


    // Set acceptor to port and name of server(ip)
    if (argc == 3)
    {
        acceptor = new TCPAcceptor(atoi(argv[1]), argv[2]);
    }

    else
    {
        acceptor = new TCPAcceptor(atoi(argv[1]));
    }



    // if the server found a valid port
    if (acceptor->start() == 0)
    {

        while (running)
        {
            stream = acceptor->accept();
            if (stream != nullptr)
            {
                long len;
                char line[256];

                while ( (len = stream->receive(line, sizeof(line))) > 0)
                {
                    std::cout << "received bytes: " << len << std::endl;
                    line[len] = 0;
                    printf("received - %s\n", line);

                    std::string checkLine = line;

                    if(checkLine == "quitServer" || checkLine == "q")
                    {
                       std::string sendSTR {"server shut down"};
                       stream->send(sendSTR.c_str(), sendSTR.size());
                       running = false;
                       break;
                    }

                    if(checkLine == "HeightMap")
                    {
                        std::string sendSTR {"Heightmap_requested: here you go\n"};
                        stream->send(sendSTR.c_str(),sendSTR.size());
                    }

                     auto var = stream->send(line, static_cast<size_t>(len));
                     std::cout << "Sent bytes: " << var << std::endl;
                }

            }
        }
    }


    // There could be many reasons for this.
    // server can't listen to the port (blocked)
    else
    {
        std::cerr << "Could not start the server" << std::endl;
    }



    // Delete the stream and accepto, if they exist.

    if(stream)
    {
        delete stream;
        stream = nullptr;
    }

    if(acceptor)
    {
        delete acceptor;
        acceptor = nullptr;
    }

    std::cout << "server shutDown\n";
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double,std::milli> serverTime = (end - start);
    std::cout << "Server was running: " << serverTime.count() << "msec\n";


}


int main(int argc, char** argv)
{

    if (argc < 2 || argc > 4)
    {
       printf("usage: server <port> [<ip>]\n");
       return 1;
    }


    std::thread serverThread{serverThreadFunc,argc,argv};


    // main neeed to do something too.
    for(auto count = 0; count < 5; count++)
    {

        std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(5));
        printf("main having fun\n");

    }

    serverThread.join();
    std::cout << "Server has joined the mainthread, and the application quitting\n";
    return 0;
}
