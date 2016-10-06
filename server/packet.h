//
//  packet.h
//
//  Created by Alexis Lecoq on 28/10/2016.
//
//
#pragma once

#include <stdlib.h>
#include <string>
#include <cstring>
#include <iostream>

typedef unsigned char BYTE;

enum PACKET_SIZE {BYTES_MAX = 21000,BYTES_16384 = 16384,BYTES_8192 = 8192, BYTES_1024 = 1024};

class Packet {

private:
    int size;
    int pcktSize;
    char* data;

public:

     Packet();
     Packet(int size, char* data);
     ~Packet();

        int getSize() const;
        char* getData();
        void setSize(int size);
        int getPcktSize() const;
        void setPcktSize(int size);
        void setData(char* data);
        void addData(char* data, int size);
};

