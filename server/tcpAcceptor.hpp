//
//  tcpAcceptorInterface.hpp
//  tcp_ip_cpp
//
//  Created by Eskil André Skotvold on 08/09/2016.
//
//

#pragma once
#include <string>
#include <netinet/in.h>
#include "tcp_stream.hpp"

class TCPAcceptor
{
public:
    TCPAcceptor(int port, const char* address="");
    ~TCPAcceptor();

    int start();
    tcp_stream* accept();

private:
    int m_listeningSocketDescriptor;
    int m_port;
    std::string m_addressInformation;
    bool m_listening;

};
