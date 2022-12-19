#define DATA_BUFFER_SIZE 516 //????512????
#define WRQ_OPCODE 2
#define ACK_OPCODE 4
#define DATA_OPCODE 3//3

#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdint.h>
#include <string.h>
using namespace std;

const int WAIT_FOR_PACKET_TIMEOUT = 3;
const int NUMBER_OF_FAILURES = 7;

struct ack_packet{
	uint16_t ack_opcode;
	uint16_t block_num;
}__attribute__((packed));

struct data_packet{
	uint16_t data_opcode;
	uint16_t block_num;
	char data[512];
}__attribute__((packed));

int main(int argc, char *argv[]){
	if(argc != 2){
		cout << "Wrong number of arguments" << endl;
		exit(1);
	}
	int servPort;
	try{
		servPort = stoi(argv[1]);
	}
	catch(...){
		cout << "Wrong arguments" << endl;
		exit(1);
	}
	/*if(servPort <= 0){
		cout << "port number is invalid" << endl;
		exit(1);
	}*/

	int sock;
	struct sockaddr_in servAddr; //local address
	struct sockaddr_in clntAddr; //client address
	unsigned int cliAddrLen; //length of incoming message
	char buffer[DATA_BUFFER_SIZE];
	fd_set rfds;
    int retval;

	if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
		perror("TTFTP_ERROR:");
		exit(1);
	}

	//zero out structure
	memset(&servAddr, 0, sizeof(servAddr));

	//internet address family
	servAddr.sin_family = AF_INET;

	//amy incoming interface
	servAddr.sin_addr.s_addr = htonl (INADDR_ANY);

	//local port
	servAddr.sin_port = htons (servPort);

	//bind to the local address
	if(bind(sock, (struct sockaddr *) &servAddr, sizeof(servAddr))<0){
		perror("TTFTP_ERROR:");
		exit(1);
	}

	

	do
	{
		FD_ZERO(&rfds);
		FD_SET(sock, &rfds);
		int fatal_flag = 0;
		int recvMsgSize = 0;
		int timeoutExpiredCount = 0;//???is this per packet? or general for all from the same client?
		//set the size of the in-out parameter
		cliAddrLen = sizeof(clntAddr);
		//block until recieve message from a client
		if((recvMsgSize = recvfrom(sock, buffer, DATA_BUFFER_SIZE, 0, (struct sockaddr *) &clntAddr, &cliAddrLen))<0){
			perror("TTFTP_ERROR:");
			exit(1);
		}
		

		if(buffer[0] != 0 || (buffer[1]) != WRQ_OPCODE){
			cout << "FLOWERROR: wrong wrq opcode" << endl;
			//???recvfail? IN:WRQ???
			fatal_flag =1;
		}
		string filename = buffer +2;
		string transmission_mode = buffer +2 + filename.size() +1;

		if(transmission_mode != "octet"){
			if(fatal_flag == 0){
				cout << "IN:WRQ," << filename <<"," << transmission_mode <<endl; 
				cout << "FLOWERROR: wrong transmission mode" << endl;	
			}
			//???what else to print??????recvfail? IN:WRQ???
			fatal_flag = 1;
		}

		if(fatal_flag == 0){

			cout << "IN:WRQ," << filename <<",octet" <<endl;
			FILE* f1 = fopen(filename.c_str(), "w");
			if (f1 == NULL){
				perror("TTFTP_ERROR:");
				exit(1);
			}

			uint16_t block_num_ack=0;
			//creating ack packet
			struct ack_packet ack;
			ack.ack_opcode = htons(ACK_OPCODE);//make sure!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			ack.block_num = htons(block_num_ack);

			if(sendto(sock, &ack, sizeof(struct ack_packet), 0, (struct sockaddr *) &clntAddr, sizeof(clntAddr)) != sizeof(struct  ack_packet)){
				perror("TTFTP_ERROR:");
				fclose(f1);
				unlink(filename.c_str());
				exit(1);
			}
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
						 if((recvMsgSize = recvfrom(sock, buffer, DATA_BUFFER_SIZE, 0, (struct sockaddr *) &clntAddr, &cliAddrLen))<0){
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
						 timeoutExpiredCount++;
						 ack.block_num = htons(block_num_ack);
						 if(sendto(sock, &ack, sizeof(struct ack_packet), 0, (struct sockaddr *) &clntAddr, sizeof(clntAddr)) != sizeof(struct  ack_packet)){
								perror("TTFTP_ERROR:");
								fclose(f1);
								unlink(filename.c_str());
								exit(1);
							}
						cout << "OUT:ACK," << block_num_ack <<endl;
					}

					if (timeoutExpiredCount>= NUMBER_OF_FAILURES)
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
					 uint16_t curr_blck_num = ntohs(((struct data_packet *)buffer)->block_num);
					 //uint16_t curr_blck_num = (uint16_t)(buffer[2] *10) + (uint16_t)buffer[3];//???stoi???
					 //cout << "uint_16(ntohs(buffer[2]) is: " << (uint16_t)(ntohs(buffer[2])) << ". and uint(nthos(buffer[3] is: " << (uint16_t)(htons(buffer[3])) << endl;
					 //cout << "buffer[2] is: " << (uint16_t)(buffer[2]) << ". and buffer[3] is: " << (uint16_t)buffer[3] << endl;
					 //cout << "the curr block num: " << curr_blck_num << endl;
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
					//remove(filename.c_str());
					exit(1);
				}
			}
		
		}
	}while(1);//works all the time, waiting for new client's wrq
}



/*questions
1. anan2.txt is too big? we get ack 127 and then try to get a new wrq but data comes so we print too many fails V
2. what happens if wesend a file that does not exist? V
3. when timeout over 7, what is the order of things? first print and do nothing? or send ack? V
4. deleting file if we fail. - remove is fine
5. not sure what to do if wrong wrq opcode-> recvfail -> waits again for wrq??? what else should happen? that is fine! V
6. htons? bla bla bla G&D
7. tests???
*/