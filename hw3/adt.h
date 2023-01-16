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

const int MAX_PACKET_SIZE = 516, MAX_DATA_SIZE = 512;
const uint16_t WRQ_OPCODE = (uint16_t)2, DATA_OPCODE = (uint16_t)3, ACK_OPCODE = (uint16_t)4, ERROR_OPCODE = (uint16_t)5;
const size_t ACK_SIZE = (size_t)4;
const size_t ERROR_HEADER_SIZE = (size_t)4;
// includes \0
// const size_t ERROR_7_MESSAGE_SIZE = (size_t)13;
// const size_t ERROR_6_MESSAGE_SIZE = (size_t)20;
// const size_t ERROR_4_MESSAGE_SIZE = (size_t)18;
// const size_t ERROR_0_MESSAGE_BAD_SIZE = (size_t)17; // Bad block number
// const size_t ERROR_0_MESSAGE_COUNTER_SIZE = (size_t)29; // Error counter larger than maximum
const std::string ERROR_7_MESSAGE = "Unknown user";
const std::string ERROR_6_MESSAGE = "File already exists";
const std::string ERROR_4_MESSAGE = "Unexpected packet";
const std::string ERROR_0_MESSAGE = "Bad block number";
const std::string ERROR_1_MESSAGE = "Abandoning file transmission";
void create_error(char *errorBuffer, uint16_t error_code, char* error_message);
void create_ack(char(&ackBuffer)[4], uint16_t new_blocknum = (uint16_t)0);
void error_handling(int sock, sockaddr_in ClientAddr,  uint16_t error_code);

class Data {
public:
	uint16_t block_number;
	char data[MAX_DATA_SIZE];
	size_t data_size;
	Data();
	Data(char raw_packet[MAX_PACKET_SIZE], size_t raw_size);
	~Data();
};


#endif
