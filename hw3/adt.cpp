#include "adt.h"


Data::Data(char raw_packet[MAX_PACKET_SIZE], int raw_size) {
	uint16_t block_number_network = ( raw_packet[3] << 8) + raw_packet[2];
	std::cout << "raw_packet[3]: " << std::hex << raw_packet[3] << " , raw_packet[2]: " << std::hex << raw_packet[2] << std::endl;
	std::cout << "block_number_network = " << block_number_network << std::endl;
	block_number = ntohs(block_number_network);
	std::cout << "block_number = " << block_number << std::endl;
	data_size = raw_size - (int)4;
	std::memcpy(data, raw_packet + 4, data_size);
}

Data::~Data() {
	
}

ack_packet create_ack( uint16_t new_blocknum) {
	struct ack_packet ack;
	ack.ack_opcode = htons(ACK_OPCODE);
	ack.block_num = htons(new_blocknum);
	return ack;
}

error_packet create_error(uint16_t error_code, char* error_message){
	struct error_packet error;
	error.error_opcode = htons(ERROR_OPCODE);
	error.error_code = htons(error_code);
	error.error_message = new char[strlen(error_message)+1];
	strcpy(error.error_message, error_message);
	return error;
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
	error_packet error_buffer;
	uint16_t error_code_valid;
	if (error_code == (uint16_t)1) {
		error_code_valid = 0;
	}
	else {
		error_code_valid = error_code;
	}
	error_buffer = create_error(error_code_valid, const_cast<char*>(error_message.c_str()));
	if (sendto(sock, (void*)&error_buffer, ERROR_HEADER_SIZE + error_message_size, 0, (struct sockaddr*)&ClientAddr, sizeof(ClientAddr)) != (ssize_t)(ERROR_HEADER_SIZE + error_message_size)) {
		//error("sendto() failed");
		//syscall
		std::perror("TTFTP_ERROR:");
		exit(1);
	}
}
