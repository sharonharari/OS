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
const int MAX_PACKET_SIZE = 516, MAX_DATA_SIZE = 512;
const uint16_t WRQ_OPCODE = (uint16_t)2, DATA_OPCODE = (uint16_t)3, ACK_OPCODE = (uint16_t)4, ERROR_OPCODE = (uint16_t)5;
const size_t ACK_SIZE = (size_t)4;


void create_ack(char(&ackBuffer)[4], uint16_t new_blocknum = (uint16_t)0);

class Data {
private:
	uint16_t block_number;
	char data[MAX_DATA_SIZE];
	size_t data_size;
public:
	Data();
	Data(char raw_packet[MAX_PACKET_SIZE], size_t raw_size);
	~Data();
	uint16_t get_block_number() const;
	char* get_data() const;
	size_t get_data_size() const;
};
#endif
