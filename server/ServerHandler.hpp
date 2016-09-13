
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
#include "tcpAcceptor.hpp"


class ServerHandler
{
public:
	ServerHandler();
	void runServer(int port);
private:
};
