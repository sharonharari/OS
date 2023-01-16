#include "adt.h"
/*
*  Missions left:
*	1) Failure counter && timing handling(Select)
*	2) Recheck requirements been achieved.
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
	int failure_counter = 0;
	uint16_t args[3];
	char ackBuffer[4];
	if (!is_valid_args(argc,argv,args)) {
		std::cerr << "TTFTP_ERROR: illegal arguments" << std::endl;
		exit(1);
	}
	uint16_t port = args[0], timeout = args[1], max_num_of_resends = args[2];
	struct timeval timeout_struct;
	timeout_struct.tv_sec = (int)timeout;
	timeout_struct.tv_usec = 0;
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		std::perror("TTFTP_ERROR:");
		exit(1);
	}
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(sock, &rfds);
	struct sockaddr_in echoServAddr = {0};
	// memset(&echoServAddr, 0, sizeof(echoServAddr));
	// if (!(&echoServAddr)) {
	// 	std::perror("TTFTP_ERROR:");//is syscall?
	// 	exit(1);
	// }
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	echoServAddr.sin_port = htons(port);
	if (bind(sock, (struct sockaddr*)&echoServAddr, sizeof(echoServAddr))) {
		std::perror("TTFTP_ERROR:");
		exit(1);
	}





	while(true) {
		// waiting for WRQ
		failure_counter = 0;
		char buffer[MAX_PACKET_SIZE] = { 0 };
		struct sockaddr_in ClientAddr = { 0 };
		socklen_t ClientAddrLen;
		
		ssize_t recvMsgSize = recvfrom(sock, (void*)buffer, MAX_PACKET_SIZE, 0,
			(struct sockaddr*)&ClientAddr, &ClientAddrLen);
		if (recvMsgSize < 0) {
			std::perror("TTFTP_ERROR:");
			exit(1);
		}
		// size_t recvMsgSize_positive_wrq = (size_t)recvMsgSize;
		uint16_t opcode_network_wrq = ( buffer[0] << 8) + buffer[1];
		uint16_t opcode_wrq = ntohs(opcode_network_wrq);
		if (opcode_wrq == WRQ_OPCODE) {
			std::string filename(buffer + 2);
			std::string transmission_mode(buffer + 2 + filename.size() + 1);
			std::ifstream file(filename);
			if (file.is_open())//file exist and can be read from
			{
				//Error file exists
				error_handling(sock, ClientAddr, (uint16_t)6);
				file.close();
				// need to check:
				// if(std::remove(filename)){
				// 	// Error deleting the file!
				// 	std::perror("TTFTP_ERROR:");
				// 	exit(1);
				// }
			}
			else {
				std::ofstream output_file(filename, std::ofstream::out);
				ackBuffer[4] = { 0 };
				create_ack(ackBuffer);
				if (sendto(sock, (void*)ackBuffer, ACK_SIZE, 0, (struct sockaddr*)&ClientAddr, sizeof(ClientAddr)) != (ssize_t)ACK_SIZE) {
					//error("sendto() failed");
					//syscall
					std::perror("TTFTP_ERROR:");
					output_file.close();
					if(std::remove(const_cast<char*>(filename.c_str()))){
						// Error deleting the file!
						std::perror("TTFTP_ERROR:");
					}
					exit(1);
				}
				bool is_finished = false;
				uint16_t wanted_block_number = (uint16_t)0;

				// Reading data in while loop
				while (!is_finished) {
					struct sockaddr_in SessionAddr = { 0 };
					socklen_t SessionAddrLen;
					int retval = select(1, &rfds, NULL, NULL, &timeout_struct);
					if (retval == -1){
						// select error
						std::perror("TTFTP_ERROR:");
						output_file.close();
						if(std::remove(const_cast<char*>(filename.c_str()))){
							// Error deleting the file!
							std::perror("TTFTP_ERROR:");
						}
						exit(1);
					}
					else if (retval){
						ssize_t recvMsgSize = recvfrom(sock, (void*)buffer, MAX_PACKET_SIZE, 0,
						(struct sockaddr*)&SessionAddr, &SessionAddrLen);
						if (recvMsgSize < 0) {
							std::perror("TTFTP_ERROR:");
							output_file.close();
							if(std::remove(const_cast<char*>(filename.c_str()))){
								// Error deleting the file!
								std::perror("TTFTP_ERROR:");
							}
							exit(1);
						}
					}
					else {
						// timeout!!
						failure_counter++;
					}
					// Continue implement Failure counter handling!!
					if ((SessionAddrLen != ClientAddrLen)||(SessionAddr.sin_addr.s_addr != ClientAddr.sin_addr.s_addr)){
						//Error recieved a packet from a different endpoint than the one in session.
						error_handling(sock, ClientAddr, (uint16_t)4); // ClientAddr or SessionAddr?????
						output_file.close();
						if(std::remove(const_cast<char*>(filename.c_str()))){
							// Error deleting the file!
							std::perror("TTFTP_ERROR:");
							exit(1);
						}
						continue;
					}
					size_t recvMsgSize_positive_data = (size_t)recvMsgSize;
					uint16_t opcode_network_data = ( buffer[0] << 8) + buffer[1];
					uint16_t opcode_data = ntohs((uint16_t)opcode_network_data);
					if (opcode_data != DATA_OPCODE) {
						//Error not a data packet - must be WRQ request from the same client!
						error_handling(sock, ClientAddr, (uint16_t)4);
						output_file.close();
						if(std::remove(const_cast<char*>(filename.c_str()))){
							// Error deleting the file!
							std::perror("TTFTP_ERROR:");
							exit(1);
						}
						continue;
					}
					else {
						Data data_block(buffer, recvMsgSize_positive_data);
						if (data_block.block_number != wanted_block_number + 1) {
							//Error Bad block number
							error_handling(sock, ClientAddr, (uint16_t)0);
							output_file.close();
							if(std::remove(const_cast<char*>(filename.c_str()))){
								// Error deleting the file!
								std::perror("TTFTP_ERROR:");
								exit(1);
							}
							continue;
						}
						else {
							wanted_block_number = data_block.block_number;
							output_file << data_block.data;
							ackBuffer[4] = { 0 };
							create_ack(ackBuffer, wanted_block_number);
							if (sendto(sock, (void*)ackBuffer, ACK_SIZE, 0, (struct sockaddr*)&ClientAddr, sizeof(ClientAddr)) != (ssize_t)ACK_SIZE) {
								//error("sendto() failed");
								//check if necessary:
								output_file.close();
								std::perror("TTFTP_ERROR:");
								if(std::remove(const_cast<char*>(filename.c_str()))){
									// Error deleting the file!
									std::perror("TTFTP_ERROR:");
								}
								exit(1);
							}
							if (data_block.data_size < MAX_DATA_SIZE) {
								is_finished = true;
							}
							else{
								int select(int nfds, fd_set *readfds, fd_set *writefds,
								fd_set *exceptfds, const struct timeval *timeout);
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
			//recived an non WRQ packet first. Error handling
			error_handling(sock, ClientAddr, (uint16_t)7);
		}

	}
	std::cout << port + timeout + max_num_of_resends;
	return 0;
}