
//
//  ServerHandler.hpp
//  tcp_ip_cpp
//
//  Created by Eskil André Skotvold on 08/09/2016.
//
//

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <string>
#include <thread>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <mutex>
#include <atomic>

#include "tcpAcceptor.hpp"
#include "packet.h"
#include "cmd.h"
#include "textureManager.hpp"


namespace SARB
{
    enum READ_COMMANDS 
    {
        SARB_READ_NOTHING = 0, 
        SARB_READ_ECHO = 1, 
        SARB_READ_HEIGHTMAP = 2,
        SARB_READ_POSITION = 3,
        SARB_READ_IMAGE_PNG = 4 
    };

    enum SEND_COMMANDS 
    {
        SARB_WRITE_NOTHING = 0, 
        SARB_WRITE_ECHO = 1, 
        SARB_WRITE_HEIGHTMAP = 2
    };

    
    class ServerHandler
    {
    
    public:
    	ServerHandler();
        ServerHandler(int port);
        void startServer();
        void stopServer();
        void detachServer();
        bool getThreadRunning();
        std::vector<std::vector<float>> getHeightMap();
        void setHeightMap(std::vector<std::vector<float>> heightMap);
        ~ServerHandler();



    private:
        // Server specific members
        bool m_threadRunning;
        std::thread m_thread;
        tcp_stream* stream 	;
        TCPAcceptor* acceptor;
        std::string receivedCommand;
        int m_port;

        // Heightmap and the texture manager
        std::vector<std::vector<float>> heightMap;
        SARB::TextureManager* m_textureManager;
        std::mutex textureManagerMutex;


        // Threading of the sending of heightmap
        std::mutex heightMapMutex;
	    std::atomic<int> mStringSizeOfHeightMap;
        std::vector<std::string> mHeightMapInStrings;



        void runServer();
        bool readData(tcp_stream* stream, void* buf, int buflen);
        bool readEcho(tcp_stream* stream, int sizeOfPackage);
        bool sendEcho(std::string command, int sizeOfPackage);
        bool sendData(tcp_stream* stream, void* buf, int buflen);
        bool sendHeightMap(std::vector<std::vector<float>> heightMap);
        bool execPackage(tcp_stream* stream, int sizeOfPackage, int packageCommand);
        bool readHeader(int& sizeOfPackage, int& packageCommand);
	    bool sendHeader(tcp_stream* stream, int sizeOfPackage, int packageCommand);
        bool readPosition(int sizeOfPackage);
        std::string updateHeaderString(std::string baseString, std::string numberString);
        std::string convertVectToStr(std::vector<float> vect, size_t &size);
        void calculateHeightMapSizeParallell(std::vector<std::vector<float>> vect, size_t iterBegin, size_t iterEnd);


    };
}
