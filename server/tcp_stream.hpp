//
//  tcp_stream.hpp
//  tcp_ip_cpp
//
//  Created by Eskil André Skotvold on 08/09/2016.
//
//
#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>

class tcp_stream {
public:
    ~tcp_stream();
    ssize_t send(const char* buffer, size_t len);
    ssize_t receive(char* buffer, size_t len);

    friend class TCPAcceptor;
   // friend class TCPConnector;

    std::string getIP();
    int getPort();

private:
    int m_socketDescriptor;
    int m_peerPort;
    std::string m_peerIP;

    tcp_stream();
    tcp_stream(int socketDescriptor, struct sockaddr_in* address);
    tcp_stream(const tcp_stream& stream);

};
