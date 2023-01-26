#ifndef ADT_H
#define ADT_H
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <string>
#include <netinet/in.h>
#include <netdb.h>
#include <climits>
#include <fstream>
#include <stdio.h>
#include <cstring>
#include <unistd.h>

struct ack_packet{
	uint16_t ack_opcode;
	uint16_t block_num;
}__attribute__((packed));

struct error_packet{
	uint16_t error_opcode;
	uint16_t error_code;
	char * error_message;
}__attribute__((packed));

struct data_packet{
	uint16_t data_opcode;
	uint16_t block_num;
	char data[512];
}__attribute__((packed));

const int MAX_PACKET_SIZE = 516, MAX_DATA_SIZE = 512;
const uint16_t WRQ_OPCODE = (uint16_t)2, DATA_OPCODE = (uint16_t)3, ACK_OPCODE = (uint16_t)4, ERROR_OPCODE = (uint16_t)5;
const size_t ACK_SIZE = (size_t)4;
const size_t ERROR_HEADER_SIZE = (size_t)4;
const std::string ERROR_7_MESSAGE = "Unknown user";
const std::string ERROR_6_MESSAGE = "File already exists";
const std::string ERROR_4_MESSAGE = "Unexpected packet";
const std::string ERROR_0_MESSAGE = "Bad block number";
const std::string ERROR_1_MESSAGE = "Abandoning file transmission";
error_packet create_error(uint16_t error_code, char* error_message);
ack_packet create_ack( uint16_t new_blocknum = (uint16_t)0);
void error_handling(int sock, sockaddr_in ClientAddr,  uint16_t error_code);

class Data {
public:
	uint16_t block_number;
	char data[MAX_DATA_SIZE];
	int data_size;
	Data();
	Data(char raw_packet[MAX_PACKET_SIZE], int raw_size, uint16_t block_num) ;
	~Data();
};


#endif
