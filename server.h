#ifndef SERVER_H
#define SERVER_H

int udpServer(){
	
	int typeCommand, localSockfd, remoteSockfd;
	char rBuff[BUFFSIZE];
	char wBuff[BUFFSIZE];
	char command[MAXL];
	int clientlen;

	struct sockaddr_in serverAddress, client_addr;
	
	localSockfd = socket(AF_INET, SOCK_DGRAM, 0); 
	if (localSockfd < 0) 
		error("ERROR opening socket");
	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(localUDP);

	bzero((char *) &serverAddress, sizeof(serverAddress));
	
	if (bind(localSockfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) 
		error("ERROR on binding");

	int n;
	clientlen = sizeof(client_addr);
	while(1) {
		bzero(rBuff, sizeof(rBuff));
		bzero(wBuff, sizeof(wBuff));
		bzero(res, sizeof(res));

		n = recvfrom(localSockfd, rBuff, sizeof(rBuff), 0, (struct sockaddr *) &client_addr, &clientlen);

		if(n > 0) {
			char** commandTokens = malloc((sizeof(char)*BUFFSIZE)*BUFFSIZE);
			printf("Command Received: %s$> ", rBuff);

			fflush(stdout);

			int tok = parse_cmd(rBuff, commandTokens);
			typeCommand = commandCheck(commandTokens, tok);

			if(typeCommand == 3) {
				FILE* file;
				file = fopen(commandTokens[2], "rb");
				size_t bytes_read;
				while(!feof(file))
				{
					bytes_read = fread(res, 1, 1024, file);
					memset(wBuff, 0, sizeof(wBuff));
					memcpy(wBuff,res,bytes_read);
					sendto(localSockfd , wBuff , bytes_read, 0, (struct sockaddr *) &client_addr, clientlen);
					memset(wBuff, 0, sizeof(wBuff));
					memset(res, 0, sizeof(res));
				}
				memcpy(wBuff,"END",3);
				sendto(localSockfd , wBuff , 3, 0, (struct sockaddr *) &client_addr, clientlen);
				memset(wBuff, 0, sizeof(wBuff));
				fclose(file);
				getdownload(commandTokens[2]);
				int k;
				strcat(wBuff, res);
				int j;
				for(j=0; j<dDataIndex; j++)
				{
					sprintf(res, "%-20s  %-10d", dData[j].filename, dData[j].size);
					strcat(wBuff,res);
					for(k = 0; k<MD5_DIGEST_LENGTH; k++) {
						sprintf(res, "%02x", dData[j].checksum[k]);
						strcat(wBuff, res);
					}
					sprintf(res, "           %-20s", ctime(&dData[j].mtime));

					strcat(wBuff, res);

				}
				strcat(wBuff, "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
				sendto(localSockfd, wBuff, strlen(wBuff), 0, (struct sockaddr *) &client_addr, clientlen);
			}
			else if(typeCommand == 2) {
				int j, k;
				
				fileHashCommand(commandTokens, tok);
				strcat(wBuff, res);
				
				for(j=0; j<checksumDataCnt; j++) {
					sprintf(res, "%-20s  ", cData[j].filename);
					strcat(wBuff,res);
					for(k = 0; k<MD5_DIGEST_LENGTH; k++) {
						sprintf(res, "%02x", cData[j].checksum[k]);
						strcat(wBuff,res);
					}
					sprintf(res, "          %-30s\n", ctime(&cData[j].mtime));
					strcat(wBuff,res);
				}
				strcat(wBuff, "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
				write(localSockfd, wBuff, strlen(wBuff));
			}
			else {
				getIndexCommand(commandTokens, tok);
				strcat(wBuff, res);
				int j;
				for(j=0; j<fileDataCnt; j++)
				{
					sprintf(res, "%-20s  %-10d  %-10c %-20s\n", fData[j].filename, fData[j].size, fData[j].type, ctime(&fData[j].mtime));
					strcat(wBuff,res);
				}
				strcat(wBuff, "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
				write(localSockfd, wBuff, strlen(wBuff));
			}
			 
		} 
	}
	close(localSockfd);
	wait(NULL);
}

int tcpServer() {
	
	char rBuff[BUFFSIZE];
	char wBuff[BUFFSIZE];
	char command[MAXL];
	int typeCommand;
	int locSocketFD, remSocketFD;
	
	struct sockaddr_in serverAddress;
	
	int n;
	locSocketFD = socket(AF_INET, SOCK_STREAM, 0); 
	if (locSocketFD < 0) 
		error("ERROR opening socket");
	
	bzero((char *) &serverAddress, sizeof(serverAddress));
	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(localTCP);

	if (bind(locSocketFD, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) 
		error("ERROR on binding");
	listen(locSocketFD, 5);
	remSocketFD = accept(locSocketFD, (struct sockaddr *)NULL, NULL);
	if (remSocketFD < 0) error("ERROR on accept");

	while(1) {
		bzero(rBuff, sizeof(rBuff));
		bzero(wBuff, sizeof(wBuff));
		bzero(res, sizeof(res));

		n = read(remSocketFD, rBuff, sizeof(rBuff));

		if(n > 0) {
			char** commandTokens = malloc((sizeof(char)*BUFFSIZE)*BUFFSIZE);
			printf("Command Received: %s$> ", rBuff);

			fflush(stdout);

			int tok = parse_cmd(rBuff, commandTokens);
			typeCommand = commandCheck(commandTokens, tok);
			if(typeCommand == 1)
			{
				getIndexCommand(commandTokens, tok);
				strcat(wBuff, res);
				int j;
				for(j=0; j<fileDataCnt; j++)
				{
					sprintf(res, "%-20s  %-10d  %-10c %-20s\n", fData[j].filename, fData[j].size, fData[j].type, ctime(&fData[j].mtime));
					strcat(wBuff,res);
				}
				strcat(wBuff, "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
				write(remSocketFD, wBuff, strlen(wBuff));
			}
			else if(typeCommand == 2) 
			{
				fileHashCommand(commandTokens, tok);
				int k;
				strcat(wBuff, res);
				int j;
				for(j=0; j<checksumDataCnt; j++)
				{
					sprintf(res, "%-20s  ", cData[j].filename);
					strcat(wBuff,res);
					for(k = 0; k<MD5_DIGEST_LENGTH; k++) {
						sprintf(res, "%02x", cData[j].checksum[k]);
						strcat(wBuff,res);
					}
					sprintf(res, "          %-30s\n", ctime(&cData[j].mtime));
					strcat(wBuff,res);
				}
				strcat(wBuff, "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
				write(remSocketFD, wBuff, strlen(wBuff));
			}
			else if(typeCommand == 3) {
				FILE* file;
				file = fopen(commandTokens[2], "rb");
				size_t bytesRead;
				while(!feof(file))
				{
					bytesRead = fread(res, 1, MAXLN, file);
					memset(wBuff, 0, sizeof(wBuff));
					memcpy(wBuff, res, bytesRead);
					write(remSocketFD, wBuff, bytesRead);
					memset(wBuff, 0, sizeof(wBuff));
					memset(res, 0, sizeof(res));
				}
				memcpy(wBuff,"End",3);
				write(remSocketFD , wBuff , 3);
				memset(wBuff, 0, sizeof(wBuff));
				fclose(file);
				getdownload(commandTokens[2]);
				int k;
				strcat(wBuff, res);
				int j;
				for(j=0; j<dDataIndex; j++)
				{
					sprintf(res, "%-20s  %-10d", dData[j].filename, dData[j].size);
					strcat(wBuff,res);
					for(k = 0; k<MD5_DIGEST_LENGTH; k++) {
						sprintf(res, "%02x", dData[j].checksum[k]);
						strcat(wBuff,res);
					}
					sprintf(res, "           %-20s", ctime(&dData[j].mtime));
					strcat(wBuff,res);
				}
				strcat(wBuff, "-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-");
				write(remSocketFD, wBuff, strlen(wBuff));
			} 
		} 
	}
	close(remSocketFD);
	close(locSocketFD);
	wait(NULL);
}


#endif