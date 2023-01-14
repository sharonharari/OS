#include "adt.h"
/*
*  Missions left:
*	1)Error handling and full implanations. (Error counter for etc.)
*	2) Timing handling (select())
*	3)Recheck requirements been achieved.
*/

bool is_number(std::string& str)
{
	for (char const& c : str) {
		// using the std::isdigit() function
		if (std::isdigit(c) == 0)
			return false;
	}
	return true;
}
bool is_valid_args(int argc, char* argv[], uint16_t(&args)[3]) {
	if (argc != 4) {
		return false;
	}
	for (int i = 1; i < argc; i++) {
		std::string temp(argv[i]);
		if (!is_number(temp)) {
			return false;
		}
		int temp2 = std::stoi(temp);
		if (temp2 <= 0 || temp2 > USHRT_MAX) {
			return false;
		}
		args[i-1] = (uint16_t)temp2;
	}
	return true;
}



int main(int argc, char* argv[]) {
	uint16_t args[3];
	if (!is_valid_args(argc,argv,args)) {
		std::cerr << "TTFTP_ERROR: illegal arguments" << std::endl;
		exit(1);
	}
	uint16_t port = args[0], timeout = args[1], max_num_of_resends = args[2];
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		std::perror("TTFTP_ERROR:");
		exit(1);
	}
	struct sockaddr_in echoServAddr;
	std::memset(&echoServAddr, 0, sizeof(echoServAddr));
	if (!echoServAddr) {
		std::perror("TTFTP_ERROR:");//is syscall?
		exit(1);
	}
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	echoServ.sin_port = htons(port);
	if (bind(sock, (struct sockaddr*)&echoServAddr, sizeof(echoServAddr))) {
		std::perror("TTFTP_ERROR:");
		exit(1);
	}
	while(true) {
		char buffer[MAX_PACKET_SIZE] = { 0 };
		struct sockaddr_in ClientAddr = { 0 };
		socklen_t ClientAddrLen;
		ssize_t recvMsgSize = recvfrom(sock, (void*)buffer, ECHOMAX, 0,
			(struct sockaddr*)&ClientAddr, &ClientAddrLen);
		if (recvMsgSize < 0) {
			std::perror("TTFTP_ERROR:");
			exit(1);
		}
		size_t recvMsgSize_positive = (size_t)recvMsgSize;
		char opcode_network[2] = { buffer[0],buffer[1] };
		uint16_t opcode = ntohs((uint16_t)opcode_network);
		if (opcode == WRQ_OPCODE) {
			std::string filename(buffer + 2);
			std::string transmission_mode(buffer + 2 + filename.size() + 1);
			std::ifstream file(filename);
			if (file.is_open())//file exist and can be read from
			{
				//Error file exists
				file.close();
			}
			else {
				std::ofstream output_file(filename, std::ofstream::out);
				char ackBuffer[4] = { 0 };
				create_ack(&ackBuffer);
				if (sendto(sock, (void*)ackBuffer, ACK_SIZE, 0, (struct sockaddr*)&ClientAddr, sizeof(ClientAddr)) != (ssize_t)ACK_SIZE) {
					//error("sendto() failed");
					//syscall
				}
				bool is_finished = false;
				uint16_t wanted_block_number = (uint16_t)0;
				while (!is_finished) {
					struct sockaddr_in SessionAddr = { 0 };
					socklen_t SessionAddrLen;
					ssize_t recvMsgSize = recvfrom(sock, (void*)buffer, ECHOMAX, 0,
						(struct sockaddr*)&SessionAddr, &SessionAddrLen);
					if (recvMsgSize < 0) {
						std::perror("TTFTP_ERROR:");
						exit(1);
					}
					if ((SessionAddrLen != ClientAddrLen)||(SessionAddr.sin_addr.s_addr != ClientAddr.sin_addr.s_addr)){
						//Error recieved a packet from a different endpoint than the one in session.
						continue;
					}
					size_t recvMsgSize_positive = (size_t)recvMsgSize;
					char opcode_network[2] = { buffer[0],buffer[1] };
					uint16_t opcode = ntohs((uint16_t)opcode_network);
					if (opcode != DATA_OPCODE) {
						//Error not a data packet
						continue;
					}
					else {
						Data data_block(buffer);
						if (data_block.get_block_number() != wanted_block_number + 1) {
							//Error Bad block number
							continue;
						}
						else {
							wanted_block_number = data_block.get_block_number();
							output_file << data_block.get_data;
							char ackBuffer[4] = { 0 };
							create_ack(&ackBuffer, wanted_block_number);
							if (sendto(sock, (void*)ackBuffer, ACK_SIZE, 0, (struct sockaddr*)&ClientAddr, sizeof(ClientAddr)) != (ssize_t)ACK_SIZE) {
								//error("sendto() failed");
								//syscall
							}
							if (data_block.get_data_size < MAX_DATA_SIZE) {
								is_finished = true;
							}
						}
					}
				}
				output_file.close();
			}
		}
		//std::cout << "Handling client " << inet_ntoa(ClientAddr.sin_addr) << std::endl;//prints address shit
		//if (sendto(sock, echoBuffer, recvMsgSize, 0, (struct sockaddr*)&ClientAddr, sizeof(ClientAddr)) != recvMsgSize)
		//	error("sendto() failed");
		else {
			//recived an non WRQ packet first. //Error handling?
		}

	}
	std::cout << port + timeout + max_num_of_resends;
	return 0;
}