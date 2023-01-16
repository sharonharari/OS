#include "adt.h"


Data::Data(char raw_packet[MAX_PACKET_SIZE], size_t raw_size) {
	uint16_t block_number_network = ( raw_packet[2] << 8) + raw_packet[3];
	block_number = ntohs(block_number_network);
	// char blocknum_network[2] = { buffer[2],buffer[3] };
	// block_number = ntohs((uint16_t)blocknum_network);
	data_size = raw_size - 4;
	std::memcpy(data, raw_packet + 4, data_size);
}

Data::~Data() {
	
}

void create_ack(char (&ackBuffer)[4], uint16_t new_blocknum) {
	uint16_t opcode_net = htons(ACK_OPCODE);
	uint16_t blocknum_net = htons(new_blocknum);
	// ackBuffer = ( opcode_net << 8) + blocknum_net;
	memcpy(ackBuffer,&opcode_net,2);
	memcpy(ackBuffer+2,&blocknum_net,2);
	// ackBuffer = { opcode_net,blocknum_net };
}

void create_error(char *errorBuffer, uint16_t error_code, char* error_message){
	uint16_t opcode_net = htons(ERROR_OPCODE);
	uint16_t error_code_net = htons(error_code);
	memcpy(errorBuffer,&opcode_net,2);
	memcpy(errorBuffer+2,&error_code_net,2);
	memcpy(errorBuffer+4, error_message, strlen(error_message)+1);
}

void error_handling(int sock, sockaddr_in ClientAddr,  uint16_t error_code){
	std::string error_message;
	switch(error_code)
	{
		case (uint16_t)7 :  error_message = ERROR_7_MESSAGE; break;
		case (uint16_t)6 :  error_message = ERROR_6_MESSAGE; break;
		case (uint16_t)4 :  error_message = ERROR_4_MESSAGE; break;
		case (uint16_t)0 :  error_message = ERROR_0_MESSAGE; break;
		case (uint16_t)1 :  error_message = ERROR_1_MESSAGE; break;
	}
	size_t error_message_size = error_message.size() + 1;
	char * errorBuffer = new char[ERROR_HEADER_SIZE + error_message_size];
	uint16_t error_code_valid;
	if (error_code == (uint16_t)1) {
		error_code_valid = 0;
	}
	else {
		error_code_valid = error_code;
	}
	create_error(errorBuffer, error_code_valid, const_cast<char*>(error_message.c_str()));
	if (sendto(sock, (void*)errorBuffer, ERROR_HEADER_SIZE + error_message_size, 0, (struct sockaddr*)&ClientAddr, sizeof(ClientAddr)) != (ssize_t)(ERROR_HEADER_SIZE + error_message_size)) {
		//error("sendto() failed");
		//syscall
		std::perror("TTFTP_ERROR:");
		exit(1);
	}
	delete[] errorBuffer;
}
