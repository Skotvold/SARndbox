//
//  tcpAcceptor.cpp
//  tcp_ip_cpp
//
//  Created by Eskil André Skotvold on 08/09/2016.
//
//


#include <stdio.h>
#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include "tcpAcceptor.hpp"


TCPAcceptor::TCPAcceptor(int port, const char* address)
    : m_listeningSocketDescriptor(0), m_port(port), m_addressInformation(address),
    m_listening(false)
{

}

TCPAcceptor::~TCPAcceptor()
{
    if (m_listeningSocketDescriptor > 0)
    {
        close(m_listeningSocketDescriptor);
    }
}

int TCPAcceptor::start()
{

    if(m_listening == true)
    {
        return 0;
    }

    m_listeningSocketDescriptor = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = PF_INET;
    address.sin_port = htons(static_cast<u_int16_t>(m_port));

    if(m_addressInformation.size() > 0)
    {
         inet_pton(PF_INET, m_addressInformation.c_str(), &(address.sin_addr));
    }

    else
    {
        address.sin_addr.s_addr = INADDR_ANY;
    }

    auto optval =1;

    setsockopt(m_listeningSocketDescriptor, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    int result = bind(m_listeningSocketDescriptor , reinterpret_cast<struct sockaddr*>(&address), sizeof(address));
    if(result != 0)
    {
        std::cerr << "bind() failed" << std::endl;
    }

    result = listen(m_listeningSocketDescriptor, 5);

    if (result != 0)
    {
      std::cerr << "listen() failed" << std::endl;
      return result;
    }

    m_listening = true;
    return result;
}





tcp_stream* TCPAcceptor::accept()
{
    if (m_listening == false)
    {
       return nullptr;
   }

   struct sockaddr_in address;
   socklen_t len = sizeof(address);
   memset(&address, 0, sizeof(address));
   int socketDescription = ::accept(m_listeningSocketDescriptor, reinterpret_cast<sockaddr*>(&address), &len);

   if (socketDescription < 0)
   {
       std::cerr<<"accept() failed"<< std::endl;
       return nullptr;
   }

   return new tcp_stream(socketDescription, &address);
}
