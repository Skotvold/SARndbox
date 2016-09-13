#include "ServerHandler.hpp"


ServerHandler::ServerHandler()
{
	
}


void ServerHandler::runServer(int port)
{
	// Declare stram and acceptor
    bool running 			{true};
    tcp_stream* stream 		{nullptr};
    TCPAcceptor* acceptor 	{nullptr};
	
	acceptor = new TCPAcceptor(port);

	// if the server found a valid port
    if (acceptor->start() == 0)
    {

        while (running)
        {
            stream = acceptor->accept();
            if (stream != nullptr)
            {
                long len;
                char line[256];

                while ( (len = stream->receive(line, sizeof(line))) > 0)
                {
                    std::cout << "received bytes: " << len << std::endl;
                    line[len] = 0;
                    printf("received - %s\n", line);

                    std::string checkLine = line;

                    if(checkLine == "quitServer" || checkLine == "q")
                    {
                       std::string sendSTR {"server shut down"};
                       stream->send(sendSTR.c_str(), sendSTR.size());
                       running = false;
                       break;
                    }

                    if(checkLine == "HeightMap")
                    {
                        std::string sendSTR {"Heightmap_requested: here you go\n"};
                        stream->send(sendSTR.c_str(),sendSTR.size());
                    }

                     auto var = stream->send(line, static_cast<size_t>(len));
                     std::cout << "Sent bytes: " << var << std::endl;
                }

            }
        }
    }


    // There could be many reasons for this.
    // server can't listen to the port (blocked)
    else
    {
        std::cerr << "Could not start the server. You could try another port"  << std::endl;
    }



    // Delete the stream and accepto, if they exist.

    if(stream)
    {
        delete stream;
        stream = nullptr;
    }

    if(acceptor)
    {
        delete acceptor;
        acceptor = nullptr;
    }

    std::cout << "server shutDown\n";
}
