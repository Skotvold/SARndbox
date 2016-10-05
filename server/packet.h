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
<<<<<<< HEAD
    
	Packet();
	Packet(BYTE cmd, BYTE size, BYTE* data);
	Packet(char *buffer);	
	~Packet();

=======


     Packet();
     Packet(BYTE cmd, BYTE size, BYTE* data);
     Packet(char *buffer);
     ~Packet();
	
>>>>>>> eb9d60e2526876ab61ddd9e9d26363b9ae767e2d
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
