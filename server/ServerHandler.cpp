#include "ServerHandler.hpp"
#include <chrono>
#include <thread>
#include <vector>
#include <cstring>
#include <algorithm>
#include <time.h>
using namespace std;

SARB::ServerHandler::ServerHandler(int port)
    : m_threadRunning(true),
      stream(nullptr),
      acceptor(nullptr),
      receivedCommand(""),
      m_port(port),
      m_textureManager(nullptr)

{
    this->m_textureManager = new SARB::TextureManager();
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
        while (this->m_threadRunning)
        {
            // Server accept the client
            stream = acceptor->accept();

            auto packageSize = 0;
            auto packageCommand = 0;
           
            // if server receive header.
            // read header and execute dependent on command.
            while((readHeader(packageSize, packageCommand)) == true)
            {
                
                execPackage(stream, packageSize, packageCommand);

                // Reset the size and command
                // Safety measure
                packageSize = 0;
                packageCommand = 0;

            }
        }
    }

    else
    {
        std::cerr << "[Port is blocked]\n"
        << "Could not start the server. You could try another port"  << std::endl;
    }

    this->~ServerHandler();

}

// Execute the server packages
bool SARB::ServerHandler::execPackage(tcp_stream* stream, int packageSize, int packageCommand){
    if(packageCommand == SARB_READ_HEIGHTMAP)
    {
        std::unique_lock<std::mutex> guard(heightMapMutex);
        if(!sendHeightMap(heightMap))
            return false;
        guard.unlock();
        std::cout << "heightmap sent!\n";
        return true;
    }

    // echo back if command not found
    else if(packageCommand == SARB_READ_ECHO)
    {
        if(!readEcho(stream, packageSize))
            return false;
        
        if(!sendHeader(stream, receivedCommand.size(), SARB_WRITE_ECHO))
            return false;
        
        if(!sendEcho(receivedCommand,receivedCommand.size()))
            return false;

        receivedCommand.erase();
        return true;
    }

    else if(packageCommand == SARB_READ_POSITION)
    {
        if(!readPosition(packageSize))
            return false;

          // erase the command,
          std::unique_lock<std::mutex> lock(textureManagerMutex);
          std::cout << "received position: " << this->m_textureManager->getX() 
          << " " << this->m_textureManager->getY()<<"\n";
          lock.unlock();

        return true;
    }

    else if(packageCommand == SARB_READ_NOTHING)
    {
        return true;
    }

    
    // echo back if command not found
    else
    {
       return true;
    }

    return true;
}

// function that start the server
// server is running it the function 
// run server by itself

void SARB::ServerHandler::startServer()
{
    if(!m_threadRunning)
    {
        m_threadRunning = true;
    }

    this->m_thread = std::thread(&ServerHandler::runServer,this);

}

// Stop the thread server is running on
// This will make the server call serverhandler destructior
// Force quit of the server
void SARB::ServerHandler::stopServer()
{
    m_threadRunning = false;
}

// Detach the server in its own thread
void SARB::ServerHandler::detachServer()
{
    m_thread.detach();
}

// Get the state of the server is running
// Or if the thread is not running
bool SARB::ServerHandler::getThreadRunning()
{
    return m_threadRunning;
}

// Destroy the ServerHandler class
SARB::ServerHandler::~ServerHandler()
{
    std::cout << "\ndestructor serverHandler()\n";

    if(this->m_textureManager)
    {
        delete this->m_textureManager;
        m_textureManager = nullptr;
    }

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
            std::cout << "[Error]: please restart server" << std::endl;
            return false;
        }

        else if(byteReceived == 0)
        {
            std::cout << "[Client Disconnect] - Server lost connection to the client" << std::endl;
            return false;
        }

        pbuf += byteReceived;
        buflen -= byteReceived;
    }

    return true;

}


// Read a package
bool SARB::ServerHandler::readEcho(tcp_stream* stream, int totalSizeOfPackage)
{
    std::vector<char> vec;
    const int bufferSize = 100;
    
    while(totalSizeOfPackage > 0)
    {
        char buffer[bufferSize];

        int sizeToRead = std::min<int>(totalSizeOfPackage, bufferSize);
        if(!readData(stream, buffer, sizeToRead))
        {
            return false;
        }
        
         int offset = 0;

        do
        {
            for(int i = offset; i < sizeToRead-offset; i++)
                {
                    vec.push_back(buffer[i]);
                }

                offset+=vec.size();

            } while (offset < sizeToRead);

        totalSizeOfPackage -= sizeToRead;
    }
    

    for(auto i : vec)
    {
        receivedCommand += i;
    }
    return true;
}

// Send a package with string command
bool SARB::ServerHandler::sendEcho(std::string message, int totalSizeOfpackage)
{
    const int storageBufferSize = 100;
    if(totalSizeOfpackage > 0)
    {
        char buffer[storageBufferSize];
        int offset = 0;
        while(totalSizeOfpackage > 0)
        {
            size_t bufferSize = std::min(totalSizeOfpackage, static_cast<int>(sizeof(buffer)));
            
            // pack the package
            for(size_t i = 0; i < bufferSize; i++)
            {
                buffer[i] = message[offset];
                offset++;
            }


            if(!sendData(stream, buffer, bufferSize))
            {
                return false;
            }

            totalSizeOfpackage -= bufferSize;
        }
    }
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
            std::cout << "[ERROR] Disconnection between Server and Client\n";
            return false;
        }

        pbuf += byteReceived;
        buflen -= byteReceived;
    }

    return true;
}



