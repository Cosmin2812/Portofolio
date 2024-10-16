#ifndef __COMMON_H__
#define __COMMON_H__

#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <math.h>

// Maximum size of the message sent by user
#define MSG_MAXSIZE 1024

int send_all(int sockfd, void *buff, size_t len);
int recv_all(int sockfd, void *buff, size_t len);
#endif