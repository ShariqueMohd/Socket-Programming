#ifndef COMMON_H
#define COMMON_H

#include "Headers.h"


void longlist() 
{
	int i = 0; 
	DIR *remoteDir;
	struct dirent *fptr;
	struct stat fileStatus;

	remoteDir = opendir ("./");

	if( remoteDir == NULL) 
		sprintf(res, "ERROR opening the directory\n");
	else
	{
		while (fptr = readdir (remoteDir))
		{
			if(stat(fptr->d_name, &fileStatus) < 0) {
				sprintf(res, "ERROR stat unsuccessful\n");
				break;
			}
			else
			{
				strcpy(fData[i].filename, fptr->d_name);
				fData[i].size = fileStatus.st_size;
				fData[i].mtime = fileStatus.st_mtime;
				fData[i].type = (S_ISDIR(fileStatus.st_mode)) ? 'd' : '-';
				i++;
			}
		}
		fileDataCnt = i;
		closedir (remoteDir);
	}
}

int shortlist(time_t sTime, time_t eTime)
{
	int i = 0;
	DIR *remoteDir;
	struct dirent *fptr;
	struct stat fileStatus;

	remoteDir = opendir ("./");

	if( remoteDir == NULL) sprintf(res, "ERROR opening the directory\n");
	else {
		while (fptr = readdir (remoteDir))
		{
			if(stat(fptr->d_name, &fileStatus) < 0) {
				sprintf(res, "ERROR stat unsuccessful\n");
				break;
			}
			else if(difftime(fileStatus.st_mtime, sTime) >= 0 && difftime(fileStatus.st_mtime, eTime) <= 0)
			{
				strcpy(fData[i].filename, fptr->d_name);
				fData[i].size = fileStatus.st_size;
				fData[i].mtime = fileStatus.st_mtime;
				fData[i].type = (S_ISDIR(fileStatus.st_mode)) ? 'd' : '-';
				i++;
			}
		}
		fileDataCnt=i;
		closedir (remoteDir);
	}
}

int regex(char *expr) 
{

	FILE *pipe;
	char command[MAXL] = "ls ";

	char line[1024];
	char rBuff[BUFFSIZE];

	strcat(command, expr);
	strcat(command, " 2>/dev/null");

	int i = 0;
	DIR *remoteDir;
	struct dirent *fptr;
	struct stat fileStatus;

	remoteDir = opendir ("./");

	if( remoteDir == NULL) sprintf(res, "ERROR opening the directory\n");
	else {
		while (fptr = readdir (remoteDir))
		{
			if(stat(fptr->d_name, &fileStatus) < 0) {
				sprintf(res, "ERROR stat unsuccessful\n");
				break;
			}
			else if(( pipe = popen(command, "r")) == NULL) {
				sprintf(res, "ERROR with popen\n");
				break;
			}
			else {
				while(fgets(rBuff, MAXLN, pipe))
				{
					if(strncmp(rBuff, fptr->d_name, strlen(rBuff) - 1) == 0) 
					{
						strcpy(fData[i].filename, fptr->d_name);
						fData[i].size = fileStatus.st_size;
						fData[i].mtime = fileStatus.st_mtime;
						fData[i].type = (S_ISDIR(fileStatus.st_mode)) ? 'd' : '-';
						i++;
					}
				}
			}
		}
		fileDataCnt=i;
		if(fileDataCnt == 0) {
			sprintf(res, "No such file or directory\n");
		}
		closedir (remoteDir);
	}
}


int getIndexCommand(char** commandTokens, int tok) {
	struct tm timeStamp;
	time_t sTime , eTime;
	if(strcmp(commandTokens[1], "longlist") == 0) {
		if(tok != 2)
			sprintf(res, "Usage: index longlist\n");

		else {
			longlist();
		}
	}
	else if(strcmp(commandTokens[1], "shortlist") == 0) {

		if(tok != 4) {
			sprintf(res, "Usage: index ​shortlist <starttimestamp> <end​timestamp>\n TimeStamp format is date-month-year-hrs:minutes:seconds\n");
			return;
		}
		if(strptime(commandTokens[2], "%d-%m-%Y-%H:%M:%S", &timeStamp) == NULL) {
			sprintf(res, "Incorrect Format for Timestamp. The correct format is: Date-Month-Year-hrs:min:sec\n");
			return;
		}
		else 
			sTime = mktime(&timeStamp);
		if(strptime(commandTokens[3], "%d-%m-%Y-%H:%M:%S", &timeStamp) == NULL) {
			sprintf(res, "Incorrect Format for Timestamp. The correct format is: Date-Month-Year-hrs:min:sec\n");
			return;
		}
		else
			eTime = mktime(&timeStamp);
		shortlist(sTime, eTime);

	}
	else if(strcmp(commandTokens[1], "regex") == 0) {
		if(tok != 3) {
			sprintf(res, "Usage: index ​regex <regular expression>\n");
			return;
		}
		regex(commandTokens[2]);
	}
	else {
		sprintf(res, "No such command\n");
		return;

	}
}

int parse_cmd(char* cmd, char** commandTokens) 
{
	int tok = 0;
	char* token = strtok(cmd, DELIMITERS);
	while(token!=NULL) {
		commandTokens[tok++] = token;
		token = strtok(NULL, DELIMITERS);
	}
	return tok;
}

