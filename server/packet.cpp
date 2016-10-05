//
//  packet.cpp
//
//  Created by Alexis Lecoq on 28/10/2016.
//
//

#include "packet.h"

//Constructors
Packet::Packet(){
	this.cmd = 0xFF;		
}
	
Packet::Packet(BYTE cmd, BYTE size, BYTE* data){
	this.cmd = cmd;
	this.size = size;
	this.data = data;
	this.crc = calcCrc();
}

Packet::Packet(char* buffer){
	int i;
	cmd = buffer[0];
	size = buffer[1];
	data = malloc(size * sizeof(BYTE));
	for(i=0;i<size;i++){
		data[i] = buffer[2+i];
	}
	crc = buffer[2+size];
}

Packet::~Packet()
{
    free(data);
}

//Getters and Setters
BYTE Packet::getCmd(){
	return cmd;
}

BYTE Packet::getSize(){
	return size;
}

BYTE* Packet::getData(){
	return data;
}

BYTE Packet::getCrc(){
	return crc;
}

BYTE Packet::setCmd(BYTE cmd){
	this.cmd = cmd;
}

BYTE Packet::setSize(BYTE size){
	this.size = size;
}

BYTE* Packet::setData(BYTE* data){
	this.data = data;
}

BYTE Packet::setCrc(BYTE crc){
	this.crc = crc;
}

/*
 * Function who calculate the CRC of a packet
 * @return return the value of the CRC
 */
BYTE calcCrc(){
	int i;
	BYTE crc;
	
	crc = data ^ size;
	for(i=0;i<size;i++){
		crc ^= data[i];
	}
	return crc;
}

/*
 * Function who return if the CRC is correct
 * @return true if it's correct
 * @return false if it's not correct
 */
bool checkCrc(){
	return (crc == calcCrc()) ? true : false;
}

/*
 * Function who return the packet in a char*
 */
char* getPacket(){
	int i;
	char* packet = malloc((2+size+1) * sizeof(char));
	
	packet[0] = cmd;
	packet[1] = size;
	for(i=0;i<size;i++){
		packet[2+i] = data[i];
	}
	packet[2+size] = crc;	
	return packet;
}

std::ostream & operator<<(std::ostream & Str, Packet const & packet) { 
  int i;
  Str << packet.getCmd() << " " << packet.getSize() << " ";
  for(i=0;i<size;i++){
	  Str << packet.getData()[i] << " ";
  }
  Str << packet.getCrc();
  return Str;
}
