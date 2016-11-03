
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

namespace SARB{
    enum RECEIVE_COMMANDS {SARB_NOTHING = 0, SARB_ECHO = 1, SARB_HEIGHTMAP = 2};

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


    private:
        bool m_threadRunning;
        std::thread m_thread;
        tcp_stream* stream 	;
        TCPAcceptor* acceptor;
        std::string receivedCommand;
        int m_port;
        std::string sendCommand;
        std::unique_ptr<SARB::TextureManager> m_textureManager;


        void runServer();
        bool readData(tcp_stream* stream, void* buf, int buflen);
        bool readSize(tcp_stream* stream, long* value);
        bool readPackages(tcp_stream* stream, int sizeOfPackage);
        bool sendSize(tcp_stream* stream, long value);
        bool sendPackage(std::string command, int sizeOfPackage);
        bool sendData(tcp_stream* stream, void* buf, int buflen);
        bool sendHeightMap(std::vector<std::vector<double>> heightMap);
        bool execPackage(tcp_stream* stream,long receivePackageSize);
        std::string convertVectToStr(int row, std::vector<std::vector<double>> vect, int &size);
        int calculateHeightMapSize(std::vector<std::vector<double>> vect);
        bool readHeader(int& sizeOfPackage, int& packageCommand);
	    bool sendHeader(tcp_stream* stream, int sizeOfPackage, int packageCommand);
        std::string updateHeaderString(std::string baseString, std::string numberString);
    };
}
