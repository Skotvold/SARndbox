#include "ServerHandler.hpp"



SARB::ServerHandler::ServerHandler(int port)
    : m_threadRunning(true),
      stream(nullptr),
      acceptor(nullptr),
      m_port(port)
{

}


void SARB::ServerHandler::runServer()
{
    std::cout << "server thread started\n";
    // Declare stram and acceptor

	
    acceptor = new TCPAcceptor(this->m_port);

	// if the server found a valid port
    if (acceptor->start() == 0)
    {

        while (this->m_threadRunning)
        {
            stream = acceptor->accept();
            if (stream != nullptr && m_threadRunning)
            {
                long len;
                char line[256];

                while ( (len = stream->receive(line, sizeof(line))) > 0)
                {
                    std::cout << "received bytes: " << len << std::endl;
                    line[len] = 0;
                    printf("received - %s\n", line);
                    std::string checkLine = line;


					if(checkLine == "file")
					{
						m_storeString = checkLine;
					}


                    if(checkLine == "quitServer" || checkLine == "q")
                    {
                        std::string sendSTR {"server shut down"};
                        auto var = stream->send(sendSTR.c_str(), sendSTR.size());
                        std::cout << "Sent bytes: " << var << std::endl;
                        this->m_threadRunning = false;
                    }

                   else if(checkLine == "HeightMap")
                   {
                        std::string sendSTR {"Heightmap_requested: here you go\n"};
                        auto var = stream->send(sendSTR.c_str(),sendSTR.size());
                        std::cout << "Sent bytes: " << var << std::endl;

                   }

                   else
                   {
                        auto var = stream->send(line, static_cast<size_t>(len));
                        std::cout << "Sent bytes: " << var << std::endl;

                   }

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

    this->~ServerHandler();

}


void SARB::ServerHandler::startServer()
{
    if(!m_threadRunning)
    {
        m_threadRunning = true;
    }

    this->m_thread = std::thread(&ServerHandler::runServer,this);

}

void SARB::ServerHandler::stopServer()
{
    m_threadRunning = false;
}

void SARB::ServerHandler::detachServer()
{
    m_thread.detach();
}

bool SARB::ServerHandler::getThreadRunning()
{
    return m_threadRunning;
}


SARB::ServerHandler::~ServerHandler()
{
    std::cout << "\ndestructor serverHandler()\n";

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

    if(m_threadRunning)
    {
        m_threadRunning = false;
    }

    std::cout << "SERVERHANDLER Destructed\n";
}


