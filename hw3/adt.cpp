#include "adt.h"


Data::Data(char raw_packet[MAX_PACKET_SIZE], size_t raw_size) {
	char blocknum_network[2] = { buffer[2],buffer[3] };
	block_number = ntohs((uint16_t)blocknum_network);
	data_size = raw_size - 4;
	std::memcpy(data, raw_packet + 4, data_size);
}
uint16_t Data::get_block_number() const{
	return this->block_number;
}
char* Data::get_data() const{
	return this->data;
}
size_t Data::get_data_size() const {
	return this->data_size;
}
void create_ack(char (&ackBuffer)[4], uint16_t new_blocknum) {
	uint16_t opcode_net = htons(ACK_OPCODE);
	uint16_t blocknum_net = htons(new_blocknum);
	ackBuffer = { opcode_net,blocknum_net };
}

