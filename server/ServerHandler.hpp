
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
#include "tcpAcceptor.hpp"
#include "packet.h"
#include "cmd.h"
#include "textureManager.hpp"
#include <mutex>


namespace SARB{
    enum RECEIVE_COMMANDS {SARB_READ_NOTHING = 0, SARB_READ_ECHO = 1, SARB_READ_HEIGHTMAP = 2};
    enum SEND_COMMANDS {SARB_WRITE_NOTHING = 0, SARB_WRITE_ECHO = 1, SARB_WRITE_HEIGHTMAP = 2};

    class ServerHandler
    {
    public:
    	ServerHandler();
        ServerHandler(int port);
        void startServer();
        void stopServer();
        void detachServer();
        bool getThreadRunning();
        std::string getCommand(){ return sendCommand; }
        void eraseCommand() { sendCommand.erase(); }
        ~ServerHandler();
        std::vector<std::vector<float>> getHeightMap();
        void setHeightMap(std::vector<std::vector<float>> heightMap);


    private:
        bool m_threadRunning;
        std::thread m_thread;
        tcp_stream* stream 	;
        TCPAcceptor* acceptor;
        std::string receivedCommand;
        int m_port;
        std::string sendCommand;
        std::vector<std::vector<float>> heightMap;
        std::unique_ptr<SARB::TextureManager> m_textureManager;
        std::mutex heightMapMutex;


        void runServer();
        bool readData(tcp_stream* stream, void* buf, int buflen);
        bool readSize(tcp_stream* stream, long* value);
        bool readPackages(tcp_stream* stream, int sizeOfPackage);
        bool sendSize(tcp_stream* stream, long value);
        bool sendPackage(std::string command, int sizeOfPackage);
        bool sendData(tcp_stream* stream, void* buf, int buflen);
        bool sendHeightMap(std::vector<std::vector<float>> heightMap);
        bool execPackage(tcp_stream* stream, int sizeOfPackage, int packageCommand);
        bool readHeader(int& sizeOfPackage, int& packageCommand);
	    bool sendHeader(tcp_stream* stream, int sizeOfPackage, int packageCommand);
        std::string updateHeaderString(std::string baseString, std::string numberString);
        std::string convertVectToStr(size_t row, std::vector<std::vector<float>> vect, size_t &size);
        size_t calculateHeightMapSize(std::vector<std::vector<float>> vect);

    };
}
