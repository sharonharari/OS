#define WRQ_OPCODE 2
#define ACK_OPCODE 4
#define DATA_OPCODE 3

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>


#include <sys/types.h>
#include <sys/socket.h>
using namespace std;

struct server_ack_packet{
	uint16_t ack_Opcode;
	uint16_t server_block_number;
}__attribute__((packed));

struct client_data_packet{
	uint16_t data_Opcode;
	uint16_t data_block_number;
	char data[512];
}__attribute__((packed));

struct client_info{
	client_data_packet* client_packet;
	bool sent_wrq;
}
int main(int argc, char *argv[])
{
	if(argc != 4){
		cout << "Wrong number of arguments" << endl;
		exit(1);
	}
	unsigned short port;
	port = atoi(argv[1]);
	if(!(port>10000) // do we need to do this check??
	{
		cerr <<"port number has to be greater then 10000" << endl;
		exit(1);
	}
	int timeout=argv[2]; // do we need to check timeout input?
	int max_num_failures = argv[3]; // do we need to check max num failures input??
	int socket_fd;
	struct sockaddr_in server; //local address
	struct sockaddr_in client; //client address
	char buffer[sizeof(struct client_data_packet)];
	socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(socket_fd < 0)
	{
		perror("TTFTP_ERROR:");
		exit(1);
	}
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl (INADDR_ANY);
	server.sin_port = htons (port);
	if(bind(socket_fd, (struct sockaddr *) &server, sizeof(server))<0)
	{
		perror("TTFTP_ERROR:");
		exit(1);
	}
	fd_set current_socket;
    int retval;
	unsigned int client_msg_len; //length of incoming message	
	do
	{
		FD_ZERO(&current_socket);
		FD_SET(socket_fd, &current_socket);
		int error_flag = 0;
		int failures = 0;
		client_msg_len = sizeof(client);
		//block until WRQ appear from client
		int recvMsgSize = recvfrom(socket_fd, buffer,sizeof(buffer),0,
			(struct sockaddr *)&client,&sizeof(client)));
		if(recvMsgSize<0)
		{
			perror("TTFTP_ERROR:");
			exit(1);
		}
		if(int(buffer[0]) != 0 || (int(buffer[1])) != WRQ_OPCODE)
		{
			string error_msg = "Illegal WRQ";
			sendto(socket_fd, &error_msg, sizeof(error_msg),4, (struct sockaddr *) &client,client_msg_len));
			error_flag =1;
		}
		string filename = buffer + 2;
		string transmission_mode = buffer +2 + filename.size() + 1;
		if(transmission_mode != "octet")
		{	
			string error_msg = "Illegal WRQ";
			sendto(socket_fd, &error_msg, sizeof(error_msg),4, (struct sockaddr *) &client,client_msg_len));
			error_flag = 1;
		}

		if(error_flag == 0)
		{

			FILE* f1 = fopen(&filename, "w");
			if (f1 == NULL){
				perror("TTFTP_ERROR:");
				exit(1);
			}
			uint16_t block_num_ack=0;
			//creating ack packet
			struct server_ack_packet ack;
			ack.ack_opcode = htons(ACK_OPCODE);
			ack.block_num = htons(block_num_ack);

			sendto(socket_fd, &ack, 
			sizeof(struct server_ack_packet), 0, (struct sockaddr *) &client,client_msg_len);
			cout << "OUT:ACK," << block_num_ack <<endl;
			int is_last = 0;
			do
			{
				//int is_last = 0;
				do
				{
					 // TODO: Wait WAIT_FOR_PACKET_TIMEOUT to see if something appears
					 // for us at the socket (we are waiting for DATA)

					struct timeval tv;
					tv.tv_sec = WAIT_FOR_PACKET_TIMEOUT;
					tv.tv_usec = 0;
					/*
					cout << "going to sleep" << endl;
					sleep(3);
					cout << "woke up" << endl;
					*/
					retval = select(sock+1, &rfds, NULL, NULL, &tv);

					

					if(retval == -1){
						perror("TTFTP_ERROR:");
						fclose(f1);
						unlink(filename.c_str());
						exit(1);
					}
					recvMsgSize = 0;
					if (retval>0)// TODO: if there was something at the socket and
					 // we are here not because of a timeout
					 {
						 // TODO: Read the DATA packet from the socket (at
						 // least we hope this is a DATA packet)
						 if((recvMsgSize = recvfrom(sock, buffer, DATA_BUFFER_SIZE, 0, (struct sockaddr *) &clntAddr, &client_msg_len))<0){
							perror("TTFTP_ERROR:");
							fclose(f1);
							unlink(filename.c_str());
							exit(1);
							}
					 }
					if (retval == 0) // TODO: Time out expired while waiting for data
					 // to appear at the socket
					 {
						//TODO: Send another ACK for the last packet
						 cout << "FLOWERROR: did not recive anything- timeout" << endl;
						 failures++;
						 ack.block_num = htons(block_num_ack);
						 if(sendto(sock, &ack, sizeof(struct ack_packet), 0, (struct sockaddr *) &clntAddr, sizeof(clntAddr)) != sizeof(struct  ack_packet)){
								perror("TTFTP_ERROR:");
								fclose(f1);
								unlink(filename.c_str());
								exit(1);
							}
						cout << "OUT:ACK," << block_num_ack <<endl;
					}

					if (failures>= NUMBER_OF_FAILURES)
					 {
					 // FATAL ERROR BAIL OUT
					 	fclose(f1);
						unlink(filename.c_str());
					 	cout << "FLOWERROR: too many failiures" << endl;
					 	cout << "RECVFAIL" << endl;
					 	fatal_flag = 1;
					 }

				}while (recvMsgSize == 0 && fatal_flag == 0); // TODO: Continue while some socket was ready
				 // but recvfrom failed to read the data (ret 0)

				if(fatal_flag == 0){
					//parse the opcode
					 if( buffer[0] != 0 || (buffer[1]) != DATA_OPCODE ) //(curr_opcode != DATA_OPCODE) // TODO: We got something else but DATA
					 {
					 // FATAL ERROR BAIL OUT
					 	fclose(f1);
					 	unlink(filename.c_str());
					 	cout << "FLOWERROR: wrong data opcode" << endl;
					 	cout << "RECVFAIL" << endl;
					 	fatal_flag = 1;

					 }
					 uint16_t curr_blck_num = ntohs(((struct client_data_packet *)buffer)->block_num);
					 if (curr_blck_num != block_num_ack +1) // TODO: The incoming block number is not what we have
					 // expected, i.e. this is a DATA pkt but the block number
					 // in DATA was wrong (not last ACK’s block number + 1)
					 {
					 // FATAL ERROR BAIL OUT
					 	fclose(f1);
						unlink(filename.c_str());
					 	cout << "IN:DATA," << block_num_ack << "," << recvMsgSize << endl;
					 	cout << "FLOWERROR: wrong block number of ack" << endl;
					 	cout << "RECVFAIL" << endl;
					 	fatal_flag = 1;
					 }
					 if(fatal_flag == 0){
						block_num_ack++;
						
						cout << "IN:DATA," << block_num_ack << "," << recvMsgSize << endl;
						int lastWriteSize = fwrite(buffer +4, 1, recvMsgSize-4, f1); // write next bulk of data
						// TODO: send ACK packet to the client
						if(lastWriteSize != recvMsgSize-4){
							perror("TTFTP_ERROR:");
							fclose(f1);
							unlink(filename.c_str());;
							exit(1);
						}
						cout << "WRITING:" << lastWriteSize << endl;
						ack.block_num = htons(block_num_ack);//hton
						if(sendto(sock, &ack, sizeof(struct ack_packet), 0, (struct sockaddr *) &clntAddr, sizeof(clntAddr)) != sizeof(struct  ack_packet)){
							perror("TTFTP_ERROR:");
							fclose(f1);
							unlink(filename.c_str());
							exit(1);
						}
						cout << "OUT:ACK," << block_num_ack <<endl;
						//updating is last
						if(recvMsgSize < DATA_BUFFER_SIZE ){
							is_last = 1;
						}
					}
				}
			}while (is_last == 0 && fatal_flag == 0); // Have blocks left to be read from client (not end of transmission)
		
			if(fatal_flag == 0){
				cout << "RECVOK" << endl;
				if(fclose(f1) != 0){
					perror("TTFTP_ERROR:");
					exit(1);
				}
			}
		
		}
	}while(1);//works all the time, waiting for new client's wrq
}
