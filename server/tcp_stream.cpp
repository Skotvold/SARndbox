//
//  tcp_stream.cpp
//  tcp_ip_cpp
//
//  Created by Eskil André Skotvold on 08/09/2016.
//
//

#include <arpa/inet.h>
#include "tcp_stream.hpp"

tcp_stream::tcp_stream()
{

}

tcp_stream::tcp_stream(int socketDescriptor, struct sockaddr_in* address) : m_socketDescriptor(socketDescriptor)
{
    char ip[50];
    inet_ntop(PF_INET, reinterpret_cast<struct in_addr* >(&(address->sin_addr.s_addr)), ip, sizeof(ip)-1);
    m_peerIP = ip;
    m_peerPort = ntohs(address->sin_port);
}

tcp_stream::~tcp_stream()
{
    close(m_socketDescriptor);
}


std::string tcp_stream::getIP()
{
    return m_peerIP;
}

int tcp_stream::getPort()
{
    return m_peerPort;
}

ssize_t tcp_stream::send(const char* buffer, size_t len)
{
    return write(m_socketDescriptor , buffer, len);
}


ssize_t tcp_stream::receive(char *buffer, size_t len)
{
    return read(m_socketDescriptor , buffer, len);
}