int commandCheck(char** commandTokens, int tok) {
	if(strcmp(commandTokens[0], "index") == 0) 
		return 1;
	else if(strcmp(commandTokens[0], "hash") == 0) 
		return 2;
	else if(strcmp(commandTokens[0], "download") == 0) 
		return 3;
	else if(strcmp(commandTokens[0], "History") == 0) {
		int i;
		for(i = 0; i<commandCnt; i++) 
			printf("%s\n", history[i]);
		printf("-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-\n");
		return 4;
	}
	else {
		printf("No such command\n");
		return -1;
	}
}

void error(const char *msg)
{
	perror(msg);
	exit(1);
}


void crossCheck(char *filename) 
{
	int i = 0; 
	DIR *remoteDir;
	FILE *file;
	struct dirent *fptr;
	struct stat fileStatus;
	char rBuff[BUFFSIZE];
	MD5_CTX c;
	unsigned long len;
	remoteDir = opendir ("./");
	if( remoteDir == NULL) sprintf(res, "ERROR opening the directory\n");
	else
	{
		while (fptr = readdir (remoteDir))
		{
			if(stat(fptr->d_name, &fileStatus) < 0) {
				sprintf(res, "ERROR stat unsuccessful\n");
				break;
			}
			else if(strcmp(filename, fptr->d_name) != 0) continue;
			else
			{
				file = fopen(fptr->d_name, "rb");
				if(!file) {
					sprintf(res, "ERROR opening file %s\n", fptr->d_name);
					break;
				}

				strcpy(cData[i].filename, fptr->d_name);
				cData[i].mtime = fileStatus.st_mtime;
				if(!MD5_Init (&c)) {
					sprintf(res, "ERROR with MD5\n");
					break;
				}
				while( (len = fread(rBuff, 1, MAXLN, file)) != 0 ) {
					MD5_Update(&c, rBuff, len);
				}
				MD5_Final(cData[i].checksum, &c);
				fclose(file);
				i++;
			}
		}
	}
	checksumDataCnt = i;
	if(checksumDataCnt == 0) {
		sprintf(res, "No such file or directory\n");
	}
	closedir (remoteDir);
}


int fileHashCommand(char** commandTokens, int tokenSize) {

	if(strcmp(commandTokens[1], "checkall") == 0) {
		if(tokenSize != 2)
			sprintf(res, "Usage: FileHash checkall\n");
		else {
			checkall();
		}
	}
	else if(strcmp(commandTokens[1], "verify") == 0) {

		if(tokenSize != 3) {
			sprintf(res, "Usage: FileHash ​verify <filename>\n");
		}
		else crossCheck(commandTokens[2]);
	}
	else {
		sprintf(res, "Usage: IndexGet ​regex <regular expression>\n");
		return;
	}
}

void checkall() {
	int i = 0; 
	DIR *remoteDir;
	FILE *file;
	struct dirent *fptr;
	struct stat fileStatus;
	char rBuff[BUFFSIZE];
	MD5_CTX c;
	unsigned long len;
	remoteDir = opendir ("./");
	if( remoteDir == NULL) sprintf(res, "ERROR opening the directory\n");
	else
	{
		while (fptr = readdir (remoteDir))
		{

			if(stat(fptr->d_name, &fileStatus) > 0)
			{
				file = fopen(fptr->d_name, "rb");
				if(!file) {
					sprintf(res, "ERROR opening file %s\n", fptr->d_name);
					break;
				}

				strcpy(cData[i].filename, fptr->d_name);
				cData[i].mtime = fileStatus.st_mtime;
				printf("%s\n", cData[i].mtime);
				if(!MD5_Init (&c)) {
					sprintf(res, "ERROR with MD5\n");
					break;
				}
				while( (len = fread(rBuff, 1, MAXLN, file)) != 0 ) {
					MD5_Update(&c, rBuff, len);
					printf("jsdjdj\n");
				}
				MD5_Final(cData[i].checksum, &c);
				//printf("%s\n", cData[i].checksum);
				fclose(file);
				i++;
			}
			else if (stat(fptr->d_name, &fileStatus) < 0) {
				sprintf(res, "ERROR stat unsuccessful\n");
				break;
			}

			
		}
	}
	checksumDataCnt = i;
	closedir (remoteDir);
}

void getdownload(char *filename) {
	struct dirent *fp;
	struct stat statFile;

	char rBuff[BUFFSIZE];
	unsigned long len;
	
	DIR *remoteDir;
	FILE *file;
	MD5_CTX c;
	
	remoteDir = opendir ("./");

	int i = 0; 
	if( remoteDir == NULL) 
		sprintf(res, "ERROR opening the directory\n");
	else{
		while (fp = readdir (remoteDir)){
			
			if(strcmp(filename, fp->d_name) != 0) 
				continue;
			else if(stat(fp->d_name, &statFile) < 0) {
				sprintf(res, "ERROR stat unsuccessful\n");
				break;
			}
			else
			{
				strcpy(dData[i].filename, fp->d_name);
				dData[i].size = statFile.st_size;
				dData[i].mtime = statFile.st_mtime;
				file = fopen(fp->d_name, "rb");
				if(!file) {
					sprintf(res, "ERROR opening file %s\n", fp->d_name);
					break;
				}
				if(!MD5_Init (&c)) {
					sprintf(res, "ERROR with MD5\n");
					break;
				}
				while( (len = fread(rBuff, 1, MAXLN, file)) != 0 ) {
					MD5_Update(&c, rBuff, len);
				}
				MD5_Final(dData[i].checksum, &c);
				fclose(file);
				i++;
			}
		}
		dDataIndex = i;
		if(dDataIndex == 0) {
			sprintf(res, "No such file or directory\n");
		}
		closedir (remoteDir);
	}
}

#endif