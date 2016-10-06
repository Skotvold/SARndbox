//
//  packet.cpp
//
//  Created by Alexis Lecoq on 28/10/2016.
//
//

#include "packet.h"



//Constructors
Packet::Packet(){		
}
	
Packet::Packet(int size, char* data){
    this->size = size;
    this->data = data;
    this->pcktSize = 21000;
}

Packet::~Packet()
{
    //free(data);
}

//Getters and Setters
int Packet::getSize() const{
	return size;
}

char* Packet::getData(){
    return data;
}

void Packet::setSize(int size){
	this->size = size;
}

void Packet::setData(char* data){
	this->data = data;
}

int Packet::getPcktSize() const{
    return pcktSize;
}

void Packet::setPcktSize(int size){
        this->pcktSize = size;
}

void Packet::addData(char* data,int size){
    strcat(this->data,data);
    this->size += size;
}
