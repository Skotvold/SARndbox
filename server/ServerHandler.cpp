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
      sendCommand(""),
      m_textureManager(new TextureManager)

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
        while (this->m_threadRunning)
        {
            // Server accept the client
            stream = acceptor->accept();

            // If the server get a size of package
            // Server always need size before the actual package.

            auto packageSize = 0;
            auto packageCommand = 0;
            while((readHeader(packageSize, packageCommand)) == true)
            {
                
                execPackage(stream, packageSize, packageCommand);
                packageSize = 0;
                packageCommand = 0;
            }
        }
    }


    // There could be many reasons for this.
    // server can't listen to the port (blocked)
    else
    {
        std::cerr << "[Port is blocked]\n"
        << "Could not start the server. You could try another port"  << std::endl;
    }

    this->~ServerHandler();

}

bool SARB::ServerHandler::execPackage(tcp_stream* stream, int packageSize, int packageCommand){

    // MERGING
    if(packageCommand == SARB_READ_HEIGHTMAP)
    {
        std::lock_guard<std::mutex> guard(heightMapMutex);
        sendCommand = "sendFile";
        //Calculating execution time
        float time;
        clock_t t1, t2;
        //start count
        t1 = clock();

        if(sendHeightMap(heightMap))
            std::cout << "Completed sending heightMap\n";
        else
            std::cout << "Failed to send heightMap\n";
        //stop count
        t2 = clock();
        //convert to float
        time = (float)(t2-t1)/CLOCKS_PER_SEC;
        cout << "time = "<< time << "\n";
    }

    // echo back if command not found
    else if(packageCommand == SARB_READ_ECHO)
    {
        readPackages(stream, packageSize);
        sendHeader(stream, receivedCommand.size(), SARB_WRITE_ECHO);
        sendPackage(receivedCommand,receivedCommand.size());
    }

    else if(packageCommand == SARB_READ_NOTHING)
    {
        // Read just for flush
        readPackages(stream, packageSize);
    }


    
    // echo back if command not found
    else
    {
       readPackages(stream, packageSize); // read for flush
    }


	sendCommand = receivedCommand;
    // erase the command,
    receivedCommand.erase();

    // We are done with the loop or package
    std::cout << "\n\n";
    return true;

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
            std::cout << "[Client Disconnect] - Server lost connection to the client" << std::endl;
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
    int storageBufferSize = 6;
    totalSizeOfbytes = totalSizeOfPackage;

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
bool SARB::ServerHandler::sendPackage(std::string message, int totalSizeOfpackage)
{
    auto numberOfpackages = 0;
    int storageBufferSize = 2;
    if(totalSizeOfpackage > 0)
    {
        char buffer[storageBufferSize];
        int offset = 0;
        do
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

            numberOfpackages++;
            totalSizeOfpackage -= bufferSize;
        } while (totalSizeOfpackage > 0);
    }
    std::cout << "number of package(s): " << numberOfpackages << std::endl;
    std::cout << "Storage Buffer was: " << storageBufferSize << " bytes"<< std::endl;
    std::cout << "Total send bytes: " << message.size() << std::endl;
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


// Send a file
bool SARB::ServerHandler::sendHeightMap(std::vector<std::vector<float>> heightMap)
{

    size_t vecSize = heightMap.size();
   
    float time;
    clock_t t1, t2;
    //start count
    t1 = clock();
    size_t stringSize = calculateHeightMapSize(heightMap);
    //stop count
    t2 = clock();
    time = (float)(t2-t1)/CLOCKS_PER_SEC;
    cout << "time = "<< time << "\n";
    

    if(!sendHeader(stream, stringSize, SARB_WRITE_HEIGHTMAP))
    {
        return false;
    }

    float timei = 0;
    clock_t t1i, t2i;

    int number_of_packages = 0;
    if(vecSize > 0)
    {
         //stop count
        t1 = clock();
        int start = 0;
        do
        {
            size_t rowSize;
            std::string row = convertVectToStr(start,heightMap,rowSize);
            char buffer[rowSize];
            strcpy(buffer,row.c_str());
            if(rowSize < 1)
                return false;

            t1i = clock();
            if(!sendData(stream, buffer, rowSize))
                return false;
            t2i = clock();
            timei += (float)(t2i-t1i)/CLOCKS_PER_SEC;
            //std::cout << "sendata: " << timei << "\n";
            number_of_packages++;
            vecSize = vecSize-1;
            start = start+1;
        } while (vecSize > 0);
        t2 = clock();
        
        time = (float)(t2-t1)/CLOCKS_PER_SEC;
        cout << "time = "<< time << "\n";
        cout << "timei = " << timei << "\n";
    }

    std::cout << "packages: "  << number_of_packages << std::endl;
    return true;
}

std::string SARB::ServerHandler::convertVectToStr(size_t row,std::vector<std::vector<float>> vect, size_t &size){
    std::stringstream oss;


  // Convert all but the last element to avoid a trailing ","
    std::copy(vect[row].begin(), vect[row].end()-1,std::ostream_iterator<float>(oss, " "));

    // Now add the last element with no delimiter
    oss << vect[row].back() << "\n" ;

    size = oss.str().size();

    return oss.str();
}


size_t SARB::ServerHandler::calculateHeightMapSize(std::vector<std::vector<float>> vect){
    size_t vecSize = vect.size();
    size_t size = 0;
    size_t start = 0;
    do{
        size_t rowSize;
        std::string row = convertVectToStr(start,vect,rowSize);
        size += rowSize;
        start++;
        vecSize--;
    }while (vecSize > 0);

    return size;
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
     std::cout << sizeOfPackage << command<<" "<<"\n";
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

