//
//  tcpAcceptorInterface.hpp
//  tcp_ip_cpp
//
//  Created by Eskil André Skotvold on 08/09/2016.
//
//

#pragma once
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <vector>
#include "tcp_stream.hpp"


class TCPAcceptor
{
public:
    TCPAcceptor(int port, const char* address="");
    ~TCPAcceptor();

    int start();
    tcp_stream* accept();
    std::vector<std::string> getIP();


private:
    int m_listeningSocketDescriptor;
    int m_port;
    std::string m_addressInformation;
    bool m_listening;
    // find public ip
    std::string getPublicIp(char* cmd);


};
