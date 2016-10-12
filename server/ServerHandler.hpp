
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
#include "tcpAcceptor.hpp"

namespace SARB{

    class ServerHandler
    {
    public:
        ServerHandler(int port = 9999);
        void startServer();
        void stopServer();
        void detachServer();
        bool getThreadRunning();
        std::string getString()
        {
        	return m_storeString;
        }
        ~ServerHandler();

    private:

        // Private member variables
        bool m_threadRunning;
        tcp_stream* stream 	;
        TCPAcceptor* acceptor;
        std::thread m_thread;
        int m_port;
        std::string m_storeString;

        // Private member functions
        void runServer();

    };
}
