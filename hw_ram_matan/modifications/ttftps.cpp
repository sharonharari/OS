#define DATA_BUFFER_SIZE 516
#define WRQ_OPCODE 2
#define DATA_OPCODE 3
#define ACK_OPCODE 4
#define MAX_CLIENTS 65536
#define ECHOMAX 255

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
//#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdint.h>
#include <string.h>
using namespace std;

struct ack_packet{
	uint16_t ack_opcode;
	uint16_t block_num;
}__attribute__((packed));

struct data_packet{
	uint16_t data_opcode;
	uint16_t block_num;
	char data[512];
}__attribute__((packed));

struct error_packet{
	uint16_t error_opcode;
	uint16_t error_code;
	char* ErrMsg;
}__attribute__((packed));

int main(int argc, char *argv[])
{
	if(argc != 4){
		cout << "Wrong number of arguments" << endl;
		exit(1);
	}
	int timeout=atoi(argv[2]); // do we need to check timeout input?
	int max_num_failures = atoi(argv[3]); // do we need to check max num failures input??
	int socket_fd;
	struct sockaddr_in server_addr; //local address
	struct sockaddr_in client_addr; //client address
	unsigned int cliAddrLen; //length of incoming message
	char buffer[DATA_BUFFER_SIZE];
    int retval;
	unsigned short port = atoi(argv[1]);
	if(!(port>0)) // do we need to do this check that port greater than 10K??
	{
		cerr <<"port number has to be greater then 10000" << endl;
		exit(1);
	}
	if((socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
		perror("TTFTP_ERROR:");
		exit(1);
	}
	cout << "socket fd is : "<< socket_fd << endl;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	server_addr.sin_port = htons (port);
	if(bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr))<0){
		perror("TTFTP_ERROR:");
		exit(1);
	}
	//cout << "listen is : " << listen(socket_fd,5) << endl;
	cliAddrLen = sizeof(client_addr);
	//int current_socket_fd = accept(socket_fd,(struct sockaddr *) &client_addr,&cliAddrLen);
	cout << client_addr.sin_family << endl;
	cout << client_addr.sin_port << endl; 
	pid_t handler_proc = fork();
	cout << "after fork" << endl;
	fd_set sock;
	if(handler_proc == 0)
	{
		cout << "inside son handler" << endl;
		do
		{
			FD_ZERO(&sock);
			FD_SET(socket_fd, &sock);
			cout << "sock and socket fd are set" << endl;
			int error_flag = 0;
			int recvMsgSize = 0;
			int timeout_counter = 0;//???is this per packet? or general for all from the same client?
			//set the size of the in-out parameter
			// cliAddrLen = sizeof(client_addr);
			//block until recieve message from a client
			if((recvMsgSize = recvfrom(socket_fd, buffer, DATA_BUFFER_SIZE, 0,(struct sockaddr *) &client_addr, &cliAddrLen))<0)
			{
				perror("TTFTP_ERROR:");
				exit(1);
			}
			string filename = buffer +2;
			string transmission_mode = buffer +2 + filename.size() +1;
			if((buffer[0] != 0 || (buffer[1]) != WRQ_OPCODE) || (transmission_mode != "octet" && !error_flag))
			{
				string error_line = "Illegal WRQ";
				struct error_packet err;
				err.error_opcode = 5;
				err.error_code = 4;
				err.ErrMsg = (char*)malloc(error_line.size());
				strcpy(err.ErrMsg,error_line.c_str());
				if((sendto(socket_fd, &err, sizeof(err),0,(struct sockaddr *) &client_addr,sizeof(client_addr)))!=sizeof(struct error_packet))
				{	
					free(err.ErrMsg);		
					perror("TTFTP_ERROR:");
					exit(1);
				}
				free(err.ErrMsg);
				error_flag=1;
			}
			if(error_flag == 0)
			{
				FILE* f1 = fopen(filename.c_str(), "w");
				if (f1 == NULL) // maybe without this line, instead put errno = 0 before fopen.
				{
					if(errno == EEXIST)
					{
						string error_line = "File already exists";
						struct error_packet err;
						err.error_opcode = 5;
						err.error_code = 6;
						err.ErrMsg = (char*)malloc(error_line.size());
						strcpy(err.ErrMsg,error_line.c_str());
						if(sendto(socket_fd, &err, sizeof(err),0,(struct sockaddr *) &client_addr, sizeof(client_addr))!=sizeof(struct error_packet))
						{
							free(err.ErrMsg);			
							perror("TTFTP_ERROR:");
							exit(1);
						}
						free(err.ErrMsg);
						error_flag=1;
					}
				}
				uint16_t block_num_ack=0;
				struct ack_packet ack;
				ack.ack_opcode = htons(ACK_OPCODE);
				ack.block_num = htons(block_num_ack);
				if(sendto(socket_fd, &ack, sizeof(struct ack_packet), 0, (struct sockaddr *) &client_addr,sizeof(client_addr)) != sizeof(struct  ack_packet))
				{
					perror("TTFTP_ERROR:");
					fclose(f1);
					unlink(filename.c_str());
					exit(1);
				}
				int is_last = 0;
				do
				{
					do
						{
						// TODO: Wait WAIT_FOR_PACKET_TIMEOUT to see if something appears
						// for us at the socket (we are waiting for DATA)
						struct timeval tv;
						tv.tv_sec = timeout;
						tv.tv_usec = 0;
						retval = select(socket_fd+1,&sock, NULL, NULL, &tv);
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
							if((recvMsgSize = recvfrom(socket_fd, buffer, DATA_BUFFER_SIZE, 0,(struct sockaddr *) &client_addr, &cliAddrLen))<0)
							{
								perror("TTFTP_ERROR:");
								fclose(f1);
								unlink(filename.c_str());
								exit(1);
							}
							if(recvMsgSize == sizeof(struct data_packet))
							{
								string error_line = "Unknown user";
								struct error_packet err;
								err.error_opcode = 5;
								err.error_code = 7;
								err.ErrMsg = (char*)malloc(error_line.size());
								strcpy(err.ErrMsg,error_line.c_str());
								if(sendto(socket_fd, &err, sizeof(err),0,(struct sockaddr *) &client_addr, sizeof(client_addr))!=sizeof(struct error_packet))
								{	
									free(err.ErrMsg);		
									perror("TTFTP_ERROR:");
									exit(1);
								}
								free(err.ErrMsg);
								error_flag=1;					
							}

							}
							if (retval == 0) // TODO: Time out expired while waiting for data
							// to appear at the socket
							{
							//TODO: Send another ACK for the last packet
							timeout_counter++;
							ack.block_num = htons(block_num_ack);
							if(sendto(socket_fd, &ack, sizeof(struct ack_packet), 0,(struct sockaddr *) &client_addr,sizeof(client_addr)) != sizeof(struct  ack_packet))
							{
								perror("TTFTP_ERROR:");
								fclose(f1);
								unlink(filename.c_str());
								exit(1);
							}
						}
						if (timeout_counter>= max_num_failures)
						{
							string error_line = "Abandoning file transmission";
							struct error_packet err;
							err.error_opcode = 5;
							err.error_code = 0;
							err.ErrMsg = (char*)malloc(error_line.size());
							strcpy(err.ErrMsg,error_line.c_str());
							if(sendto(socket_fd, &err, sizeof(err),0,(struct sockaddr *) &client_addr, sizeof(client_addr))!=sizeof(struct error_packet))
							{	
								free(err.ErrMsg);		
								perror("TTFTP_ERROR:");
								exit(1);
							}					
							free(err.ErrMsg);	
						}
					}
					while (recvMsgSize == 0 && error_flag == 0); // TODO: Continue while some socket was ready
					// but recvfrom failed to read the data (ret 0)
					// If Im here then error occured or i got some data to check
					if(error_flag == 0)
					{
						if(buffer[0] == 0 && buffer [1] == 2) //WRQ
						{
							string error_line = "Unexpected WRQ request";
							struct error_packet err;
							err.error_opcode = 5;
							err.error_code = 4;
							err.ErrMsg = (char*)malloc(error_line.size());
							strcpy(err.ErrMsg,error_line.c_str());
							if(sendto(socket_fd, &err, sizeof(err),0,(struct sockaddr *) &client_addr, sizeof(client_addr))!=sizeof(struct error_packet))
							{	
								free(err.ErrMsg);	
								perror("TTFTP_ERROR:");
								exit(1);
							}	
							free(err.ErrMsg);	
							error_flag = 1;		
						}			
					}
					//parse the opcode
					if((buffer[0] != 0) || (buffer[1] != DATA_OPCODE)) //(curr_opcode != DATA_OPCODE) // TODO: We got something else but DATA
					{
						string error_line = "Illegal TFTP operation";
						struct error_packet err;
						err.error_opcode = 5;
						err.error_code = 4;
						err.ErrMsg = (char*)malloc(error_line.size());
						strcpy(err.ErrMsg,error_line.c_str());
						if(sendto(socket_fd, &err, sizeof(err),0,(struct sockaddr *) &client_addr, sizeof(client_addr))!=sizeof(struct error_packet))
						{	
							free(err.ErrMsg);		
							perror("TTFTP_ERROR:");
							exit(1);
						}	
						free(err.ErrMsg);
						error_flag = 1;
					}
					uint16_t data_block_num = ntohs(((struct data_packet *)buffer)->block_num);
					if (data_block_num != block_num_ack +1) // TODO: The incoming block number is not what we have
					// expected, i.e. this is a DATA pkt but the block number
					// in DATA was wrong (not last ACK’s block number + 1)
					{
					// FATAL ERROR BAIL OUT
						string error_line = "Bad block number";
						struct error_packet err;
						err.error_opcode = 5;
						err.error_code = 0;
						err.ErrMsg = (char*)malloc(error_line.size());
						strcpy(err.ErrMsg,error_line.c_str());
						if(sendto(socket_fd, &err, sizeof(err),0,(struct sockaddr *) &client_addr, sizeof(client_addr))!=sizeof(struct error_packet))
						{	
							free(err.ErrMsg);		
							perror("TTFTP_ERROR:");
							exit(1);
						}	
						free(err.ErrMsg);
						error_flag = 1;
					}
					if(error_flag == 0){
						block_num_ack++;		
						int lastWriteSize = fwrite(buffer +4, 1, recvMsgSize-4, f1); // write next bulk of data
						// TODO: send ACK packet to the client
						if(lastWriteSize != recvMsgSize-4){
							perror("TTFTP_ERROR:");
							fclose(f1);
							unlink(filename.c_str());;
							exit(1);
						}
						ack.block_num = htons(block_num_ack);
						if(sendto(socket_fd, &ack, sizeof(struct ack_packet), 0, (struct sockaddr *) &client_addr, sizeof(client_addr)) != sizeof(struct  ack_packet))
						{
							perror("TTFTP_ERROR:");
							fclose(f1);
							unlink(filename.c_str());
							exit(1);
						}
						//updating is last
						if(recvMsgSize < DATA_BUFFER_SIZE ){
							is_last = 1;
						}
					}
				}
				while (is_last == 0 && error_flag == 0); // Have blocks left to be read from client (not end of transmission)
			}
		}	while(1);//works all the time, waiting for new client's wrq
	}
	//close(sock);
	while(1);//works all the time, waiting for new client's wrq
}