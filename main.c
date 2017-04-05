#include "Headers.h"
#include "common.h"
#include "server.h"
#include "client.h"

int main(int argc, char *argv[])
{
	if (argc != 6) {
		fprintf(stderr, "Usage: %s <remoteIP address> <tcp remote port> <tcp local port> <udp remote port> <udp local port>\n", argv[0]);
		exit(1);
	}
	remoteTCP = atoi(argv[2]);  
	localTCP = atoi(argv[3]);  
	remoteUDP = atoi(argv[4]);  
	localUDP = atoi(argv[5]);  

	pid_t pid = fork();
	
	if(pid < 0) 
		error("Fork error\n");
	else if(pid == 0) {
		pid_t pd = fork();
		if(pd < 0) 
			error("Fork error\n");
		if(pd == 0) 
			tcpServer();
		else 
			udpServer();
	}

	else if(pid > 0) 
		client(argv[1]);
	return 0;
}
  