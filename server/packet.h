//
//  packet.h
//
//  Created by Alexis Lecoq on 28/10/2016.
//
//
#pragma once

#include <stdlib.h>
#include <string>
#include <iostream>

typedef unsigned char BYTE;

class Packet {

private:
    BYTE cmd;
    BYTE size;
    BYTE* data;
	BYTE crc;

public:
    ~Packet();
    
	void Packet();
	void Packet(BYTE cmd, BYTE size, BYTE* data);
    void Packet(char *buffer);	
	
	BYTE getCmd();
	BYTE getSize();
	BYTE* getData();
	BYTE getCrc();
	BYTE setCmd(BYTE cmd);
	BYTE setSize(BYTE size);
	BYTE* setData(BYTE* data);
	BYTE setCrc(BYTE crc);
	
	BYTE calcCrc();
	bool checkCrc();
	char* getPacket();
	void toString();
	
};
std::ostream & operator<<(std::ostream & Str, Packet const & packet);