// Send the heightmap to the client in strings
bool SARB::ServerHandler::sendHeightMap(std::vector<std::vector<float>> pheightMap)
{  
    // Calculate the number of threads the pc can use
    // gives better performance on wireless network connection 
    std::vector<std::thread> threads(std::thread::hardware_concurrency());
    int itemsPerThread  = pheightMap.size() / threads.size();

    // atomic total size of characters of the heightmap.
    this->mStringSizeOfHeightMap = 0;

    // storageBuffer for the heightmap in strings
    this->mHeightMapInStrings.resize(640); 

    // Start the threads to iterate each area
    // of the heightmap and convert them to strings 
    // increment the atomic size of the heightmap also.
    for(size_t i = 0; i < threads.size(); i++)
    {
        auto iterBegin = i*itemsPerThread;
        auto iterEnd = (i+1)*itemsPerThread;
        threads[i] = std::thread(&ServerHandler::calculateHeightMapSizeParallell,this, pheightMap, iterBegin, iterEnd);
    }

    // Join the threads
    for(auto& t : threads )
    {
        t.join();
    }
    
    // send the header with the size
    auto packageSize = this->mStringSizeOfHeightMap.load();
    if(!sendHeader(stream, packageSize, SARB_WRITE_HEIGHTMAP))
    {
        return false;
    }
    
    // Send the heightmap
    if(packageSize > 0)
    {
        for(size_t i  = 0; i < mHeightMapInStrings.size(); i++)
        {
            char buffer[mHeightMapInStrings[i].size()];
            strcpy(buffer,mHeightMapInStrings[i].c_str());
            if(!sendData(stream, buffer, mHeightMapInStrings[i].size()))
                return false;
        }
    }

    this->mHeightMapInStrings.clear();
    return true;
}

// Convert the serverhandler to string
std::string SARB::ServerHandler::convertVectToStr(std::vector<float> vect, size_t &size){
    std::stringstream oss;
    std::copy(vect.begin(), vect.end(),std::ostream_iterator<int>(oss, " "));
    size = oss.str().size();
    return oss.str();
}

// calculates the heightmap size in strings and also convert from float array to strings
// strings representing the integers
void SARB::ServerHandler::calculateHeightMapSizeParallell(std::vector<std::vector<float>> vect, size_t iterBegin, size_t iterEnd)
{
    size_t accum = 0;
    size_t rowSize = 0;
    for(size_t i = iterBegin; i < iterEnd; i++)
    {
        this->mHeightMapInStrings[i] = convertVectToStr(vect[i], rowSize);
        accum += rowSize;
    }

    this->mStringSizeOfHeightMap+=accum;
}



 bool SARB::ServerHandler::readHeader(int& sizeOfPackage, int& command)
 {

     char header[15];

     header[sizeof(header)-1] = '\0';
     if(!readData(stream, header,14))
     {
         return false;
     }

     std::cout << header << std::endl;
     std::istringstream iss(header);
     char temp;
     iss >> sizeOfPackage >> temp >> command;
     return true;
 }


bool SARB::ServerHandler::sendHeader(tcp_stream *stream, int sizeOfPackage, int command)
{
    // Pre format for header
    std::string header = "000000000";
    std::stringstream ss;

    //convert the size to string
    ss << sizeOfPackage;
    std::string buffer = ss.str();
    header = updateHeaderString(header, buffer);
    std::stringstream().swap(ss);
    buffer.clear();

    ss << command;
    buffer = ss.str();

    std::string commandHeader = "0000";
    header = header +"|"+ updateHeaderString(commandHeader, buffer);
    char* arr = &header[0];
    return this->sendData(stream, arr, header.size());
}

std::string SARB::ServerHandler::updateHeaderString(std::string baseString, std::string numberString)
{
    auto start = baseString.size()-1;
    auto end = start - numberString.size();
    int offset = numberString.size()-1;

    for(auto i = start; i > end; i--)
    {
        baseString[i] = numberString[offset];
        offset--;
    }

    return baseString;
}



std::vector<std::vector<float>> SARB::ServerHandler::getHeightMap()
{
    return heightMap;
}

void SARB::ServerHandler::setHeightMap(std::vector<std::vector<float>> heightMap)
{
    this->heightMap = heightMap;
}


bool SARB::ServerHandler::readPosition(int sizeOfPackage)
{
    std::vector<char> vec;
    if(sizeOfPackage > 0)
    {
        char buffer[10];
     
        while(sizeOfPackage > 0)
        {
            size_t sizeToRead = std::min<int>(sizeOfPackage, sizeof(buffer));
            if(!readData(stream, buffer, sizeToRead))
            {
                return false;
            }

             size_t offset = 0;

            do
            {
                for(size_t i = offset; i < (sizeToRead-offset); i++)
                {
                    vec.emplace_back(buffer[i]);
                }

                offset+=vec.size();

            } while (offset < sizeToRead);

            sizeOfPackage -= sizeToRead;
        }
    }
    
    string temp = "";
    for(auto i : vec)
    {
        temp += i;
    }
        

    std::istringstream ss(temp);
    int x,y;  
    ss >> x >> y;
    if(this->m_textureManager != nullptr)
    {
       std::unique_lock<std::mutex> lock(textureManagerMutex);
       m_textureManager->setXY(x,y);
       lock.unlock();
    }
        
    return true;
}
