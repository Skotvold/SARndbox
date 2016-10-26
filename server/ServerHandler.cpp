#include "ServerHandler.hpp"
#include <chrono>
#include <thread>
#include <vector>
#include <cstring>
#include <algorithm>

using namespace std;

SARB::ServerHandler::ServerHandler(int port)
    : m_threadRunning(true),
      stream(nullptr),
      acceptor(nullptr),
      receivedCommand(""),
      m_port(port),
      sendCommand(""	)
{

}


void SARB::ServerHandler::runServer()
{
    std::cout << "server thread started\n";
    // Declare stram and acceptor

	
    acceptor = new TCPAcceptor(this->m_port);

    // Print the public and local IP
	std::cout << "Port: " << this->m_port << std::endl
	<< "local IP: " << acceptor->getIP()[0] << "Public IP: " << acceptor->getIP()[1]
	<< acceptor->getIP()[2]<< std::endl;

	// if the server found a valid port
    if (acceptor->start() == 0)
    {
        // Total size of the package serve will receive
        long receivePackageSize;

        while (this->m_threadRunning)
        {
            // Server accept the client
            stream = acceptor->accept();

            // If the server get a size of package
            // Server always need size before the actual package.
            while(readSize(stream, &receivePackageSize))
            {
                execPackage(stream,receivePackageSize);
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

bool SARB::ServerHandler::execPackage(tcp_stream* stream,long receivePackageSize){
    // Read the package. Remember some strings are too big to be
    // Stored on the stack, and need to be added to a file.
    readPackages(stream, receivePackageSize);
    std::cout <<"Received Command: " << receivedCommand << std::endl;

    // Handle commands function for example
    if(receivedCommand == "sendFile")
    {
        // Send  a file ( sending size is built in the file function)
        std::cout << "Starting sending file\n";
        if(sendFile("test"))
            std::cout << "Completed sending a file\n";
        else
            std::cout << "Failed to send file\n";
 	}
    // echo back if command not found
    else
    {
        sendSize(stream, receivedCommand.size());
        sendPackage(receivedCommand);
    }
	sendCommand = receivedCommand;
    // erase the command,
    receivedCommand.erase();

    // We are done with the loop or package
    std::cout << "\n\n";

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

// Read data of all packages
bool SARB::ServerHandler::readData(tcp_stream* stream, void* buf, int buflen)
{
    char* pbuf = (char*) buf;

    while(buflen > 0)
    {
        auto byteReceived = stream->receive(pbuf,buflen);

        if(byteReceived == -1)
        {
            std::cout << "HANDLE ERRORS" << std::endl;
            return false;
        }

        else if(byteReceived == 0)
        {
            std::cout << "byte received 0" << std::endl;
            return false;
        }

        pbuf += byteReceived;
        buflen -= byteReceived;
    }

    return true;

}

// Read the the size of a package
bool SARB::ServerHandler::readSize(tcp_stream* stream, long* value)
{
    if(!readData(stream, value,sizeof(value)))
    {
        return false;
    }

    *value = ntohl(*value);
    return true;
}


// Read a package
bool SARB::ServerHandler::readPackages(tcp_stream* stream, int totalSizeOfPackage)
{
    std::vector<char> vec;
    auto totalSizeOfbytes = 0;
    auto packagesReceived = 0;
    totalSizeOfbytes = totalSizeOfPackage;
    int storageBufferSize = 6;
    if(totalSizeOfPackage > 0)
    {
        char buffer[storageBufferSize];
        do
        {
            int minNumber = std::min<int>(totalSizeOfPackage, sizeof(buffer));
            if(!readData(stream, buffer, minNumber))
            {
                return false;
            }

            packagesReceived++;
            int offset = 0;

            do
            {
                for(int i = offset; i < minNumber-offset; i++)
                {
                    vec.push_back(buffer[i]);
                }

                offset+=vec.size();

            } while (offset < minNumber);

            totalSizeOfPackage -= minNumber;

        }while(totalSizeOfPackage > 0);
    }


    for(auto& i : vec)
    {
        std::cout << i;
        receivedCommand+=i;
    }
    std::cout << "\nreceived: " << packagesReceived <<" package(s)"<< std::endl;
    std::cout << "Storage Buffer was: " << storageBufferSize << " bytes"<< std::endl;
    std::cout << "total bytes received: " << totalSizeOfbytes << std::endl;
    return true;
}

// Send the size of a package
bool SARB::ServerHandler::sendSize(tcp_stream* stream, long value)
{
    value = htonl(value);
    return this->sendData(stream,&value,sizeof(value));
}

// Send a package with string command
bool SARB::ServerHandler::sendPackage(std::string command)
{
    auto totalSizeOfpackage  = command.size(); // cheat for now // could be 10
    auto numberOfpackages = 0;
    int storageBufferSize = 2;
    if(totalSizeOfpackage > 0)
    {
        char buffer[storageBufferSize];
        int offset = 0;
        do
        {
            size_t bufferSize = std::min(totalSizeOfpackage, sizeof(buffer));
            // pack the package
            for(int i = 0; i < bufferSize; i++)
            {
                buffer[i] = command[offset];
                offset++;
            }


            if(!sendData(stream, buffer, bufferSize))
            {
                return false;
            }

            numberOfpackages++;
            totalSizeOfpackage -= bufferSize;
        } while (totalSizeOfpackage > 0);
    }
    std::cout << "number of package(s): " << numberOfpackages << std::endl;
    std::cout << "Storage Buffer was: " << storageBufferSize << " bytes"<< std::endl;
    std::cout << "Total send bytes: " << command.size() << std::endl;
    return true;
}

// Send the data of a package
bool SARB::ServerHandler::sendData(tcp_stream* stream, void* buf, int buflen)
{
    char* pbuf = (char*) buf;

    while(buflen > 0)
    {
        auto byteReceived = stream->send(pbuf,buflen);

        if(byteReceived == -1)
        {
            std::cout << "HANDLE ERRORS" << std::endl;
            return false;
        }

        pbuf += byteReceived;
        buflen -= byteReceived;
    }

    return true;
}


// Send a file
bool SARB::ServerHandler::sendFile(std::string path)
{

    FILE* f = fopen(path.c_str(), "rb");
    // Find the size of file(package)
    fseek(f,0,SEEK_END);
    long filesize = ftell(f);
    rewind(f);

    if(filesize == EOF)
    {
        return false;
    }

    if(!sendSize(stream, filesize))
    {
        return false;
    }

    std::cout << "Filesize sending: " << filesize << std::endl;
    int number_of_packages = 0;
    int storageBufferSize = 2048;
    if(filesize > 0)
    {
        char buffer[storageBufferSize];
        do
        {
            size_t num = std::min<size_t>(filesize, sizeof(buffer));
            num = fread(buffer,1,num,f);

            if(num < 1)
                return false;

            if(!sendData(stream, buffer, num))
                return false;

            number_of_packages++;
            filesize -= num;

        } while (filesize > 0);
    }

    std::cout << "packages: "  << number_of_packages << std::endl;
    std::cout << "Storage Buffer was: " << storageBufferSize << " bytes"<< std::endl;
    fclose(f);
    return true;
}
