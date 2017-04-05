#ifndef HEADERS_H
#define HEADERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/md5.h>
#include <openssl/hmac.h>
#include <dirent.h>
#include <netdb.h>

// Macros
#define BUFFSIZE 10240 
#define MAXL 100
#define MAXLN 1024
#define DELIMITERS " \t\n"

struct file {
	char filename[MAXL];
	off_t size;
	time_t mtime;
	char type;
} fData[BUFFSIZE];

struct checkSum {
	char filename[MAXL];
	time_t mtime;
	unsigned char checksum[MD5_DIGEST_LENGTH];
} cData[BUFFSIZE];

struct download {
	char filename[MAXL];
	off_t size;
	time_t mtime;
	unsigned char checksum[MD5_DIGEST_LENGTH];
} dData[BUFFSIZE];

char history[MAXLN][MAXL], res[BUFFSIZE];
int commandCnt;
int fileDataCnt, checksumDataCnt, dDataIndex;
int remoteTCP, remoteUDP;
int localTCP, localUDP;

#endif