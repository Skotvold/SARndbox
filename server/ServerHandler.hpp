
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
#include <thread>
#include "tcpAcceptor.hpp"

namespace SARB{

    class ServerHandler
    {
    public:
        ServerHandler();
        void startServer();
        void stopServer();
        void detachServer();
        bool getThreadRunning();
        ~ServerHandler();

    private:
        bool m_threadRunning;
        std::thread m_thread;
        void runServer(int port);
        tcp_stream* stream 	;
        TCPAcceptor* acceptor;

    };
}
