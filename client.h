#ifndef CLIENT_H
#define CLIENT_H

int client(char *remoteIP) {
	FILE* Fp;
	
	char rBuff[BUFFSIZE];
	char wBuff[BUFFSIZE];
	char command[BUFFSIZE];
	char downloadFilename[MAXL];

	int st, n, flagDownload = 0;
	int serverlen;
	int typeCommand;
	int localSockfd, localSockfdUdp;
	
	struct sockaddr_in serverAddress, serverAddress_udp;
	localSockfd = socket(AF_INET, SOCK_STREAM, 0);
	localSockfdUdp = socket(AF_INET, SOCK_DGRAM, 0); 
	
	if (localSockfdUdp < 0) 
		error("ERROR opening socket");
	
	if (localSockfd < 0) 
		error("ERROR opening socket");
	bzero((char *) &serverAddress, sizeof(serverAddress));
	bzero((char *) &serverAddress_udp, sizeof(serverAddress_udp));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(remoteTCP);
	serverAddress_udp.sin_family = AF_INET;
	serverAddress_udp.sin_port = htons(remoteUDP);
	if ((st = inet_pton(AF_INET, remoteIP, &serverAddress.sin_addr)) <= 0) error("ERROR with inet_pton");
	if ((st = inet_pton(AF_INET, remoteIP, &serverAddress_udp.sin_addr)) <= 0) error("ERROR with inet_pton");
	while(1) {
		if (connect(localSockfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0)
			continue;
		else {
			printf("\nConnected!\n\n");
			break;
		}
	}
	serverlen = sizeof(serverAddress_udp);
	while(1) {
		memset(rBuff, 0, sizeof(rBuff));
		bzero(wBuff, sizeof(wBuff)); 
		bzero(command, sizeof(command)); 
		bzero(downloadFilename, sizeof(command)); 
		
		if(flagDownload == 2) {
			n = recvfrom(localSockfdUdp, rBuff, sizeof(rBuff) - 1, 0, &serverAddress_udp, &serverlen );
			if (n < 0) error("ERROR reading from socket");
			printf("%s\n", rBuff);
			flagDownload = 0;

		}
		else if(flagDownload == 1) {
			n = read(localSockfd, rBuff, sizeof(rBuff) - 1);
			if (n < 0) error("ERROR reading from socket");
			printf("%s\n", rBuff);
			flagDownload = 0;

		}

		printf("$> ");
		fgets(wBuff, sizeof(wBuff), stdin);
		
		if(strlen(wBuff) < 2) 
			continue;
		
		char** commandTokens = malloc((sizeof(char)*BUFFSIZE)*BUFFSIZE);

		strcpy(history[commandCnt++], wBuff);
		strcpy(command, wBuff);
		
		int tok = parse_cmd(command, commandTokens);
		
		typeCommand = commandCheck(commandTokens, tok);
		
		if(typeCommand == 3) {
			if(tok != 3 || !(strcmp(commandTokens[1], "TCP") == 0 || strcmp(commandTokens[1], "UDP") == 0)) {
				fprintf(stderr, "Usage: FileDownload <protocol> <filename>\n");
				continue;
			}
			if(strcmp(remoteIP, "127.0.0.1") == 0) 
				strcat(downloadFilename, "downloaded_");
			strcat(downloadFilename, commandTokens[2]);
			Fp = fopen(downloadFilename, "wb");
			
			if(strcmp(commandTokens[1], "TCP") == 0) 
				n = write(localSockfd, wBuff, strlen(wBuff));
			else
				n = sendto(localSockfdUdp, wBuff, strlen(wBuff), 0, &serverAddress_udp, serverlen);
			
			if (n < 0) 
				error("ERROR writing to socket");
			
			while(1) {
				memset(rBuff, 0, strlen(rBuff));
				if(strcmp(commandTokens[1], "TCP") == 0) 
					n = read(localSockfd, rBuff, sizeof(rBuff) - 1);
				else
					n = recvfrom(localSockfdUdp, rBuff, sizeof(rBuff) - 1, 0, &serverAddress_udp, &serverlen);
				if(n < 0) {
					fprintf(stderr, "ERROR reading from socket");
					break;
				}
				rBuff[n] = 0;
				if(rBuff[n-3] == 'E' && rBuff[n-2] == 'N' && rBuff[n-1] == 'D') {
					rBuff[n-3] = 0;
					fwrite(rBuff, 1 , n-3, Fp);
					fclose(Fp);
					printf("File successfully downloaded!\n");
					if(strcmp(commandTokens[1], "TCP") == 0) flagDownload = 1;
					else flagDownload = 2;
					break;
				}
				else 
					fwrite(rBuff, 1, n, Fp);
			}

		}
		else if(typeCommand == 1 || typeCommand == 2){
			n = write(localSockfd, wBuff, strlen(wBuff));

			if (n < 0) 
				error("ERROR writing to socket");

			n = read(localSockfd, rBuff, sizeof(rBuff) - 1);
			if (n < 0) 
				error("ERROR reading from socket");
			printf("%s\n", rBuff);
		}
		
	}
	close(localSockfd);
	close(localSockfdUdp);
}

#endif