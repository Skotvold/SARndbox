//
//  cmd.h
//
//  Created by Alexis Lecoq on 28/10/2016.
//
//
#ifndef CMD_H
#define CMD_H

typedef unsigned char BYTE;

#define BYTE	CMD_QUIT							0x01
#define BYTE	CMD_GET_HEIGHTMAP					0x02
#define BYTE	CMD_ACK								0x03

#define BYTE	DATA_ACK							0x11
#define BYTE	DATA_WRONG_CRC						0x12
#define	BYTE	DATA_CMD_UNKNOWN					0x13

#endif